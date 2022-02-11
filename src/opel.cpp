#include "../inc/opel.h"


/**
 * @brief Opel instance
 * 
 * @return Opel& 
 */
Opel* Opel::instance()
{
    static Opel *opel = new Opel();
    return opel;
}


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
