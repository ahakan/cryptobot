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
#include "elog.h"

// Standard libraries

class Opel
{
    private:
        bool                mIsActive;
    
    public:
                            Opel();
                            ~Opel();
        
        static Opel*        instance();
        void                setIsActive(bool isActive);
        bool                getIsActive();
};

#endif

