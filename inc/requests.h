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
#include <iostream>
#include <thread>
#include <chrono>

class Requests
{
    private:

    protected:
        Opel                    *mOpel = Opel::instance();

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
        float                   *pCandle;
        void                    mainLoop();

        bool                    getAccountStatus();
        bool                    getAPIKeyPermission();

    public:
                                BinanceRequests(BinanceUtilities *pBu);
                                ~BinanceRequests();

        void                    init(float *candle);
        
};

#endif