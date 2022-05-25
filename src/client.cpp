#include "../inc/client.h"


/**
 * @brief Construct a new Client::Client object
 * 
 * @param pU
 */
Client::Client(std::shared_ptr<BinanceUtilities> pU)
    : pBu(pU)
{
    mBase                   = pBu.get()->getAPIBase();
    mAPI_KEY                = pBu.get()->getAPIKEY();
    mSECRET_KEY             = pBu.get()->getAPISECRETKEY();

    ELOG(INFO, "Client constructor initialized.");
}


/**
 * @brief Destroy the Client::Client object
 * 
 */
Client::~Client()
{
    ELOG(INFO, "Client destructor.");
}


/**
 * @brief Construct a new BinanceClient::BinanceClient object
 * 
 * @param pBu 
 */
BinanceClient::BinanceClient(std::shared_ptr<BinanceUtilities> pU)
    : Client(pU)
{
    ELOG(INFO, "BinanceClient constructor initialized.");
}


/**
 * @brief Destroy the BinanceClient::BinanceClient object
 * 
 */
BinanceClient::~BinanceClient()
{
    ELOG(INFO, "BinanceClient destructor.");
}


/**
 * @brief Get request
 * 
 * @param endpoint 
 * @param parameters 
 * @param headers 
 * @return std::string 
 */
