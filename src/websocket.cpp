#include "../inc/websocket.h"


/**
 * @brief Construct a new Websocket::Websocket object
 * 
 * @param ioc 
 * @param ctx 
 */
Websocket::Websocket(net::io_context& ioc, ssl::context& ctx)
    : mResolver(net::make_strand(ioc))
    , mWs(net::make_strand(ioc), ctx)
{
    ELOG(INFO, "Websocket constructor initialized.");
}


/**
 * @brief Destroy the Websocket::Websocket object
 * 
 */
Websocket::~Websocket()
{
    ELOG(INFO, "Websocket destructor.");
}


/**
 * @brief Websocket run function
 * 
 * @param host 
 * @param port 
 * @param endpoint 
 */
void Websocket::run(std::string host, std::string port, std::string endpoint)
{
    // Save these for later
    mHost       = host;
    mEndpoint   = endpoint;

    ELOG(INFO, "Websocket is running.");

    // Look up the domain name
    mResolver.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &Websocket::resolve,
            shared_from_this()));
}


/**
 * @brief Websocket resolve function
 * 
 * @param ec 
 * @param results 
 */
void Websocket::resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
    {
        ELOG(ERROR, "Received a Socket Error. Error: %s", ec.message().c_str());
    }

    ELOG(INFO, "Websocket on resolve.");

    // Set a timeout on the operation
    beast::get_lowest_layer(mWs).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(mWs).async_connect(
        results,
        beast::bind_front_handler(
            &Websocket::connect,
            shared_from_this()));
}


/**
 * @brief Websocket connect function
 * 
 * @param ec 
 * @param ep 
 */
void Websocket::connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        ELOG(ERROR, "Received a Socket Error. Error: %s", ec.message().c_str());
    }

    ELOG(INFO, "Websocket connected successfully.");

    // Update the mHost string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    mHost += ':' + std::to_string(ep.port());

    // Set a timeout on the operation
    beast::get_lowest_layer(mWs).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (! SSL_set_tlsext_host_name(
            mWs.next_layer().native_handle(),
            mHost.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
    }

    // Perform the SSL handshake
    mWs.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &Websocket::sslHandshake,
            shared_from_this()));
}


/**
 * @brief Websocket ssl handshake function
 * 
 * @param ec 
 */
void Websocket::sslHandshake(beast::error_code ec)
{
    if (ec)
    {
        ELOG(ERROR, "Received a Socket Error. Error: %s", ec.message().c_str());
    }

    ELOG(INFO, "Websocket SSL handshake did successfully.");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(mWs).expires_never();

    // Set suggested timeout settings for the websocket
    mWs.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    mWs.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " cryptobot");
        }));

    // Perform the websocket handshake
    mWs.async_handshake(mHost, mEndpoint,
        beast::bind_front_handler(
            &Websocket::handshake,
            shared_from_this()));
    
}


/**
 * @brief Websocket handshake function
 * 
 * @param ec 
 */
void Websocket::handshake(beast::error_code ec)
{
    if (ec)
    {
        ELOG(ERROR, "Received a Socket Error. Error: %s", ec.message().c_str());
    }

    ELOG(INFO, "Websocket handshake did successfully.");

    // Read a message into our buffer
    mWs.async_read(
        mBuffer,
        beast::bind_front_handler(
            &Websocket::read,
            shared_from_this()));
}


/**
 * @brief Websocket read function
 * 
 * @param ec 
 * @param bytes_transferred 
 */
