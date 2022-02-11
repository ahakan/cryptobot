#include "../inc/requests.h"

/**
 * @brief Construct a new Requests::Requests object
 * 
 * @param pBu 
 */
Requests::Requests(BinanceUtilities *pBu)
{
    this->pBu       = pBu;
    mBase           = pBu->getAPIBase();
    mAPI_KEY        = pBu->getAPIKEY();
    mSECRET_KEY     = pBu->getAPISECRETKEY();

    // Opel mOpel           = opel.instance();

    if (mAPI_KEY.length() == 0 || mSECRET_KEY.length() == 0)
    {
        ELOG(ERROR, "Failed to initialize Requests constructor.");
        exit(0);
    }

    ELOG(INFO, "Requests constructor initialized.");
}


/**
 * @brief Destroy the Requests::Requests object
 * 
 */
Requests::~Requests()
{
    ELOG(INFO, "Requests destructor.");
}


/**
 * @brief Construct a new BinanceRequests::BinanceRequests object
 * 
 * @param pBu 
 */
BinanceRequests::BinanceRequests(BinanceUtilities *pBu)
    : Requests(pBu)
{
    ELOG(INFO, "BinanceRequests constructor initialized.");
}


/**
 * @brief Destroy the BinanceRequests::BinanceRequests object
 * 
 */
BinanceRequests::~BinanceRequests()
{
    ELOG(INFO, "BinanceRequests destructor.");
}


/**
 * @brief Requests initializing.
 * 
 * @param candle 
 */
void BinanceRequests::init(float *candle)
{
    pCandle                         = candle;
    
    while (1)
    {
        bool mAccountStatus         = getAccountStatus();

        if (!mAccountStatus)
        {
            ELOG(ERROR, "User account status is not normal. Can't enter main loop.");
        }

        bool mAPIKeyPermission      = getAPIKeyPermission();

        if (!mAPIKeyPermission)
        {
            ELOG(ERROR, "User API Spot and Margin trading is not active. Can't enter main loop.");
        }

        if (mAccountStatus && mAPIKeyPermission)
        {
            break;
        }

        ELOG(INFO, "Bot is active?: %d", mOpel->getIsActive());

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
    
    ELOG(INFO, "User account is normal. Entering while loop.");

    mainLoop();
}


/**
 * @brief Requests main loop
 * 
 */
void BinanceRequests::mainLoop()
{
    while (1)
    {
        if (mOpel->getIsActive())
        {
            ELOG(INFO, "Sent request.");

            // getAPIKeyPermission();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}


/**
 * @brief Get user account status
 * 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getAccountStatus()
{
    std::string mBaseURL    = mBase + "/sapi/v1/account/status";

    std::string mTimestamp  = pBu->getTimestamp();

    std::string mParameters = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature  = pBu->getSignature(mParameters);

    cpr::Response mReq      = cpr::Get(
                                cpr::Url{mBaseURL},
                                cpr::Header{{"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}},
                                cpr::Parameters{
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}});

    ELOG(INFO, "Get Account Status Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Get Account Status Response body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse( mReq.text.c_str(), mAPIJson );

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    std::string mAccountStatusData = mAPIJson["data"].asString();

    if (mAccountStatusData != "Normal")
    {
        ELOG(ERROR, "User account status is not normal.");
        return false;
    }

    ELOG(INFO, "User account status is normal.");

    return true;
}


/**
 * @brief Get user API key permissions
 * 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getAPIKeyPermission()
{
    std::string mBaseURL    = mBase + "/sapi/v1/account/apiRestrictions";

    std::string mTimestamp  = pBu->getTimestamp();

    std::string mParameters = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature  = pBu->getSignature(mParameters);

    cpr::Response mReq      = cpr::Get(
                                cpr::Url{mBaseURL},
                                cpr::Header{{"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}},
                                cpr::Parameters{
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}});

    ELOG(INFO, "Get API KEY Permission Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Get API KEY Permission Response body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse( mReq.text.c_str(), mAPIJson );

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    bool mEnableSpotAndMarginTrading = mAPIJson["enableSpotAndMarginTrading"].asBool();

    if (!mEnableSpotAndMarginTrading)
    {
        ELOG(ERROR, "User API Spot and Margin trading is not active.");
        return false;
    }

    ELOG(INFO, "User API Spot and Margin trading is active.");

    return true;
}