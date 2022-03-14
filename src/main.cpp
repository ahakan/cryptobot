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
#include <memory>
#include <chrono>
#include <signal.h>


void signalHandler(int signal)
{
    (void) signal;

    Opel *iOpel = Opel::instance();

    iOpel->setExitSignal(0);
}


void checkExit(BinanceUtilities *util, BinanceWebsocket *ws, BinanceRequests *req)
{
    Opel *iOpel = Opel::instance();

    while (true)
    {
        if (iOpel->getExitSignal() == 0)
        {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    
    ELOG(INFO, "Handled an exit signal. Exiting...");

    ELOG(INFO, "Thread checkExit detacted.");

    std::this_thread::sleep_for(std::chrono::milliseconds(7500));

    delete ws;
    delete req;
    delete util;
}


int main()
{
    struct sigaction            sigIntHandler;

    sigIntHandler.sa_handler    = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags      = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    std::shared_ptr<Sql>        pSql(new Sql);

    BinanceUtilities *bUtil     = new BinanceUtilities();
    BinanceRequests  *bReq      = new BinanceRequests(bUtil);
    BinanceWebsocket *bWs       = new BinanceWebsocket(bUtil);

    std::thread sqlTh           = std::thread(&Sql::init, pSql);
    std::thread wsTh            = std::thread(&BinanceWebsocket::init, bWs);
    std::thread reqTh           = std::thread(&BinanceRequests::init, bReq);
    std::thread cExit           = std::thread(checkExit, bUtil, bWs, bReq);

    sqlTh.join();
    wsTh.join();
    reqTh.join();
    cExit.join();

    std::cout << "--Cryptobot has been closed." << std::endl;

    return 0;
}