void Websocket::read(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    Opel *iOpel = Opel::instance();

    if (ec)
    {
        ELOG(ERROR, "Received a Socket Error. Error: %s", ec.message().c_str());

        iOpel->setExitSignal(0);
    }
    
    if (!iOpel->getExitSignal())
    {
        mWs.next_layer().next_layer().release_socket();

        mWs.next_layer().next_layer().close();

        mWs.close(boost::beast::websocket::close_code::normal, ec);
    }
    else
    {
        if (mBuffer.size() > 0)
        {
            std::string bufferJson = beast::buffers_to_string(mBuffer.data());

            Json::Value     rCandlestickJson;
            Json::Reader    reader;

            bool parsingSuccessful = reader.parse( bufferJson.c_str(), rCandlestickJson );

            if ( !parsingSuccessful )
            {
                ELOG(ERROR, "Failed to JSON parse.");
            }
            
            Json::Value rKData              = rCandlestickJson["k"];

            bool rIsClosed                  = rCandlestickJson["k"].get("x", true).asBool();

            std::string rSymbol             = rCandlestickJson["s"].asString();
            std::string rTimestamp          = rKData["t"].asString();
            std::string rOpenPrice          = rKData["o"].asString();
            std::string rClosePrice         = rKData["c"].asString();
            std::string rHighPrice          = rKData["h"].asString();
            std::string rLowPrice           = rKData["l"].asString();
            std::string rVolume             = rKData["v"].asString();

            ELOG(INFO, "WS -> %s: %s, Volume: %s.", rSymbol.c_str(), rClosePrice.c_str(), rVolume.c_str());

            if (rSymbol == iOpel->getTradeSymbol())
            {
                struct Candle *pTradeCandleData    = Opel::getTradeCandleStruct();

                pTradeCandleData->lock();

                pTradeCandleData->isClosed      = rIsClosed;
                pTradeCandleData->isUpdated     = true;

                pTradeCandleData->symbol        = rSymbol;
                pTradeCandleData->timestamp     = rTimestamp;
                pTradeCandleData->openPrice     = rOpenPrice;
                pTradeCandleData->closePrice    = rClosePrice;
                pTradeCandleData->highPrice     = rHighPrice;
                pTradeCandleData->lowPrice      = rLowPrice;
                pTradeCandleData->volume       = rVolume;

                pTradeCandleData->unlock();
            }
            else
            {
                struct Candle *pFollowCandleData = Opel::getFollowCandleStruct();

                pFollowCandleData->lock();

                pFollowCandleData->isClosed     = rIsClosed;
                pFollowCandleData->isUpdated    = true;

                pFollowCandleData->symbol       = rSymbol;
                pFollowCandleData->timestamp    = rTimestamp;
                pFollowCandleData->openPrice    = rOpenPrice;
                pFollowCandleData->closePrice   = rClosePrice;
                pFollowCandleData->highPrice    = rHighPrice;
                pFollowCandleData->lowPrice     = rLowPrice;
                pFollowCandleData->volume      = rVolume;

                pFollowCandleData->unlock();
            }
            
            // Clear the buffer
            mBuffer.consume(mBuffer.size());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Read a message into our buffer
        mWs.async_read(
            mBuffer,
            beast::bind_front_handler(
                &Websocket::read,
                shared_from_this()));
    }
}


/**
 * @brief Websocket close function
 * 
 */
void Websocket::close()
{
    ELOG(INFO, "Websocket has been closed. Buffer has been cleared.");

    mBuffer.consume(mBuffer.size());
}


/**
 * @brief Construct a new BinanceWebsocket::BinanceWebsocket object
 * 
 * @param pBu 
 */
BinanceWebsocket::BinanceWebsocket(std::shared_ptr<BinanceUtilities> pBu)
{
    mHost           = pBu.get()->getWebsocketBase();
    mPort           = pBu.get()->getWebsocketPort();
    mEndpointT      = pBu.get()->getWebsocketEndpointT();
    mEndpointF      = pBu.get()->getWebsocketEndpointF();
    mTradeSymbol    = pBu.get()->getTradeSymbol();
    mFollowSymbol   = pBu.get()->getFollowSymbol();


    Opel *iOpel     = Opel::instance(); 

    iOpel->setTradeSymbol(mTradeSymbol);
    iOpel->setFollowSymbol(mFollowSymbol);


    ELOG(INFO, "Websocket constructor initialized. Host: %s, Port: %s, Trade Endp: %s, Follow Endp: %s.", 
                mHost.c_str(),
                mPort.c_str(), 
                mEndpointT.c_str(), 
                mEndpointF.c_str());
}


/**
 * @brief Destroy the BinanceWebsocket::BinanceWebsocket object
 * 
 */
BinanceWebsocket::~BinanceWebsocket()
{
    ELOG(INFO, "BinanceWebsocket destructor.");
}


/**
 * @brief İnitialize
 * 
 */
void BinanceWebsocket::init()
{
    ELOG(INFO, "Websocket is initializing.");

    // This holds the root certificate used for verification
    load_root_certificates(mCtx);

    // Launch the asynchronous operation
    std::shared_ptr<Websocket> wsT = std::make_shared<Websocket>(mIoc, mCtx);

    wsT->run(mHost, mPort, mEndpointT);

    std::shared_ptr<Websocket> wsF = std::make_shared<Websocket>(mIoc, mCtx);

    wsF->run(mHost, mPort, mEndpointF);

    ELOG(INFO, "Websocket initialized.");

    // Run the I/O service. The call will return when
    // the socket is closed.
    mIoc.run();

    // Clear buffers when websockets closed
    wsT->close();

    wsF->close();
    
    ELOG(INFO, "Websocket -> detached.");
}