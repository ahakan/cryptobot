#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "elog.h"
#include "json/json.h"

#include "../inc/websocket.h"
#include "../inc/requests.h"
#include "../inc/utilities.h"
#include "../inc/sql.h"

void foo(float *candle, Sql *mSql)
{
    while (true)
    {
        if (*candle != 0)
        {
            std::cout << "Candle: " << *candle << std::endl;
            mSql->addClosedKlinePrice(*candle);
            *candle = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void inp(BinanceUtilities *wsu, BinanceWebsocket *ws, BinanceRequests *breq)
{
    char a[2];
    while( true )
    {
        // std::cout << "Değer giriniz(Quit=x|X): \n";

        std::cin >> a;

        if ( *a == 'x' || *a == 'X' )
        {
            ELOG(INFO, "Pressed X. Exiting...");

            delete wsu;
            delete ws;
            delete breq;

            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}


int main()
{
    float a                 = 0;
    float *mCandle          = &a;

    Sql *mSql               = new Sql;

    BinanceUtilities *bUtil = new BinanceUtilities();
    BinanceRequests  *bReq  = new BinanceRequests(bUtil);
    BinanceWebsocket *bWs   = new BinanceWebsocket(bUtil, mCandle);

    std::thread th1         = std::thread(&BinanceRequests::init, bReq, mCandle);
    std::thread th2         = std::thread(&BinanceWebsocket::init, bWs);
    std::thread th3         = std::thread(foo, mCandle, mSql);
    std::thread th4         = std::thread(inp, bUtil, bWs, bReq);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    
    return 0;
}
