/**
 * @file server.h
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
#include <map>
#include <iterator>

// Namespaces
namespace   beast           = boost::beast;         // from <boost/beast.hpp>
namespace   http            = beast::http;           // from <boost/beast/http.hpp>
namespace   net             = boost::asio;            // from <boost/asio.hpp>
using       tcp             = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using       SoldOrdersMap   = std::multimap<int, std::map<std::string, std::string>>;

namespace html_page
{
    inline std::string soldOrderTable()
    {
        Opel *iOpel = Opel::instance();

        SoldOrdersMap *mSoldOrders = iOpel->getSoldOrdersMap();

        std::string table = "";

        if (mSoldOrders->size() > 0)
        {
            table = "<h2>Sold Orders</h2><table><tr><th>Order ID</th><th>Symbol</th><th>Quantity</th><th>Bought Price</th><th>Sold Price</th><th> Sold Time </th></tr><tr>";
        
            for (SoldOrdersMap::iterator i = (*mSoldOrders).begin(); i != (*mSoldOrders).end(); ++i)
            {
                table = table + "<td>" + std::to_string(i->first) + "</td>";
                table = table + "<td>" + i->second["Symbol"] + "</td>";
                table = table + "<td>" + i->second["Quantity"] + "</td>";
                table = table + "<td>" + i->second["BoughtPrice"] + "</td>";
                table = table + "<td>" + i->second["SoldPrice"] + "</td>";
                table = table + "<td>" + i->second["Timestamp"] + "</td>";

                table = table + "</tr><tr>";
            }
            table = table + "</table>\n";
        }

        return table;
    }
}

class Server : public std::enable_shared_from_this<Server>
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
                                Server(tcp::socket socket);
                                ~Server();

        void                    start();
    
};


class BinanceServer
{
    private:
        net::io_context         mIoc{1};

        std::string             mWebserverBase;
        unsigned short          mWebserverSocket;

    public:
                                BinanceServer(std::shared_ptr<BinanceUtilities> pBu);
                                ~BinanceServer();

        void                    httpServer(tcp::acceptor& acceptor, tcp::socket& socket);
        void                    init();
        void                    checkExitSignal();
};

#endif