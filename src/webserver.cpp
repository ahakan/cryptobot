#include "../inc/webserver.h"


/**
 * @brief Construct a new Webserver::Webserver object
 * 
 * @param socket 
 */
Webserver::Webserver(tcp::socket socket)
    : mSocket(std::move(socket))
{
    ELOG(INFO, "Webserver constructor initialized.");
}


/**
 * @brief Destroy the Webserver::Webserver object
 * 
 */
Webserver::~Webserver()
{
    ELOG(INFO, "Webserver destructor.");
}


/**
 * @brief Start webserver
 * 
 */
void Webserver::start()
{
    ELOG(INFO, "Webserver is started.");

    readRequest();
}


/**
 * @brief Read requests
 * 
 */
void Webserver::readRequest()
{
    auto self = shared_from_this();

    ELOG(INFO, "Webserver read request.");

    http::async_read(
        mSocket,
        mBuffer,
        mRequest,
        [self](beast::error_code ec,
            std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            if(!ec)
                self->processRequest();
        });
}


/**
 * @brief Process requests
 * 
 */
void Webserver::processRequest()
{
    mResponse.version(mRequest.version());
    mResponse.keep_alive(false);

    ELOG(INFO, "Webserver process request.");

    switch(mRequest.method())
    {
    case http::verb::get:
        mResponse.result(http::status::ok);
        mResponse.set(http::field::server, "Cryptobot");
        createResponse();
        break;

    default:
        // We return responses indicating an error if
        // we do not recognize the request method.
        mResponse.result(http::status::bad_request);
        mResponse.set(http::field::content_type, "text/plain");
        beast::ostream(mResponse.body())
            << "Invalid request-method '"
            << std::string(mRequest.method_string())
            << "'";
        break;
    }

    writeResponse();
}


/**
 * @brief Create responses
 * 
 */
void Webserver::createResponse()
{
    ELOG(INFO, "Webserver create response.");

    if(mRequest.target() == "/")
    {
        mResponse.set(http::field::content_type, "text/html");
        beast::ostream(mResponse.body())
            << "<html>\n"
            <<  "<head>\n"
            <<   "<style>th, td {border:1px solid black;} th {font-size: 18px; padding-left: 5px; padding-right: 5px;} td {font-size: 16px;}</style>\n"
            <<   "<title>Cryptobot</title>\n"
            <<  "</head>\n"
            <<  "<body>\n"
            <<   "<h1>Cryptobot</h1>\n"
            <<   html_page::soldOrderTable()
            <<  "</body>\n"
            << "</html>\n";
    }
    else if(mRequest.target() == "/exit")
    {
        mResponse.set(http::field::content_type, "text/html");
        beast::ostream(mResponse.body())
            << "<html>\n"
            <<  "<head><title>Cryptobot</title></head>\n"
            <<  "<body>\n"
            <<  "<h4>Cryptobot has been closed.</h4>\n"
            <<  "</body>\n"
            <<  "</html>\n";

        Opel *iOpel = Opel::instance();
        
        iOpel->setExitSignal(0);
    }
    else
    {
        mResponse.result(http::status::not_found);
        mResponse.set(http::field::content_type, "text/plain");
        beast::ostream(mResponse.body()) << "File not found\r\n";
    }
}


/**
 * @brief Write responses
 * 
 */
void Webserver::writeResponse()
{
    auto self = shared_from_this();

    mResponse.content_length(mResponse.body().size());

    ELOG(INFO, "Webserver write response.");

    http::async_write(
        mSocket,
        mResponse,
        [self](beast::error_code ec, std::size_t)
        {
            self->mSocket.shutdown(tcp::socket::shutdown_send, ec);
            self->mDeadline.cancel();
        });

    checkDeadline();
}


/**
 * @brief Check deadline
 * 
 */
void Webserver::checkDeadline()
{
    auto self = shared_from_this();

    mDeadline.async_wait(
        [self](beast::error_code ec)
        {
            self->mSocket.close(ec);
        });
    
    ELOG(INFO, "Webserver closed. Socket released.");
}


/**
 * @brief Construct a new BinanceWebserver::BinanceWebserver object
 * 
 */
BinanceWebserver::BinanceWebserver(std::shared_ptr<BinanceUtilities> pBu)
{
    mWebserverBase      = pBu.get()->getWebserverBase();
    mWebserverSocket    = pBu.get()->getWebserverPort();
    
    ELOG(INFO, "BinanceWebserver constructor initialized. Base: %s, Socket: %d.", mWebserverBase.c_str(), mWebserverSocket);
}


/**
 * @brief Destroy the BinanceWebserver::BinanceWebserver object
 * 
 */
BinanceWebserver::~BinanceWebserver()
{
    ELOG(INFO, "BinanceWebserver destructor.");
}


/**
 * @brief Http server
 * 
 * @param acceptor 
 * @param socket 
 */
void BinanceWebserver::httpServer(tcp::acceptor& acceptor, tcp::socket& socket)
{
    ELOG(INFO, "BinanceWebserver http server.");

    acceptor.async_accept(socket,
        [&](beast::error_code ec)
        {
            if(!ec)
                std::make_shared<Webserver>(std::move(socket))->start();

            httpServer(acceptor, socket);
        });
}


/**
 * @brief Initialize webserver
 * 
 */
void BinanceWebserver::init()
{
    // Create a thread for check exit signal
    std::thread         sigTh(&BinanceWebserver::checkExitSignal, this);

    // Create a acceptor and socket
    tcp::acceptor       acceptor{mIoc, {net::ip::make_address(mWebserverBase), mWebserverSocket}};
    tcp::socket         socket{mIoc};

    httpServer(acceptor, socket);

    ELOG(INFO, "BinanceWebserver initialized.");

    mIoc.run();

    sigTh.join();
}


/**
 * @brief Check exit signal
 * 
 */
void BinanceWebserver::checkExitSignal()
{
    Opel *iOpel = Opel::instance();

    while(1)
    {
        if (!iOpel->getExitSignal())
        {
            mIoc.stop();

            ELOG(INFO, "Thread Webserver detached.");

            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
