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

// Definitions

// Sides
#define BINANCE_BUY             "BUY"
#define BINANCE_SELL            "SELL"

// Statuses
#define BINANCE_NEW             "NEW"
#define BINANCE_PARTIALLY       "PARTIALLY_FILLED"
#define BINANCE_FILLED          "FILLED"
#define BINANCE_CANCELED        "CANCELED"

// Types
#define BINANCE_LIMIT           "LIMIT"
#define BINANCE_MARKET          "MARKET"
#define BINANCE_STOP_LOSS       "STOP_LOSS"
#define BINANCE_STOP_LOSS_LIMIT "STOP_LOSS_LIMIT"

// ORDER RESPONSE TYPE
#define ORDER_RESP_TYPE_ACK     "ACK"
#define ORDER_RESP_TYPE_RESULT  "RESULT"
#define ORDER_RESP_TYPE_FULL    "FULL"


// Using && Namespaces
using CandleVector  = std::vector<std::string>;

// Structs

// Order struct
struct Order : public std::mutex
{
    uint32_t        orderId;
    uint64_t        transactTime;

    std::string     side;
    std::string     type;
    std::string     status;
    std::string     symbol;
    std::string     price;
    std::string     stopPrice;
    std::string     quantity;
    std::string     executedQty;
    std::string     soldPrice;
    std::string     boughtPrice;

    std::string     expectedPrice;
    std::string     expectedStopPrice;
    std::string     expectedAverage;
    std::string     expectedQuantity;
};

// Coin struct
struct Symbol : public std::mutex
{
    std::string     coinName;       // SOL
    std::string     coinQuantity;

    std::string     symbol;         // SOLUSDT

    int             tickSize;

    std::string     price;
    std::string     volume;
    std::string     quoteVolume;
};

// Candlesticks struct
struct Candlesticks : public std::mutex
{
    bool            isUpdated;
    
    std::string     symbol;         // SOLUSDT
    std::string     interval;       // 4h
    std::string     candleDuration;

    int             tickSize;

    int             RSIPeriod;
    std::string     RSIOversold;
    std::string     RSIOverbought;

    std::string     startTime;
    std::string     lastCandleTimestamp = "0";

    std::string     closeRSI;
    std::string     oldCloseRSI;

    std::string     openPricesAverage;
    std::string     highPricesAverage;
    std::string     lowPricesAverage;
    std::string     closePricesAverage;

    std::string     lowestPrice;
    std::string     highestPrice;
    std::string     totalChanges;

    CandleVector    openPrices;
    CandleVector    highPrices;
    CandleVector    lowPrices;
    CandleVector    closePrices;

    CandleVector    quoteVolumes;
    CandleVector    percentChange;
};


#endif