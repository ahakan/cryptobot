#include "../inc/opel.h"


/**
 * @brief Construct a new Opel::Opel object
 * 
 */
Opel::Opel()
{
    ELOG(INFO, "Operational elements constructor initialized.");
}


/**
 * @brief Destroy the Opel::Opel object
 * 
 */
Opel::~Opel()
{
    ELOG(INFO, "Operational elements destructor.");
}


/**
 * @brief Opel instance
 * 
 * @return Opel* 
 */
Opel* Opel::instance()
{
    static Opel *pInstance;

    if (pInstance == NULL)
    {
        pInstance = new Opel();
    }

    return pInstance;
}


/**
 * @brief Trade Candle data struct
 * 
 * @return struct Candle* 
 */
struct Candle* Opel::getTradeCandleStruct()
{
    static struct Candle *pTradeCandleData;

    if (pTradeCandleData == NULL)
    {
        pTradeCandleData = new Candle;
    }
    
    return pTradeCandleData;
}


/**
 * @brief Follow Candle data struct
 * 
 * @return struct Candle* 
 */
struct Candle* Opel::getFollowCandleStruct()
{
    static struct Candle *pFollowCandleData;

    if (pFollowCandleData == NULL)
    {
        pFollowCandleData = new Candle;
    }
    
    return pFollowCandleData;
}


/**
 * @brief Set exit signal
 * 
 * @param signal 
 */
void Opel::setExitSignal(bool signal)
{
    mExitSignal = signal;
}


/**
 * @brief Get exit signal
 * 
 * @return true 
 * @return false 
 */
bool Opel::getExitSignal()
{
    return mExitSignal;
}


/**
 * @brief Set is active
 * 
 * @param isActive 
 */
void Opel::setIsActive(bool isActive)
{
    mIsActive = isActive;
}


/**
 * @brief Get is active
 * 
 * @return true 
 * @return false 
 */
bool Opel::getIsActive()
{
    return mIsActive;
}


/**
 * @brief Set trade symbol
 * 
 * @param symbol 
 */
void Opel::setTradeSymbol(std::string symbol)
{
    mTradeSymbol = symbol;
}


/**
 * @brief Get trade symbol
 * 
 * @return std::string 
 */
std::string Opel::getTradeSymbol()
{
    return mTradeSymbol;
}


/**
 * @brief Set follow symbol
 * 
 * @param symbol 
 */
void Opel::setFollowSymbol(std::string symbol)
{
    mFollowSymbol = symbol;
}


/**
 * @brief Get follow symbol
 * 
 * @return std::string 
 */
std::string Opel::getFollowSymbol()
{
    return mFollowSymbol;
}


// /**
//  * @brief Set buy orders map
//  * 
//  * @param orders 
//  */
// void Opel::setBuyOrdersMap(AllOrdersMap *orders)
// {
//     mBuyOrdersMap = orders;
// }


// /**
//  * @brief Get buy orders map
//  * 
//  * @return AllOrdersMap* 
//  */
// AllOrdersMap *Opel::getBuyOrdersMap()
// {
//     return mBuyOrdersMap;
// }

