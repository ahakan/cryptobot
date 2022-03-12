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

// Libraries
#include "elog.h"
#include "json/json.h"

// Standard libraries
#include <iostream>
#include <string>
#include <fstream>
#include <openssl/hmac.h>
#include <vector>


/**
 * @brief Utilities base class
 * 
 */
class Utilities
{
    private:
    protected:
        std::ifstream           mConfigFile;
        Json::Value             mConfigJson;
        Json::Value             mTradeJson;
        Json::Value             mExchangeJson;
        std::string             mExchangeName;

    public:
                                Utilities();
        virtual                 ~Utilities();

        std::string             getTimestamp();
        std::string             getOldTimestamp();
        std::string             getSymbol();
        std::string             getFollowSymbol();
        std::string             getInterval();
        std::string             getQuantity();
        std::string             getBalanceSymbol();
        std::string             getBalanceAmount();
        std::string             getAverageAmount();

        bool                    getAverageAutoCalculate();

        int                     getRSIPeriod();
        std::string             getRSIOversold();
        std::string             getRSIOverbought();

        std::string             upperToLower(std::string data);
        std::string             roundPrice(std::string price, int tickSize);
        std::string             calculateAverage(std::vector<std::string> vector);
        std::string             calculateRSI(std::vector<std::string> vector);
        std::string             addTwoStrings(std::string number1, std::string number2);
        std::string             subTwoStrings(std::string number1, std::string number2);
        
        bool                    comparePrice(std::string firstPrice, std::string secondPrice);

        int                     getTickSize(std::string data);
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