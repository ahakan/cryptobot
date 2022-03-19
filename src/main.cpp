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
#include "../inc/webserver.h"
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


/**
 * @brief Signal handler
 * 
 * @param signal 
 */
void signalHandler(int signal)
{
    (void) signal;

    Opel *iOpel = Opel::instance();

    iOpel->setExitSignal(0);
}


/**
 * @brief Main function
 * 
 * @return int 
 */
int main()
{
    struct sigaction                    sigIntHandler;

    std::shared_ptr<Sql>                pSql(new Sql);

    std::shared_ptr<BinanceUtilities>   bUtil(new BinanceUtilities);
    std::shared_ptr<BinanceWebsocket>   bSoc(new BinanceWebsocket(bUtil));
    std::shared_ptr<BinanceRequests>    bReq(new BinanceRequests(bUtil));
    std::shared_ptr<BinanceWebserver>   bWeb(new BinanceWebserver(bUtil));


    sigemptyset(&sigIntHandler.sa_mask);

    sigIntHandler.sa_handler    = signalHandler;
    sigIntHandler.sa_flags      = 0;

    
    std::thread sqlTh           = std::thread(&Sql::init, pSql);
    std::thread reqTh           = std::thread(&BinanceRequests::init, bReq);
    std::thread socTh           = std::thread(&BinanceWebsocket::init, bSoc);
    std::thread webTh           = std::thread(&BinanceWebserver::init, bWeb);

    sigaction(SIGINT, &sigIntHandler, NULL);

    sqlTh.join();
    reqTh.join();
    socTh.join();
    webTh.join();
    

    std::cout << "--Cryptobot has been closed." << std::endl;

    return 0;
}
