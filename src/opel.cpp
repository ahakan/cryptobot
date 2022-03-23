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
 * @brief Socket error struct
 * 
 * @return struct socket_error* 
 */
struct socket_error* Opel::getSocketErrorStruct()
{
    static struct socket_error *pSocketErrorData;

    if (pSocketErrorData == NULL)
    {
        pSocketErrorData = new socket_error;
    }
    
    return pSocketErrorData;
}


/**
 * @brief Trade Candle data struct
 * 
 * @return struct candle_data* 
 */
struct candle_data* Opel::getTradeCandleStruct()
{
    static struct candle_data *pTradeCandleData;

    if (pTradeCandleData == NULL)
    {
        pTradeCandleData = new candle_data;
    }
    
    return pTradeCandleData;
}


/**
 * @brief Follow Candle data struct
 * 
 * @return struct candle_data* 
 */
struct candle_data* Opel::getFollowCandleStruct()
{
    static struct candle_data *pFollowCandleData;

    if (pFollowCandleData == NULL)
    {
        pFollowCandleData = new candle_data;
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


/**
 * @brief Set bought orders map
 * 
 * @param orders 
 */
void Opel::setBoughtOrdersMap(AllOrdersMap *orders)
{
    mBoughtOrdersMap = orders;
}


/**
 * @brief Get bought orders map
 * 
 * @return AllOrdersMap* 
 */
AllOrdersMap *Opel::getBoughtOrdersMap()
{
    return mBoughtOrdersMap;
}


/**
 * @brief Set sell orders map
 * 
 * @param orders 
 */
void Opel::setSellOrdersMap(AllOrdersMap *orders)
{
    mSellOrdersMap = orders;
}


/**
 * @brief Get sell orders map
 * 
 * @return AllOrdersMap* 
 */
AllOrdersMap *Opel::getSellOrdersMap()
{
    return mSellOrdersMap;
}


/**
 * @brief Set sold orders map
 * 
 * @param orders 
 */
void Opel::setSoldOrdersMap(SoldOrdersMap *orders)
{
    mSoldOrdersMap = orders;
}


/**
 * @brief Get sold orders map
 * 
 * @return SoldOrdersMap* 
 */
SoldOrdersMap *Opel::getSoldOrdersMap()
{
    return mSoldOrdersMap;
}