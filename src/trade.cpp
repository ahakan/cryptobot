#include "../inc/trade.h"
#include <string>


/**
 * @brief Construct a new Trade::Trade object
 * 
 * @param pU 
 */
Trade::Trade(std::shared_ptr<BinanceUtilities> pU, std::shared_ptr<BinanceClient> pR)
    : pBu(pU),
      pReq(pR)
{
    mOrderType              = pBu.get()->getType();
    mOrderQuantity          = pBu.get()->getQuantity();

    mAverageAmount          = pBu.get()->getAverageAmount();
    mAverageAutoCalculate   = pBu.get()->getAverageAutoCalculate();

    std::string interval            = pBu.get()->getInterval();
    std::string tradeSymbol         = pBu.get()->getTradeSymbol();
    std::string followSymbol        = pBu.get()->getFollowSymbol();
    std::string balanceSymbol       = pBu.get()->getBalanceSymbol();
    std::string balanceAmount       = pBu.get()->getBalanceAmount();
    std::string tradeCoinSymbol     = pBu.get()->getCoinSymbol(); 
    std::string algorithmInterval   = pBu.get()->getFollowInterval();

    int RSIPeriod                   = pBu.get()->getRSIPeriod();
    std::string RSIOversold         = pBu.get()->getRSIOversold();
    std::string RSIOverbought       = pBu.get()->getRSIOverbought();
    
    // Coin structs
    mTradeSymbolInfo.symbol         = tradeSymbol;
    mTradeSymbolInfo.coinName       = tradeCoinSymbol;

    mFollowSymbolInfo.symbol        = followSymbol;

    mBalanceSymbolInfo.coinName     = balanceSymbol;
    mBalanceSymbolInfo.coinQuantity = balanceAmount;

    // Candlesticks struct
    mTradeCandlesticks.symbol               = tradeSymbol;
    mTradeCandlesticks.interval             = interval;
    mTradeCandlesticks.RSIPeriod            = RSIPeriod;
    mTradeCandlesticks.RSIOversold          = RSIOversold;
    mTradeCandlesticks.RSIOverbought        = RSIOverbought;
    mTradeCandlesticks.candleDuration       = std::to_string(pBu.get()->getCandlestickDuration(1, interval));

    mFollowCandlesticks.symbol              = followSymbol;
    mFollowCandlesticks.interval            = interval;
    mFollowCandlesticks.RSIPeriod           = RSIPeriod;
    mFollowCandlesticks.RSIOversold         = RSIOversold;
    mFollowCandlesticks.RSIOverbought       = RSIOverbought;
    mFollowCandlesticks.candleDuration      = std::to_string(pBu.get()->getCandlestickDuration(1, interval));


    // Algorithm candlesticks struct
    mAlgorithmTradeCandlesticks.symbol          = tradeSymbol;
    mAlgorithmTradeCandlesticks.interval        = algorithmInterval;
    mAlgorithmTradeCandlesticks.RSIPeriod       = RSIPeriod;
    mAlgorithmTradeCandlesticks.RSIOversold     = RSIOversold;
    mAlgorithmTradeCandlesticks.RSIOverbought   = RSIOverbought;
    mAlgorithmTradeCandlesticks.candleDuration  = std::to_string(pBu.get()->getCandlestickDuration(1, algorithmInterval));

    mAlgorithmFollowCandlesticks.symbol         = followSymbol;
    mAlgorithmFollowCandlesticks.interval       = algorithmInterval;
    mAlgorithmFollowCandlesticks.RSIPeriod      = RSIPeriod;
    mAlgorithmFollowCandlesticks.RSIOversold    = RSIOversold;
    mAlgorithmFollowCandlesticks.RSIOverbought  = RSIOverbought;
    mAlgorithmFollowCandlesticks.candleDuration = std::to_string(pBu.get()->getCandlestickDuration(1, algorithmInterval));


    ELOG(INFO, "Trade constructor initialized. "
                "Trade Symbol: %s, Follow Symbol: %s, Interval: %s, "
                "Balance Symbol: %s, Balance Amount: %s.", 
                    mTradeSymbolInfo.symbol.c_str(), 
                    mFollowSymbolInfo.symbol.c_str(), 
                    mTradeCandlesticks.interval.c_str(), 
                    mBalanceSymbolInfo.coinName.c_str(), 
                    mBalanceSymbolInfo.coinQuantity.c_str());
}


