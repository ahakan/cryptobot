#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "elog.h"
#include "json/json.h"

#include "../inc/websocket.h"
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
        // std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void inp(WebsocketUtils *wsu, Websocket *ws)
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

            exit(0);
        }
    }
}


int main()
{
    float a = 0;
    float *candle = &a;

    Sql *mSql = new Sql;

    WebsocketUtils  *wsu = new WebsocketUtils();
    Websocket       *ws = new Websocket(wsu, candle);

    std::thread     th1 = std::thread(&Websocket::init, ws);

    std::thread     th2 = std::thread(foo, candle, mSql);
    std::thread     th3 = std::thread(inp, wsu, ws);

    th1.join();
    th2.join();
    th3.join();

    return 0;
}

