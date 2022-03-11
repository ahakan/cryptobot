#include "../inc/requests.h"
#include <string>


/**
 * @brief Construct a new Requests::Requests object
 * 
 * @param pBu 
 */
Requests::Requests(BinanceUtilities *pBu)
{
    this->pBu               = pBu;
    mBase                   = pBu->getAPIBase();
    mAPI_KEY                = pBu->getAPIKEY();
    mSECRET_KEY             = pBu->getAPISECRETKEY();

    mSymbol                 = pBu->getSymbol();
    mInterval               = pBu->getInterval();
    mFollowSymbol           = pBu->getFollowSymbol();
    mBalanceSymbol          = pBu->getBalanceSymbol();
    mBalanceAmount          = pBu->getBalanceAmount();
    mAverageAmount          = pBu->getAverageAmount();
    mAverageAutoCalculate   = pBu->getAverageAutoCalculate();
    mRSISize                = pBu->getRSISize();

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
 * @brief Calculate new sell order price average
 * 
 * @return true 
 * @return false 
 */
bool Requests::calcSellPriceAverage()
{
    if (mAverageAutoCalculate)
    {
        float highestPrice          = std::stof(*std::max_element(mTradeCandlesHighPrices.begin(), mTradeCandlesHighPrices.end()));
        float lowestPrice           = std::stof(*std::min_element(mTradeCandlesLowPrices.begin(), mTradeCandlesLowPrices.end()));

        float calculatedAverage     = (highestPrice-lowestPrice)/(mRSISize/2.0);

        mSellPriceCalculatedAverage = pBu->roundPrice(std::to_string(calculatedAverage), mSymbolTickSize); 

        ELOG(INFO, "Calculated New Trade Average. New Average: %s.", mSellPriceCalculatedAverage.c_str());
        
        return true;
    }

    if (!mAverageAutoCalculate)
    {
        mSellPriceCalculatedAverage = mAverageAmount;

        ELOG(INFO, "Average Auto Calculated Disable. Average: %s.", mSellPriceCalculatedAverage.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate New Trade Average.");

    return false;
}


/**
 * @brief Calculate Symbol RSI
 * 
 * @return true 
 * @return false 
 */
bool Requests::calcSymbolRSI()
{
    if (mTradeCandlesClosePrices.size() != 0)
    {
        mTradeCandlesCloseRSI               = pBu->calculateRSI(mTradeCandlesClosePrices);

        ELOG(INFO, "Calculated Symbol RSI. Trade Candles Close RSI: %s.", mTradeCandlesCloseRSI.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate Symbol RSI.");
    
    return false;
}


/**
 * @brief Calculate follow symbol RSI
 * 
 * @return true 
 * @return false 
 */
bool Requests::calcFollowRSI()
{
    if (mFollowCandlesClosePrices.size() != 0)
    {
        mFollowCandlesCloseRSI               = pBu->calculateRSI(mFollowCandlesClosePrices);

        ELOG(INFO, "Calculated Follow Symbol RSI. Follow Candles Close RSI: %s.", mFollowCandlesCloseRSI.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate Follow Symbol RSI.");
    
    return false;
}


/**
 * @brief Calculate symbol candles average
 * 
 * @return true 
 * @return false 
 */
bool Requests::calcSymbolAverages()
{
    if (mTradeCandlesClosePrices.size() != 0)
    {
        mTradeCandlesOpenPricesAverage      = pBu->calculateAverage(mTradeCandlesOpenPrices);
        mTradeCandlesHighPricesAverage      = pBu->calculateAverage(mTradeCandlesHighPrices);
        mTradeCandlesLowPricesAverage       = pBu->calculateAverage(mTradeCandlesLowPrices);
        mTradeCandlesClosePricesAverage     = pBu->calculateAverage(mTradeCandlesClosePrices);

        ELOG(INFO, "Calculated Symbol Averages. Trade Close Average: %s.", mTradeCandlesClosePricesAverage.c_str());

        mTradeCandlesOpenPricesAverage      = pBu->roundPrice(mTradeCandlesOpenPricesAverage, mSymbolTickSize);
        mTradeCandlesHighPricesAverage      = pBu->roundPrice(mTradeCandlesHighPricesAverage, mSymbolTickSize);
        mTradeCandlesLowPricesAverage       = pBu->roundPrice(mTradeCandlesLowPricesAverage, mSymbolTickSize);
        mTradeCandlesClosePricesAverage     = pBu->roundPrice(mTradeCandlesClosePricesAverage, mSymbolTickSize);

        ELOG(INFO, "Rounded Trade Averages. TOA: %s, THA: %s, TLA: %s, TCA: %s", mTradeCandlesOpenPricesAverage.c_str(), mTradeCandlesHighPricesAverage.c_str(), mTradeCandlesLowPricesAverage.c_str(), mTradeCandlesClosePricesAverage.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate Symbol Averages.");
    
    return false;
}


/**
 * @brief Calculate follow symbol candles average
 * 
 * @return true 
 * @return false 
 */
bool Requests::calcFollowAverages()
{
    if (mFollowCandlesClosePrices.size() != 0)
    {
        mFollowCandlesOpenPricesAverage     = pBu->calculateAverage(mFollowCandlesOpenPrices);
        mFollowCandlesHighPricesAverage     = pBu->calculateAverage(mFollowCandlesHighPrices);
        mFollowCandlesLowPricesAverage      = pBu->calculateAverage(mFollowCandlesLowPrices);
        mFollowCandlesClosePricesAverage    = pBu->calculateAverage(mFollowCandlesClosePrices);

        ELOG(INFO, "Calculated Follow Symbol Averages. Follow Close Average: %s.", mFollowCandlesClosePricesAverage.c_str());

        mFollowCandlesOpenPricesAverage     = pBu->roundPrice(mFollowCandlesOpenPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesHighPricesAverage     = pBu->roundPrice(mFollowCandlesHighPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesLowPricesAverage      = pBu->roundPrice(mFollowCandlesLowPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesClosePricesAverage    = pBu->roundPrice(mFollowCandlesClosePricesAverage, mFollowSymbolTickSize);

        ELOG(INFO, "Rounded Follow Averages. FOA: %s, FHA: %s, FLA: %s, FCA: %s", mFollowCandlesOpenPricesAverage.c_str(), mFollowCandlesHighPricesAverage.c_str(), mFollowCandlesLowPricesAverage.c_str(), mFollowCandlesClosePricesAverage.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate Follow Symbol Averages.");

    return false;
}


/**
 * @brief Check candle data struct
 * 
 * @return true 
 * @return false 
 */
bool Requests::readCandleData()
{
    struct candle_data *pTradeCandleData = Opel::getTradeCandleStruct();

    pTradeCandleData->lock();

    if (pTradeCandleData->isUpdated)
    {
        if (pTradeCandleData->isClosed)
        {
            bool areAdded = addClosedCandlePrices(pTradeCandleData->symbol, pTradeCandleData->openPrice, pTradeCandleData->closePrice, pTradeCandleData->highPrice, pTradeCandleData->lowPrice);

            if (areAdded)
            {
                bool mCalculateSymbolAverages   = calcSymbolAverages();
                bool mCalculateSymbolRSI        = calcSymbolRSI();

                if (mCalculateSymbolAverages && mCalculateSymbolRSI)
                {
                    ELOG(INFO, "Added new symbol closed price data and calculated symbol averages and RSI.");
                }
            }
        }
        else
        {
            mSymbolLivePrice = pTradeCandleData->closePrice;

            ELOG(INFO, "Current Trade Symbol: %s, Live Price: %s.", pTradeCandleData->symbol.c_str(), mSymbolLivePrice.c_str());
        }

        pTradeCandleData->isUpdated = false;
    }

    pTradeCandleData->unlock();


    struct candle_data *pFollowCandleData = Opel::getFollowCandleStruct();

    pFollowCandleData->lock();

    if (pFollowCandleData->isUpdated)
    {
        if (pFollowCandleData->isClosed)
        {
            bool areAdded = addClosedCandlePrices(pFollowCandleData->symbol, pFollowCandleData->openPrice, pFollowCandleData->closePrice, pFollowCandleData->highPrice, pFollowCandleData->lowPrice);

            if (areAdded)
            {
                bool mCalculateFollowAverages   = calcFollowAverages();
                bool mCalculateFollowRSI        = calcFollowRSI();

                if (mCalculateFollowAverages && mCalculateFollowRSI)
                {
                    ELOG(INFO, "Added new follow symbol closed price data and calculated symbol averages and RSI.");
                }
            }
        }
        else
        {
            mFollowLivePrice = pFollowCandleData->closePrice;

            ELOG(INFO, "Current Follow Symbol: %s, Live Price: %s.", pFollowCandleData->symbol.c_str(), mFollowLivePrice.c_str());
        }

        pFollowCandleData->isUpdated = false;
    }

    pFollowCandleData->unlock();

    return true;
}


/**
 * @brief Add closed candle datas to vectors.
 * 
 * @param symbol 
 * @param open 
 * @param close 
 * @param high 
 * @param low 
 * @return true 
 * @return false 
 */
bool Requests::addClosedCandlePrices(std::string symbol, std::string open, std::string close, std::string high, std::string low)
{
    if (symbol==mSymbol)
    {
        mTradeCandlesOpenPrices.push_back(open);
        mTradeCandlesOpenPrices.erase(mTradeCandlesOpenPrices.begin());

        mTradeCandlesClosePrices.push_back(close);
        mTradeCandlesClosePrices.erase(mTradeCandlesClosePrices.begin());

        mTradeCandlesHighPrices.push_back(high);
        mTradeCandlesHighPrices.erase(mTradeCandlesHighPrices.begin());

        mTradeCandlesLowPrices.push_back(low);
        mTradeCandlesLowPrices.erase(mTradeCandlesLowPrices.begin());

        ELOG(INFO, "Added new trade symbol closed prices. OpenPricesSize: %d, ClosePricesSize: %d, HighPricesSize: %d, LowPricesSize: %d.", mTradeCandlesOpenPrices.size(), mTradeCandlesClosePrices.size(), mTradeCandlesHighPrices.size(), mTradeCandlesLowPrices.size());
    
        return true;
    }
    else if (symbol==mFollowSymbol)
    {
        mFollowCandlesOpenPrices.push_back(open);
        mFollowCandlesOpenPrices.erase(mFollowCandlesOpenPrices.begin());

        mFollowCandlesClosePrices.push_back(close);
        mFollowCandlesClosePrices.erase(mFollowCandlesClosePrices.begin());

        mFollowCandlesHighPrices.push_back(high);
        mFollowCandlesHighPrices.erase(mFollowCandlesHighPrices.begin());

        mFollowCandlesLowPrices.push_back(low);
        mFollowCandlesLowPrices.erase(mFollowCandlesLowPrices.begin());

        ELOG(INFO, "Added new follow symbol closed prices. OpenPricesSize: %d, ClosePricesSize: %d, HighPricesSize: %d, LowPricesSize: %d.", mFollowCandlesOpenPrices.size(), mFollowCandlesClosePrices.size(), mFollowCandlesHighPrices.size(), mFollowCandlesLowPrices.size());

        return true;
    }
    
    ELOG(ERROR, "Failed to add new closed prices data to vectors.");

    return false;
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
        while (!mAccountStatus)
        {
            mAccountStatus         = getAccountStatus();

            if (!mAccountStatus)
            {
                ELOG(ERROR, "User account status is not normal. Can't enter main loop.");
            }

            if (mAccountStatus)
            {
                mAPIKeyPermission      = getAPIKeyPermission();

                if (!mAPIKeyPermission)
                {
                    ELOG(ERROR, "User API Spot and Margin trading is not active. Can't enter main loop.");
                }

                if (mAPIKeyPermission)
                {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
        
        bool  mGetSymbolTickSize = getTickSize(mSymbol);
        bool  mGetFollowTickSize = getTickSize(mFollowSymbol);

        if (mGetSymbolTickSize && mGetFollowTickSize)
        {
            bool mGetTradeSymbolCandles     = getCandlesticksData(mSymbol, mInterval, pBu->getOldTimestamp());
            bool mGetFollowSymbolCandles    = getCandlesticksData(mFollowSymbol, mInterval, pBu->getOldTimestamp());

            if (mGetTradeSymbolCandles && mGetFollowSymbolCandles)
            {
                bool mCalculateSymbolAverages   = calcSymbolAverages();
                bool mCalculateSymbolRSI        = calcSymbolRSI();

                bool mCalculateFollowAverages   = calcFollowAverages();
                bool mCalculateFollowRSI        = calcFollowRSI();

                if (mCalculateSymbolAverages && mCalculateFollowAverages && mCalculateSymbolRSI && mCalculateFollowRSI)
                {
                    bool mGetWalletBalance  = getCoinBalance(mBalanceSymbol);

                    if (mGetWalletBalance)
                    {
                        ELOG(INFO, "Checked User Account and Wallet. Calculated Averages and RSI. Starting Trade...");

                        break;
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    newBuy();
}


/**
 * @brief Requests main loop
 * 
 */
void BinanceRequests::newBuy()
{
    while (1)
    {
        Opel *pOpel = Opel::instance();


        while(pOpel->getIsActive())
        {
            readCandleData();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // if ()
        // {
        //     if (mBuyOrders.size() > 0)
        //         queryOrder(mSymbol, mBuyOrders.begin()->first);

        //     if (mSellOrders.size() > 0)
        //         for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
        //             queryOrder(mSymbol, it->first);

        //     if (mBoughtOrders.size() > 0)
        //         // createNewOrder(mSymbol, "SELL", "LIMIT", "0.20", "91.95");

        //     if (mEnough == false)
        //         if (mBuyOrders.size() < 1)
        //             // createNewOrder(mSymbol, "BUY", "LIMIT", "0.20", "90.4");
        
        //     mEnough = true;
        //     // currentOpenOrders(mSymbol);

        //     // cancelOrder(mSymbol, mBuyOrders.begin()->first);

        //     // cancelAllOpenOrders(mSymbol);
        // }

        // ELOG(INFO, "mBuyOrders: %d, mBoughtOrders: %d, mSellOrders: %d, mSoldOrders: %d.", mBuyOrders.size(), mBoughtOrders.size(), mSellOrders.size(), mSoldOrders.size());

        // std::cout << "Buy Orders" << std::endl;
        // for (AllOrdersMap::iterator it = mBuyOrders.begin(); it != mBuyOrders.end(); it++)
        // {
        //     std::cout << it->first << ": " << it->second["Price"] << std::endl;
        // }

        // std::cout << "Bought Orders" << std::endl;
        // for (AllOrdersMap::iterator it = mBoughtOrders.begin(); it != mBoughtOrders.end(); it++)
        // {
        //     std::cout << it->first << ": " << it->second["BoughtPrice"] << std::endl;
        // }

        // std::cout << "Sell Orders" << std::endl;
        // for (AllOrdersMap::iterator it = mSellOrders.begin(); it != mSellOrders.end(); it++)
        // {
        //     std::cout << it->first << ": " << it->second["Price"] << std::endl;
        // }

        // std::cout << "Sold Orders" << std::endl;
        // for (AllOrdersMap::iterator it = mSoldOrders.begin(); it != mSoldOrders.end(); it++)
        // {
        //     std::cout << it->first << ": " << it->second["SoldPrice"] << " - " << it->second["BoughtPrice"] << std::endl;
        // }

        // std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}


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
    else
    {
        auto err = res.error();

        std::string errorMessage = "{'Error': '"+httplib::to_string(err)+"'}"; 

        return errorMessage;
    }
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
    else
    {
        auto err = res.error();

        std::string errorMessage = "{'Error': '"+httplib::to_string(err)+"'}"; 

        return errorMessage;
    }
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
    else
    {
        auto err = res.error();

        std::string errorMessage = "{'Error': '"+httplib::to_string(err)+"'}"; 

        return errorMessage;
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
    std::string mTimestamp              = pBu->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/account/status";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};

    
    ELOG(INFO, "Get Account Status Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

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
    

    ELOG(INFO, "Get API KEY Permission Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

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


    ELOG(INFO, "Get Coin Balance Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

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
            std::string mWalletBalanceAmount = mAPIJson[i]["free"].asString();

            ELOG(INFO, "Get Coin Balance Symbol: %s, Balance: %s.", mAPIJson[i]["coin"].asCString(), mAPIJson[i]["free"].asCString());

            bool isAmountEnough = pBu->comparePrice(mWalletBalanceAmount, mBalanceAmount);

            if (!isAmountEnough)
            {
                ELOG(WARNING, "Balance Amount is Greater Than Wallet Balance Amount. Balance Amount: %s, Wallet Balance: %s.", mBalanceAmount.c_str(), mWalletBalanceAmount.c_str());
                
                return false;
            }
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


    ELOG(INFO, "Get Klines/Candlesticks Data Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParams, mHeaders);

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


    ELOG(INFO, "Get Tick Size Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParams, mHeaders);

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
                return true;
            }

            if (symbol == mFollowSymbol)
            {
                mFollowSymbolTickSize = pBu->getTickSize(mFiltersJson[i]["tickSize"].toStyledString());
                return true;
            }
        }
    }

    return false;
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


    ELOG(INFO, "Create New Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = postRequest(mEndpoint, mParams, mSignatureParams, mHeaders);

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


    ELOG(INFO, "Cancel Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = deleteRequest(mEndpoint, mParamsWithSignature, mHeaders);

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
    uint32_t mOrderId   = mAPIJson["orderId"].asInt();
    std::string mSide   = mAPIJson["side"].asString();

    if (mStatus!="CANCELED")
    {
        ELOG(ERROR, "Failed to Cancel the Order. Order id: %d.", mOrderId);
        return false;
    }

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


    ELOG(INFO, "Cancel All Open Orders Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = deleteRequest(mEndpoint, mParamsWithSignature, mHeaders);

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
        ELOG(ERROR, "Failed to Cancel the All Open Orders.");
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


    ELOG(INFO, "Query Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

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


    ELOG(INFO, "Current Open Orders Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

    std::string mResponseBody           = getRequest(mEndpoint, mParamsWithSignature, mHeaders);

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