/**
 * @brief Destroy the Trade::Trade object
 * 
 */
Trade::~Trade()
{
    ELOG(INFO, "Trade destructor.");
}


/**
 * @brief 
 * 
 */
void Trade::calculates()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        // Get WS candlesticks
        pBu.get()->getWSCandlesticks(mTradeCandlesticks, mTradeSymbolInfo);
        pBu.get()->getWSCandlesticks(mFollowCandlesticks, mFollowSymbolInfo);

        // Get algorithm candlesticks
        bool getAlgorithTradeCandlesticks   = getCandlesticks(mAlgorithmTradeCandlesticks);
        bool getAlgorithmFollowCandlesticks = getCandlesticks(mAlgorithmFollowCandlesticks);

        if (getAlgorithTradeCandlesticks && getAlgorithmFollowCandlesticks)
        {
            ELOG(INFO, "Got new algorithm trade and follow symbol candlesticks.");

            // Get daily volume
            pReq.get()->getDailyVolume(mTradeSymbolInfo);
            pReq.get()->getDailyVolume(mFollowSymbolInfo);
        }


        // pBu.get()->calcNewOrderAverage(mBuyOrder, mAlgorithmTradeCandlesticks);

        checkBuyOrder();

        checkStopOrder();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    ELOG(INFO, "Calculates -> detached.");
}


/**
 * @brief Get candlesticks data and calculate RSI and average
 * 
 * @param candlestick 
 * @return true 
 * @return false 
 */
bool Trade::getCandlesticks(struct Candlesticks &candlestick)
{
    candlestick.lock();

    std::string timestamp           = pBu.get()->getTimestamp();

    std::string nextCandleTimestamp = pBu.get()->atlts(candlestick.lastCandleTimestamp, 
                                                        candlestick.candleDuration);

    if (pBu.get()->ctscl(timestamp, nextCandleTimestamp))
    {
        // Add start time to candlesticks structs for candlesticks info
        candlestick.startTime       = pBu.get()->getRSITimestamp(candlestick.RSIPeriod, 
                                                                candlestick.interval);

        bool getCandles             = pReq.get()->getCandlesticksData(candlestick);

        ELOG(INFO, "Candlesticks -> %s(%s). Candle duration: %s, Timestamp: %s, Next: %s, Last: %s.", 
            candlestick.symbol.c_str(), 
            candlestick.interval.c_str(), 
            candlestick.candleDuration.c_str(),
            timestamp.c_str(), 
            nextCandleTimestamp.c_str(), 
            candlestick.lastCandleTimestamp.c_str());

        if (getCandles)
        {
            candlestick.isUpdated = true;

            pBu.get()->calculateRSI(candlestick);

            pBu.get()->calculateChange(candlestick);
        
            pBu.get()->calculateAverage(candlestick);

            pBu.get()->getHighestLowestPrice(candlestick);

            candlestick.unlock();

            return true;
        }
    }
        
    candlestick.unlock();

    return false;
}


/**
 * @brief Check buy orders
 * 
 * @return true 
 * @return false 
 */
