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
    unsigned long long int  timestamp;
    float                   openPrice;
    float                   closePrice;
    float                   highPrice;
    float                   lowPrice;
    bool                    isClosed;
};

class Opel
{
    private:
        bool                        mIsActive;
        std::mutex                  mMutex;

    public:
                                    Opel();
                                    ~Opel();
                                    Opel(const Opel&)               = delete;
                                    Opel& operator=(const Opel&)    = delete;
                                    
        static Opel                 *instance();
        static struct candle_data   *getCandleDataStruct();

        void                        setIsActive(bool isActive);
        bool                        getIsActive();
};

#endif

