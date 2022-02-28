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

    mSymbol         = pBu->getSymbol();
    mInterval       = pBu->getInterval();
    mFollowSymbol   = pBu->getFollowSymbol();
    mBalanceSymbol  = pBu->getBalanceSymbol();
    mBalanceAmount  = pBu->getBalanceAmount();

    if (mAPI_KEY.length() == 0 || mSECRET_KEY.length() == 0)
    {
        ELOG(ERROR, "Failed to initialize Requests constructor.");
        exit(0);
    }

    ELOG(INFO, "Requests constructor initialized. mSymbol: %s, mFollowSymbol: %s, mInterval: %s, mBalanceSymbol: %s, mBalanceAmount: %s.", mSymbol.c_str(), mFollowSymbol.c_str(), mInterval.c_str(), mBalanceSymbol.c_str(), mBalanceAmount.c_str());

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

        if (mAccountStatus)
        {
            bool mAPIKeyPermission      = getAPIKeyPermission();

            if (!mAPIKeyPermission)
            {
                ELOG(ERROR, "User API Spot and Margin trading is not active. Can't enter main loop.");
            }

            if (mAPIKeyPermission)
            {
                // break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        int day = 0;
        int hour = 12;
        int minute = 0;
        int second = 0;
        int millisecond = 0;

        getCandlesticksData(mSymbol, mInterval, pBu->getOldTimestamp(day, hour, minute, second, millisecond));

        // getCandlesticksData(mFollowSymbol, mInterval, pBu->getOldTimestamp(day, hour, minute, second, millisecond));

        // // // std::cout << "mCandlesSize: " << mCandlesSize << std::endl;
        // std::cout << "mTradeCandlesOpenPrices: " << mTradeCandlesOpenPrices.size() << " Average: " << pBu->getAverage(mTradeCandlesOpenPrices) << std::endl;
        // std::cout << "mFollowCandlesOpenPrices: " << mFollowCandlesOpenPrices.size() << " Average: " << pBu->getAverage(mFollowCandlesOpenPrices) << std::endl;

        // mTradeCandlesOpenPricesAverage = pBu->getAverage(mTradeCandlesOpenPrices);

        // mFollowCandlesOpenPricesAverage = pBu->getAverage(mFollowCandlesOpenPrices);

        getTickSize(mSymbol);
        // getTickSize(mFollowSymbol);

        // std::cout << pBu->roundPrice(mTradeCandlesOpenPricesAverage, mSymbolTickSize) << std::endl;
        // std::cout << pBu->roundPrice(mFollowCandlesOpenPricesAverage, mFollowSymbolTickSize) << std::endl;

        // std::cout << "Compare: " << pBu->comparePrice("11.22", "11.2") << std::endl;
    }


    getCoinBalance(mBalanceSymbol);


    ELOG(INFO, "User account is normal. Entering while loop.");

    // buy();
}


// /**
//  * @brief Requests main loop
//  * 
//  */
// void BinanceRequests::buy()
// {
//     bool mEnough = false;
//     while (1)
//     {
//         Opel *pOpel = Opel::instance();

//         if (pOpel->getIsActive())
//         {
//             if (mBuyOrders.size() > 0)
//                 queryOrder(mSymbol, mBuyOrders.begin()->first);

//             if (mSellOrders.size() > 0)
//                 for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
//                     queryOrder(mSymbol, it->first);

//             if (mBoughtOrders.size() > 0)
//                 // createNewOrder(mSymbol, "SELL", "LIMIT", "0.20", "91.95");

//             if (mEnough == false)
//                 if (mBuyOrders.size() < 1)
//                     // createNewOrder(mSymbol, "BUY", "LIMIT", "0.20", "90.4");
        
//             mEnough = true;
//             // currentOpenOrders(mSymbol);

//             // cancelOrder(mSymbol, mBuyOrders.begin()->first);

//             // cancelAllOpenOrders(mSymbol);
//         }

