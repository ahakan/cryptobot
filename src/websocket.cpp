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
        ELOG(ERROR, "Resolve error: %s", ec.message().c_str());

        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
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
        ELOG(ERROR, "Connect error: %s", ec.message().c_str());

        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
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
        // return fail(ec, "connect");
    }

    Opel *iOpel = Opel::instance();

    if (!iOpel->getExitSignal())
    {
        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
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
        ELOG(ERROR, "SSL Handshake error: %s", ec.message().c_str());

        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
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

    Opel *iOpel = Opel::instance();

    if (!iOpel->getExitSignal())
    {
        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
    }

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
        ELOG(ERROR, "Handshake error: %s", ec.message().c_str());

        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
    }

    ELOG(INFO, "Websocket handshake did successfully.");

    Opel *iOpel = Opel::instance();

    if (!iOpel->getExitSignal())
    {
        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
    }

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
void Websocket::read( beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        ELOG(ERROR, "Read error: %s", ec.message().c_str());

        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
    }

    Opel *iOpel = Opel::instance();

    if (!iOpel->getExitSignal())
    {
        // Close the WebSocket connection
        mWs.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Websocket::close,
                shared_from_this()));
    }
    else
    {
        if (mBuffer.size() > 0)
        {
            std::string bufferJson = beast::buffers_to_string(mBuffer.data());

            Json::Value mCandlestickJson;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse( bufferJson.c_str(), mCandlestickJson );

            if ( !parsingSuccessful )
            {
                ELOG(ERROR, "Failed to JSON parse.");
            }
            
            Json::Value mKData              = mCandlestickJson["k"];

            bool mIsClosed                  = mCandlestickJson["k"].get("x", true).asBool();

            std::string mSymbol             = mCandlestickJson["s"].asString();
            std::string mTimestamp          = mKData["t"].asString();
            std::string mOpenPrice          = mKData["o"].asString();
            std::string mClosePrice         = mKData["c"].asString();
            std::string mHighPrice          = mKData["h"].asString();
            std::string mLowPrice           = mKData["l"].asString();

            ELOG(INFO, "Websocket on read. Symbol: %s, Price: %s, Buffer size: %dKB.", mSymbol.c_str(), mClosePrice.c_str(), mBuffer.size());

            Opel *iOpel = Opel::instance();
            
            if (mSymbol == iOpel->getTradeSymbol())
            {
                struct candle_data *pTradeCandleData    = Opel::getTradeCandleStruct();

                pTradeCandleData->lock();
                pTradeCandleData->isUpdated             = true;
                pTradeCandleData->symbol                = mSymbol;
                pTradeCandleData->timestamp             = mTimestamp;
                pTradeCandleData->openPrice             = mOpenPrice;
                pTradeCandleData->closePrice            = mClosePrice;
                pTradeCandleData->highPrice             = mHighPrice;
                pTradeCandleData->lowPrice              = mLowPrice;
                pTradeCandleData->isClosed              = mIsClosed;
                pTradeCandleData->unlock();
            }
            else
            {
                struct candle_data *pFollowCandleData = Opel::getFollowCandleStruct();

                pFollowCandleData->lock();
                pFollowCandleData->isUpdated            = true;
                pFollowCandleData->symbol               = mSymbol;
                pFollowCandleData->timestamp            = mTimestamp;
                pFollowCandleData->openPrice            = mOpenPrice;
                pFollowCandleData->closePrice           = mClosePrice;
                pFollowCandleData->highPrice            = mHighPrice;
                pFollowCandleData->lowPrice             = mLowPrice;
                pFollowCandleData->isClosed             = mIsClosed;
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
 * @param ec 
 */
void Websocket::close(beast::error_code ec)
{
    if (ec)
    {
        struct socket_error *pSocketError = Opel::getSocketErrorStruct();

        pSocketError->message = ec.message();

        ELOG(INFO, "Websocket closed. Code: %s", ec.message().c_str());
    }

    mBuffer.consume(mBuffer.size());

    mWs.next_layer().next_layer().release_socket();

    mWs.next_layer().next_layer().close();

    mWs.close(boost::beast::websocket::close_code::normal, ec);
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
    mTradeSymbol    = pBu.get()->getSymbol();
    mFollowSymbol   = pBu.get()->getFollowSymbol();


    Opel *iOpel     = Opel::instance(); 

    iOpel->setTradeSymbol(mTradeSymbol);
    iOpel->setFollowSymbol(mFollowSymbol);


    ELOG(INFO, "Websocket constructor initialized. Host: %s, Port: %s, Trade Endpoint: %s, Follow Endpoint: %s.", 
                mHost.c_str(), mPort.c_str(), mEndpointT.c_str(), mEndpointF.c_str());
}


/**
 * @brief Destroy the BinanceWebsocket::BinanceWebsocket object
 * 
 */
BinanceWebsocket::~BinanceWebsocket()
{
    ELOG(INFO, "BinanceWebsocket destructor.");

    mIoc.stop();
}


/**
 * @brief İnitialize
 * 
 */
void BinanceWebsocket::init()
{
    Opel *iOpel = Opel::instance();
    
    ELOG(INFO, "Websocket is initializing.");

    while (iOpel->getExitSignal())
    {
        struct socket_error *pSocketError = Opel::getSocketErrorStruct();

        if (pSocketError->message.length() > 0)
        {
            mIoc.stop();

            ELOG(INFO, "Received a Socket Error. Restarting socket connection. Error: %s.", pSocketError->message.c_str());

            pSocketError->message = "";
        }

        if (iOpel->getExitSignal())
        {
            // This holds the root certificate used for verification
            load_root_certificates(mCtx);

            // Launch the asynchronous operation
            std::make_shared<Websocket>(mIoc, mCtx)->run(mHost, mPort, mEndpointT);

            std::make_shared<Websocket>(mIoc, mCtx)->run(mHost, mPort, mEndpointF);

            ELOG(INFO, "Websocket initialized.");

            // Run the I/O service. The call will return when
            // the socket is closed.
            mIoc.run();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ELOG(INFO, "Thread Websocket detached.");
}