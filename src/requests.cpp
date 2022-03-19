#include "../inc/requests.h"


/**
 * @brief Construct a new Requests::Requests object
 * 
 * @param pBu 
 */
Requests::Requests(std::shared_ptr<BinanceUtilities> pBu)
{
    this->pBu               = pBu;
    mBase                   = pBu.get()->getAPIBase();
    mAPI_KEY                = pBu.get()->getAPIKEY();
    mSECRET_KEY             = pBu.get()->getAPISECRETKEY();

    mSymbol                 = pBu.get()->getSymbol();
    mInterval               = pBu.get()->getInterval();
    mQuantity               = pBu.get()->getQuantity();
    mFollowSymbol           = pBu.get()->getFollowSymbol();
    mBalanceSymbol          = pBu.get()->getBalanceSymbol();
    mBalanceAmount          = pBu.get()->getBalanceAmount();
    mAverageAmount          = pBu.get()->getAverageAmount();
    mAverageAutoCalculate   = pBu.get()->getAverageAutoCalculate();

    mRSIPeriod              = pBu.get()->getRSIPeriod();
    mRSIOversold            = pBu.get()->getRSIOversold();
    mRSIOverbought          = pBu.get()->getRSIOverbought();

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
 * @brief Calculate new sell order price
 * 
 * @param boughtPrice 
 * @return std::string 
 */
std::string Requests::calcNewSellPrice(std::string boughtPrice)
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
std::string Requests::calcNewBuyPrice()
{
    bool isBuyAverageCalculated    = calcOrderPriceAverage();

    if (!isBuyAverageCalculated)
    {
        ELOG(ERROR, "Failed to Calculated New Buy Price Average.");
    }

    ELOG(INFO, "Calculate New Buy Price. Live Price: %s, Calculated Average: %s", mSymbolLivePrice.c_str(), mNewOrderCalculatedAverage.c_str());

    std::string calculatedPrice = pBu.get()->subTwoStrings(mSymbolLivePrice, mNewOrderCalculatedAverage);

    ELOG(INFO, "Calculated New Sell Price. Live Price: %s, Sell Price: %s.", mSymbolLivePrice.c_str(), calculatedPrice.c_str());

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
bool Requests::calcNewBalanceAmount(std::string side, std::string price, std::string quantity)
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
bool Requests::calcOrderPriceAverage()
{
    if (mAverageAutoCalculate)
    {
        float highestPrice          = std::stof(*std::max_element(mTradeCandlesHighPrices.begin(), mTradeCandlesHighPrices.end()));
        float lowestPrice           = std::stof(*std::min_element(mTradeCandlesLowPrices.begin(), mTradeCandlesLowPrices.end()));

        float calculatedAverage     = (highestPrice-lowestPrice)/(mRSIPeriod/2.0);

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
bool Requests::calcSymbolRSI()
{
    if (mTradeCandlesClosePrices.size() != 0)
    {
        mOldTradeCandlesCloseRSI            = mTradeCandlesCloseRSI.length() > 0 ? mTradeCandlesCloseRSI : "Empty";
        mTradeCandlesCloseRSI               = pBu.get()->calculateRSI(mTradeCandlesClosePrices);

        mBuyOrdersNewTradeRSI               = mOldTradeCandlesCloseRSI == "Empty" ? false : true;   // ignore to first calculation for cancel order
        mSellOrdersNewTradeRSI              = mOldTradeCandlesCloseRSI == "Empty" ? false : true;   // ignore to first calculation for cancel order

        ELOG(INFO, "Calculated Symbol RSI. Trade Candles New Close RSI: %s, Old Close RSI: %s, mBuyOrdersNewTradeRSI: %d, mSellOrdersNewTradeRSI: %d.", mTradeCandlesCloseRSI.c_str(), mOldTradeCandlesCloseRSI.c_str(), mBuyOrdersNewTradeRSI, mSellOrdersNewTradeRSI);

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
        mOldFollowCandlesCloseRSI           = mFollowCandlesCloseRSI.length() > 0 ? mFollowCandlesCloseRSI : "Empty";
        mFollowCandlesCloseRSI              = pBu.get()->calculateRSI(mFollowCandlesClosePrices);

        mBuyOrdersNewFollowRSI              = mOldFollowCandlesCloseRSI == "Empty" ? false : true;   // ignore to first calculation for cancel order
        mSellOrdersNewFollowRSI             = mOldFollowCandlesCloseRSI == "Empty" ? false : true;   // ignore to first calculation for cancel order


        ELOG(INFO, "Calculated Follow Symbol RSI. Follow Candles New Close RSI: %s, Old Close RSI: %s, mBuyOrdersNewFollowRSI: %d, mSellOrdersNewFollowRSI: %d.", mFollowCandlesCloseRSI.c_str(), mOldFollowCandlesCloseRSI.c_str(), mBuyOrdersNewFollowRSI, mSellOrdersNewFollowRSI);

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
bool Requests::calcFollowAverages()
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
                bool mCalculateSymbolAverages       = calcSymbolAverages();
                bool mCalculateSymbolRSI            = calcSymbolRSI();
                bool mCalculateOrderPriceAverage    = calcOrderPriceAverage();

                if (mCalculateSymbolAverages && mCalculateSymbolRSI && mCalculateOrderPriceAverage)
                {
                    ELOG(INFO, "Added new symbol closed price data and calculated symbol averages, RSI and order price average. Calculated Average: %s.", mNewOrderCalculatedAverage.c_str());
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

        ELOG(INFO, "mBuyOrderSize: %d, mBoughtOrderSize: %d, mSellOrderSize: %d, mSoldOrderSize: %d.", mBuyOrders.size(), mBoughtOrders.size(), mSellOrders.size(), mSoldOrders.size());
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
BinanceRequests::BinanceRequests(std::shared_ptr<BinanceUtilities> pBu)
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

        if (pOpel->getExitSignal())
        {
            bool  mGetSymbolTickSize = getTickSize(mSymbol);
            bool  mGetFollowTickSize = getTickSize(mFollowSymbol);

            if (mGetSymbolTickSize && mGetFollowTickSize)
            {
                bool mGetTradeSymbolCandles     = getCandlesticksData(mSymbol, mInterval, pBu.get()->getOldTimestamp());
                bool mGetFollowSymbolCandles    = getCandlesticksData(mFollowSymbol, mInterval, pBu.get()->getOldTimestamp());

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
                            break;
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (pOpel->getExitSignal())
    {
        ELOG(INFO, "Checked User Account and Wallet. Calculated Averages and RSI. Starting Trade...");

        binance();
    }

    if (mBuyOrders.size() > 0)
    {
        ELOG(INFO, "Closed last buy order. Order id: %d.", mBuyOrders.begin()->first);

        cancelOrder(mSymbol, mBuyOrders.begin()->first);
    }

    ELOG(INFO, "Thread Requests detached.");
}


/**
 * @brief Binance main loop
 * 
 */
void BinanceRequests::binance()
{
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
}


/**
 * @brief Create a new buy order
 * 
 * @return true 
 * @return false 
 */
bool BinanceRequests::newBuyOrder()
{
    // if RSI is smaller than mRSIOversold, we create a new buy order
    if (!pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOversold))
    {
        // if we have not a buy order or bought order we create a new buy order
        if (mBuyOrders.size() < 1 && mBoughtOrders.size() < 1)
        {
            // check user wallet balance
            bool walletBalanceAmount    = getCoinBalance(mBalanceSymbol);

            if (walletBalanceAmount)
            {
                // calculate new buy price
                std::string newPrice        = calcNewBuyPrice();

                // calculate total order price
                std::string totalOrderPrice = pBu.get()->multiplyTwoStrings(newPrice, mQuantity);

                // check balance amount
                std::string orderPrice      = pBu.get()->roundString(totalOrderPrice, mSymbolTickSize);

                if (pBu.get()->compareTwoStrings(mBalanceAmount, orderPrice))
                {
                    bool isCreated          = createNewOrder(mSymbol, mBuySide, mOrderType, mQuantity, newPrice);

                    if (isCreated)
                    {
                        ELOG(INFO, "Created a New BUY Order. User balance amount is sufficient. Price: %s. ", newPrice.c_str());

                        return true;
                    }
                }
                else
                {
                    ELOG(WARNING, "Insufficient user balance. Balance: %s. ", mBalanceAmount.c_str());
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
bool BinanceRequests::newSellOrder()
{
    // if RSI is higher than mRSIOverbought, we create a new sell order
    if (pBu.get()->compareTwoStrings(mTradeCandlesCloseRSI, mRSIOverbought))
    {
        // if we bought a coin we'll create a sell order
        if (mBoughtOrders.size() > 0)
        {
            std::string newPrice    = calcNewSellPrice(mBoughtOrders.begin()->second["BoughtPrice"]);

            bool isCreated          = createNewOrder(mSymbol, mSellSide, mOrderType, mBoughtOrders.begin()->second["Quantity"], newPrice);

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
bool BinanceRequests::checkBuyOrders()
{
    // Check buy orders
    if (mBuyOrders.size() > 0)
    {
        // Check new RSI
        if (mBuyOrdersNewTradeRSI && mBuyOrdersNewFollowRSI)
        {
            bool mIsBuyOrderCanceled = cancelOrder(mSymbol, mBuyOrders.begin()->first);

            if (mIsBuyOrderCanceled)
            {
                mBuyOrdersNewTradeRSI     = false;
                mBuyOrdersNewFollowRSI    = false;
            }
        }

        bool checkOrder = queryOrder(mSymbol, mBuyOrders.begin()->first);

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
bool BinanceRequests::checkSellOrders()
{
    // Check sell orders
    if (mSellOrders.size() > 0)
    {
        // Cancel to filled or canceled status orders
        for (AllOrdersMap::iterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
        {
            int mOrderId = i->first;

            if (i->second["Status"] == "FILLED")
            {
                ELOG(DEBUG, "Removed Filled Sell Order. Order id: %d, Status: %s, BoughtPrice: %s, SoldPrice: %s.", mOrderId, i->second["Status"].c_str(), i->second["BoughtPrice"].c_str(), i->second["SoldPrice"].c_str());
                
                mSellOrders.erase(i);
            }
            else if (i->second["Status"] == "CANCELED")
            {
                ELOG(DEBUG, "Removed Canceled Sell Order. Order id: %d, Status: %s, BoughtPrice: %s.", mOrderId, i->second["Status"].c_str(), i->second["BoughtPrice"].c_str());

                mSellOrders.erase(i);
            }
        }

        // Check new RSI and cancel all order
        if (mSellOrdersNewTradeRSI && mSellOrdersNewFollowRSI)
        {
            for (AllOrdersMap::iterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
            {
                int mOrderId = i->first;

                cancelOrder(mSymbol, mOrderId);

                ELOG(INFO, "Calculated New RSI. Canceled Sell Order. Order id: %d. ", mOrderId);
            }

            mSellOrdersNewTradeRSI     = false;
            mSellOrdersNewFollowRSI    = false;
        }     

        // Check sell orders
        for (AllOrdersMap::iterator i = mSellOrders.begin(); i != mSellOrders.end(); ++i)
        {
            int mOrderId = i->first;

            queryOrder(mSymbol, mOrderId);

            ELOG(INFO, "Checked Sell Orders. Order id: %d. ", mOrderId);
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
std::string BinanceRequests::getRequest(std::string endpoint, std::string parameters, httplib::Headers headers)
{
    httplib::Client cli(mBase);

    cli.set_connection_timeout(1, 500000);  // 1 second 500 milliseconds
    cli.set_read_timeout(2, 0);        // 2 seconds
    cli.set_write_timeout(2, 0);       // 2 seconds

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
    cli.set_read_timeout(1, 500000);        // 1 second 500 milliseconds
    cli.set_write_timeout(1, 500000);       // 1 second 500 milliseconds

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
    cli.set_read_timeout(1, 500000);        // 1 second 500 milliseconds
    cli.set_write_timeout(1, 500000);       // 1 second 500 milliseconds

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
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/account/status";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
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
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/account/apiRestrictions";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
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
bool BinanceRequests::getCoinBalance(std::string symbol)
{
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/sapi/v1/capital/config/getall";


    std::string mParams                 = "timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    for (int i=0; i< static_cast<int>(mAPIJson.size()); i++)
    {
        if (mAPIJson[i]["coin"] == symbol)
        {
            std::string mWalletBalanceAmount = mAPIJson[i]["free"].asString();

            ELOG(INFO, "Get Coin Balance Symbol: %s, Balance: %s.", mAPIJson[i]["coin"].asCString(), mAPIJson[i]["free"].asCString());

            bool isAmountEnough = pBu.get()->compareTwoStrings(mWalletBalanceAmount, mBalanceAmount);

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
    std::string mTimestamp              = pBu.get()->getTimestamp();

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

    if (!mAPIJson.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    int mCandlesSize = static_cast<int>(mAPIJson.size());

    if (symbol == mSymbol)
    {
        for (int i = 0; i < mCandlesSize; i++)
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
    std::string mTimestamp              = pBu.get()->getTimestamp();

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    Json::Value mFiltersJson = mAPIJson["symbols"][0]["filters"];

    for (int i = 0; i<static_cast<int>(mFiltersJson.size()); i++)
    {
        if (mFiltersJson[i]["filterType"] == "PRICE_FILTER")
        {
            if (symbol == mSymbol)
            {
                mSymbolTickSize = pBu.get()->getTickSize(mFiltersJson[i]["tickSize"].toStyledString());

                ELOG(INFO, "%s Tick Size: %d.", mSymbol.c_str(), mSymbolTickSize);

                return true;
            }

            if (symbol == mFollowSymbol)
            {
                mFollowSymbolTickSize = pBu.get()->getTickSize(mFiltersJson[i]["tickSize"].toStyledString());

                ELOG(INFO, "%s Tick Size: %d.", mFollowSymbol.c_str(), mSymbolTickSize);

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
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";

    std::string mRoundedQuantity        = pBu.get()->roundString(quantity, mSymbolTickSize);


    std::string mParams                 = "symbol="+symbol+"&side="+side;
                mParams                 += "&type="+type+"&timeInForce=GTC";
                mParams                 += "&quantity="+quantity+"&price="+price;
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    int mOrderId                = mAPIJson["orderId"].asInt();
    std::string mSide           = mAPIJson["side"].asString();
    std::string mStatus         = mAPIJson["status"].asString();
    std::string mSymbol         = mAPIJson["symbol"].asString();
    std::string mPrice          = mAPIJson["price"].asString();
    std::string mQuantity       = mAPIJson["origQty"].asString();
    std::string mExecutedQty    = mAPIJson["executedQty"].asString();
    std::string mTransactTime   = mAPIJson["transactTime"].asString();

    std::string mError          = mAPIJson["Error"].asString();

    if (mError.size() == 0)
    {
        OrderMap mOrder;

        if (mStatus == "NEW")
        {
            if (mSide == "BUY")
            {
                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("Price", mPrice);
                mOrder.emplace("Quantity", mQuantity);
                mOrder.emplace("TransactTime", mTransactTime);

                mBuyOrders.emplace(mOrderId, mOrder);

                ELOG(INFO, "Created a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());

                return true;
            }
            else if (mSide == "SELL")
            {
                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("Price", mPrice);
                mOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                mOrder.emplace("Quantity", mQuantity);
                mOrder.emplace("TransactTime", mTransactTime);

                mSellOrders.emplace(mOrderId, mOrder);

                mBoughtOrders.clear(); ///////!!!!!!!!

                ELOG(INFO, "Created a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());

                return true;
            }
        }
        else if (mStatus == "FILLED")
        {
            // check filled commission
            Json::Value mAPIFills   = mAPIJson["fills"][0];
            std::string mCommission = mAPIFills["commission"].asString();

            mQuantity               = pBu.get()->subTwoStrings(mQuantity, mCommission);

            ELOG(INFO, "Paid a commission. Commission: %s, Last quantity: %s.", mCommission.c_str(), mQuantity.c_str());

            if (mSide == "BUY")
            {
                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("BoughtPrice", mPrice);
                mOrder.emplace("Quantity", mQuantity);
                mOrder.emplace("BoughtTime", mTransactTime);

                mBoughtOrders.emplace(mOrderId, mOrder);

                ELOG(INFO, "Created and Filled a New Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());

                return true;
            }
            else if (mSide == "SELL")
            {
                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("Price", mPrice);
                mOrder.emplace("BoughtPrice", mBoughtOrders.begin()->second["BoughtPrice"]);
                mOrder.emplace("Quantity", mQuantity);
                mOrder.emplace("SoldTime", mTransactTime);

                mSoldOrders.emplace(mOrderId, mOrder);

                mBoughtOrders.clear();  ////!!!!!!!11

                ELOG(INFO, "Created and Filled  a New Sell Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTransactTime.c_str());

                return true;
            }
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
bool BinanceRequests::cancelOrder(std::string symbol, int orderId)
{
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";


    std::string mParams                 = "symbol="+symbol+"&orderId="+std::to_string(orderId);
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    int mOrderId        = mAPIJson["orderId"].asInt();
    std::string mSide   = mAPIJson["side"].asString();
    std::string mStatus = mAPIJson["status"].asString();

    if (mStatus!="CANCELED")
    {
        ELOG(ERROR, "Failed to Cancel the Order. Order id: %d.", mOrderId);
        return false;
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
bool BinanceRequests::cancelAllOpenOrders(std::string symbol)
{
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/api/v3/openOrders";


    std::string mParams                 = "symbol="+symbol+"&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
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
bool BinanceRequests::queryOrder(std::string symbol, int orderId)
{
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/api/v3/order";


    std::string mParams                 = "symbol="+symbol+"&orderId="+std::to_string(orderId);
                mParams                 += "&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

    std::string mParamsWithSignature    = mParams+"&signature="+mSignature;


    httplib::Headers mHeaders           = {{"content-type", "application/json"}, {"X-MBX-APIKEY", mAPI_KEY}};


    // ELOG(INFO, "Query Order Request Timestamp: %s, Endpoint: %s", mTimestamp.c_str(), mEndpoint.c_str());

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

    if (!mAPIJson.isObject())
    {
        ELOG(ERROR, "JSON is not object.");
        return false;
    }

    int mOrderId                = mAPIJson["orderId"].asInt();
    std::string mSide           = mAPIJson["side"].asString();
    std::string mStatus         = mAPIJson["status"].asString();
    std::string mSymbol         = mAPIJson["symbol"].asString();
    std::string mPrice          = mAPIJson["price"].asString();
    std::string mQuantity       = mAPIJson["origQty"].asString();
    std::string mExecutedQty    = mAPIJson["executedQty"].asString();
    std::string mTime           = mAPIJson["updateTime"].asString();

    std::string mError          = mAPIJson["Error"].asString();

    if (mError.size() == 0)
    {
        ELOG(INFO, "Side: %s, Status: %s, Order ID: %d, Order Price: %s", mSide.c_str(), mStatus.c_str(), mOrderId, mPrice.c_str());

        if (mStatus == "FILLED")
        {
            if (mSide == mBuySide)
            {
                OrderMap mOrder;

                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("BoughtPrice", mPrice);
                mOrder.emplace("Quantity", mExecutedQty);
                mOrder.emplace("BoughtTime", mTime);

                if (mBoughtOrders.find(orderId) == mBoughtOrders.end())
                    mBoughtOrders.emplace(mOrderId, mOrder);

                if (mBuyOrders.find(orderId) != mBuyOrders.end())
                    mBuyOrders.erase(orderId);

                ELOG(INFO, "Filled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str(), mTime.c_str());
            
                // calculate new balance amount
                calcNewBalanceAmount(mSide, mPrice, mQuantity);

                return true;
            }
            else if (mSide == mSellSide)
            {
                OrderMap mOrder;

                mOrder.emplace("Symbol", mSymbol);
                mOrder.emplace("SoldPrice", mPrice);
                mOrder.emplace("Quantity", mQuantity);
                mOrder.emplace("SoldTime", mTime);

                mSoldOrders.emplace(mOrderId, mOrder);

                ELOG(INFO, "Added Sold Order Item. Order id: %d, Map size: %d.", mOrderId, mSoldOrders.size());
            
                AllOrdersMap::iterator find = mSellOrders.find(mOrderId);

                if (find != mSellOrders.end())
                {
                    mOrder.emplace("Status", "FILLED");

                    find->second = mOrder;

                    ELOG(DEBUG, "Added Status to Filled Order. Status: %s, Second Status: %s.", mOrder["Status"].c_str(), find->second["Status"].c_str());
                }

                ELOG(INFO, "Filled a Sell Order. OrderId: %d, Symbol: %s, SoldPrice: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), mQuantity.c_str(), mTime.c_str());

                // calculate new balance amount
                calcNewBalanceAmount(mSide, mPrice, mQuantity);

                return true;
            }
        }
        else if (mStatus == "CANCELED")
        {
            bool isHigherThanZero       = pBu.get()->compareTwoStrings("00.00", mExecutedQty);   // if return false
            bool isLowerThanQuantity    = pBu.get()->compareTwoStrings(mExecutedQty, mQuantity); // if return false
                                                                                            // that means partially filled
            if (!isHigherThanZero && !isLowerThanQuantity)
            {
                if (mSide == mBuySide)
                {
                    OrderMap mOrder;

                    mOrder.emplace("Symbol", mSymbol);
                    mOrder.emplace("BoughtPrice", mPrice);
                    mOrder.emplace("Quantity", mExecutedQty);
                    mOrder.emplace("BoughtTime", mTime);

                    if (mBoughtOrders.find(orderId) == mBoughtOrders.end())
                        mBoughtOrders.emplace(mOrderId, mOrder);

                    if (mBuyOrders.find(orderId) != mBuyOrders.end())
                        mBuyOrders.erase(orderId);

                    ELOG(INFO, "Partially Filled and Canceled a Buy Order. OrderId: %d, Symbol: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mExecutedQty.c_str(), mTime.c_str());

                    // calculate new balance amount
                    calcNewBalanceAmount(mSide, mPrice, mQuantity);

                    return true;
                }
                else if (mSide == mSellSide)
                {
                    OrderMap mOrder;

                    mOrder.emplace("Symbol", mSymbol);
                    mOrder.emplace("SoldPrice", mPrice);
                    mOrder.emplace("Quantity", mQuantity);
                    mOrder.emplace("SoldQuantity", mExecutedQty);
                    mOrder.emplace("SoldTime", mTime);

                    mSoldOrders.emplace(mOrderId, mOrder);

                    ELOG(INFO, "Added Sold Order Item. Order id: %d, Map size: %d.", mOrderId, mSoldOrders.size());

                    AllOrdersMap::iterator find = mSellOrders.find(mOrderId);

                    if (find != mSellOrders.end())
                    {
                        // added bought price and new quantity
                        mOrder.emplace("BoughtPrice", find->second["BoughtPrice"]);
                        mOrder["Quantity"] = pBu.get()->subTwoStrings(mQuantity, mExecutedQty);

                        // check if not exists add sell order to bought order list
                        if (mBoughtOrders.find(mOrderId) == mBoughtOrders.end())
                            mBoughtOrders.emplace(orderId, mOrder);

                        // update sell coin value
                        mOrder.emplace("Status", "CANCELED");
                        find->second = mOrder;

                        ELOG(DEBUG, "Added Status to Partially Filled Order. Status: %s, Second Status: %s, Quantity: %s.", mOrder["Status"].c_str(), find->second["Status"].c_str(), find->second["Quantity"].c_str());
                    }

                    ELOG(INFO, "Partially Filled and Canceled a Sell Order. OrderId: %d, Symbol: %s, SoldPrice: %s, BoughtPrice: %s, Quantity: %s, SoldTime: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), mExecutedQty.c_str(), mTime.c_str());

                    // calculate new balance amount
                    calcNewBalanceAmount(mSide, mPrice, mQuantity);

                    return true;
                }
            }
            
            if (mSide == mBuySide)
            {
                if (mBuyOrders.find(orderId) != mBuyOrders.end())
                    mBuyOrders.erase(orderId);

                ELOG(INFO, "Canceled a Buy Order. OrderId: %d, Symbol: %s, Price: %s, Quantity: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mQuantity.c_str());

                return true;
            }
            else if (mSide == mSellSide)
            {
                AllOrdersMap::iterator findSell     = mSellOrders.find(mOrderId);
                AllOrdersMap::iterator findBought   = mBoughtOrders.find(mOrderId);

                if (findBought == mBoughtOrders.end())
                    if (findSell != mSellOrders.end())
                        mBoughtOrders.emplace(orderId, findSell->second);

                if (findSell != mSellOrders.end())
                {
                    OrderMap mOrder;

                    mOrder.emplace("Status", "CANCELED");

                    findSell->second = mOrder;

                    ELOG(DEBUG, "Added Status to Canceled Order. mOrder Status: %s, Second Status: %s.", mOrder["Status"].c_str(), findSell->second["Status"].c_str());
                }

                ELOG(INFO, "Canceled a Sell Order. OrderId: %d, Symbol: %s, Price: %s, Bought Price: %s, Quantity: %s.", mOrderId, mSymbol.c_str(), mPrice.c_str(), mSellOrders.find(orderId)->second["BoughtPrice"].c_str(), mQuantity.c_str());
            
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
bool BinanceRequests::currentOpenOrders(std::string symbol)
{
    std::string mTimestamp              = pBu.get()->getTimestamp();

    std::string mEndpoint               = "/api/v3/openOrders";


    std::string mParams                 = "symbol="+symbol+"&timestamp="+mTimestamp+"&recvWindow="+mRecvWindow;

    std::string mSignature              = pBu.get()->getSignature(mParams);

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

    if (!mAPIJson.isArray())
    {
        ELOG(ERROR, "JSON is not array.");
        return false;
    }

    ELOG(INFO, "Current Open Orders.");

    return true;
}