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

// Using && Namespaces
using OrderMap          = std::map<std::string, std::string>;
using AllOrdersMap      = std::map<int, OrderMap>;
using SoldOrdersMap     = std::multimap<int, OrderMap>;
using MapIterator       = std::map<int, OrderMap>::iterator;

using AverageVector     = std::vector<std::string>;

class Client
{
    private:

    protected:
        std::shared_ptr<BinanceUtilities> pBu = NULL; 

        std::string             mBase;
        std::string             mAPI_KEY;
        std::string             mSECRET_KEY;
        
        std::string             mInterval;
        std::string             mQuantity;
        std::string             mCoinSymbol;
        std::string             mCoinQuantity;
        std::string             mTradeSymbol;
        std::string             mFollowSymbol;
        std::string             mBalanceSymbol;
        std::string             mBalanceAmount;
        std::string             mAverageAmount;

        bool                    mAverageAutoCalculate;
        
        int                     mSymbolTickSize;
        int                     mFollowSymbolTickSize;

        int                     mRSIPeriod;
        std::string             mRSIOversold;
        std::string             mRSIOverbought;


        bool                    mAccountStatus = false;
        bool                    mAPIKeyPermission = false;


        std::string             mBuySide    = "BUY";
        std::string             mSellSide   = "SELL";
        std::string             mOrderType  = "LIMIT";
        std::string             mRecvWindow = "10000";


        bool                    mSymbolLivePriceRead;
        
        std::string             mSymbolLivePrice;
        std::string             mFollowLivePrice;
        std::string             mNewOrderCalculatedAverage;

        std::string             mTradeSymbolDailyVolume;
        std::string             mTradeSymbolDailyQuoteVolume;
        std::string             mFollowSymbolDailyVolume;
        std::string             mFollowSymbolDailyQuoteVolume;

        AllOrdersMap            mBuyOrders;
        AllOrdersMap            mBoughtOrders;

        int                     mSellOrderFilledSize = 0;

        AllOrdersMap            mSellOrders;
        SoldOrdersMap           mSoldOrders;


        std::string             mTradeCandlesOpenPricesAverage;
        std::string             mTradeCandlesHighPricesAverage;
        std::string             mTradeCandlesLowPricesAverage;
        std::string             mTradeCandlesClosePricesAverage;


        bool                    mBuyOrdersNewTradeRSI;
        bool                    mBuyOrdersNewFollowRSI;
        bool                    mSellOrdersNewTradeRSI;
        bool                    mSellOrdersNewFollowRSI;


        std::string             mTradeCandlesCloseRSI;
        std::string             mOldTradeCandlesCloseRSI;

        AverageVector           mTradeCandlesOpenPrices;
        AverageVector           mTradeCandlesHighPrices;
        AverageVector           mTradeCandlesLowPrices;
        AverageVector           mTradeCandlesClosePrices;


        std::string             mFollowCandlesOpenPricesAverage;
        std::string             mFollowCandlesHighPricesAverage;
        std::string             mFollowCandlesLowPricesAverage;
        std::string             mFollowCandlesClosePricesAverage;


        std::string             mFollowCandlesCloseRSI;
        std::string             mOldFollowCandlesCloseRSI;

        AverageVector           mFollowCandlesOpenPrices;
        AverageVector           mFollowCandlesHighPrices;
        AverageVector           mFollowCandlesLowPrices;
        AverageVector           mFollowCandlesClosePrices;


        std::string             calcNewSellPrice(std::string boughtPrice);
        std::string             calcNewBuyPrice();

        bool                    calcNewBalanceAmount(std::string side, std::string price, std::string quantity);
        bool                    calcOrderPriceAverage();
        bool                    calcSymbolRSI();
        bool                    calcFollowRSI();
        bool                    calcSymbolAverages();
        bool                    calcFollowAverages();
        bool                    calcNewBuyOrder(std::string price);
        bool                    calcNewSellOrder();
        bool                    readCandleData();
        bool                    addClosedCandlePrices(std::string symbol, std::string open, std::string close, std::string high, std::string low);

    public:
                                Client(std::shared_ptr<BinanceUtilities> pBu);
        virtual                 ~Client();
};

class BinanceClient : public Client
{
    private:        
        void                    binance();

        bool                    newBuyOrder();
        bool                    newSellOrder();
        bool                    checkBuyOrders();
        bool                    checkSellOrders();
        bool                    checkBoughtOrders();

        std::string             getRequest(std::string endpoint, std::string parameters, httplib::Headers headers);
        std::string             postRequest(std::string endpoint, std::string parameters, httplib::Params signature, httplib::Headers headers);
        std::string             deleteRequest(std::string endpoint, std::string parameters, httplib::Headers headers);

        // Wallet Endpoints
        bool                    getAccountStatus();
        bool                    getAPIKeyPermission();
        bool                    getCoinBalance(std::string symbol);

        // Market Data Endpoints
        bool                    getCandlesticksData(std::string symbol, std::string interval, std::string startTime);
        bool                    getTickSize(std::string symbol);
        bool                    getDailyVolume(std::string symbol);

        // Spot Account/Trade
        bool                    createNewOrder(std::string symbol, std::string side, std::string type, std::string quantity, std::string price);
        bool                    cancelOrder(std::string symbol, int orderId);
        bool                    cancelAllOpenOrders(std::string symbol);
        bool                    queryOrder(std::string symbol, int orderId);
        bool                    currentOpenOrders(std::string symbol);

    public:
                                BinanceClient(std::shared_ptr<BinanceUtilities> pBu);
                                ~BinanceClient();

        void                    init();
        
};

#endif
