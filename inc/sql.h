#ifndef SQL_H
#define SQL_H

// Includes
#include "opel.h"

// Libraries
#include "elog.h"
#include "sqlite3.h"

// Standard libraries
#include <iostream>
#include <string>
#include <vector>
#include <thread>

using Record    = std::vector<std::pair<std::string, std::string>>;

class Sql
{
    private:
        Opel                        *mOpel = Opel::instance();

        sqlite3                     *db;
        std::string                 sql;
        char                        *zErrMsg = 0;
        int                         rc;
        int                         mId = 0;

        Record                      mRecord;
        Record                      selectQuery(std::string query);

        bool                        allQuery(std::string query);

        static int                  selectCallback(void *pData, int numFields, char **pFields, char **pColNames);
        static int                  allCallback(void *pData, int numFields, char **pFields, char **pColNames);

    public:
                                    Sql();
                                    ~Sql();

        void                        init();
        bool                        getIsActive();
        bool                        addUserData(std::string status, bool read, bool spot, bool transfer);
        bool                        addClosedKlinePrice(float candle);
};

#endif