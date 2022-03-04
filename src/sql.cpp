#include "../inc/sql.h"


/**
 * @brief Construct a new Sql::Sql object
 * 
 */
Sql::Sql()
{
    // Create USER Table 
    sql = "CREATE TABLE IF NOT EXISTS BOT("  \
        "id                     INT PRIMARY KEY     NOT NULL," \
        "status                 INT,"   \
        "isActive               INT,"   \
        "symbol                 TEXT,"  \
        "balanceSymbol          TEXT);";

    allQuery(sql);

    // Add user data
    sql = "INSERT OR IGNORE INTO BOT(id, status, isActive, symbol, balanceSymbol)" \
          "VALUES(1, 1, 0, '', '');";

    allQuery(sql);


    // Create USER Table
    sql = "CREATE TABLE IF NOT EXISTS USER("  \
        "id                     INT PRIMARY KEY     NOT NULL," \
        "status                 TEXT," \
        "enableReading          INT," \
        "enableSpotAndMargin    INT," \
        "enableTransfer         INT);";

    allQuery(sql);

    // Drop CLOSEDKLINES table
    sql = "DROP TABLE IF EXISTS CLOSEDKLINES";

    allQuery(sql);

    // Create CLOSEDKLINES Table 
    sql = "CREATE TABLE CLOSEDKLINES("  \
        "timestamp          TEXT PRIMARY KEY     NOT NULL," \
        "openPrice          TEXT," \
        "closePrice         TEXT," \
        "highPrice          TEXT," \
        "lowPrice           TEXT," \
        "datelocal          DATETIME DEFAULT (datetime('now','localtime')));";

    allQuery(sql);

    ELOG(INFO, "Sql constructor initialized.");
}


/**
 * @brief Destroy the Sql::Sql object
 * 
 */
Sql::~Sql() {}


/**
 * @brief Sql init
 * 
 */
void Sql::init()
{
    while (true)
    {
        // getBOTTable();

        struct candle_data *pTradeCandleData = Opel::getTradeCandleStruct();

        pTradeCandleData->lock();
        if (pTradeCandleData->isUpdated)
        {
            std::cout << "Timestamp:" << pTradeCandleData->timestamp << std::endl;
            std::cout << "Symbol:" << pTradeCandleData->symbol << std::endl;
            std::cout << "Open:" << pTradeCandleData->openPrice << std::endl;
            std::cout << "Close:" <<  pTradeCandleData->closePrice << std::endl;
            std::cout << "High:" <<  pTradeCandleData->highPrice << std::endl;
            std::cout << "Low:" <<  pTradeCandleData->lowPrice << std::endl;
            std::cout << "isClosed:" <<  pTradeCandleData->isClosed << std::endl;

            addClosedKlinePrice(pTradeCandleData->timestamp, pTradeCandleData->openPrice, pTradeCandleData->closePrice, pTradeCandleData->highPrice, pTradeCandleData->lowPrice);

            pTradeCandleData->isUpdated = false;
        }
        pTradeCandleData->unlock();

        struct candle_data *pFollowCandleData = Opel::getFollowCandleStruct();

        pFollowCandleData->lock();
        if (pFollowCandleData->isUpdated)
        {
            std::cout << "Timestamp:" << pFollowCandleData->timestamp << std::endl;
            std::cout << "Symbol:" << pFollowCandleData->symbol << std::endl;
            std::cout << "Open:" << pFollowCandleData->openPrice << std::endl;
            std::cout << "Close:" <<  pFollowCandleData->closePrice << std::endl;
            std::cout << "High:" <<  pFollowCandleData->highPrice << std::endl;
            std::cout << "Low:" <<  pFollowCandleData->lowPrice << std::endl;
            std::cout << "isClosed:" <<  pFollowCandleData->isClosed << std::endl;

            addClosedKlinePrice(pFollowCandleData->timestamp, pFollowCandleData->openPrice, pFollowCandleData->closePrice, pFollowCandleData->highPrice, pFollowCandleData->lowPrice);

            pFollowCandleData->isUpdated = false;
        }
        pFollowCandleData->unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // ELOG(INFO, "getIsActive: %d", pOpel->getIsActive());

    }
}


/**
 * @brief Select query
 * 
 * @param query 
 * @return Record 
 */
