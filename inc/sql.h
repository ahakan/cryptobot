#ifndef Sql_h
#define Sql_h

#include <iostream>
#include <string>

#include "elog.h"
#include "sqlite3.h"

class Sql
{
    private:
        sqlite3             *db;
        std::string         sql;
        char                *zErrMsg = 0;
        int                 rc;
        int                 mId = 0;

        
        static int          sqliteCallback (void *NotUsed, int argc, char **argv, char **azColName);

    public:
                            Sql();
                            ~Sql();

        bool                addClosedKlinePrice(float candle);
};

#endif