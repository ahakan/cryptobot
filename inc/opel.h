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
#include <mutex>
#include <map>

// Namespaces
using OrderMap          = std::map<std::string, std::string>;
using AllOrdersMap      = std::map<int, OrderMap>;
using SoldOrdersMap     = std::multimap<int, OrderMap>;

// Structs
struct socket_error : public std::mutex
{
    std::string         message;
    std::string         symbol;
};

struct candle_data  : public std::mutex
{
    bool                isUpdated = false;
    std::string         symbol;
    std::string         timestamp;
    std::string         openPrice;
    std::string         closePrice;
    std::string         highPrice;
    std::string         lowPrice;
    bool                isClosed;
};


class Opel
{
    private:
        bool                        mExitSignal = 1;
        bool                        mIsActive;

        std::string                 mTradeSymbol;
        std::string                 mFollowSymbol;

        AllOrdersMap                *mBuyOrdersMap;
        AllOrdersMap                *mBoughtOrdersMap;
        AllOrdersMap                *mSellOrdersMap;
        SoldOrdersMap               *mSoldOrdersMap;
        

    protected:

                                    Opel();
                                    ~Opel();

    public:
                                    Opel(const Opel&)               = delete;       // Singletons should not be cloneable.
                                    Opel& operator=(const Opel&)    = delete;       // Singletons should not be assignable.
                                    
        static Opel                 *instance();

        static struct socket_error  *getSocketErrorStruct();
        static struct candle_data   *getTradeCandleStruct();
        static struct candle_data   *getFollowCandleStruct();
        
        void                        setExitSignal(bool signal);
        bool                        getExitSignal();

        void                        setIsActive(bool isActive);
        bool                        getIsActive();

        void                        setTradeSymbol(std::string symbol);
        std::string                 getTradeSymbol();

        void                        setFollowSymbol(std::string symbol);
        std::string                 getFollowSymbol();

        void                        setBuyOrdersMap(AllOrdersMap *orders);
        AllOrdersMap                *getBuyOrdersMap();

        void                        setBoughtOrdersMap(AllOrdersMap *orders);
        AllOrdersMap                *getBoughtOrdersMap();

        void                        setSellOrdersMap(AllOrdersMap *orders);
        AllOrdersMap                *getSellOrdersMap();

        void                        setSoldOrdersMap(SoldOrdersMap *orders);
        SoldOrdersMap               *getSoldOrdersMap();

};

#endif

