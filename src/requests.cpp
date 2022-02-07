#include "../inc/requests.h"

Requests::Requests(BinanceUtilities *pBu)
{
    this->pBu       = pBu;
    mBase           = pBu->getAPIBase();
    mAPI_KEY        = pBu->getAPIKEY();
    mSECRET_KEY     = pBu->getAPISECRETKEY();

    if (mAPI_KEY.length() == 0 || mSECRET_KEY.length() == 0)
    {
        ELOG(ERROR, "Failed to initialize Requests constructor.");
        exit(0);
    }

    ELOG(INFO, "Requests constructor initialized.");
}

Requests::~Requests()
{
    ELOG(INFO, "Requests destructor.");
}

BinanceRequests::BinanceRequests(BinanceUtilities *pBu)
    : Requests(pBu)
{
    ELOG(INFO, "BinanceRequests constructor initialized.");
}

BinanceRequests::~BinanceRequests()
{
    ELOG(INFO, "BinanceRequests destructor.");
}

void BinanceRequests::init(float *candle)
{
    while ( 1 )
    {
        getAPIKeyPermission();

        if (*candle != 0)
        {
            ELOG(INFO, "Sent request.");

            getAPIKeyPermission();

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void BinanceRequests::getAPIKeyPermission()
{
    std::string mBaseURL    = mBase + "/sapi/v1/account/apiRestrictions";

    std::string mTimestamp  = pBu->getTimestamp();

    std::string mSignature  = pBu->getSignature("timestamp="+mTimestamp);

    cpr::Response mReq      = cpr::Get(
                                cpr::Url{mBaseURL},
                                cpr::Header{{"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}},
                                cpr::Parameters{
                                            {"timestamp", mTimestamp}, 
                                            {"signature", mSignature}});

    // std::cout << mReq.url << std::endl;                  
    // std::cout << mReq.status_code << std::endl;
    // std::cout << mReq.header["content-type"] << std::endl;
    // std::cout << mReq.text << std::endl;

    Json::Value mAPIJson;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( mReq.text.c_str(), mAPIJson );

    if ( !parsingSuccessful )
    {
        ELOG(ERROR, "Failed to JSON parse.");
    }

    bool enableSpotAndMarginTrading = mAPIJson.get("enableSpotAndMarginTrading", true).asBool();

    std::cout << enableSpotAndMarginTrading << std::endl;
}