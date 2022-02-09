#include "../inc/websocket.h"
#include <boost/beast/core/buffers_to_string.hpp>


Websocket::Websocket(net::io_context& ioc, ssl::context& ctx)
    : mResolver(net::make_strand(ioc))
    , ws_(net::make_strand(ioc), ctx)
{
    ELOG(INFO, "Websocket constructor initialized.");
}

Websocket::~Websocket()
{
    ELOG(INFO, "Websocket destructor.");
}

void Websocket::run(std::string host, std::string port, std::string endpoint, float *cand)
{
    // Save these for later
    mHost       = host;
    mEndpoint   = endpoint;
    pCandle     = cand;

    ELOG(INFO, "BinanceWebsocket is running.");

    // Look up the domain name
    mResolver.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &Websocket::on_resolve,
            shared_from_this()));
}

void Websocket::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if(ec)
    {
        ELOG(ERROR, "Resolve error: %s", ec.message().c_str());
    }

    ELOG(INFO, "BinanceWebsocket on resolve.");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(
            &Websocket::on_connect,
            shared_from_this()));
}

void Websocket::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if(ec)
    {
        ELOG(ERROR, "Connect error: %s", ec.message().c_str());
    }

    ELOG(INFO, "BinanceWebsocket connected successfully.");

    // Update the mHost string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    mHost += ':' + std::to_string(ep.port());

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if(! SSL_set_tlsext_host_name(
            ws_.next_layer().native_handle(),
            mHost.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
        // return fail(ec, "connect");
    }

    // Perform the SSL handshake
    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &Websocket::on_ssl_handshake,
            shared_from_this()));
}

void Websocket::on_ssl_handshake(beast::error_code ec)
{
    if(ec)
    {
        ELOG(ERROR, "SSL Handshake error: %s", ec.message().c_str());
    }
    ELOG(INFO, "BinanceWebsocket SSL handshake did successfully.");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " cryptobot");
        }));

    // Perform the websocket handshake
    ws_.async_handshake(mHost, mEndpoint,
        beast::bind_front_handler(
            &Websocket::on_handshake,
            shared_from_this()));
}

void Websocket::on_handshake(beast::error_code ec)
{
    if(ec)
    {
        ELOG(ERROR, "Handshake error: %s", ec.message().c_str());
    }

    ELOG(INFO, "BinanceWebsocket handshake did successfully.");

    // Read a message into our buffer
    ws_.async_read(
        mBuffer,
        beast::bind_front_handler(
            &Websocket::on_read,
            shared_from_this()));
}

void Websocket::on_read( beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec)
    {
        ELOG(ERROR, "Read error: %s", ec.message().c_str());
    }

    if (mBuffer.size() > 0)
    {
        ELOG(INFO, "BinanceWebsocket on read. Buffer size: %dKB", mBuffer.size());

        std::string bufferJson = beast::buffers_to_string(mBuffer.data());

        Json::Value mCandlestickJson;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( bufferJson.c_str(), mCandlestickJson );

        if ( !parsingSuccessful )
        {
            ELOG(ERROR, "Failed to JSON parse.");
        }
        
        Json::Value mKData = mCandlestickJson["k"];

        bool isClosed = mCandlestickJson["k"].get("x", true).asBool();

        std::string mCandlePrice = mKData["c"].asString();

        std::cout << "x: " << isClosed << " c: " << mCandlePrice << std::endl;

        if (isClosed == 1)
            *pCandle = std::stof(mCandlePrice);
        
        // Clear the buffer
        mBuffer.consume(mBuffer.size());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    // Read a message into our buffer
    ws_.async_read(
        mBuffer,
        beast::bind_front_handler(
            &Websocket::on_read,
            shared_from_this()));
}

void Websocket::on_close(beast::error_code ec)
{
    if(ec)
    {
        ELOG(ERROR, "Read error: %s", ec.message().c_str());
    }

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable(mBuffer.data()) << std::endl;
}


BinanceWebsocket::BinanceWebsocket(BinanceUtilities *pBu, float *cand)
{
    mHost       = pBu->getWebsocketBase();
    mPort       = pBu->getWebsocketPort();

    mEndpoint   = pBu->getWebsocketEndpoint();

    // std::cout << cand << std::endl;
    // std::cout << *cand << std::endl;
    pCand = cand;

    ELOG(INFO, "Websocket constructor initialized. Host: %s, Port: %s, Endpoint: %s.", 
                mHost.c_str(), mPort.c_str(), mEndpoint.c_str());
}

void BinanceWebsocket::init()
{
    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // This holds the root certificate used for verification
    load_root_certificates(ctx);

    ELOG(INFO, "Websocket init function.");

    // Launch the asynchronous operation
    std::make_shared<Websocket>(ioc, ctx)->run(mHost, mPort, mEndpoint, pCand);

    // Run the I/O service. The call will return when
    // the socket is closed.

    ioc.run();
}