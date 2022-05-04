/**
 * @file utilities.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
 
#ifndef UTILITIES_H
#define UTILITIES_H

// Includes
#include "opel.h"
#include "structs.h"

// Libraries
#include "elog.h"
#include "json/json.h"

// Standard libraries
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <openssl/hmac.h>
#include <vector>
#include <thread>


/**
 * @brief Utilities base class
 * 
 */
class Utilities
{
    private:
        std::string             mBuySide                = "BUY";
        std::string             mSellSide               = "SELL";
    
    protected:
        std::ifstream           mConfigFile;

        Json::Value             mConfigJson;
        Json::Value             mTradeJson;
        Json::Value             mExchangeJson;
        Json::Value             mWebserverJson;

        std::string             mExchangeName;

        std::string             RSI(std::vector<std::string> vector);
        std::string             Average(std::vector<std::string> vector);

    public:
                                Utilities();
        virtual                 ~Utilities();

        void                    setExitSignal(bool signal);

        std::string             getTimestamp();
        std::string             getRSITimestamp(int rsiPeriod, std::string interval);
        std::string             getCoinSymbol();
        std::string             getTradeSymbol();
        std::string             getFollowSymbol();
        std::string             getInterval();
        std::string             getQuantity();
        std::string             getType();
        std::string             getBalanceSymbol();
        std::string             getBalanceAmount();
        std::string             getAverageAmount();

        bool                    getAverageAutoCalculate();

        int                     getRSIPeriod();
        std::string             getRSIOversold();
        std::string             getRSIOverbought();

        std::string             getWebserverBase();
        unsigned short          getWebserverPort();

        std::string             upperToLower(std::string data);
        std::string             roundString(std::string price, int tickSize);
        std::string             addTwoStrings(std::string number1, std::string number2);
        std::string             subTwoStrings(std::string number1, std::string number2);
        std::string             multiplyTwoStrings(std::string number1, std::string number2);

        bool                    compareTwoStrings(std::string firstPrice, std::string secondPrice);

        int                     getTickSize(std::string data);

        bool                    getTradeSymbolCandle(struct Candlesticks& candles);
        bool                    getFollowSymbolCandle(struct Candlesticks& candles);

        bool                    calculateRSI(struct Candlesticks& candles);
        bool                    calculateAverage(struct Candlesticks& candles);

        bool                    calcNewBuyPrice(struct Order& order, 
                                                    struct Coin& coin,
                                                    struct Candlesticks& candles);
        bool                    calcNewSellPrice(struct Order& order, 
                                                    struct Coin& coin,
                                                    struct Candlesticks& candles);
        bool                    calcNewOrderAverage(struct Order& order, 
                                                        struct Candlesticks& candles);
        bool                    calcNewBalanceAmount(struct Order& order, 
                                                        struct Coin& balance,
                                                        struct Coin& coin);
};


/**
 * @brief BinanceUtilities class
 * 
 */
class BinanceUtilities : public Utilities
{
    private:
        Json::Value             mWebsocketJson;
        Json::Value             mAPIJson;

    public:
                                BinanceUtilities();
                                ~BinanceUtilities();

        std::string             getWebsocketBase();
        std::string             getWebsocketPort();
        std::string             getWebsocketEndpointT();
        std::string             getWebsocketEndpointF();

        std::string             getAPIBase();
        std::string             getAPIKEY();
        std::string             getAPISECRETKEY();

        std::string             encryptWithHMAC(const char* key, const char* data);
        std::string             getSignature(std::string query);
};

#endif