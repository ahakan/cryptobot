#ifndef BINANCE_WEBSOCKET_H
#define BINANCE_WEBSOCKET_H

#include "elog.h"
#include "json/json.h"

#include "utilities.h"
#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace                       beast = boost::beast;         // from <boost/beast.hpp>
namespace                       http = beast::http;           // from <boost/beast/http.hpp>
namespace                       websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace                       net = boost::asio;            // from <boost/asio.hpp>
namespace                       ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using                           tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class Websocket
{
    private:
        std::string             mHost;
        std::string             mPort;
        std::string             mEndpoint;
        float                   *pCand = NULL;

    public:
                                Websocket(BinanceUtilities *a, float *cand);

        void                    init();
};

class BinanceWebsocket : public std::enable_shared_from_this<BinanceWebsocket>
{
private:
    tcp::resolver               mResolver;
    websocket::stream
    <beast::ssl_stream
    <beast::tcp_stream>>        ws_;
    beast::flat_buffer          mBuffer;
    std::string                 host_;
    std::string                 mEndpoint;

    float                       *pCandle = NULL;

    void                        on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void                        on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void                        on_ssl_handshake(beast::error_code ec);
    void                        on_handshake(beast::error_code ec);
    void                        on_read( beast::error_code ec, std::size_t bytes_transferred);
    void                        on_close(beast::error_code ec);

public:
    explicit                    BinanceWebsocket(net::io_context& ioc, ssl::context& ctx);
                                ~BinanceWebsocket();

    void                        run(std::string host, std::string port, std::string endpoint, float *cand);
};

#endif