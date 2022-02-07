#ifndef Utilities_h
#define Utilities_h

// Libraries
#include "elog.h"
#include "json/json.h"

// Standart libraries
#include <iostream>
#include <string>
#include <fstream>
#include <openssl/hmac.h>


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
        Json::Value             mExchangeJson;
        std::string             mExchangeName;

    public:
                                Utilities();
        virtual                 ~Utilities();

        std::string             getTimestamp();

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
        std::string             getWebsocketEndpoint();

        std::string             getAPIBase();
        std::string             getAPIKEY();
        std::string             getAPISECRETKEY();

        std::string             encryptWithHMAC(const char* key, const char* data);
        std::string             getSignature(std::string query);
};

#endif