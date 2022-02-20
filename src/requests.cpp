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

        Opel *pOpel = Opel::instance();
        std::cout << pOpel->getSymbol() << std::endl;

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
    bool mEnough = false;
    while (1)
    {
        Opel *pOpel = Opel::instance();

        if (pOpel->getIsActive())
        {
            if (mBuyOrders.size() > 0)
                queryOrder("SOLBUSD", mBuyOrders.begin()->first);

            if (mSellOrders.size() > 0)
                for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
                    queryOrder("SOLBUSD", it->first);

            if (mBoughtOrders.size() > 0)
                // createNewOrder("SOLBUSD", "SELL", "LIMIT", "0.20", "91.95");

            if (mEnough == false)
                if (mBuyOrders.size() < 1)
                    // createNewOrder("SOLBUSD", "BUY", "LIMIT", "0.20", "90.4");
        
            mEnough = true;
            // currentOpenOrders("SOLBUSD");

            // cancelOrder("SOLBUSD", mBuyOrders.begin()->first);

            // cancelAllOpenOrders("SOLBUSD");
        }

        ELOG(INFO, "mBuyOrders: %d, mBoughtOrders: %d, mSellOrders: %d, mSoldOrders: %d.", mBuyOrders.size(), mBoughtOrders.size(), mSellOrders.size(), mSoldOrders.size());

        std::cout << "Buy Orders" << std::endl;
        for (AllOrdersMap::iterator it = mBuyOrders.begin(); it != mBuyOrders.end(); it++)
        {
            std::cout << it->first << ": " << it->second["Price"] << std::endl;
        }

        std::cout << "Bought Orders" << std::endl;
        for (AllOrdersMap::iterator it = mBoughtOrders.begin(); it != mBoughtOrders.end(); it++)
        {
            std::cout << it->first << ": " << it->second["BoughtPrice"] << std::endl;
        }

        std::cout << "Sell Orders" << std::endl;
        for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
        {
            std::cout << it->first << ": " << it->second["Price"] << std::endl;
        }

        std::cout << "Sold Orders" << std::endl;
        for (AllOrdersMap::iterator it = mSoldOrders.begin(); it != mSoldOrders.end(); it++)
        {
            std::cout << it->first << ": " << it->second["SoldPrice"] << " - " << it->second["BoughtPrice"] << std::endl;
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
 * @param price 
 * @return true 
 * @return false 
 */
bool BinanceRequests::createNewOrder(std::string symbol, std::string side, std::string type, std::string quantity, std::string price)
{
    std::string mBaseURL        = mBase + "/api/v3/order";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "symbol="+symbol+"&side="+side;
                mParameters     += "&type="+type+"&timeInForce=GTC";
                mParameters     += "&quantity="+quantity+"&price="+price;
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
                                            {"quantity", quantity},
                                            {"price", price},
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

    if (mStatus!="NEW") //!!!!!!!!!!!
    {
        ELOG(ERROR, "Order could not be created.");
        return false;
    }

    uint32_t mOrderId           = mAPIJson["orderId"].asInt();
    std::string mSide           = mAPIJson["side"].asString();
    std::string mSymbol         = mAPIJson["symbol"].asString();
    std::string mPrice          = mAPIJson["price"].asString();
    std::string mQuantity       = mAPIJson["origQty"].asString();
    std::string mTransactTime   = mAPIJson["transactTime"].asString();

    OrderMap mOrder;

    if (mSide == "BUY")
    {
        mOrder.emplace("Symbol", mSymbol);
        mOrder.emplace("Price", mPrice);
        mOrder.emplace("Quantity", mQuantity);
        mOrder.emplace("TransactTime", mTransactTime);

        mBuyOrders.emplace(mOrderId, mOrder);

        ELOG(INFO, "Created a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());
    }
    else if (mSide == "SELL")
    {
        mOrder.emplace("Symbol", mSymbol);
        mOrder.emplace("Price", mPrice);
        mOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
        mOrder.emplace("Quantity", mQuantity);
        mOrder.emplace("TransactTime", mTransactTime);

        mSellOrders.emplace(mOrderId, mOrder);
        mBoughtOrders.clear();

        ELOG(INFO, "Created a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());
    }
    
    return true;
}



/**
 * @brief Cancel order
 * 
 * @param symbol 
 * @param orderId 
 * @return true 
 * @return false 
 */
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
    std::string mSide = mAPIJson["side"].asString();

    if (mSide == "BUY")
    {
        if (mBuyOrders.count(orderId) == 1)
        {
            ELOG(INFO, "Canceled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSellOrders.find(orderId)->second["Symbol"].c_str(), mSellOrders.find(orderId)->second["Price"].c_str(), mSellOrders.find(orderId)->second["Quantity"].c_str(), mSellOrders.find(orderId)->second["TransactTime"].c_str());
            
            mBuyOrders.erase(orderId);
        }
    }
    // else if (mSide == "SELL")
    // {
    //     if (mSellOrders.count(orderId) == 1)
    //     {
    //         mBuyOrders.erase(orderId);

    //         ELOG(INFO, "Canceled a Order. Order ID: %d", mOrderId);
    //     }
    // }
    
    
    return true;
}



/**
 * @brief Cancel all open orders
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceRequests::cancelAllOpenOrders(std::string symbol)
{
    std::string mBaseURL        = mBase + "/api/v3/openOrders";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "symbol="+symbol;
                mParameters     += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature      = pBu->getSignature(mParameters);

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"symbol", symbol},
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = deleteRequest(url, headers, parameters);

    ELOG(INFO, "Cancel All Open Orders Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Cancel All Open Orders Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    std::string mStatus = mAPIJson[0]["status"].asString();

    if (mStatus!="CANCELED")
    {
        ELOG(ERROR, "Order could not be cancelled.");
        return false;
    }

    ELOG(INFO, "Canceled All Open Orders.");

    return true;
}


/**
 * @brief Query order
 * 
 * @param symbol 
 * @param orderId 
 * @return true 
 * @return false 
 */
bool BinanceRequests::queryOrder(std::string symbol, uint32_t orderId)
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

    cpr::Response mReq          = getRequest(url, headers, parameters);

    ELOG(INFO, "Query Order Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Query Order Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    uint32_t mOrderId           = mAPIJson["orderId"].asInt();
    std::string mSide           = mAPIJson["side"].asString();
    std::string mStatus         = mAPIJson["status"].asString();
    std::string mSymbol         = mAPIJson["symbol"].asString();
    std::string mPrice          = mAPIJson["price"].asString();
    std::string mQuantity       = mAPIJson["origQty"].asString();
    std::string mTime           = mAPIJson["updateTime"].asString();

    ELOG(INFO, "Query Order. Status: %s, Order ID: %d", mStatus.c_str(), mOrderId);

    if (mStatus == "FILLED")
    {
        if (mSide == "BUY")
        {
            OrderMap mOrder;

            mOrder.emplace("Symbol", mSymbol);
            mOrder.emplace("BoughtPrice", mPrice);
            mOrder.emplace("Quantity", mQuantity);
            mOrder.emplace("BoughtTime", mTime);

            mBoughtOrders.emplace(mOrderId, mOrder);
            mBuyOrders.erase(orderId);

            ELOG(INFO, "Filled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTime.c_str());
        }
        else if (mSide == "SELL")
        {
            OrderMap mOrder;

            mOrder.emplace("Symbol", mSymbol);
            mOrder.emplace("SoldPrice", mPrice);
            mOrder.emplace("BoughtPrice", mSellOrders.find(orderId)->second["BoughtPrice"]);
            mOrder.emplace("Quantity", mQuantity);
            mOrder.emplace("SoldTime", mTime);

            mSoldOrders.emplace(mOrderId, mOrder);
            mSellOrders.erase(orderId);

            ELOG(INFO, "Filled a Sell Order. OrderId: %d, Symbol: %s, SoldPrice: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), mQuantity.c_str(), mTime.c_str());
        }
    }
 
    return true;
}



/**
 * @brief Current open orders
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceRequests::currentOpenOrders(std::string symbol)
{
    std::string mBaseURL        = mBase + "/api/v3/openOrders";

    std::string mTimestamp      = pBu->getTimestamp();

    std::string mParameters     = "symbol="+symbol;
                mParameters     += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature      = pBu->getSignature(mParameters);

    cpr::Url url                = cpr::Url{mBaseURL};
    cpr::Header headers         = cpr::Header{
                                            {"content-type", "application/json"}, 
                                            {"X-MBX-APIKEY", mAPI_KEY}};
    cpr::Parameters parameters  = cpr::Parameters{
                                            {"symbol", symbol},
                                            {"timestamp", mTimestamp},
                                            {"recvWindow", mRecvWindow},
                                            {"signature", mSignature}};

    cpr::Response mReq          = getRequest(url, headers, parameters);

    ELOG(INFO, "Current Open Orders Request Timestamp: %s, URL: %s", mTimestamp.c_str(), mReq.url.c_str());

    ELOG(INFO, "Current Open Orders Response Body: %s.", mReq.text.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mReq.text.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    // std::string mStatus = mAPIJson[0]["status"].asString();

    ELOG(INFO, "Current Open Orders.");

    return true;
}