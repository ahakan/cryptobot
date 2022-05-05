/**
 * @file structs.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-05-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef STRUCTS_H
#define STRUCTS_H

// Standard Libraries
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>


// Using && Namespaces
using AverageVector     = std::vector<std::string>;

// Structs

// Order struct
struct Order
{
    uint32_t    orderId;
    uint32_t    transactTime;

    std::string side;
    std::string type;
    std::string status;
    std::string symbol;
    std::string price;
    std::string stopPrice;
    std::string boughtPrice;
    std::string soldPrice;
    std::string quantity;
    std::string executedQty;

    std::string expectedPrice;
    std::string expectedAverage;
};

// Coin struct
struct Symbol : public std::mutex
{
    std::string coinName;       // SOL
    std::string coinQuantity;

    std::string symbol;         // SOLUSDT

    int         tickSize;

    std::string price;
    std::string volume;
    std::string quoteVolume;
};

// Candlesticks struct
struct Candlesticks : public std::mutex
{
    std::string     symbol;     // SOLUSDT
    std::string     interval;   // 4h
    std::string     startTime;

    int             tickSize;

    int             RSIPeriod;
    std::string     RSIOversold;
    std::string     RSIOverbought;

    bool            newBuyRSI;
    bool            newSellRSI;

    std::string     closeRSI;
    std::string     oldCloseRSI;

    AverageVector   openPrices;
    AverageVector   highPrices;
    AverageVector   lowPrices;
    AverageVector   closePrices;

    std::string     openPricesAverage;
    std::string     highPricesAverage;
    std::string     lowPricesAverage;
    std::string     closePricesAverage;
};


#endif