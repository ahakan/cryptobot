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
#include <elog.h>
#include <json/json.h>

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
        Json::Value             mUserJson;
        Json::Value             mExchangeJson;
        std::string             mExchangeName;

    public:
                                Utilities();
        virtual                 ~Utilities();

        std::string             getTimestamp();
        std::string             getOldTimestamp(int day = 0, int hour = 0, int minute = 0, int second = 0, int millisecond = 0);
        std::string             getSymbol();
        std::string             getFollowSymbol();
        std::string             getInterval();
        std::string             getBalanceSymbol();
        std::string             getBalanceAmount();

        std::string             upperToLower(std::string data);

        float                   getAverage(std::vector<float> vector);

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