/**
 * @file webserver.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-03-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

// Includes
#include "opel.h"
#include "utilities.h"

// Libraries
#include "elog.h"

// Boost libraries
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

// Standard libraries
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Namespaces
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


class Webserver : public std::enable_shared_from_this<Webserver>
{
    private:
        tcp::socket             mSocket;
        beast::flat_buffer      mBuffer{8192};
        http::request
        <http::dynamic_body>    mRequest;
        http::response
        <http::dynamic_body>    mResponse;
        net::steady_timer       mDeadline{mSocket.get_executor(), std::chrono::seconds(3)};

        void                    readRequest();
        void                    processRequest();
        void                    createResponse();
        void                    writeResponse();
        void                    checkDeadline();

    public:
                                Webserver(tcp::socket socket);
                                ~Webserver();

        void                    start();
    
};


class BinanceWebserver
{
    private:
        net::io_context         mIoc{1};

        std::string             mWebserverBase;
        unsigned short          mWebserverSocket;

    public:
                                BinanceWebserver(std::shared_ptr<BinanceUtilities> pBu);
                                ~BinanceWebserver();

        void                    httpServer(tcp::acceptor& acceptor, tcp::socket& socket);
        void                    init();
        void                    checkExitSignal();
};

#endif