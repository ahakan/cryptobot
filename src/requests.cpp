#include "../inc/requests.h"
#include "utilities.h"

Requests::Requests(RequestsUtils *pReqU)
{
    mBase           = pReqU->getBase();
    mAPI_KEY        = pReqU->getAPIKEY();
    mSECRET_KEY     = pReqU->getSECRETKEY();

    if (mAPI_KEY.length() == 0 || mSECRET_KEY.length() == 0)
    {
        ELOG(ERROR, "Failed to initialize Requests constructor.");
        exit(0);
    }

    ELOG(INFO, "Requests constructor initialized.");
    ELOG(INFO, "mBase: %s, mAPI_KEY: %s, mSECRET_KEY: %s.", mBase.c_str(), mAPI_KEY.c_str(), mSECRET_KEY.c_str());
}

Requests::~Requests()
{

}

BinanceRequests::BinanceRequests(RequestsUtils *pReqU)
    : Requests(pReqU)
{
    ELOG(INFO, "BinanceRequests constructor initialized.");
}

BinanceRequests::~BinanceRequests()
{

}

void BinanceRequests::sendRequests(float *candle)
{
    while (1)
    {
        if (*candle != 0)
        {
            ELOG(INFO, "Sent request.");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}