Record Sql::selectQuery(std::string query)
{
    Record records;

    // Open database
    rc = sqlite3_open("crypto.sqlite", &db);
    
    if (rc) 
    {
        ELOG(ERROR, "Can't open database: %s", sqlite3_errmsg(db));

        sqlite3_close(db);
    }

    // Execute SQL statement
    rc = sqlite3_exec(db, query.c_str(), selectCallback, &records, &zErrMsg);

    if (rc != SQLITE_OK) 
    {
        ELOG(ERROR, "Error in select statement. Query: %s - Error message: %s", query.c_str(), sqlite3_errmsg(db));
    }

    // Close the connection
    sqlite3_close(db);

    return records;
}


/**
 * @brief All query
 * 
 * @param query 
 * @return true 
 * @return false 
 */
bool Sql::allQuery(std::string query)
{
    // Open database
    rc = sqlite3_open("crypto.sqlite", &db);

    if (rc)
    {
        ELOG(ERROR, "Can't open database: %s", sqlite3_errmsg(db));

        sqlite3_close(db);

        return false;
    }

    // Execute SQL statement
    rc = sqlite3_exec(db, query.c_str(), allCallback, 0, &zErrMsg);

    if (rc != SQLITE_OK) 
    {
        ELOG(ERROR, "Error in statement. Query: %s - Error message: %s", query.c_str(), sqlite3_errmsg(db));

        return false;
    }

    // Close the connection
    sqlite3_close(db);

    return true;
}


/**
 * @brief Select callback
 * 
 * @param NotUsed 
 * @param argc 
 * @param argv 
 * @param azColName 
 * @return int 
 */
int Sql::selectCallback(void *pData, int numFields, char **pFields, char **pColNames)
{
    Record     *records = static_cast<Record *>(pData);

    for (int i = 0; i < numFields; i++)
    {
        records->emplace_back(std::make_pair(static_cast<std::string>(pColNames[i]), static_cast<std::string>(pFields[i])));

        // ELOG(INFO, "Added to map. ColName: %s, Field: %s.", pColNames[i], pFields[i]);
    }

    return 0;
}


/**
 * @brief All callback
 * 
 * @param NotUsed 
 * @param argc 
 * @param argv 
 * @param azColName 
 * @return int 
 */
int Sql::allCallback(void *pData, int numFields, char **pFields, char **pColNames)
{
    (void)pData;
    (void)numFields;
    (void)pFields;
    (void)pColNames;

    return 0;
}


/**
 * @brief Get BOT table
 * 
 */
void Sql::getBOTTable()
{
    // Create SQL statement
    sql = "SELECT * FROM BOT WHERE id=1";

    ELOG(INFO, "Bot activation check. Query: %s.", sql.c_str());

    Record records          = selectQuery(sql);

    bool mIsActive          = std::stof(records[2].second);

    Opel *pOpel = Opel::instance();

    pOpel->setIsActive(mIsActive);
}


/**
 * @brief Add user data
 * 
 * @param status 
 * @param read 
 * @param spot 
 * @param transfer 
 * @return true 
 * @return false 
 */
bool Sql::addUserData(std::string status, bool read, bool spot, bool transfer)
{
    // Create SQL statement
    sql = "INSERT INTO USER (id, status, enableReading, enableSpotAndMargin, enableTransfer) "  \
            "VALUES (1, '" \
            + status + "', " \
            + std::to_string(read) + ", " \
            + std::to_string(spot) + ", " \
            + std::to_string(transfer) + ");";

    return allQuery(sql);
}


/**
 * @brief Add closed kline price
 * 
 * @param openPrice 
 * @param closePrice 
 * @param higPrice 
 * @param lowPrice 
 * @return true 
 * @return false 
 */
bool Sql::addClosedKlinePrice(std::string timestamp, std::string openPrice, std::string closePrice, std::string highPrice, std::string lowPrice)
{
    // Create SQL statement
    sql = "INSERT INTO CLOSEDKLINES (timestamp, openPrice, closePrice, highPrice, lowPrice) "  \
            "VALUES (" + timestamp + \
            ", " + openPrice + \
            ", " + closePrice + \
            ", " + highPrice + \
            ", " + lowPrice + ");";

    return allQuery(sql);
}