bool Trade::checkBuyOrder()
{
    if (mBuyOrders.size() > 0)
    {
        // Erase canceled orders
        for (OrdersMapIterator order = mBuyOrders.begin(); order != mBuyOrders.end(); ++order)
        {
            // order->second->lock();

            if (order->second.get()->status == BINANCE_CANCELED)
            {
                mBuyOrders.erase(order);

                ELOG(INFO, "Erase -> %s/%s(%d). Status: %s, Price: %s, Quantity: %s.", 
                                order->second.get()->side.c_str(),
                                order->second.get()->type.c_str(),
                                order->second.get()->orderId,
                                order->second.get()->status.c_str(),
                                order->second.get()->price.c_str(),
                                order->second.get()->quantity.c_str());
            }

            // if create a stop loss order
            // delete buy order
            if (order->second.get()->type == BINANCE_STOP_LOSS_LIMIT)
            {
                mBuyOrders.erase(order);

                ELOG(INFO, "Erase -> %s/%s(%d). Status: %s, Price: %s, Quantity: %s.", 
                                order->second.get()->side.c_str(),
                                order->second.get()->type.c_str(),
                                order->second.get()->orderId,
                                order->second.get()->status.c_str(),
                                order->second.get()->price.c_str(),
                                order->second.get()->quantity.c_str());
            }

            // order->second->unlock();
        }

        // Check orders
        for (OrdersMapIterator order = mBuyOrders.begin(); order != mBuyOrders.end(); ++order)
        {
            order->second->lock();

            bool checkOrderQuery = pReq.get()->queryOrder(order->second);

            if (checkOrderQuery)
            {
                ELOG(INFO, "Check -> %s/%s(%d). Status: %s, Price: %s, Quantity: %s.", 
                            order->second.get()->side.c_str(),
                            order->second.get()->type.c_str(),
                            order->second.get()->orderId,
                            order->second.get()->status.c_str(),
                            order->second.get()->price.c_str(),
                            order->second.get()->quantity.c_str());


                if (order->second.get()->status != BINANCE_FILLED)
                {
                    bool checkOrderPrice = pBu.get()->checkBuyOrder(order->second,
                                                                    mTradeSymbolInfo,
                                                                    mTradeCandlesticks,
                                                                    mAlgorithmTradeCandlesticks);
                    
                    if (!checkOrderPrice)
                    {
                        bool cancelOrder = pReq.get()->cancelOrder(order->second);

                        if (cancelOrder)
                        {
                            ELOG(INFO, "Cancel -> %s/%s(%d). Status: %s, Price: %s, Quantity: %s.", 
                                        order->second.get()->side.c_str(),
                                        order->second.get()->type.c_str(),
                                        order->second.get()->orderId,
                                        order->second.get()->status.c_str(),
                                        order->second.get()->price.c_str(),
                                        order->second.get()->quantity.c_str());
                        }
                    }
                }

                // if buy order filled
                // we create a stop loss sell order
                if (order->second.get()->status == BINANCE_FILLED)
                {
                    // TO-DO: ADD COIN QUANTITY

                    createNewStopOrder(order->second);
                }
            }

            order->second->unlock();
        }

        return true;
    }

    return false;
}


/**
 * @brief Check sell orders
 * 
 * @return true 
 * @return false 
 */
bool Trade::checkSellOrder()
{
    if (mSellOrders.size() > 0)
    {
        // Check orders
        for (OrdersMapIterator order = mSellOrders.begin(); order != mSellOrders.end(); ++order)
        {
            order->second->lock();

            bool checkOrderQuery = pReq.get()->queryOrder(order->second);

            if (checkOrderQuery)
            {
                ELOG(INFO, "Check -> %s/%s(%d). Status: %s, Price: %s, Stop Price: %s, "
                            "Bought Price: %s, Quantity: %s.", 
                            order->second.get()->side.c_str(),
                            order->second.get()->type.c_str(),
                            order->second.get()->orderId,
                            order->second.get()->status.c_str(),
                            order->second.get()->price.c_str(),
                            order->second.get()->stopPrice.c_str(),
                            order->second.get()->boughtPrice.c_str(),
                            order->second.get()->quantity.c_str());


                if (order->second.get()->status == BINANCE_FILLED)
                {
                    mSellOrders.erase(order);

                    ELOG(INFO, "Erase -> %s/%s(%d). Status: %s, Price: %s, Stop Price: %s, Quantity: %s.", 
                                    order->second.get()->side.c_str(),
                                    order->second.get()->type.c_str(),
                                    order->second.get()->orderId,
                                    order->second.get()->status.c_str(),
                                    order->second.get()->price.c_str(),
                                    order->second.get()->stopPrice.c_str(),
                                    order->second.get()->quantity.c_str());
                }
            }

            order->second->unlock();
        }
    }
    return true;
}


