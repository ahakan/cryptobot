/**
 * @file client.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef REQUESTS_H
#define REQUESTS_H

// Includes
#include "opel.h"
#include "utilities.h"
#include "structs.h"

// Libraries
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json/json.h"

// Standard Libraries
#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <iterator>
#include <mutex>

// Using && Namespaces
// using OrdersMap         = std::vector<std::unique_ptr<Order>>;
// using OrdersMapIterator = std::vector<std::unique_ptr<Order>>::iterator;

// Class
class Client
{
    protected:
        std::shared_ptr
            <BinanceUtilities>  pBu             = NULL; 

        std::string             mBase;
        std::string             mAPI_KEY;
        std::string             mSECRET_KEY;

        std::string             mRecvWindow     = "30000";

    public:
                                Client(std::shared_ptr<BinanceUtilities> pU);
        virtual                 ~Client();
};

class BinanceClient : public Client
{
    private:        
        std::string             getRequest(std::string endpoint, std::string parameters, 
                                                httplib::Headers headers);
        std::string             postRequest(std::string endpoint, std::string parameters, 
                                                httplib::Params signature, 
                                                httplib::Headers headers);
        std::string             deleteRequest(std::string endpoint, std::string parameters, 
                                                httplib::Headers headers);

        

    public:
                                BinanceClient(std::shared_ptr<BinanceUtilities> pU);
                                ~BinanceClient();

        // Wallet Endpoints
        bool                    getAccountStatus();
        bool                    getAPIKeyPermission();
        bool                    getCoinBalance(struct Symbol& coin);

        // Market Data Endpoints
        bool                    getCandlesticksData(struct Candlesticks& candlestick);
        bool                    getTickSize(struct Symbol& coin);
        bool                    getDailyVolume(struct Symbol& coin);

        // Spot Account/Trade
        bool                    createNewOrder(std::shared_ptr<Order> order, 
                                                struct Symbol& coin);    
        bool                    cancelOrder(std::shared_ptr<Order> order);
        bool                    cancelAllOpenOrders(std::string symbol);
        bool                    queryOrder(std::shared_ptr<Order> order);
        bool                    currentOpenOrders(std::string symbol);
};

#endif
