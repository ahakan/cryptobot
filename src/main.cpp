/**
 * @file main.cpp
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// Includes
#include "../inc/opel.h"
#include "../inc/websocket.h"
#include "../inc/requests.h"
#include "../inc/utilities.h"
#include "../inc/sql.h"

// Libraries
#include "elog.h"
#include "json/json.h"

// Standard libraries
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void inp(BinanceUtilities *util, BinanceWebsocket *ws, BinanceRequests *req)
{
    char a[2];

    while (true)
    {
        std::cin >> a;

        if (*a == 'x' || *a == 'X')
        {
            ELOG(INFO, "Pressed X. Exiting...");

            delete util;
            delete ws;
            delete req;

            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}


int main()
{
    Sql *pSql                   = new Sql;

    BinanceUtilities *bUtil     = new BinanceUtilities();
    BinanceRequests  *bReq      = new BinanceRequests(bUtil);
    BinanceWebsocket *bWs       = new BinanceWebsocket(bUtil);

    std::thread sqlTh           = std::thread(&Sql::init, pSql);
    std::thread reqTh           = std::thread(&BinanceRequests::init, bReq);
    std::thread wsTh            = std::thread(&BinanceWebsocket::init, bWs);
    std::thread th4             = std::thread(inp, bUtil, bWs, bReq);

    sqlTh.join();
    reqTh.join();
    wsTh.join();
    th4.join();
    
    return 0;
}
