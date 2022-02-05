#ifndef Utilities_h
#define Utilities_h

#include <iostream>
#include <string>
#include <fstream>

#include "elog.h"
#include "json/json.h"

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

class WebsocketUtils : public Utilities
{
    private:
        Json::Value             mWebsocketJson;

    public:
                                WebsocketUtils();
                                ~WebsocketUtils();

        std::string             getHost();
        std::string             getPort();
        std::string             getEndpoint();
};

#endif