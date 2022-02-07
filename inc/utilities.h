#ifndef Utilities_h
#define Utilities_h

#include <iostream>
#include <string>
#include <fstream>

#include "elog.h"
#include "json/json.h"


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

};


/**
 * @brief WebsocketUtils class
 * 
 */
class WebsocketUtils : public Utilities
{
    private:
        Json::Value             mWebsocketJson;

    public:
                                WebsocketUtils();
                                ~WebsocketUtils();

        std::string             getBase();
        std::string             getPort();
        std::string             getEndpoint();
};


/**
 * @brief RequestsUtils class
 * 
 */
class RequestsUtils : public Utilities
{
    private:
        Json::Value             mRequestsJson;

    public:
                                RequestsUtils();
                                ~RequestsUtils();
        
        std::string             getBase();
        std::string             getAPIKEY();
        std::string             getSECRETKEY();
};

#endif