/**
 * @brief Check stop orders
 * 
 * @return true 
 * @return false 
 */
bool Trade::checkStopOrder()
{
    if (mStopLossOrders.size() > 0)
    {
        // Erase canceled orders
        for (OrdersMapIterator order = mStopLossOrders.begin(); order != mStopLossOrders.end(); ++order)
        {
            if (order->second.get()->status == BINANCE_CANCELED)
            {
                mStopLossOrders.erase(order);

                ELOG(INFO, "Erase -> %s/%s(%d). Status: %s, Price: %s, Stop Price: %s, Quantity: %s.", 
                                order->second.get()->side.c_str(),
                                order->second.get()->type.c_str(),
                                order->second.get()->orderId,
                                order->second.get()->status.c_str(),
                                order->second.get()->price.c_str(),
                                order->second.get()->stopPrice.c_str(),
                                order->second.get()->quantity.c_str());
            }

            if (order->second.get()->status == BINANCE_FILLED)
            {
                mStopLossOrders.erase(order);

                ELOG(INFO, "Erase -> %s/%s(%d). Status: %s, Price: %s, Stop Price: %s, Quantity: %s.", 
                                order->second.get()->side.c_str(),
                                order->second.get()->type.c_str(),
                                order->second.get()->orderId,
                                order->second.get()->status.c_str(),
                                order->second.get()->price.c_str(),
                                order->second.get()->stopPrice.c_str(),
                                order->second.get()->quantity.c_str());
            }
        }

        // Check orders
        for (OrdersMapIterator order = mStopLossOrders.begin(); order != mStopLossOrders.end(); ++order)
        {
            order->second->lock();

            bool checkOrderQuery = pReq.get()->queryOrder(order->second);

            if (checkOrderQuery)
            {
                ELOG(INFO, "Check -> %s/%s(%d). Status: %s, Price: %s, Stop Price: %s, "
                            "Bought Price: %s, Quantity: %s.", 
                            order->second.get()->side.c_str(),
                            order->second.get()->type.c_str(),
                            order->second.get()->orderId,
                            order->second.get()->status.c_str(),
                            order->second.get()->price.c_str(),
                            order->second.get()->stopPrice.c_str(),
                            order->second.get()->boughtPrice.c_str(),
                            order->second.get()->quantity.c_str());

                if (order->second.get()->status != BINANCE_FILLED)
                {
                    // if order price is okay
                    // we will cancel stop loss order
                    // and we create sell order
                    bool checkStopOrder = pBu.get()->checkStopOrder(order->second,
                                                                    mTradeSymbolInfo,
                                                                    mTradeCandlesticks,
                                                                    mAlgorithmTradeCandlesticks);
                    
                    if (checkStopOrder)
                    {
                        bool cancelOrder = pReq.get()->cancelOrder(order->second);

                        if (cancelOrder)
                        {
                            ELOG(INFO, "Cancel -> %s/%s(%d). Status: %s, Price: %s, Quantity: %s.", 
                                        order->second.get()->side.c_str(),
                                        order->second.get()->type.c_str(),
                                        order->second.get()->orderId,
                                        order->second.get()->status.c_str(),
                                        order->second.get()->stopPrice.c_str(),
                                        order->second.get()->quantity.c_str());

                            createNewSellOrder(order->second);
                        }
                    }

                }
            }

            order->second->unlock();
        }
    }
    return true;
}


/**
 * @brief Create a new buy order
 * 
 * @return true 
 * @return false 
 */
