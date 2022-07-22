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
#include "../inc/server.h"
#include "../inc/client.h"
#include "../inc/trade.h"
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


/**
 * @brief Signal handler
 * 
 * @param signal 
 */
void signalHandler(int signal)
{
    (void) signal;

    Opel *iOpel = Opel::instance();

    iOpel->setIsActive(0);

    iOpel->setExitSignal(0);

    ELOG(INFO, "Sent exit signal.");
}


/**
 * @brief Main function
 * 
 * @return int 
 */
int main()
{
    ELOG_OUTPUT(Elog::LogOutput::File);

    struct sigaction                    sigIntHandler;

    sigemptyset(&sigIntHandler.sa_mask);

    sigIntHandler.sa_handler    = signalHandler;
    sigIntHandler.sa_flags      = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);


    std::shared_ptr<Sql>                pSql(new Sql);

    std::shared_ptr<BinanceUtilities>   bUtil(new BinanceUtilities);
    std::shared_ptr<BinanceWebsocket>   bSoc(new BinanceWebsocket(bUtil));
    std::shared_ptr<BinanceClient>      bReq(new BinanceClient(bUtil));
    std::shared_ptr<BinanceTrade>       bTrd(new BinanceTrade(bUtil, bReq));
    std::shared_ptr<BinanceServer>      bWeb(new BinanceServer(bUtil));

    std::thread sqlTh           = std::thread(&Sql::init, pSql);
    std::thread trdTh           = std::thread(&BinanceTrade::init, bTrd);
    std::thread socTh           = std::thread(&BinanceWebsocket::init, bSoc);
    std::thread webTh           = std::thread(&BinanceServer::init, bWeb);

    sqlTh.join();
    trdTh.join();
    socTh.join();
    webTh.join();
    

    std::cout << "--Cryptobot has been closed." << std::endl;

    return 0;
}
