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
 * @brief Candle data struct
 * 
 * @return struct candle_data* 
 */
struct candle_data* Opel::getCandleDataStruct()
{
    static struct candle_data *pCandleData;

    if (pCandleData == NULL)
    {
        pCandleData = new candle_data;
    }
    
    return pCandleData;
}


/**
 * @brief Set is active
 * 
 * @param isActive 
 */
void Opel::setIsActive(bool isActive)
{
    mMutex.lock();

    mIsActive = isActive;

    mMutex.unlock();
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