//         ELOG(INFO, "mBuyOrders: %d, mBoughtOrders: %d, mSellOrders: %d, mSoldOrders: %d.", mBuyOrders.size(), mBoughtOrders.size(), mSellOrders.size(), mSoldOrders.size());

//         std::cout << "Buy Orders" << std::endl;
//         for (AllOrdersMap::iterator it = mBuyOrders.begin(); it != mBuyOrders.end(); it++)
//         {
//             std::cout << it->first << ": " << it->second["Price"] << std::endl;
//         }

//         std::cout << "Bought Orders" << std::endl;
//         for (AllOrdersMap::iterator it = mBoughtOrders.begin(); it != mBoughtOrders.end(); it++)
//         {
//             std::cout << it->first << ": " << it->second["BoughtPrice"] << std::endl;
//         }

//         std::cout << "Sell Orders" << std::endl;
//         for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
//         {
//             std::cout << it->first << ": " << it->second["Price"] << std::endl;
//         }

//         std::cout << "Sold Orders" << std::endl;
//         for (AllOrdersMap::iterator it = mSoldOrders.begin(); it != mSoldOrders.end(); it++)
//         {
//             std::cout << it->first << ": " << it->second["SoldPrice"] << " - " << it->second["BoughtPrice"] << std::endl;
//         }

//         std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//     }
// }


/**
 * @brief Get request
 * 
 * @param endpoint 
 * @param parameters 
 * @param headers 
 * @return std::string 
 */
std::string BinanceRequests::getRequest(std::string endpoint, std::string parameters, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1 second 500 milliseconds
    cli.set_read_timeout(2, 0);             // 2 seconds
    cli.set_write_timeout(2, 0);            // 2 seconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Get Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Get(endpointWithParameters.c_str(), headers)) 
    {
        return res->body;
    } 
    
    // auto err = res.error();
        
    return "{'Message': 'Error.'}";
}


/**
 * @brief Post request
 * 
 * @param endpoint 
 * @param parameters 
 * @param headers 
 * @return httplib::Result 
 */
std::string BinanceRequests::postRequest(std::string endpoint, std::string parameters, httplib::Params signature, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1 second 500 milliseconds
    cli.set_read_timeout(2, 0);             // 2 seconds
    cli.set_write_timeout(2, 0);            // 2 seconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Post Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Post(endpointWithParameters.c_str(), headers, signature)) 
    {
        return res->body;
    } 
    
    // auto err = res.error();
        
    return "{'Message': 'Error.'}";
}


/**
 * @brief Delete request
 * 
 * @param endpoint 
 * @param parameters 
 * @param headers 
 * @return httplib::Result 
 */
std::string BinanceRequests::deleteRequest(std::string endpoint, std::string parameters, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1 second 500 milliseconds
    cli.set_read_timeout(2, 0);             // 2 seconds
    cli.set_write_timeout(2, 0);            // 2 seconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Delete Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Delete(endpointWithParameters.c_str(), headers)) 
    {
        return res->body;
    } 
    
    // auto err = res.error();
        
    return "{'Message': 'Error.'}";
}


