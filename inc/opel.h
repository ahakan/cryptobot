/**
 * @file opel.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef OPEL_H
#define OPEL_H

// Includes

// Libraries
#include <elog.h>

// Standard libraries
#include <mutex>

// Structs
struct candle_data
{
    bool                    isUpdated = false;
    std::string             timestamp;
    std::string             openPrice;
    std::string             closePrice;
    std::string             highPrice;
    std::string             lowPrice;
    bool                    isClosed;
};

class Opel
{
    private:
        std::mutex                  mMutex;

        bool                        mIsActive;
        std::string                 mSymbol;

    protected:

                                    Opel();
                                    ~Opel();

    public:
                                    Opel(const Opel&)               = delete;       // Singletons should not be cloneable.
                                    Opel& operator=(const Opel&)    = delete;       // Singletons should not be assignable.
                                    
        static Opel                 *instance();
        static struct candle_data   *getCandleDataStruct();

        void                        setIsActive(bool isActive);
        bool                        getIsActive();
};

#endif

