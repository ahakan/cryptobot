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
#include "cpr/cpr.h"
#include "json/json.h"

// Standard Libraries
#include <cpr/response.h>
#include <iostream>
#include <thread>
#include <chrono>

class Requests
{
    private:

    protected:
        Opel                    *pOpel = Opel::instance();

        std::string             mBase;
        std::string             mAPI_KEY;
        std::string             mSECRET_KEY;

        std::string             mRecvWindow = "10000";

        BinanceUtilities        *pBu; 

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
        bool                    createNewOrder(std::string symbol, std::string side, std::string type, float quantity, float price);

    public:
                                BinanceRequests(BinanceUtilities *pBu);
                                ~BinanceRequests();

        void                    init();
        
};

#endif