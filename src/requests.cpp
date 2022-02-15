#include "../inc/requests.h"
#include <cpr/timeout.h>

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
void BinanceRequests::init()
{
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

        ELOG(INFO, "Bot is active?: %d", pOpel->getIsActive());

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
    
    ELOG(INFO, "User account is normal. Entering while loop.");

    requestsLoop();
}


/**
 * @brief Requests main loop
 * 
 */
void BinanceRequests::requestsLoop()
{
    while (1)
    {
        if (pOpel->getIsActive())
        {
            ELOG(INFO, "Sent request.");

            // createNewOrder("SOLBUSD", "SELL", "LIMIT", 0.1, 150.0);

            // cancelOrder("SOLBUSD", 704450650);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}


/**
 * @brief Get request
 * 
 * @param url 
 * @param header 
 * @param parameters 
 * @return cpr::Response 
 */
cpr::Response BinanceRequests::getRequest (cpr::Url url, cpr::Header headers, cpr::Parameters parameters)
{
    cpr::Session session;

    session.SetUrl(url);
    session.SetHeader(headers);
    session.SetParameters(parameters);
    session.SetTimeout(cpr::Timeout{2000});

    cpr::AsyncResponse fr = cpr::GetAsync(url, headers, parameters);
    fr.wait();                      // This waits until the request is complete
    cpr::Response res = fr.get();   // Since the request is complete, this returns immediately

    return res;
}


/**
 * @brief Post request
 * 
 * @param url 
 * @param headers 
 * @param parameters 
 * @return cpr::Response 
 */
cpr::Response BinanceRequests::postRequest(cpr::Url url, cpr::Header headers, cpr::Parameters parameters)
{
    cpr::Session session;

    session.SetUrl(url);
    session.SetHeader(headers);
    session.SetParameters(parameters);
    session.SetTimeout(cpr::Timeout{2000});

    cpr::AsyncResponse fr = cpr::PostAsync(url, headers, parameters);
    fr.wait();                      // This waits until the request is complete
    cpr::Response res = fr.get();   // Since the request is complete, this returns immediately

    return res;
}


/**
 * @brief Delete request
 * 
 * @param url 
 * @param headers 
 * @param parameters 
 * @return cpr::Response 
 */
cpr::Response BinanceRequests::deleteRequest(cpr::Url url, cpr::Header headers, cpr::Parameters parameters)
{
    cpr::Session session;

    session.SetUrl(url);
    session.SetHeader(headers);
    session.SetParameters(parameters);
    session.SetTimeout(cpr::Timeout{2000});

    cpr::AsyncResponse fr = cpr::DeleteAsync(url, headers, parameters);
    fr.wait();                      // This waits until the request is complete
    cpr::Response res = fr.get();   // Since the request is complete, this returns immediately

    return res;
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

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = getRequest(url, headers, parameters);

    ELOG(INFO, "Get Account Status Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Get Account Status Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

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
    std::string mBaseURL        = mBase + "/sapi/v1/account/apiRestrictions";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature      = pBu->getSignature(mParameters);

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = getRequest(url, headers, parameters);

    ELOG(INFO, "Get API KEY Permission Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Get API KEY Permission Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

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


/**
 * @brief Create a new order
 * 
 * @param symbol 
 * @param side 
 * @param type 
 * @param quantity 
 * @return true 
 * @return false 
 */
bool BinanceRequests::createNewOrder(std::string symbol, std::string side, std::string type, float quantity, float price)
{
    std::string mBaseURL        = mBase + "/api/v3/order";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "symbol="+symbol+"&side="+side;
                mParameters     += "&type="+type+"&timeInForce=GTC";
                mParameters     += "&quantity="+std::to_string(quantity)+"&price="+std::to_string(price);
                mParameters     += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature      = pBu->getSignature(mParameters);

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"symbol", symbol},
                                            {"side", side},
                                            {"type", type},
                                            {"timeInForce", "GTC"},
                                            {"quantity", std::to_string(quantity)},
                                            {"price", std::to_string(price)},
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = postRequest(url, headers, parameters);

    ELOG(INFO, "Create New Order Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Create New Order Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    std::string mStatus = mAPIJson["status"].asString();

    if (mStatus!="NEW")
    {
        ELOG(ERROR, "Order could not be created.");
        return false;
    }

    uint32_t mOrderId = mAPIJson["orderId"].asInt();

    ELOG(INFO, "Created a New Order. Order ID: %d", mOrderId);

    return true;
}


bool BinanceRequests::cancelOrder(std::string symbol, uint32_t orderId)
{
    std::string mBaseURL        = mBase + "/api/v3/order";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "symbol="+symbol+"&orderId="+std::to_string(orderId);
                mParameters     += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature      = pBu->getSignature(mParameters);

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"symbol", symbol},
                                            {"orderId", std::to_string(orderId)},
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = deleteRequest(url, headers, parameters);

    ELOG(INFO, "Cancel Order Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Cancel Order Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    std::string mStatus = mAPIJson["status"].asString();

    if (mStatus!="CANCELED")
    {
        ELOG(ERROR, "Order could not be cancelled.");
        return false;
    }

    uint32_t mOrderId = mAPIJson["orderId"].asInt();

    ELOG(INFO, "Canceled a Order. Order ID: %d", mOrderId);

    return true;
}