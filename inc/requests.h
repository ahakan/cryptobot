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
        std::string mBase;
        std::string mAPI_KEY;
        std::string mSECRET_KEY;

    public:
        Requests(RequestsUtils *pReqU);
        virtual ~Requests();
};

class BinanceRequests : public Requests
{
    private:

    public:
        BinanceRequests(RequestsUtils *pReqU);
        ~BinanceRequests();

        void sendRequests(float *candle);
};

#endif