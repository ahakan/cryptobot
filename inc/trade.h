/**
 * @file trade.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-05-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TRADE_H
#define TRADE_H

// Includes
#include "opel.h"
#include "utilities.h"
#include "client.h"
#include "structs.h"

// Libraries
#include "elog.h"

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
using OrdersMap         = std::map<uint32_t, std::shared_ptr<Order>>;
using OrdersMapIterator = std::map<uint32_t, std::shared_ptr<Order>>::iterator;

using CandleVector      = std::vector<std::string>;


// Class
class Trade
{
    protected:
        std::shared_ptr
            <BinanceUtilities>  pBu                 = NULL; 
        std::shared_ptr
            <BinanceClient>     pReq                = NULL;

      
        std::string             mOrderType;
        std::string             mOrderQuantity;
        
        std::string             mAverageAmount;
        bool                    mAverageAutoCalculate;

        int                     mSymbolTickSize;
        int                     mFollowSymbolTickSize;

        bool                    mAccountStatus      = false;
        bool                    mAPIKeyPermission   = false;

        OrdersMap               mBuyOrders;
        OrdersMap               mSellOrders;
        OrdersMap               mStopLossOrders;

        struct Symbol           mTradeSymbolInfo;
        struct Symbol           mFollowSymbolInfo; 
        struct Symbol           mBalanceSymbolInfo; 

        struct Candlesticks     mTradeCandlesticks;
        struct Candlesticks     mFollowCandlesticks;

        struct Candlesticks     mAlgorithmTradeCandlesticks;
        struct Candlesticks     mAlgorithmFollowCandlesticks;

        void                    calculates();

        bool                    getCandlesticks(struct Candlesticks &candlestick);

    public:
                                Trade(std::shared_ptr<BinanceUtilities> pU, 
                                        std::shared_ptr<BinanceClient> pR);
        virtual                 ~Trade();

    protected:
        bool                    checkBuyOrder();
        bool                    checkSellOrder();
        bool                    checkStopOrder();

        bool                    createNewBuyOrder();
        bool                    createNewSellOrder(std::shared_ptr<Order> order);
        bool                    createNewStopOrder(std::shared_ptr<Order> order);
};


class BinanceTrade : public Trade
{
    protected:
        std::shared_ptr
            <BinanceClient>     pReq                = NULL;

    public:
                                BinanceTrade(std::shared_ptr<BinanceUtilities> pU, 
                                                std::shared_ptr<BinanceClient> pR);
                                ~BinanceTrade();

    void                        init();
    void                        requests();
};

#endif