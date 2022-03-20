/**
 * @file websocket.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H


// Includes
#include "opel.h"
#include "utilities.h"
#include "certificates/certificates.hpp"

// Libraries
#include "elog.h"
#include "json/json.h"

// Boost libraries
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>

// Standard libraries
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Namespaces
namespace beast         = boost::beast;                 // from <boost/beast.hpp>
namespace websocket     = beast::websocket;             // from <boost/beast/websocket.hpp>
namespace net           = boost::asio;                  // from <boost/asio.hpp>
namespace ssl           = boost::asio::ssl;             // from <boost/asio/ssl.hpp>
using     tcp           = boost::asio::ip::tcp;         // from <boost/asio/ip/tcp.hpp>

class Websocket : public std::enable_shared_from_this<Websocket>
{
    private:
        tcp::resolver               mResolver;
        websocket::stream
        <beast::ssl_stream
        <beast::tcp_stream>>        mWs;
        beast::flat_buffer          mBuffer;
        std::string                 mHost;
        std::string                 mEndpoint;

        void                        resolve(beast::error_code ec, tcp::resolver::results_type results);
        void                        connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
        void                        sslHandshake(beast::error_code ec);
        void                        handshake(beast::error_code ec);
        void                        read( beast::error_code ec, std::size_t bytes_transferred);
        void                        close(beast::error_code ec);

    public:
        explicit                    Websocket(net::io_context& ioc, ssl::context& ctx);
                                    ~Websocket();

        void                        run(std::string host, std::string port, std::string endpoint);
    
};

class BinanceWebsocket 
{
    private:
        net::io_context             mIoc;
        ssl::context                mCtx{ssl::context::tlsv12_client};

        std::string                 mHost;
        std::string                 mPort;
        std::string                 mEndpointT;
        std::string                 mEndpointF;
        std::string                 mTradeSymbol;
        std::string                 mFollowSymbol;


    public:
                                    BinanceWebsocket(std::shared_ptr<BinanceUtilities> pBu);
                                    ~BinanceWebsocket();

        void                        init();

};

#endif