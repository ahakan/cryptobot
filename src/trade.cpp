#include "../inc/trade.h"


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
    mTradeCandlesticks.symbol           = tradeSymbol;
    mTradeCandlesticks.interval         = interval;
    mTradeCandlesticks.RSIPeriod        = RSIPeriod;
    mTradeCandlesticks.RSIOversold      = RSIOversold;
    mTradeCandlesticks.RSIOverbought    = RSIOverbought;

    mFollowCandlesticks.symbol          = followSymbol;
    mFollowCandlesticks.interval        = interval;
    mFollowCandlesticks.RSIPeriod       = RSIPeriod;
    mFollowCandlesticks.RSIOversold     = RSIOversold;
    mFollowCandlesticks.RSIOverbought   = RSIOverbought;


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


void Trade::calculates()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {
        // Add start time to candlesticks structs for candlesticks info
        mTradeCandlesticks.startTime    = pBu.get()->getRSITimestamp(mTradeCandlesticks.RSIPeriod, 
                                                                        mTradeCandlesticks.interval);

        mFollowCandlesticks.startTime   = pBu.get()->getRSITimestamp(mFollowCandlesticks.RSIPeriod, 
                                                                        mFollowCandlesticks.interval);

        bool getTradeSymbolCandles      = pReq.get()->getCandlesticksData(mTradeCandlesticks);
        bool getFollowSymbolCandles     = pReq.get()->getCandlesticksData(mFollowCandlesticks);

        if (getTradeSymbolCandles && getFollowSymbolCandles)
        {
            bool calculateSymbolRSI     = pBu.get()->calculateRSI(mTradeCandlesticks);
            bool calculateFollowRSI     = pBu.get()->calculateRSI(mFollowCandlesticks);

            if (calculateSymbolRSI && calculateFollowRSI)
            {
                bool getTradeDailyVolume    = pReq.get()->getDailyVolume(mTradeSymbolInfo);
                bool getFollowDailyVolume   = pReq.get()->getDailyVolume(mFollowSymbolInfo);

                if (getTradeDailyVolume && getFollowDailyVolume) 
                {

                    pBu.get()->calculateAverage(mTradeCandlesticks);

                    ELOG(INFO, "%s - %s - %s - %s", mTradeCandlesticks.symbol.c_str(), 
                                                    mTradeCandlesticks.interval.c_str(),
                                                    mTradeCandlesticks.closeRSI.c_str(),
                                                    mTradeCandlesticks.closePricesAverage.c_str());

                    // break;
                }
            }
        
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ELOG(INFO, "Calculates -> detached.");

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
            mAccountStatus         = pReq.get()->getAccountStatus();

            if (!mAccountStatus)
            {
                ELOG(ERROR, "User account status is not normal. Can't enter main loop.");
            }

            if (mAccountStatus)
            {
                mAPIKeyPermission      = pReq.get()->getAPIKeyPermission();

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
            Opel *iOpel = Opel::instance();

            iOpel->setExitSignal(0);

            break;
        }

        if (pOpel->getExitSignal())
        {
            bool  getSymbolTickSize = pReq.get()->getTickSize(mTradeSymbolInfo);
            bool  getFollowTickSize = pReq.get()->getTickSize(mFollowSymbolInfo);

            if (getSymbolTickSize && getFollowTickSize)
            {
                // Add tick size to candlesticks structs
                mTradeCandlesticks.tickSize     = mTradeSymbolInfo.tickSize;
                mFollowCandlesticks.tickSize    = mFollowSymbolInfo.tickSize;

                bool getWalletBalance           = pReq.get()->getCoinBalance(mBalanceSymbolInfo);

                if (getWalletBalance)
                {
                    break;
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    std::thread tradeTh1    = std::thread(&BinanceTrade::calculates, this);
    std::thread tradeTh2    = std::thread(&BinanceTrade::requests, this);

    tradeTh1.join();
    tradeTh2.join();

    ELOG(INFO, "Trade -> detached.");
}




void BinanceTrade::requests()
{
    Opel *pOpel = Opel::instance();

    while (pOpel->getExitSignal())
    {

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ELOG(INFO, "Requests -> detached.");
}