/**
 * @brief Get user account status
 * 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getAccountStatus()
{
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/account/status";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Get Account Status Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Get Account Status Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/account/apiRestrictions";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Get API KEY Permission Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Get API KEY Permission Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
 * @brief Get coin balance
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getCoinBalance (std::string symbol)
{
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/capital/config/getall";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Get Coin Balance Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Get Coin Balance Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    for (int i=0; i< static_cast<int>(mAPIJson.size()); i++)
    {
        if (mAPIJson[i]["coin"] == symbol)
        {
            std::cout << mAPIJson[i]["free"] << std::endl;

            ELOG(INFO, "Get Coin Balance Symbol: %s, Balance: %s.", mAPIJson[i]["coin"].asCString(), mAPIJson[i]["free"].asCString());
        }
    }

    return true;
}


/**
 * @brief Get candlesticks data
 * 
 * @param symbol 
 * @param interval 
 * @param startTime 
 * @param limit 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getCandlesticksData(std::string symbol, std::string interval, std::string startTime)
{
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/klines";


    std::string mParams                 = "symbol="+symbol+"&interval="+interval+"&startTime="+startTime+"&limit=1000";


    httplib::Headers mHeaders           = {{"content-type", "application/json"}};


    std::string mResponseBody           = getRequest(mEndpoint, mParams, mHeaders);

    ELOG(INFO, "Get Klines/Candlesticks Data Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Get Klines/Candlesticks Data Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    // std::cout << mAPIJson.size() << std::endl;

    mCandlesSize = static_cast<int>(mAPIJson.size());

    if (symbol == mSymbol)
    {
        for (int i = 0; i<static_cast<int>(mAPIJson.size()); i++)
        {
            // std::cout << mAPIJson[i][4] << std::endl;

            mTradeCandlesOpenPrices.push_back(mAPIJson[i][1].asString());
            mTradeCandlesHighPrices.push_back(mAPIJson[i][2].asString());
            mTradeCandlesLowPrices.push_back(mAPIJson[i][3].asString());
            mTradeCandlesClosePrices.push_back(mAPIJson[i][4].asString());
        }
    }

    if (symbol == mFollowSymbol)
    {
        for (int i = 0; i<static_cast<int>(mAPIJson.size()); i++)
        {
            mFollowCandlesOpenPrices.push_back(mAPIJson[i][1].asString());
            mFollowCandlesHighPrices.push_back(mAPIJson[i][2].asString());
            mFollowCandlesLowPrices.push_back(mAPIJson[i][3].asString());
            mFollowCandlesClosePrices.push_back(mAPIJson[i][4].asString());
        }
    }
    
    return true;
}


/**
 * @brief Get tick size
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceRequests::getTickSize (std::string symbol)
{
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/exchangeInfo";


    std::string mParams                 = "symbol="+symbol;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}};


    std::string mResponseBody           = getRequest(mEndpoint, mParams, mHeaders);

    ELOG(INFO, "Get Tick Size Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Get Tick Size Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    Json::Value mFiltersJson = mAPIJson["symbols"][0]["filters"];

    for (int i = 0; i<static_cast<int>(mFiltersJson.size()); i++)
    {
        if (mFiltersJson[i]["filterType"] == "PRICE_FILTER")
        {
            if (symbol == mSymbol)
            {
                mSymbolTickSize = pBu->getTickSize(mFiltersJson[i]["tickSize"].toStyledString());
                break;
            }

            if (symbol == mFollowSymbol)
            {
                mFollowSymbolTickSize = pBu->getTickSize(mFiltersJson[i]["tickSize"].toStyledString());
                break;
            }
        }
    }

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";


    std::string mParams                 = "symbol="+symbol+"&side="+side;
                mParams                 += "&type="+type+"&timeInForce=GTC";
                mParams                 += "&quantity="+quantity+"&price="+price;
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    httplib::Params mSignatureParams    = {{ "signature", mSignature }};


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = postRequest(mEndpoint, mParams, mSignatureParams, mHeaders);

    ELOG(INFO, "Create New Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Create New Order Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";


    std::string mParams                 = "symbol="+symbol+"&orderId="+std::to_string(orderId);
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = deleteRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Cancel Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Cancel Order Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/openOrders";


    std::string mParams                 = "symbol="+symbol+"&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = deleteRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Cancel All Open Orders Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Cancel All Open Orders Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";


    std::string mParams                 = "symbol="+symbol+"&orderId="+std::to_string(orderId);
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Query Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Query Order Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/api/v3/openOrders";


    std::string mParams                 = "symbol="+symbol+"&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

    ELOG(INFO, "Current Open Orders Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    ELOG(INFO, "Current Open Orders Response Body: %s.", mResponseBody.c_str());

    Json::Value  mAPIJson;
    Json::Reader mReader;
    bool         mParsingSuccessful = mReader.parse(mResponseBody.c_str(), mAPIJson);

    if (!mParsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    ELOG(INFO, "Current Open Orders.");

    return true;
}