std::string BinanceClient::getRequest(std::string endpoint, std::string parameters, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1500 milliseconds
    cli.set_read_timeout(2, 0);        // 2500 milliseconds
    cli.set_write_timeout(2, 0);       // 2500 milliseconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Get Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Get(endpointWithParameters.c_str(), headers)) 
    {
        return res->body;
    } 
    else
    {
        if (res == nullptr)
        {
            std::string errorMessage = "{'Error': 'nullptr'}"; 

            return errorMessage;
        }

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
std::string BinanceClient::postRequest(std::string endpoint, std::string parameters, httplib::Params signature, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1500 milliseconds
    cli.set_read_timeout(2, 0);        // 2500 milliseconds
    cli.set_write_timeout(2, 0);       // 2500 milliseconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Post Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Post(endpointWithParameters.c_str(), headers, signature)) 
    {
        return res->body;
    } 
    else
    {
        if (res == nullptr)
        {
            std::string errorMessage = "{'Error': 'nullptr'}"; 

            return errorMessage;
        }

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
std::string BinanceClient::deleteRequest(std::string endpoint, std::string parameters, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1500 milliseconds
    cli.set_read_timeout(2, 0);        // 2500 milliseconds
    cli.set_write_timeout(2, 0);       // 2500 milliseconds

    std::string endpointWithParameters = endpoint + "?" + parameters;

    ELOG(INFO, "Delete Request URL: %s", (mBase+endpointWithParameters).c_str());

    if (auto res = cli.Delete(endpointWithParameters.c_str(), headers)) 
    {
        return res->body;
    } 
    else
    {
        if (res == nullptr)
        {
            std::string errorMessage = "{'Error': 'nullptr'}"; 

            return errorMessage;
        }

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
bool BinanceClient::getAccountStatus()
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/sapi/v1/account/status";


    std::string reqParams           = "timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};

    
    ELOG(INFO, "Get Account Status Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Get Account Status Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    std::string accountStatusData = parsedResponse["data"].asString();

    if (accountStatusData != "Normal")
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
bool BinanceClient::getAPIKeyPermission()
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/sapi/v1/account/apiRestrictions";


    std::string reqParams           = "timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};
    

    ELOG(INFO, "Get API KEY Permission Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Get API KEY Permission Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    bool enableSpotAndMarginTrading = parsedResponse["enableSpotAndMarginTrading"].asBool();

    if (!enableSpotAndMarginTrading)
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
 * @param coin 
 * @return true 
 * @return false 
 */
bool BinanceClient::getCoinBalance(struct Symbol& coin)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/sapi/v1/capital/config/getall";


    std::string reqParams           = "timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    ELOG(INFO, "Get Coin Balance Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Get Coin Balance Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    if (parsedResponse.size() > 0)
    {
        for (unsigned int i=0; i<parsedResponse.size(); i++)
        {
            coin.lock();

            if (parsedResponse[i]["coin"] == coin.coinName)
            {
                std::string walletBalanceAmount = parsedResponse[i]["free"].asString();

                bool isQuantityEnough = pBu.get()->ctscf(walletBalanceAmount, coin.coinQuantity);

                if (!isQuantityEnough)
                {
                    ELOG(WARNING, "Coin Quantity is Greater Than Wallet Coin Quantity. Coin Quantity: %s, "
                                    "Wallet Coin Quantity: %s.", 
                                        coin.coinQuantity.c_str(), 
                                        walletBalanceAmount.c_str());
                    
                    coin.unlock();

                    return false;
                }

                ELOG(INFO, "Sufficient coin quantity. Quantity: %s.", walletBalanceAmount.c_str());

                // coin.coinQuantity   = pBu.get()->roundString(walletBalanceAmount, coin.tickSize);

                coin.unlock();

                return true;
            }

            coin.unlock();
        }
    }

    return false;
}


/**
 * @brief Get candlestick data
 * 
 * @param candlestick 
 * @return true 
 * @return false 
 */
bool BinanceClient::getCandlesticksData(struct Candlesticks& candlestick)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/klines";


    std::string reqParams           = "symbol="+candlestick.symbol+"&interval="+candlestick.interval;
                reqParams           += "&startTime="+candlestick.startTime+"&limit=1000";


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Klines/Candlesticks Data Request Timestamp: %s, Endpoint: %s, Start Timestamp: %s.", 
                    reqTimestamp.c_str(), 
                    reqEndpoint.c_str(), 
                    candlestick.startTime.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParams, reqHeaders);

    ELOG(INFO, "Get Klines/Candlesticks Data Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    int candlesSize = static_cast<int>(parsedResponse.size());

    ELOG(INFO, "Got Candlesticks Data. Symbol: %s, Candles size: %d, Interval: %s.", 
                    candlestick.symbol.c_str(), 
                    candlesSize, 
                    candlestick.interval.c_str());

    int priceSize = candlestick.openPrices.size();

    if (priceSize > 0)
    {
        candlestick.openPrices.clear();
        candlestick.highPrices.clear();
        candlestick.lowPrices.clear();
        candlestick.closePrices.clear();
        candlestick.quoteVolumes.clear();
        candlestick.percentChange.clear();
    }

    for (int i = 0; i < candlesSize; i++)
    {
        // std::cout << parsedResponse[i][4] << std::endl;

        candlestick.openPrices.push_back(parsedResponse[i][1].asString());
        candlestick.highPrices.push_back(parsedResponse[i][2].asString());
        candlestick.lowPrices.push_back(parsedResponse[i][3].asString());
        candlestick.closePrices.push_back(parsedResponse[i][4].asString());
        candlestick.quoteVolumes.push_back(parsedResponse[i][5].asString());

        candlestick.lastCandleTimestamp = std::to_string(parsedResponse[i][6].asLargestInt());
    }

    return true;
}


/**
 * @brief Get tick size
 * 
 * @param coin 
 * @return true 
 * @return false 
 */
bool BinanceClient::getTickSize(struct Symbol& coin)
{
    coin.lock();

    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/exchangeInfo";


    std::string reqParams           = "symbol="+coin.symbol;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Tick Size Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    coin.unlock();

    std::string responseBody        = getRequest(reqEndpoint, reqParams, reqHeaders);

    ELOG(INFO, "Get Tick Size Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    Json::Value filtersJson = parsedResponse["symbols"][0]["filters"];

    for (int i = 0; i<static_cast<int>(filtersJson.size()); i++)
    {
        if (filtersJson[i]["filterType"] == "PRICE_FILTER")
        {
            coin.lock();

            coin.tickSize = pBu.get()->getTickSize(filtersJson[i]["tickSize"].toStyledString());

            ELOG(INFO, "%s -> Tick Size: %d.", 
                            coin.symbol.c_str(), 
                            coin.tickSize);

            coin.unlock();

            return true;
        }
    }

    return false;
}


/**
 * @brief Get daily coin volume
 * 
 * @param coin 
 * @return true 
 * @return false 
 */
bool BinanceClient::getDailyVolume(struct Symbol& coin)
{
    coin.lock();

    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/ticker/24hr";


    std::string reqParams           = "symbol="+coin.symbol;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Daily Volume Size Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    coin.unlock();

    std::string responseBody        = getRequest(reqEndpoint, reqParams, reqHeaders);

    ELOG(INFO, "Get Daily Volume Size Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["lastPrice"].asString();
    std::string rVolume         = parsedResponse["volume"].asString();
    std::string rQuoteVolume    = parsedResponse["quoteVolume"].asString();

    coin.lock();
    coin.volume                 = rVolume;
    coin.quoteVolume            = rQuoteVolume;
    coin.unlock();

    ELOG(INFO, "Volume -> %s, Daily Volume: %s, Quote Volume: %s.", 
                    rSymbol.c_str(), 
                    rVolume.c_str(),
                    rQuoteVolume.c_str());

    return true;
}


/**
 * @brief Create new order
 * 
 * @param order 
 * @param coin 
 * @return true 
 * @return false 
 */
bool BinanceClient::createNewOrder(std::shared_ptr<Order> order, struct Symbol& coin)
{
    std::string reqTimestamp            = pBu.get()->getTimestamp();

    std::string reqEndpoint             = "/api/v3/order";

    std::string roundedQuantity         = pBu.get()->roundString(order.get()->quantity, coin.tickSize);


    std::string reqParams               = "symbol="+order.get()->symbol+"&side="+order.get()->side;
                reqParams               += "&type="+order.get()->type+"&timeInForce=GTC";
                reqParams               += "&quantity="+roundedQuantity+"&price="+order.get()->expectedPrice;

                if (order.get()->type == BINANCE_STOP_LOSS_LIMIT) 
                reqParams               += "&stopPrice="+order.get()->expectedStopPrice;

                reqParams               += "&timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature            = pBu.get()->getSignature(reqParams);

    httplib::Params reqParamsWithSign   = {{ "signature", reqSignature }};


    httplib::Headers reqHeaders         = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    ELOG(INFO, "Create New Order Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody            = postRequest(reqEndpoint, reqParams, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Create New Order Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful      = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    uint32_t rOrderId           = parsedResponse["orderId"].asLargestUInt();
    uint64_t rTransactTime      = parsedResponse["transactTime"].asUInt64();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rType           = parsedResponse["type"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    std::string rQuantity       = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();

    std::string rStopPrice;

    if (order.get()->type == BINANCE_STOP_LOSS_LIMIT) 
        rStopPrice              = parsedResponse["stopPrice"].asString();

    std::string rError          = parsedResponse["Error"].asString();

    if (rError.size() == 0)
    {
        order.get()->orderId        = rOrderId;
        order.get()->price          = rPrice;
        order.get()->status         = rStatus;
        order.get()->executedQty    = rExecutedQty;
        order.get()->transactTime   = rTransactTime;

        if (order.get()->type == BINANCE_STOP_LOSS_LIMIT)
        {
            order.get()->stopPrice  = rStopPrice;

            ELOG(INFO, "%s -> %s/%s(%d). Price: %s, Stop Price: %s, Quantity: %s,"
                        " ExecutedQty: %s, TransactTime: %llu.", 
                        rStatus.c_str(),
                        rSide.c_str(), 
                        rSymbol.c_str(), 
                        rOrderId, 
                        rPrice.c_str(), 
                        rStopPrice.c_str(), 
                        rQuantity.c_str(), 
                        rExecutedQty.c_str(), 
                        rTransactTime);

            return true;
        }

        ELOG(INFO, "%s -> %s/%s(%d). Price: %s, Quantity: %s," 
                    " ExecutedQty: %s, TransactTime: %llu.", 
                    rStatus.c_str(),
                    rSide.c_str(), 
                    rSymbol.c_str(), 
                    rOrderId, 
                    rPrice.c_str(), 
                    rQuantity.c_str(), 
                    rExecutedQty.c_str(), 
                    rTransactTime);

        return true;
    }
    
    ELOG(ERROR, "Failed to Create a New Order.");

    return false;
}


/**
 * @brief Cancel order
 * 
 * @param order 
 * @return true 
 * @return false 
 */
bool BinanceClient::cancelOrder(std::shared_ptr<Order> order)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/order";


    std::string reqParams           = "symbol="+order.get()->symbol+"&orderId="+std::to_string(order.get()->orderId);
                reqParams           += "&timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    ELOG(INFO, "Cancel Order Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = deleteRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Cancel Order Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    int rOrderId                = parsedResponse["orderId"].asInt();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    std::string rOrigQty        = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();

    if (rStatus!=BINANCE_CANCELED)
    {
        ELOG(ERROR, "Failed to Cancel the Order. Order id: %d.", order.get()->orderId);
        return false;
    }

    order.get()->orderId    = rOrderId;
    order.get()->side       = rSide;
    order.get()->status     = rStatus;

    ELOG(INFO, "%s -> %s / %d => Symbol: %s, Price: %s, Quantity: %s, ExecutedQty: %s.", 
                    rStatus.c_str(), rSide.c_str(), 
                    rOrderId, rSymbol.c_str(), 
                    rPrice.c_str(), rOrigQty.c_str(), 
                    rExecutedQty.c_str());

    return true;
}


/**
 * @brief Cancel all open orders
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceClient::cancelAllOpenOrders(std::string symbol)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/openOrders";


    std::string reqParams           = "symbol="+symbol+"&timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    ELOG(INFO, "Cancel All Open Orders Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = deleteRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Cancel All Open Orders Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    int responseSize = static_cast<int>(parsedResponse.size());

    ELOG(INFO, "Cancel All Open Orders. List size: %d.", responseSize);

    for (int i = 0; i < responseSize; i++)
    {
        uint32_t rOrderId           = parsedResponse[i]["orderId"].asLargestUInt();
        uint64_t rTransactTime      = parsedResponse[i]["transactTime"].asUInt64();
        std::string rStatus         = parsedResponse[i]["status"].asString();
        std::string rSide           = parsedResponse[i]["side"].asString();
        std::string rSymbol         = parsedResponse[i]["symbol"].asString();
        std::string rPrice          = parsedResponse[i]["price"].asString();
        std::string rOrigQty        = parsedResponse[i]["origQty"].asString();
        std::string rExecutedQty    = parsedResponse[i]["executedQty"].asString();

        ELOG(INFO, "%s -> %s / %d => Symbol: %s, Price: %s, Quantity: %s, ExecutedQty: %s, TransactTime: %llu.", 
                    rStatus.c_str(), rSide.c_str(), 
                    rOrderId, rSymbol.c_str(), 
                    rPrice.c_str(), rOrigQty.c_str(), 
                    rExecutedQty.c_str(), rTransactTime);
        
    }

    ELOG(INFO, "Canceled All Open Orders.");

    return true;
}


/**
 * @brief Query order
 * 
 * @param order 
 * @return true 
 * @return false 
 */
bool BinanceClient::queryOrder(std::shared_ptr<Order> order)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/order";


    std::string reqParams           = "symbol="+order.get()->symbol+"&orderId="+std::to_string(order.get()->orderId);
                reqParams           += "&timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    // ELOG(INFO, "Query Order Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Query Order Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    uint32_t rOrderId           = parsedResponse["orderId"].asLargestUInt();
    uint64_t rTransactTime      = parsedResponse["updateTime"].asUInt64();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rType           = parsedResponse["type"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    std::string rQuantity       = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();

    std::string rError          = parsedResponse["Error"].asString();

    if (rError.size() == 0)
    {
        order.get()->status         = rStatus;
        order.get()->executedQty    = rExecutedQty;
        order.get()->transactTime   = rTransactTime;

        if (rStatus == BINANCE_FILLED)
        {
            // add bought or sold price
            if (rSide == BINANCE_BUY)
            {
                order.get()->status         = rStatus;
                order.get()->boughtPrice    = rPrice;
            }
            else
            {
                order.get()->status         = rStatus;
                order.get()->soldPrice      = rPrice;
            }
        }

        if (rStatus == BINANCE_CANCELED)
        {
            // if return false
            bool isHigherThanZero           = pBu.get()->ctscf("00.00", rExecutedQty);

            // if return false    
            bool isLowerThanQuantity        = pBu.get()->ctscf(rExecutedQty, rQuantity);    

            // that means partially filled                                                                
            if (!isHigherThanZero && !isLowerThanQuantity)
            {
                // add bought or sold price
                if (rSide == BINANCE_BUY)
                {
                    // partially filled
                    order.get()->status         = BINANCE_FILLED;
                    order.get()->boughtPrice    = rPrice;
                }
                else
                {
                    order.get()->status         = "CANCELED_PARTIALLY_FILLED";
                    order.get()->soldPrice      = rPrice;
                }
            }
        }

        ELOG(INFO, "%s -> %s/%s(%d). Price: %s, Quantity: %s," 
                    " ExecutedQty: %s, TransactTime: %llu.", 
                    rStatus.c_str(), 
                    rSide.c_str(), 
                    rSymbol.c_str(), 
                    rOrderId, 
                    rPrice.c_str(), 
                    rQuantity.c_str(), 
                    rExecutedQty.c_str(), 
                    rTransactTime);
        
        return true;
    }
 
    return false;
}


/**
 * @brief Current open orders
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceClient::currentOpenOrders(std::string symbol)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/openOrders";


    std::string reqParams           = "symbol="+symbol+"&timestamp="+reqTimestamp+"&recvWindow="+mRecvWindow;

    std::string reqSignature        = pBu.get()->getSignature(reqParams);

    std::string reqParamsWithSign   = reqParams+"&signature="+reqSignature;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    ELOG(INFO, "Current Open Orders Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

    std::string responseBody        = getRequest(reqEndpoint, reqParamsWithSign, reqHeaders);

    ELOG(INFO, "Current Open Orders Response Body: %s.", responseBody.c_str());

    Json::Value  parsedResponse;
    Json::Reader parserReader;
    bool         parsingSuccessful  = parserReader.parse(responseBody.c_str(), parsedResponse);

    if (!parsingSuccessful)
    {
        ELOG(ERROR, "Failed to JSON parse.");
        return false;
    }

    if (!parsedResponse.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    ELOG(INFO, "Current Open Orders.");

    return true;
}