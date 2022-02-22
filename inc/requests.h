/**
 * @file requests.h
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

// Libraries
#include <cpr/cpr.h>
#include <cpr/response.h>
#include <cpr/timeout.h>
#include <json/json.h>

// Standard Libraries
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <iterator>

using OrderMap      = std::map<std::string, std::string>;
using AllOrdersMap  = std::map<int, OrderMap>;

using AverageVector = std::vector<float>;

class Requests
{
    private:

    protected:
        BinanceUtilities        *pBu = NULL; 

        std::string             mBase;
        std::string             mAPI_KEY;
        std::string             mSECRET_KEY;
        
        std::string             mSymbol;
        std::string             mInterval;
        std::string             mBalanceSymbol;
        std::string             mBalanceAmount;

        std::string             mRecvWindow = "10000";

        AllOrdersMap            mBuyOrders;
        AllOrdersMap            mBoughtOrders;

        AllOrdersMap            mSellOrders;
        AllOrdersMap            mSoldOrders;

        int                     mCandlesSize;

        float                   mCandlesOpenPriceAverage;
        float                   mCandlesHighPriceAverage;
        float                   mCandlesLowPriceAverage;
        float                   mCandlesClosePriceAverage;

        AverageVector           mCandlesOpenPrice;
        AverageVector           mCandlesHighPrice;
        AverageVector           mCandlesLowPrice;
        AverageVector           mCandlesClosePrice;

    public:
                                Requests(BinanceUtilities *pBu);
        virtual                 ~Requests();
};

class BinanceRequests : public Requests
{
    private:        
        void                    requestsLoop();

        cpr::Response           getRequest(cpr::Url url, cpr::Header headers, cpr::Parameters parameters);
        cpr::Response           postRequest(cpr::Url url, cpr::Header headers, cpr::Parameters parameters);
        cpr::Response           deleteRequest(cpr::Url url, cpr::Header headers, cpr::Parameters parameters);

        bool                    getAccountStatus();
        bool                    getAPIKeyPermission();
        bool                    createNewOrder(std::string symbol, std::string side, std::string type, std::string quantity, std::string price);
        bool                    cancelOrder(std::string symbol, uint32_t orderId);
        bool                    cancelAllOpenOrders(std::string symbol);
        bool                    queryOrder(std::string symbol, uint32_t orderId);
        bool                    currentOpenOrders(std::string symbol);

        bool                    getCandlesticksData(std::string symbol, std::string interval, std::string startTime);

    public:
                                BinanceRequests(BinanceUtilities *pBu);
                                ~BinanceRequests();

        void                    init();
        
};

#endif
