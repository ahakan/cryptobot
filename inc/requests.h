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

#ifndef Requests_h
#define Requests_h

// Libraries
#include "cpr/cpr.h"
#include "json/json.h"

// Includes
#include "utilities.h"

// Standart Libraries
#include <iostream>
#include <thread>
#include <chrono>

class Requests
{
    private:

    protected:
        std::string         mBase;
        std::string         mAPI_KEY;
        std::string         mSECRET_KEY;

        BinanceUtilities    *pBu; 

    public:
                            Requests(BinanceUtilities *pBu);
        virtual             ~Requests();
};

class BinanceRequests : public Requests
{
    private:

    public:
                            BinanceRequests(BinanceUtilities *pBu);
                            ~BinanceRequests();

        void                init(float *candle);
        void                getAPIKeyPermission();
};

#endif