bool Trade::createNewBuyOrder()
{
    if (mBuyOrders.size() < 1)
    {
        std::shared_ptr<Order> newOrder(new Order());

        newOrder->side      = BINANCE_BUY;
        newOrder->type      = mOrderType;
        newOrder->symbol    = mTradeSymbolInfo.symbol;
        newOrder->quantity  = mOrderQuantity;

        // calculate new buy price
        bool calcBuyPrice   = pBu.get()->calcNewBuyPrice(newOrder, 
                                                        mTradeSymbolInfo,
                                                        mTradeCandlesticks, 
                                                        mAlgorithmTradeCandlesticks);

        if (calcBuyPrice)
        {
            // check user wallet balance
            bool walletBalanceAmount    = pReq.get()->getCoinBalance(mBalanceSymbolInfo);
            
            if (walletBalanceAmount)
            {
                bool isCreated  = pReq.get()->createNewOrder(newOrder, mTradeSymbolInfo);

                if (isCreated)
                {
                    mBuyOrders.emplace(newOrder->orderId, newOrder);

                    ELOG(INFO, "New Order -> %s/%s(%d). Price: %s, Quantity: %s, Expected Average: %s.", 
                                newOrder->side.c_str(),
                                newOrder->type.c_str(),
                                newOrder->orderId,
                                newOrder->price.c_str(),
                                newOrder->quantity.c_str(),
                                newOrder->expectedAverage.c_str());

                    return true;
                }
            }
        }

        return false;
    }

    return false;
}


/**
 * @brief Create a new sell order
 * 
 * @param order 
 * @return true Order created
 * @return false Order didn't create
 */
bool Trade::createNewSellOrder(std::shared_ptr<Order> order)
{
    // check user wallet balance
    // bool walletCoinAmount = pReq.get()->getCoinBalance(mTradeSymbolInfo);

    // if (walletCoinAmount)
    // {
        std::shared_ptr<Order> newOrder(new Order());

        newOrder->side          = BINANCE_SELL;
        newOrder->type          = BINANCE_MARKET;
        newOrder->symbol        = order.get()->symbol;
        newOrder->quantity      = order.get()->quantity;
        newOrder->boughtPrice   = order.get()->boughtPrice;
        newOrder->stopPrice     = order.get()->stopPrice;

        // calculate new sell price
        bool calcSellPrice      = pBu.get()->calcNewSellPrice(newOrder, 
                                                            mTradeSymbolInfo, 
                                                            mAlgorithmTradeCandlesticks);
        
        if (calcSellPrice)
        {
            bool isOrderCreated = pReq.get()->createNewOrder(newOrder, mTradeSymbolInfo);

            if (isOrderCreated)
            {
                mSellOrders.emplace(newOrder->orderId, newOrder);

                ELOG(INFO, "New Order -> %s/%s(%d). Price: %s, Quantity: %s, Expected Average: %s.", 
                            newOrder->side.c_str(),
                            newOrder->type.c_str(),
                            newOrder->orderId,
                            newOrder->price.c_str(),
                            newOrder->quantity.c_str(),
                            newOrder->expectedAverage.c_str());


                order.get()->type = BINANCE_LIMIT;

                return true;
            }
        }
    // }

    order.get()->type = BINANCE_LIMIT;

    return false;
}


/**
 * @brief Create a new sell stop order
 * 
 * @param order Buy Order struct
 * @return true Order created
 * @return false Order didn't create
 */
bool Trade::createNewStopOrder(std::shared_ptr<Order> order)
{
    // check user wallet balance
    // bool walletCoinAmount = pReq.get()->getCoinBalance(mTradeSymbolInfo);

    // if (walletCoinAmount)
    // {
        std::shared_ptr<Order> newOrder(new Order());

        newOrder->side          = BINANCE_SELL;
        newOrder->type          = BINANCE_STOP_LOSS_LIMIT;
        newOrder->symbol        = order.get()->symbol;
        newOrder->quantity      = order.get()->executedQty;
        newOrder->boughtPrice   = order.get()->boughtPrice;

        // calculate new stop price
        bool calcStopPrice      = pBu.get()->calcNewStopPrice(newOrder, 
                                                            mTradeSymbolInfo, 
                                                            mAlgorithmTradeCandlesticks);
        
        if (calcStopPrice)
        {
            bool isOrderCreated = pReq.get()->createNewOrder(newOrder, mTradeSymbolInfo);

            if (isOrderCreated)
            {
                mStopLossOrders.emplace(newOrder->orderId, newOrder);

                ELOG(INFO, "New Order -> %s/%s(%d). Price: %s, Quantity: %s, Expected Average: %s.", 
                            newOrder->side.c_str(),
                            newOrder->type.c_str(),
                            newOrder->orderId,
                            newOrder->price.c_str(),
                            newOrder->quantity.c_str(),
                            newOrder->expectedAverage.c_str());


                order.get()->type = BINANCE_STOP_LOSS_LIMIT;

                return true;
            }
        }
    // }

    order.get()->type = BINANCE_STOP_LOSS_LIMIT;

    return false;
}



