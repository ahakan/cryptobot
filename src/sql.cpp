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
          "VALUES(1, 1, 1, '', '');";

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

    // Create Bought Coin Table 
    sql = "CREATE TABLE IF NOT EXISTS BOUGHTCOIN("  \
        "orderid            INT PRIMARY KEY     NOT NULL," \
        "symbol             TEXT," \
        "price              TEXT," \
        "quantity           TEXT," \
        "timestamp          TEXT," \
        "datelocal          DATETIME DEFAULT (datetime('now','localtime')));";

    allQuery(sql);

    ELOG(INFO, "Sql constructor initialized.");
}


/**
 * @brief Destroy the Sql::Sql object
 * 
 */
Sql::~Sql() 
{
    ELOG(INFO, "Sql destructor.");
}


/**
 * @brief Sql init
 * 
 */
void Sql::init()
{
    Opel *iOpel = Opel::instance();

    getBotTable();

    getBoughtCoin();

    while (iOpel->getExitSignal())
    {
        // struct candle_data *pTradeCandleData = Opel::getTradeCandleStruct();

        // pTradeCandleData->lock();
        // if (pTradeCandleData->isUpdated)
        // {
        //     std::cout << "Timestamp:" << pTradeCandleData->timestamp << std::endl;
        //     std::cout << "Symbol:" << pTradeCandleData->symbol << std::endl;
        //     std::cout << "Open:" << pTradeCandleData->openPrice << std::endl;
        //     std::cout << "Close:" <<  pTradeCandleData->closePrice << std::endl;
        //     std::cout << "High:" <<  pTradeCandleData->highPrice << std::endl;
        //     std::cout << "Low:" <<  pTradeCandleData->lowPrice << std::endl;
        //     std::cout << "isClosed:" <<  pTradeCandleData->isClosed << std::endl;

        //     // addClosedKlinePrice(pTradeCandleData->timestamp, pTradeCandleData->openPrice, pTradeCandleData->closePrice, pTradeCandleData->highPrice, pTradeCandleData->lowPrice);

        //     pTradeCandleData->isUpdated = false;
        // }
        // pTradeCandleData->unlock();

        // struct candle_data *pFollowCandleData = Opel::getFollowCandleStruct();

        // pFollowCandleData->lock();
        // if (pFollowCandleData->isUpdated)
        // {
        //     std::cout << "Timestamp:" << pFollowCandleData->timestamp << std::endl;
        //     std::cout << "Symbol:" << pFollowCandleData->symbol << std::endl;
        //     std::cout << "Open:" << pFollowCandleData->openPrice << std::endl;
        //     std::cout << "Close:" <<  pFollowCandleData->closePrice << std::endl;
        //     std::cout << "High:" <<  pFollowCandleData->highPrice << std::endl;
        //     std::cout << "Low:" <<  pFollowCandleData->lowPrice << std::endl;
        //     std::cout << "isClosed:" <<  pFollowCandleData->isClosed << std::endl;

        //     // addClosedKlinePrice(pFollowCandleData->timestamp, pFollowCandleData->openPrice, pFollowCandleData->closePrice, pFollowCandleData->highPrice, pFollowCandleData->lowPrice);

        //     pFollowCandleData->isUpdated = false;
        // }
        // pFollowCandleData->unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Record all bought coins
    if (!iOpel->getExitSignal())
    {
        AllOrdersMap *mBoughtOrders = iOpel->getBoughtOrdersMap();
        AllOrdersMap *mSellOrders   = iOpel->getSellOrdersMap();

        if (mBoughtOrders->size() > 0)
        {
            for (MapIterator order = mBoughtOrders->begin(); order != mBoughtOrders->end(); ++order)
            {
                int orderId             = order->first;
                std::string symbol      = order->second["Symbol"];
                std::string price       = order->second["BoughtPrice"];
                std::string quantity    = order->second["Quantity"];
                std::string time        = order->second["BoughtTime"];

                addBoughtCoin(orderId, symbol, price, quantity, time);
            }
        }

        if (mSellOrders->size() > 0)
        {
            for (MapIterator order = mSellOrders->begin(); order != mSellOrders->end(); ++order)
            {
                int orderId             = order->first;
                std::string symbol      = order->second["Symbol"];
                std::string price       = order->second["BoughtPrice"];
                std::string quantity    = order->second["Quantity"];
                std::string time        = order->second["BoughtTime"];

                addBoughtCoin(orderId, symbol, price, quantity, time);

                ELOG(INFO, "Added sell coin. Order id: %d, Symbol: %s, Price: %s.", orderId, symbol.c_str(), price.c_str());
            }
        }
    }

    ELOG(INFO, "Thread SQL detached.");
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
void Sql::getBotTable()
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


/**
 * @brief Add bought coin
 * 
 * @param orderid 
 * @param symbol 
 * @param price 
 * @param quantity 
 * @param timestamp 
 * @return true 
 * @return false 
 */
bool Sql::addBoughtCoin(int id, std::string boughtsymbol, std::string boughtprice, std::string boughtquantity, std::string boughttimestamp)
{
    // Create SQL statement
    sql = "INSERT INTO BOUGHTCOIN (orderid, symbol, price, quantity, timestamp) "  \
            "VALUES (" + std::to_string(id) + \
            ", '" + boughtsymbol + \
            "', '" + boughtprice + \
            "', '" + boughtquantity + \
            "', '" + boughttimestamp + "');";

    ELOG(INFO, "Added bought coin. Order id: %d, Symbol: %s, Price: %s, Quantity: %s.", id, boughtsymbol.c_str(), boughtprice.c_str(), boughtquantity.c_str());

    return allQuery(sql);
}


/**
 * @brief Get bought coin
 * 
 */
void Sql::getBoughtCoin()
{
    // Create SQL statement
    sql = "SELECT * FROM BOUGHTCOIN";

    ELOG(INFO, "Bot activation check. Query: %s.", sql.c_str());

    Record records          = selectQuery(sql);

    int recordSize  = 6;
    int orderSize   = static_cast<int>(records.size())/6;

    for(int i = 0; i < orderSize; i++)
    {
        int orderId             = std::stoi(records[i*recordSize].second);
        std::string symbol      = records[i*recordSize + 1].second;
        std::string price       = records[i*recordSize + 2].second;
        std::string quantity    = records[i*recordSize + 3].second;
        std::string timestamp   = records[i*recordSize + 4].second;


        OrderMap order;

        order.emplace("Symbol", symbol);
        order.emplace("BoughtPrice", price);
        order.emplace("Quantity", quantity);
        order.emplace("BoughtTime", timestamp);

        Opel *iOpel = Opel::instance();

        AllOrdersMap *mBoughtOrders = iOpel->getBoughtOrdersMap();

        mBoughtOrders->emplace(orderId, order);

        // Remove bought coin
        removeBoughtCoin(orderId);

        ELOG(INFO, "Bought Coin Added to Bought Orders Map. Order id: %d, Symbol: %s, Price: %s, Quantity: %s.", orderId, symbol.c_str(), price.c_str(), quantity.c_str());
    }
}


/**
 * @brief Remove bought coin
 * 
 * @param orderid 
 * @return true 
 * @return false 
 */
bool Sql::removeBoughtCoin(int id)
{
    // Create SQL statement
    sql = "DELETE FROM BOUGHTCOIN "  \
            "WHERE orderid = " + std::to_string(id) + ";";

    ELOG(INFO, "Bought Coin Removed to Database. Order id: %d", id);

    return allQuery(sql);
}
