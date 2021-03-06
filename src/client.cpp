#include "../inc/client.h"


/**
 * @brief Construct a new Client::Client object
 * 
 * @param pBu 
 */
Client::Client(std::shared_ptr<BinanceUtilities> pBu)
{
    this->pBu               = pBu;
    mBase                   = pBu.get()->getAPIBase();
    mAPI_KEY                = pBu.get()->getAPIKEY();
    mSECRET_KEY             = pBu.get()->getAPISECRETKEY();

    mInterval               = pBu.get()->getInterval();
    mQuantity               = pBu.get()->getQuantity();
    mOrderType              = pBu.get()->getType();
    mCoinSymbol             = pBu.get()->getCoinSymbol(); 
    mTradeSymbol            = pBu.get()->getTradeSymbol();
    mFollowSymbol           = pBu.get()->getFollowSymbol();
    mBalanceSymbol          = pBu.get()->getBalanceSymbol();
    mBalanceAmount          = pBu.get()->getBalanceAmount();
    mAverageAmount          = pBu.get()->getAverageAmount();
    mAverageAutoCalculate   = pBu.get()->getAverageAutoCalculate();

    mRSIPeriod              = pBu.get()->getRSIPeriod();
    mRSIOversold            = pBu.get()->getRSIOversold();
    mRSIOverbought          = pBu.get()->getRSIOverbought();


    Opel *iOpel             = Opel::instance();

    iOpel->setBuyOrdersMap(&mBuyOrders);
    iOpel->setBoughtOrdersMap(&mBoughtOrders);
    iOpel->setSellOrdersMap(&mSellOrders);
    iOpel->setSoldOrdersMap(&mSoldOrders);

    ELOG(INFO, "Client constructor initialized. mTradeSymbol: %s, mFollowSymbol: %s, mInterval: %s, mBalanceSymbol: %s, mBalanceAmount: %s.", mTradeSymbol.c_str(), mFollowSymbol.c_str(), mInterval.c_str(), mBalanceSymbol.c_str(), mBalanceAmount.c_str());
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
 * @brief Calculate new sell order price
 * 
 * @param boughtPrice 
 * @return std::string 
 */
std::string Client::calcNewSellPrice(std::string boughtPrice)
{
    if (boughtPrice.length() > 0)
    {
        bool isSellAverageCalculated    = calcOrderPriceAverage();

        if (!isSellAverageCalculated)
        {
            ELOG(ERROR, "Failed to Calculated New Sell Price Average.");
        }

        bool compareLiveAndBoughtPrice  = pBu.get()->compareTwoStrings(mSymbolLivePrice, boughtPrice);     // if return true live price is high, return false bought price is high
        
        if (compareLiveAndBoughtPrice)
        {
            std::string calculatedPrice = pBu.get()->addTwoStrings(mSymbolLivePrice, mNewOrderCalculatedAverage);

            ELOG(INFO, "Calculated New Sell Price. Live Price: %s, Bought Price: %s, Sell Price: %s.", mSymbolLivePrice.c_str(), boughtPrice.c_str(), calculatedPrice.c_str());

            return pBu.get()->roundString(calculatedPrice, mSymbolTickSize);
        }

        std::string calculatedPrice     = pBu.get()->addTwoStrings(boughtPrice, mNewOrderCalculatedAverage);

        ELOG(INFO, "Calculated New Sell Price. Bought Price: %s, Sell Price: %s, Bought Price: %s.", boughtPrice.c_str(), calculatedPrice.c_str(), boughtPrice.c_str());

        return pBu.get()->roundString(calculatedPrice, mSymbolTickSize);
    }

    return "ERROR";
}


/**
 * @brief Calculate new buy order price
 * 
 * @return std::string 
 */
std::string Client::calcNewBuyPrice()
{
    bool isBuyAverageCalculated    = calcOrderPriceAverage();

    if (!isBuyAverageCalculated)
    {
        ELOG(ERROR, "Failed to Calculated New Buy Price Average.");
    }

    std::string calculatedPrice = pBu.get()->subTwoStrings(mSymbolLivePrice, mNewOrderCalculatedAverage);

    ELOG(INFO, "Calculate New Buy Price. Live Price: %s, Calculated Average: %s, Buy Price: %s.", mSymbolLivePrice.c_str(), mNewOrderCalculatedAverage.c_str(), calculatedPrice.c_str());

    return pBu.get()->roundString(calculatedPrice, mSymbolTickSize);
}


/**
 * @brief Calculate new balance amount
 * 
 * @param side 
 * @param price 
 * @param quantity 
 * @return true 
 * @return false 
 */
bool Client::calcNewBalanceAmount(std::string side, std::string price, std::string quantity)
{
    std::string totalOrderPrice     = pBu.get()->multiplyTwoStrings(price, quantity);
    std::string roundedTotalPrice   = pBu.get()->roundString(totalOrderPrice, mSymbolTickSize);

    if (side == "BUY")
    {
        mBalanceAmount = pBu.get()->subTwoStrings(mBalanceAmount, roundedTotalPrice);

        mBalanceAmount = pBu.get()->roundString(mBalanceAmount, mSymbolTickSize);

        ELOG(INFO, "Calculated New Balance Amount. New Amount: %s, Order Price: %s.", mBalanceAmount.c_str(), roundedTotalPrice.c_str());

        return true;
    }
    else if (side == "SELL")
    {
        mBalanceAmount = pBu.get()->addTwoStrings(mBalanceAmount, roundedTotalPrice);

        mBalanceAmount = pBu.get()->roundString(mBalanceAmount, mSymbolTickSize);

        ELOG(INFO, "Calculated New Balance Amount. New Amount: %s, Order Price: %s.", mBalanceAmount.c_str(), roundedTotalPrice.c_str());

        return true;
    }

    ELOG(INFO, "Failed to Calculate New Balance Amount.");

    return false;
}


/**
 * @brief Calculate new order price average
 * 
 * @return true 
 * @return false 
 */
bool Client::calcOrderPriceAverage()
{
    if (mAverageAutoCalculate)
    {
        float highestPrice          = std::stof(*std::max_element(mTradeCandlesHighPrices.begin(), mTradeCandlesHighPrices.end()));
        float lowestPrice           = std::stof(*std::min_element(mTradeCandlesLowPrices.begin(), mTradeCandlesLowPrices.end()));

        float calculatedAverage     = (highestPrice-lowestPrice)/mRSIPeriod;

        mNewOrderCalculatedAverage  = pBu.get()->roundString(std::to_string(calculatedAverage), mSymbolTickSize); 

        ELOG(INFO, "Calculated New Trade Average. New Average: %s.", mNewOrderCalculatedAverage.c_str());
        
        return true;
    }

    if (!mAverageAutoCalculate)
    {
        mNewOrderCalculatedAverage = mAverageAmount;

        ELOG(INFO, "Average Auto Calculated Disable. Average: %s.", mNewOrderCalculatedAverage.c_str());

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
bool Client::calcSymbolRSI()
{
    if (mTradeCandlesClosePrices.size() != 0)
    {
        mOldTradeCandlesCloseRSI            = mTradeCandlesCloseRSI.length() > 0 ? mTradeCandlesCloseRSI : "00.00";
        mTradeCandlesCloseRSI               = pBu.get()->calculateRSI(mTradeCandlesClosePrices);

        mBuyOrdersNewTradeRSI               = mOldTradeCandlesCloseRSI == "00.00" ? false : true;   // ignore to first calculation for cancel order
        mSellOrdersNewTradeRSI              = mOldTradeCandlesCloseRSI == "00.00" ? false : true;   // ignore to first calculation for cancel order

        ELOG(INFO, "Calculated Symbol RSI. Trade Candles New Close RSI: %s, Old Close RSI: %s.", mTradeCandlesCloseRSI.c_str(), mOldTradeCandlesCloseRSI.c_str());

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
bool Client::calcFollowRSI()
{
    if (mFollowCandlesClosePrices.size() != 0)
    {
        mOldFollowCandlesCloseRSI           = mFollowCandlesCloseRSI.length() > 0 ? mFollowCandlesCloseRSI : "00.00";
        mFollowCandlesCloseRSI              = pBu.get()->calculateRSI(mFollowCandlesClosePrices);

        mBuyOrdersNewFollowRSI              = mOldFollowCandlesCloseRSI == "00.00" ? false : true;   // ignore to first calculation for cancel order
        mSellOrdersNewFollowRSI             = mOldFollowCandlesCloseRSI == "00.00" ? false : true;   // ignore to first calculation for cancel order


        ELOG(INFO, "Calculated Follow Symbol RSI. Follow Candles New Close RSI: %s, Old Close RSI: %s.", mFollowCandlesCloseRSI.c_str(), mOldFollowCandlesCloseRSI.c_str());

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
bool Client::calcSymbolAverages()
{
    if (mTradeCandlesClosePrices.size() != 0)
    {
        mTradeCandlesOpenPricesAverage      = pBu.get()->calculateAverage(mTradeCandlesOpenPrices);
        mTradeCandlesHighPricesAverage      = pBu.get()->calculateAverage(mTradeCandlesHighPrices);
        mTradeCandlesLowPricesAverage       = pBu.get()->calculateAverage(mTradeCandlesLowPrices);
        mTradeCandlesClosePricesAverage     = pBu.get()->calculateAverage(mTradeCandlesClosePrices);

        ELOG(INFO, "Calculated Symbol Averages. Trade Close Average: %s.", mTradeCandlesClosePricesAverage.c_str());

        mTradeCandlesOpenPricesAverage      = pBu.get()->roundString(mTradeCandlesOpenPricesAverage, mSymbolTickSize);
        mTradeCandlesHighPricesAverage      = pBu.get()->roundString(mTradeCandlesHighPricesAverage, mSymbolTickSize);
        mTradeCandlesLowPricesAverage       = pBu.get()->roundString(mTradeCandlesLowPricesAverage, mSymbolTickSize);
        mTradeCandlesClosePricesAverage     = pBu.get()->roundString(mTradeCandlesClosePricesAverage, mSymbolTickSize);

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
bool Client::calcFollowAverages()
{
    if (mFollowCandlesClosePrices.size() != 0)
    {
        mFollowCandlesOpenPricesAverage     = pBu.get()->calculateAverage(mFollowCandlesOpenPrices);
        mFollowCandlesHighPricesAverage     = pBu.get()->calculateAverage(mFollowCandlesHighPrices);
        mFollowCandlesLowPricesAverage      = pBu.get()->calculateAverage(mFollowCandlesLowPrices);
        mFollowCandlesClosePricesAverage    = pBu.get()->calculateAverage(mFollowCandlesClosePrices);

        ELOG(INFO, "Calculated Follow Symbol Averages. Follow Close Average: %s.", mFollowCandlesClosePricesAverage.c_str());

        mFollowCandlesOpenPricesAverage     = pBu.get()->roundString(mFollowCandlesOpenPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesHighPricesAverage     = pBu.get()->roundString(mFollowCandlesHighPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesLowPricesAverage      = pBu.get()->roundString(mFollowCandlesLowPricesAverage, mFollowSymbolTickSize);
        mFollowCandlesClosePricesAverage    = pBu.get()->roundString(mFollowCandlesClosePricesAverage, mFollowSymbolTickSize);

        ELOG(INFO, "Rounded Follow Averages. FOA: %s, FHA: %s, FLA: %s, FCA: %s", mFollowCandlesOpenPricesAverage.c_str(), mFollowCandlesHighPricesAverage.c_str(), mFollowCandlesLowPricesAverage.c_str(), mFollowCandlesClosePricesAverage.c_str());

        return true;
    }

    ELOG(ERROR, "Failed to Calculate Follow Symbol Averages.");

    return false;
}


/**
 * @brief Calculate new buy order with some parameters
 * 
 * @param price 
 * @return true 
 * @return false 
 */
bool Client::calcNewBuyOrder(std::string price)
{
    // if RSI is Less than mRSIOversold, we create a new buy order
    bool isNewTradeRSILessOversold      = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOversold);
    bool isNewFollowRSILessOversold     = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mRSIOversold);

    if (!isNewTradeRSILessOversold && !isNewFollowRSILessOversold)
    {
        // calculate total order price
        std::string totalOrderPrice     = pBu.get()->multiplyTwoStrings(price, mQuantity);

        // check balance amount
        std::string orderPrice          = pBu.get()->roundString(totalOrderPrice, mSymbolTickSize);

        // compare balance amount and order price 
        bool compareBalanceOrderPrice   = pBu.get()->compareTwoStrings(mBalanceAmount, orderPrice);

        if (compareBalanceOrderPrice)
        {
            return true;
        }
        else
        {
            ELOG(WARNING, "Insufficient user balance. Balance: %s. ", mBalanceAmount.c_str());
        }
    }

    return false;
}


/**
 * @brief Calcuate new sell order with some parameters
 * 
 * @return true 
 * @return false 
 */
bool Client::calcNewSellOrder()
{
    // if RSI is higher than mRSIOverbought, we create a new sell order
    bool isNewTradeRSIHighOverbought    = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOverbought);
    bool isNewFollowRSIHighOverbought   = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mRSIOverbought);

    if (isNewTradeRSIHighOverbought && isNewFollowRSIHighOverbought)
    {
        return true;
    }

    return false;
}


/**
 * @brief Check candle data struct
 * 
 * @return true 
 * @return false 
 */
bool Client::readCandleData()
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
                bool mCalculateSymbolRSI            = calcSymbolRSI();
                bool mCalculateOrderPriceAverage    = calcOrderPriceAverage();

                if (mCalculateSymbolRSI && mCalculateOrderPriceAverage)
                {
                    ELOG(INFO, "Added new symbol closed price data, RSI and order price average. Calculated Average: %s.", mNewOrderCalculatedAverage.c_str());
                }
            }
        }
        else
        {
            mSymbolLivePrice        = pTradeCandleData->closePrice;

            mSymbolLivePriceRead    = true;

            ELOG(INFO, "Current Trade Symbol: %s, Live Price: %s.", pTradeCandleData->symbol.c_str(), mSymbolLivePrice.c_str());
        }

        pTradeCandleData->isUpdated = false;

        ELOG(INFO, "BuyOrderSize: %d, BoughtOrderSize: %d, SellOrderSize: %d, SoldOrderSize: %d.", mBuyOrders.size(), mBoughtOrders.size(), mSellOrders.size(), mSoldOrders.size());
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
                bool mCalculateFollowRSI        = calcFollowRSI();

                if (mCalculateFollowRSI)
                {
                    ELOG(INFO, "Added new follow symbol closed price data and RSI.");
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
bool Client::addClosedCandlePrices(std::string symbol, std::string open, std::string close, std::string high, std::string low)
{
    if (symbol==mTradeSymbol)
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
 * @brief Construct a new BinanceClient::BinanceClient object
 * 
 * @param pBu 
 */
BinanceClient::BinanceClient(std::shared_ptr<BinanceUtilities> pBu)
    : Client(pBu)
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
 * @brief Client initializing.
 * 
 * @param candle 
 */
void BinanceClient::init()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        while (!mAccountStatus && pOpel->getExitSignal())
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

        if (!mAccountStatus || !mAPIKeyPermission)
        {
            break;
        }

        if (pOpel->getExitSignal())
        {
            bool  getSymbolTickSize = getTickSize(mTradeSymbol);
            bool  getFollowTickSize = getTickSize(mFollowSymbol);

            if (getSymbolTickSize && getFollowTickSize)
            {
                bool getTradeSymbolCandles     = getCandlesticksData(mTradeSymbol, mInterval, pBu.get()->getRSITimestamp(mRSIPeriod, mInterval));
                bool getFollowSymbolCandles    = getCandlesticksData(mFollowSymbol, mInterval, pBu.get()->getRSITimestamp(mRSIPeriod, mInterval));

                if (getTradeSymbolCandles && getFollowSymbolCandles)
                {
                    bool calculateSymbolRSI         = calcSymbolRSI();

                    bool calculateFollowRSI         = calcFollowRSI();

                    if (calculateSymbolRSI && calculateFollowRSI)
                    {
                        bool getWalletBalance       = getCoinBalance(mBalanceSymbol);

                        if (getWalletBalance)
                        {
                            bool getTradeDailySymbol    = getDailyVolume(mTradeSymbol);
                            bool getFollowDailySymbol   = getDailyVolume(mFollowSymbol);

                            if (getTradeDailySymbol && getFollowDailySymbol) 
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (pOpel->getExitSignal())
    {
        ELOG(INFO, "Checked User Account and Wallet. Calculated Averages and RSI. Starting Trade...");

        binance();
    }

    ELOG(INFO, "Thread Client detached.");
}


/**
 * @brief Binance main loop
 * 
 */
void BinanceClient::binance()
{
    // Check bought orders
    // if we have old bought orders
    // we check and we sell
    checkBoughtOrders();

    // Main loop
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        while(pOpel->getIsActive())
        {
            if (!pOpel->getExitSignal())
                break;

            readCandleData();

            if (mSymbolLivePrice.length() > 0)
            {

                if (mSymbolLivePriceRead)
                {
                    checkBuyOrders();
                    checkSellOrders();

                    mSymbolLivePriceRead = false;
                }
                
                newBuyOrder();
                newSellOrder();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    // Cancel buy orders
    while (mBuyOrders.size() > 0)
    {
        for (MapIterator i = mBuyOrders.begin(); i != mBuyOrders.end(); ++i)
        {
            cancelOrder(i->second["Symbol"], i->first);
        }

        // Remove canceled orders
        for (MapIterator i = mBuyOrders.begin(); i != mBuyOrders.end(); ++i)
        {
            if (i->second["Status"] == "CANCELED")
            {
                ELOG(INFO, "Removed Canceled Buy Order. Order id: %d, Status: %s.", i->first, i->second["Status"].c_str());

                mBuyOrders.erase(i);
            }
        }
    }

    // Cancel sell orders
    // while (mSellOrders.size() > 0)
    // {
    //     for (MapIterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
    //     {
    //         cancelOrder(i->second["Symbol"], i->first);
    //     }

    //     // Remove canceled orders
    //     for (MapIterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
    //     {
    //         if (i->second["Status"] == "CANCELED")
    //         {
    //             ELOG(INFO, "Removed Canceled Sell Order. Order id: %d, Status: %s.", i->first, i->second["Status"].c_str());

    //             mSellOrders.erase(i);
    //         }
    //     }
    // }
}


/**
 * @brief Create a new buy order
 * 
 * @return true 
 * @return false 
 */
bool BinanceClient::newBuyOrder()
{
    // if we have not a buy order we create a new buy order
    if (mBuyOrders.size() < 1)
    {
        // check user wallet balance
        bool walletBalanceAmount    = getCoinBalance(mBalanceSymbol);

        if (walletBalanceAmount)
        {
            if (mOrderType == "LIMIT")
            {
                // calculate new buy price
                std::string buyPrice        = calcNewBuyPrice();

                // calculate new buy order
                bool isCalcNewBuyOrder      = calcNewBuyOrder(buyPrice);

                if (isCalcNewBuyOrder)
                {
                    bool isCreated          = createNewOrder(mTradeSymbol, mBuySide, mOrderType, mQuantity, buyPrice);

                    if (isCreated)
                    {
                        ELOG(INFO, "Created a New Limit BUY Order. User balance amount is sufficient. Price: %s. ", buyPrice.c_str());

                        return true;
                    }
                }
            }

            if (mOrderType == "STOP_LOSS_LIMIT")
            {
                // calculate new buy price
                std::string stopPrice       = calcNewBuyPrice();

                // calculate new buy order
                bool isCalcNewBuyOrder      = calcNewBuyOrder(stopPrice);

                if (isCalcNewBuyOrder)
                {
                    bool isCreated          = createNewOrder(mTradeSymbol, mBuySide, mOrderType, mQuantity, stopPrice);

                    if (isCreated)
                    {
                        ELOG(INFO, "Created a New Stop Loss BUY Order. User balance amount is sufficient. Price: %s. ", stopPrice.c_str());

                        return true;
                    }
                }
            }
        }
    }

    return false;
}


/**
 * @brief Create a new sell order
 * 
 * @return true 
 * @return false 
 */
bool BinanceClient::newSellOrder()
{
    // if we bought a coin we'll create a sell order
    if (mBoughtOrders.size() > 0)
    {
        bool isCalcNewSellOrder = calcNewSellOrder();

        if (isCalcNewSellOrder)
        {
            // calculate new sell price
            std::string newPrice    = calcNewSellPrice(mBoughtOrders.begin()->second["BoughtPrice"]);

            bool isCreated          = createNewOrder(mTradeSymbol, mSellSide, mOrderType, mBoughtOrders.begin()->second["Quantity"], newPrice);

            if (isCreated)
            {
                ELOG(INFO, "Created a New SELL Order. RSI: %s, Price: %s.", mTradeCandlesCloseRSI.c_str(), newPrice.c_str());

                return true;
            }
        }
    }

    return false;
}


/**
 * @brief Check buy orders
 * 
 * @return true 
 * @return false 
 */
bool BinanceClient::checkBuyOrders()
{
    // Check buy orders
    if (mBuyOrders.size() > 0)
    {
        // Check new RSI
        if (mBuyOrdersNewTradeRSI && mBuyOrdersNewFollowRSI)
        {
            // if new rsi is less than oversold rsi it returns true.
            bool isNewTradeRSILessOversold      = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOversold);
            bool isNewFollowRSILessOversold     = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mRSIOversold);

            if (!isNewTradeRSILessOversold && !isNewFollowRSILessOversold)
            {
                // if new rsi higher than old rsi it returns true.
                bool isNewTradeRSIHigh          = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mOldTradeCandlesCloseRSI);
                bool isNewFollowRSIHigh         = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mOldFollowCandlesCloseRSI);

                if (!isNewTradeRSIHigh && !isNewFollowRSIHigh)
                {
                    int orderId = mBuyOrders.begin()->first;

                    cancelOrder(mTradeSymbol, orderId);

                    ELOG(INFO, "New RSI is higher than old RSI. Canceled Buy Order. Order id: %d. ", orderId);
                }
            }

            mBuyOrdersNewTradeRSI     = false;
            mBuyOrdersNewFollowRSI    = false;
        }

        bool checkOrder = queryOrder(mTradeSymbol, mBuyOrders.begin()->first);

        if (checkOrder)
        {
            ELOG(INFO, "Checked Buy Order. Order id: %d.", mBuyOrders.begin()->first);

            return true;
        }
    }

    return false;
}


/**
 * @brief Check sell orders
 * 
 * @return true 
 * @return false 
 */
bool BinanceClient::checkSellOrders()
{
    // Check sell orders
    if (mSellOrders.size() > 0)
    {
        // Cancel to filled or canceled status orders
        for (MapIterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
        {
            int orderId = i->first;

            if (i->second["Status"] == "FILLED")
            {
                ELOG(INFO, "Removed Filled Sell Order. Order id: %d, Status: %s, BoughtPrice: %s, SoldPrice: %s.", orderId, i->second["Status"].c_str(), i->second["BoughtPrice"].c_str(), i->second["SoldPrice"].c_str());
                
                mSellOrders.erase(i);
            }
            else if (i->second["Status"] == "CANCELED")
            {
                ELOG(INFO, "Removed Canceled Sell Order. Order id: %d, Status: %s, BoughtPrice: %s.", orderId, i->second["Status"].c_str(), i->second["BoughtPrice"].c_str());

                mSellOrders.erase(i);
            }
        }

        // Check new RSI and cancel all order
        if (mSellOrdersNewTradeRSI && mSellOrdersNewFollowRSI)
        {
            // if new rsi is higher than overbought rsi it returns true.
            bool isNewTradeRSIHighOverbought    = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOverbought);
            bool isNewFollowRSIHighOverbought   = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mRSIOverbought);

            if (isNewTradeRSIHighOverbought && isNewFollowRSIHighOverbought)
            {
                // if new rsi is higher than old rsi it returns true.
                bool isNewTradeRSIHigh          = pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mOldTradeCandlesCloseRSI);
                bool isNewFollowRSIHigh         = pBu.get()->compareTwoStrings(mFollowCandlesCloseRSI, mOldFollowCandlesCloseRSI);

                if (isNewTradeRSIHigh && isNewFollowRSIHigh)
                {
                    for (MapIterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
                    {
                        int orderId = i->first;

                        cancelOrder(mTradeSymbol, orderId);

                        ELOG(INFO, "New RSI is higher than old RSI. Canceled Sell Order. Order id: %d. ", orderId);
                    }
                }
            }
            
            mSellOrdersNewTradeRSI     = false;
            mSellOrdersNewFollowRSI    = false;
        }     

        // Check sell orders
        for (MapIterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
        {
            int orderId = i->first;

            queryOrder(mTradeSymbol, orderId);

            ELOG(INFO, "Checked Sell Orders. Order id: %d. ", orderId);
        }
    }
        
    return true;
}


/**
 * @brief Check old bought orders to start selling new
 * 
 * @return true 
 * @return false 
 */
bool BinanceClient::checkBoughtOrders()
{
    // Check bought orders map and update balance amount
    // if we have bought orders and we have not enough balance
    // we must sell first and we make money after that we can create a buy order
    if (mBoughtOrders.size() > 0)
    {
        // Do we have this orders?
        // Check user wallet
        mCoinQuantity           = "00.00";

        for (MapIterator order = mBoughtOrders.begin(); order != mBoughtOrders.end(); ++order)
        {
            mCoinQuantity = pBu.get()->addTwoStrings(mCoinQuantity, order->second["Quantity"]);
        }

        mCoinQuantity           = pBu.get()->roundString(mCoinQuantity, mSymbolTickSize);

        bool checkCoinBalance   = getCoinBalance(mCoinSymbol);

        if (checkCoinBalance)
        {
            ELOG(WARNING, "Sufficient coin quantity. Balance amount will be recalculated. Quantity: %s.", mCoinQuantity.c_str());

            for (MapIterator order = mBoughtOrders.begin(); order != mBoughtOrders.end(); ++order)
            {
                calcNewBalanceAmount(mBuySide, order->second["BoughtPrice"], order->second["Quantity"]);
            }
        }
        else
        {
            ELOG(WARNING, "Unsufficient coin quantity. Bougt orders map will be cleared. Quantity: %s.", mCoinQuantity.c_str());

            mBoughtOrders.clear();
        }
    }

    return true;
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
    cli.set_read_timeout(2, 500000);        // 2500 milliseconds
    cli.set_write_timeout(2, 500000);       // 2500 milliseconds

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
    cli.set_read_timeout(2, 500000);        // 2500 milliseconds
    cli.set_write_timeout(2, 500000);       // 2500 milliseconds

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
    cli.set_read_timeout(2, 500000);        // 2500 milliseconds
    cli.set_write_timeout(2, 500000);       // 2500 milliseconds

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
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceClient::getCoinBalance(std::string symbol)
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

    for (int i=0; i< static_cast<int>(parsedResponse.size()); i++)
    {
        if (parsedResponse[i]["coin"] == symbol)
        {
            std::string walletBalanceAmount = parsedResponse[i]["free"].asString();

            ELOG(INFO, "Get Coin Balance Symbol: %s, Balance: %s.", parsedResponse[i]["coin"].asCString(), parsedResponse[i]["free"].asCString());

            if (parsedResponse[i]["coin"] == mBalanceSymbol)
            {
                bool isAmountEnough = pBu.get()->compareTwoStrings(walletBalanceAmount, mBalanceAmount);
                
                if (!isAmountEnough)
                {
                    ELOG(WARNING, "Balance Amount is Greater Than Wallet Balance Amount. Balance Amount: %s, Wallet Balance: %s.", mBalanceAmount.c_str(), walletBalanceAmount.c_str());
                    
                    return false;
                }

                return true;
            }

            if (parsedResponse[i]["coin"] == mCoinSymbol)
            {
                bool isQuantityEnough = pBu.get()->compareTwoStrings(walletBalanceAmount, mCoinQuantity);

                if (!isQuantityEnough)
                {
                    ELOG(WARNING, "Coin Quantity is Greater Than Wallet Coin Quantity. Coin Quantity: %s, Wallet Coin Quantity: %s.", mCoinQuantity.c_str(), walletBalanceAmount.c_str());
                    
                    return false;
                }

                return true;
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
bool BinanceClient::getCandlesticksData(std::string symbol, std::string interval, std::string startTime)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/klines";


    std::string reqParams           = "symbol="+symbol+"&interval="+interval+"&startTime="+startTime+"&limit=1000";


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Klines/Candlesticks Data Request Timestamp: %s, Endpoint: %s, Start Timestamp: %s.", reqTimestamp.c_str(), reqEndpoint.c_str(), startTime.c_str());

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

    ELOG(INFO, "Got Candlesticks Data. Candles size: %d.", candlesSize);

    if (symbol == mTradeSymbol)
    {
        for (int i = 0; i < candlesSize; i++)
        {
            // std::cout << parsedResponse[i][4] << std::endl;

            mTradeCandlesOpenPrices.push_back(parsedResponse[i][1].asString());
            mTradeCandlesHighPrices.push_back(parsedResponse[i][2].asString());
            mTradeCandlesLowPrices.push_back(parsedResponse[i][3].asString());
            mTradeCandlesClosePrices.push_back(parsedResponse[i][4].asString());
        }
    }

    if (symbol == mFollowSymbol)
    {
        for (int i = 0; i<static_cast<int>(parsedResponse.size()); i++)
        {
            mFollowCandlesOpenPrices.push_back(parsedResponse[i][1].asString());
            mFollowCandlesHighPrices.push_back(parsedResponse[i][2].asString());
            mFollowCandlesLowPrices.push_back(parsedResponse[i][3].asString());
            mFollowCandlesClosePrices.push_back(parsedResponse[i][4].asString());
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
bool BinanceClient::getTickSize (std::string symbol)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/exchangeInfo";


    std::string reqParams           = "symbol="+symbol;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Tick Size Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

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
            if (symbol == mTradeSymbol)
            {
                mSymbolTickSize = pBu.get()->getTickSize(filtersJson[i]["tickSize"].toStyledString());

                ELOG(INFO, "%s Tick Size: %d.", mTradeSymbol.c_str(), mSymbolTickSize);

                return true;
            }

            if (symbol == mFollowSymbol)
            {
                mFollowSymbolTickSize = pBu.get()->getTickSize(filtersJson[i]["tickSize"].toStyledString());

                ELOG(INFO, "%s Tick Size: %d.", mFollowSymbol.c_str(), mSymbolTickSize);

                return true;
            }
        }
    }

    return false;
}


/**
 * @brief Get daily coin volume
 * 
 * @param symbol 
 * @return true 
 * @return false 
 */
bool BinanceClient::getDailyVolume(std::string symbol)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/ticker/24hr";


    std::string reqParams           = "symbol="+symbol;


    httplib::Headers reqHeaders     = {{"content-type", "application/json"}};


    ELOG(INFO, "Get Daily Volume Size Request Timestamp: %s, Endpoint: %s", reqTimestamp.c_str(), reqEndpoint.c_str());

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
    std::string rVolume         = parsedResponse["volume"].asString();
    std::string rQuoteVolume    = parsedResponse["quoteVolume"].asString();


    if (rSymbol == mTradeSymbol)
    {
        mTradeSymbolDailyVolume         = rVolume;
        mTradeSymbolDailyQuoteVolume    = rQuoteVolume;
        
        ELOG(INFO, "%s Volume Size: %s, Quote Volume Size: %s.", symbol.c_str(), mTradeSymbolDailyVolume.c_str(), mTradeSymbolDailyQuoteVolume.c_str());

        return true;
    }

    if (rSymbol == mFollowSymbol)
    {
        mFollowSymbolDailyVolume        = rVolume;
        mFollowSymbolDailyQuoteVolume   = rQuoteVolume;
        
        ELOG(INFO, "%s Volume Size: %s, Quote Volume Size: %s.", symbol.c_str(), mFollowSymbolDailyVolume.c_str(), mFollowSymbolDailyQuoteVolume.c_str());

        return true;
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
bool BinanceClient::createNewOrder(std::string symbol, std::string side, std::string type, std::string quantity, std::string price)
{
    std::string reqTimestamp            = pBu.get()->getTimestamp();

    std::string reqEndpoint             = "/api/v3/order";

    std::string roundedQuantity        = pBu.get()->roundString(quantity, mSymbolTickSize);


    std::string reqParams               = "symbol="+symbol+"&side="+side;
                reqParams               += "&type="+type+"&timeInForce=GTC";
                reqParams               += "&quantity="+roundedQuantity+"&price="+price;
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

    int rOrderId                = parsedResponse["orderId"].asInt();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    std::string rQuantity       = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();
    std::string rTransactTime   = parsedResponse["transactTime"].asString();

    std::string rError          = parsedResponse["Error"].asString();

    if (rError.size() == 0)
    {
        OrderMap newOrder;

        if (rStatus == "NEW")
        {
            if (rSide == "BUY")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("TransactTime", rTransactTime);

                mBuyOrders.emplace(rOrderId, newOrder);

                ELOG(INFO, "Created a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());

                return true;
            }
            else if (rSide == "SELL")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", mBoughtOrders.begin()->second["BoughtTime"]);
                newOrder.emplace("TransactTime", rTransactTime);

                mSellOrders.emplace(rOrderId, newOrder);

                mBoughtOrders.erase(mBoughtOrders.begin());

                ELOG(INFO, "Created a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());

                return true;
            }
        }
        else if (rStatus == "FILLED")
        {
            // check filled commission
            Json::Value mAPIFills   = parsedResponse["fills"][0];
            std::string mCommission = mAPIFills["commission"].asString();

            rQuantity               = pBu.get()->subTwoStrings(rQuantity, mCommission);

            ELOG(INFO, "Paid a commission. Commission: %s, Last quantity: %s.", mCommission.c_str(), rQuantity.c_str());

            if (rSide == "BUY")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("BoughtPrice", rPrice);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", rTransactTime);

                mBoughtOrders.emplace(rOrderId, newOrder);

                ELOG(INFO, "Created and Filled a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());
            }
            else if (rSide == "SELL")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", mBoughtOrders.begin()->second["BoughtTime"]);
                newOrder.emplace("SoldTime", rTransactTime);

                mSoldOrders.emplace(rOrderId, newOrder);

                mBoughtOrders.erase(mBoughtOrders.begin());

                ELOG(INFO, "Created and Filled  a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());
            }

            // calculate new balance amount
            calcNewBalanceAmount(rSide, rPrice, rQuantity);

            return true;
        }
    }
    
    ELOG(ERROR, "Failed to Create a New Order.");

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
 * @param stop 
 * @return true 
 * @return false 
 */
bool BinanceClient::createNewOrder(std::string symbol, std::string side, std::string type, std::string quantity, std::string price, std::string stop)
{
    std::string reqTimestamp            = pBu.get()->getTimestamp();

    std::string reqEndpoint             = "/api/v3/order";

    std::string roundedQuantity        = pBu.get()->roundString(quantity, mSymbolTickSize);


    std::string reqParams               = "symbol="+symbol+"&side="+side;
                reqParams               += "&type="+type+"&timeInForce=GTC";
                reqParams               += "&quantity="+roundedQuantity+"&price="+price+"&stopPrice="+stop;
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

    int rOrderId                = parsedResponse["orderId"].asInt();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    // std::string rStopPrice      = parsedResponse["stopPrice"].asString();
    std::string rQuantity       = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();
    std::string rTransactTime   = parsedResponse["transactTime"].asString();

    std::string rError          = parsedResponse["Error"].asString();

    if (rError.size() == 0)
    {
        OrderMap newOrder;

        if (rStatus == "NEW")
        {
            if (rSide == "BUY")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("TransactTime", rTransactTime);

                mBuyOrders.emplace(rOrderId, newOrder);

                ELOG(INFO, "Created a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());

                return true;
            }
            else if (rSide == "SELL")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", mBoughtOrders.begin()->second["BoughtTime"]);
                newOrder.emplace("TransactTime", rTransactTime);

                mSellOrders.emplace(rOrderId, newOrder);

                mBoughtOrders.erase(mBoughtOrders.begin());

                ELOG(INFO, "Created a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());

                return true;
            }
        }
        else if (rStatus == "FILLED")
        {
            // check filled commission
            Json::Value mAPIFills   = parsedResponse["fills"][0];
            std::string mCommission = mAPIFills["commission"].asString();

            rQuantity               = pBu.get()->subTwoStrings(rQuantity, mCommission);

            ELOG(INFO, "Paid a commission. Commission: %s, Last quantity: %s.", mCommission.c_str(), rQuantity.c_str());

            if (rSide == "BUY")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("BoughtPrice", rPrice);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", rTransactTime);

                mBoughtOrders.emplace(rOrderId, newOrder);

                ELOG(INFO, "Created and Filled a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());
            }
            else if (rSide == "SELL")
            {
                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("Price", rPrice);
                newOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("BoughtTime", mBoughtOrders.begin()->second["BoughtTime"]);
                newOrder.emplace("SoldTime", rTransactTime);

                mSoldOrders.emplace(rOrderId, newOrder);

                mBoughtOrders.erase(mBoughtOrders.begin());

                ELOG(INFO, "Created and Filled  a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTransactTime.c_str());
            }

            // calculate new balance amount
            calcNewBalanceAmount(rSide, rPrice, rQuantity);

            return true;
        }
    }
    
    ELOG(ERROR, "Failed to Create a New Order.");

    return false;
}



/**
 * @brief Cancel order
 * 
 * @param symbol 
 * @param orderId 
 * @return true 
 * @return false 
 */
bool BinanceClient::cancelOrder(std::string symbol, int orderId)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/order";


    std::string reqParams           = "symbol="+symbol+"&orderId="+std::to_string(orderId);
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

    int rOrderId        = parsedResponse["orderId"].asInt();
    std::string rSide   = parsedResponse["side"].asString();
    std::string rStatus = parsedResponse["status"].asString();

    if (rStatus!="CANCELED")
    {
        ELOG(ERROR, "Failed to Cancel the Order. Order id: %d.", rOrderId);
        return false;
    }

    if (rSide == mBuySide)
    {
        MapIterator findOrder = mBuyOrders.find(rOrderId);

        if (findOrder != mBuyOrders.end())
        {
            OrderMap newOrder;

            newOrder.emplace("Status", rStatus);

            // Update order
            findOrder->second = newOrder;

            ELOG(INFO, "Canceled Order. Order id: %d, Status: %s, Side: %s.", rOrderId, rStatus.c_str(), rSide.c_str());
        }
    }
    else
    {
        MapIterator findOrder = mSellOrders.find(rOrderId);

        if (findOrder != mSellOrders.end())
        {
            OrderMap newOrder;

            newOrder.emplace("Status", rStatus);

            // Update order
            findOrder->second = newOrder;

            ELOG(INFO, "Canceled Order. Order id: %d, Status: %s, Side: %s.", rOrderId, rStatus.c_str(), rSide.c_str());
        }
    }
    
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
        if (parsedResponse[i]["side"] == mSellSide)
        {
            int rOrderId                = parsedResponse[i]["orderId"].asInt();
            std::string rStatus         = parsedResponse[i]["status"].asString();
            std::string rSymbol         = parsedResponse[i]["symbol"].asString();
            std::string rPrice          = parsedResponse[i]["price"].asString();
            std::string rOrigQty        = parsedResponse[i]["origQty"].asString();
            std::string rExecutedQty    = parsedResponse[i]["executedQty"].asString();


            MapIterator findOrder       = mSellOrders.find(rOrderId);

            if (findOrder != mSellOrders.end())
            {
                OrderMap newOrder;

                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("BoughtPrice", findOrder->second["BoughtPrice"]);
                newOrder.emplace("BoughtTime", findOrder->second["BoughtTime"]);

                ELOG(INFO, "Canceled Order. Order id: %d, Status: %s, Symbol: %s, Bought Price: %s.", rOrderId, rStatus.c_str(), rSymbol.c_str(), findOrder->second["BoughtPrice"].c_str());

                bool isHigherThanZero = pBu.get()->compareTwoStrings("00.00", rExecutedQty);       // if return false partially filled

                // Partially filled
                if (!isHigherThanZero)
                {
                    std::string lastQuantity = pBu.get()->subTwoStrings(rOrigQty, rExecutedQty);

                    newOrder.emplace("Quantity", lastQuantity);
                }
                else
                {
                    newOrder.emplace("Quantity", rOrigQty);
                }
            
                // Update order
                findOrder->second = newOrder;

                ELOG(INFO, "Updated Order in Sell Orders Map. Status: %s, Order id: %d.", rStatus.c_str(), rOrderId);
            }
        }
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
bool BinanceClient::queryOrder(std::string symbol, int orderId)
{
    std::string reqTimestamp        = pBu.get()->getTimestamp();

    std::string reqEndpoint         = "/api/v3/order";


    std::string reqParams           = "symbol="+symbol+"&orderId="+std::to_string(orderId);
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

    int rOrderId                = parsedResponse["orderId"].asInt();
    std::string rSide           = parsedResponse["side"].asString();
    std::string rStatus         = parsedResponse["status"].asString();
    std::string rSymbol         = parsedResponse["symbol"].asString();
    std::string rPrice          = parsedResponse["price"].asString();
    std::string rQuantity       = parsedResponse["origQty"].asString();
    std::string rExecutedQty    = parsedResponse["executedQty"].asString();
    std::string rTime           = parsedResponse["updateTime"].asString();

    std::string rError          = parsedResponse["Error"].asString();

    if (rError.size() == 0)
    {
        ELOG(INFO, "Side: %s, Status: %s, Order ID: %d, Order Price: %s", rSide.c_str(), rStatus.c_str(), rOrderId, rPrice.c_str());

        if (rStatus == "FILLED")
        {
            if (rSide == mBuySide)
            {
                OrderMap newOrder;

                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("BoughtPrice", rPrice);
                newOrder.emplace("Quantity", rExecutedQty);
                newOrder.emplace("BoughtTime", rTime);

                if (mBoughtOrders.find(rOrderId) == mBoughtOrders.end())
                    mBoughtOrders.emplace(rOrderId, newOrder);

                if (mBuyOrders.find(orderId) != mBuyOrders.end())
                    mBuyOrders.erase(orderId);

                ELOG(INFO, "Filled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str(), rTime.c_str());
            
                // calculate new balance amount
                calcNewBalanceAmount(rSide, rPrice, rQuantity);

                return true;
            }
            else if (rSide == mSellSide)
            {
                OrderMap newOrder;

                newOrder.emplace("Symbol", rSymbol);
                newOrder.emplace("SoldPrice", rPrice);
                newOrder.emplace("Quantity", rQuantity);
                newOrder.emplace("SoldTime", rTime);

                mSoldOrders.emplace(rOrderId, newOrder);

                ELOG(INFO, "Added Sold Order Item. Order id: %d, Map size: %d.", rOrderId, mSoldOrders.size());
            
                MapIterator find = mSellOrders.find(rOrderId);

                if (find != mSellOrders.end())
                {
                    newOrder.emplace("Status", "FILLED");

                    find->second = newOrder;

                    ELOG(DEBUG, "Added Status to Filled Order. Status: %s, Second Status: %s.", newOrder["Status"].c_str(), find->second["Status"].c_str());
                }

                ELOG(INFO, "Filled a Sell Order. OrderId: %d, Symbol: %s, SoldPrice: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), rQuantity.c_str(), rTime.c_str());

                // calculate new balance amount
                calcNewBalanceAmount(rSide, rPrice, rQuantity);

                return true;
            }
        }
        else if (rStatus == "CANCELED")
        {
            bool isHigherThanZero       = pBu.get()->compareTwoStrings("00.00", rExecutedQty);      // if return false
            bool isLowerThanQuantity    = pBu.get()->compareTwoStrings(rExecutedQty, rQuantity);    // if return false
                                                                                                    // that means partially filled
            if (!isHigherThanZero && !isLowerThanQuantity)
            {
                if (rSide == mBuySide)
                {
                    OrderMap newOrder;

                    newOrder.emplace("Symbol", rSymbol);
                    newOrder.emplace("BoughtPrice", rPrice);
                    newOrder.emplace("Quantity", rExecutedQty);
                    newOrder.emplace("BoughtTime", rTime);

                    if (mBoughtOrders.find(rOrderId) == mBoughtOrders.end())
                        mBoughtOrders.emplace(rOrderId, newOrder);

                    if (mBuyOrders.find(orderId) != mBuyOrders.end())
                        mBuyOrders.erase(orderId);

                    ELOG(INFO, "Partially Filled and Canceled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rExecutedQty.c_str(), rTime.c_str());

                    // calculate new balance amount
                    calcNewBalanceAmount(rSide, rPrice, rQuantity);

                    return true;
                }
                else if (rSide == mSellSide)
                {
                    OrderMap newOrder;

                    newOrder.emplace("Symbol", rSymbol);
                    newOrder.emplace("SoldPrice", rPrice);
                    newOrder.emplace("Quantity", rQuantity);
                    newOrder.emplace("SoldQuantity", rExecutedQty);
                    newOrder.emplace("SoldTime", rTime);

                    mSoldOrders.emplace(rOrderId, newOrder);

                    ELOG(INFO, "Added Sold Order Item. Order id: %d, Map size: %d.", rOrderId, mSoldOrders.size());

                    MapIterator find = mSellOrders.find(rOrderId);

                    if (find != mSellOrders.end())
                    {
                        // added bought price and new quantity
                        newOrder.emplace("BoughtPrice", find->second["BoughtPrice"]);
                        newOrder.emplace("BoughtTime", find->second["BoughtTime"]);
                        newOrder["Quantity"] = pBu.get()->subTwoStrings(rQuantity, rExecutedQty);

                        // check if not exists add sell order to bought order list
                        if (mBoughtOrders.find(orderId) == mBoughtOrders.end())
                            mBoughtOrders.emplace(orderId, newOrder);

                        // update sell coin value
                        newOrder.emplace("Status", "CANCELED");
                        find->second = newOrder;

                        ELOG(DEBUG, "Added Status to Partially Filled Order. Status: %s, Second Status: %s, Quantity: %s.", newOrder["Status"].c_str(), find->second["Status"].c_str(), find->second["Quantity"].c_str());
                    }

                    ELOG(INFO, "Partially Filled and Canceled a Sell Order. OrderId: %d, Symbol: %s, SoldPrice: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), rExecutedQty.c_str(), rTime.c_str());

                    // calculate new balance amount
                    calcNewBalanceAmount(rSide, rPrice, rQuantity);

                    return true;
                }
            }
            
            if (rSide == mBuySide)
            {
                if (mBuyOrders.find(orderId) != mBuyOrders.end())
                    mBuyOrders.erase(orderId);

                ELOG(INFO, "Canceled a Buy Order. OrderId: %d, Symbol: %s, Price: %s, Quantity: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), rQuantity.c_str());

                return true;
            }
            else if (rSide == mSellSide)
            {
                MapIterator findSell     = mSellOrders.find(rOrderId);
                MapIterator findBought   = mBoughtOrders.find(rOrderId);

                if (findBought == mBoughtOrders.end())
                    if (findSell != mSellOrders.end())
                        mBoughtOrders.emplace(orderId, findSell->second);

                if (findSell != mSellOrders.end())
                {
                    OrderMap newOrder;

                    newOrder.emplace("Status", "CANCELED");
                    newOrder.emplace("BoughtPrice", findSell->second["BoughtPrice"]);

                    findSell->second = newOrder;

                    ELOG(DEBUG, "Added Status to Canceled Order. newOrder Status: %s, Second Status: %s.", newOrder["Status"].c_str(), findSell->second["Status"].c_str());
                }

                ELOG(INFO, "Canceled a Sell Order. OrderId: %d, Symbol: %s, Price: %s, Bought Price: %s, Quantity: %s.", rOrderId, rSymbol.c_str(), rPrice.c_str(), findSell->second["BoughtPrice"].c_str(), rQuantity.c_str());
            
                return true;
            }
        }
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