/**
 * @brief Construct a new BinanceTrade::BinanceTrade object
 * 
 * @param pU 
 * @param pR 
 */
BinanceTrade::BinanceTrade(std::shared_ptr<BinanceUtilities> pU, std::shared_ptr<BinanceClient> pR)
    : Trade(pU, pR),
      pReq(pR)
{
    ELOG(INFO, "BinanceTrade constructor.");
}


/**
 * @brief Destroy the BinanceTrade::BinanceTrade object
 * 
 */
BinanceTrade::~BinanceTrade()
{
    ELOG(INFO, "BinanceTrade destructor.");
}


/**
 * @brief Trade initializing
 * 
 */
void BinanceTrade::init()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        while (!mAccountStatus && pOpel->getExitSignal())
        {
            mAccountStatus          = pReq.get()->getAccountStatus();

            if (!mAccountStatus)
            {
                ELOG(ERROR, "User account status is not normal. Can't enter main loop.");
            }

            if (mAccountStatus)
            {
                mAPIKeyPermission   = pReq.get()->getAPIKeyPermission();

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

        if (pOpel->getExitSignal())
        {
            bool  getSymbolTickSize = pReq.get()->getTickSize(mTradeSymbolInfo);
            bool  getFollowTickSize = pReq.get()->getTickSize(mFollowSymbolInfo);

            if (getSymbolTickSize && getFollowTickSize)
            {
                // Add tick size to candlesticks structs
                mTradeCandlesticks.tickSize     = mTradeSymbolInfo.tickSize;
                mBalanceSymbolInfo.tickSize     = mTradeSymbolInfo.tickSize;
                mFollowCandlesticks.tickSize    = mFollowSymbolInfo.tickSize;
                
                mAlgorithmTradeCandlesticks.tickSize    = mTradeSymbolInfo.tickSize;
                mAlgorithmFollowCandlesticks.tickSize   = mFollowSymbolInfo.tickSize;

                bool getWalletBalance           = pReq.get()->getCoinBalance(mBalanceSymbolInfo);

                if (getWalletBalance)
                {
                    ELOG(INFO, "Quantity -> %s: %s.", 
                            mBalanceSymbolInfo.coinName.c_str(), 
                            mBalanceSymbolInfo.coinQuantity.c_str());

                    // Get algorithm candlesticks
                    bool getTradeCandlesticks   = getCandlesticks(mTradeCandlesticks);
                    bool getFollowCandlesticks  = getCandlesticks(mFollowCandlesticks);

                    if (getTradeCandlesticks && getFollowCandlesticks)
                    {
                        // Get daily volume
                        bool getTradeVolume     = pReq.get()->getDailyVolume(mTradeSymbolInfo);
                        bool getFollowVolume    = pReq.get()->getDailyVolume(mFollowSymbolInfo);

                        if (getTradeVolume && getFollowVolume)
                        {
                            break;
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ELOG(INFO, "Trade -> inialized.");
    ELOG(INFO, "Trade -> starting...");

    std::thread tradeTh1    = std::thread(&BinanceTrade::calculates, this);
    std::thread tradeTh2    = std::thread(&BinanceTrade::requests, this);

    tradeTh1.join();
    tradeTh2.join();

    Opel *iOpel = Opel::instance();

    iOpel->setIsActive(0);

    iOpel->setExitSignal(0);

    ELOG(INFO, "Trade -> detached.");
}


/**
 * @brief 
 * 
 */
void BinanceTrade::requests()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        while(pOpel->getIsActive())
        {
            if (mTradeSymbolInfo.price.size() > 0)
            {
                createNewBuyOrder();

                // createNewSellOrder();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    pReq.get()->cancelAllOpenOrders(mTradeSymbolInfo.symbol);

    ELOG(INFO, "Requests -> detached.");
}



