#include "../inc/sql.h"
#include <string>


/**
 * @brief Construct a new Sql::Sql object
 * 
 */
Sql::Sql()
{
    // Create USER Table 
    sql = "CREATE TABLE IF NOT EXISTS BOT("  \
        "id                     INT PRIMARY KEY     NOT NULL," \
        "status                 INT," \
        "isActive               INT);";

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
        "timestamp          INT PRIMARY KEY     NOT NULL," \
        "openPrice          REAL," \
        "closePrice         REAL," \
        "highPrice          REAL," \
        "lowPrice           REAL," \
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
        bool isActive = getIsActive();

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        pOpel->setIsActive(isActive);

        struct candle_data *pCandleData = pOpel->getCandleDataStruct();

        if (pCandleData->isUpdated && pCandleData->isClosed)
        {
            std::cout << "Timestamp:" << pCandleData->timestamp << std::endl;
            std::cout << "Open:" << pCandleData->openPrice << std::endl;
            std::cout << "Close:" <<  pCandleData->closePrice << std::endl;
            std::cout << "High:" <<  pCandleData->highPrice << std::endl;
            std::cout << "Low:" <<  pCandleData->lowPrice << std::endl;
            std::cout << "isClosed:" <<  pCandleData->isClosed << std::endl;

            addClosedKlinePrice(pCandleData->timestamp, pCandleData->openPrice, pCandleData->closePrice, pCandleData->highPrice, pCandleData->lowPrice);

            pCandleData->isUpdated = false;
        }

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
 * @brief Get is bot active
 * 
 * @return true 
 * @return false 
 */
bool Sql::getIsActive()
{
    // Create SQL statement
    sql = "SELECT * FROM BOT WHERE id=1";

    Record records = selectQuery(sql);

    ELOG(INFO, "Bot activation check. Query: %s.", sql.c_str());

    for (auto& record : records) {
        // ELOG(INFO, "Check vector data: Field: %s.", record[0]);
        // std::cout << "Key: " << record.first << " Value: " << record.second << std::endl;

        if (record.first == "isActive" && record.second == "1")
            return true;
    }

    return false;
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
bool Sql::addClosedKlinePrice(unsigned long long int timestamp, float openPrice, float closePrice, float highPrice, float lowPrice)
{
    // Create SQL statement
    sql = "INSERT INTO CLOSEDKLINES (timestamp, openPrice, closePrice, highPrice, lowPrice) "  \
            "VALUES (" + std::to_string(timestamp) + \
            ", " + std::to_string(openPrice) + \
            ", " + std::to_string(closePrice) + \
            ", " + std::to_string(highPrice) + \
            ", " + std::to_string(lowPrice) + ");";

    return allQuery(sql);
}