/**
 * @file sql.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
#include <iterator>

// Namespaces
using Record            = std::vector<std::pair<std::string, std::string>>;
using OrderMap          = std::map<std::string, std::string>;
using AllOrdersMap      = std::map<int, OrderMap>;
using MapIterator       = std::map<int, OrderMap>::iterator;

class Sql
{
    private:
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
        void                        getBotTable();
        bool                        addUserData(std::string status, bool read, bool spot, bool transfer);
        bool                        addClosedKlinePrice(std::string timestamp, std::string openPrice, std::string closePrice, std::string highPrice, std::string lowPrice);
        
        bool                        addBoughtCoin(int id, std::string boughtsymbol, std::string boughtprice, std::string boughtquantity, std::string boughttimestamp);
        void                        getBoughtCoin();
        bool                        removeBoughtCoin(int id);
};

#endif