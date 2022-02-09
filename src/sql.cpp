#include "../inc/sql.h"
#include <cstdio>
#include <string>


/**
 * @brief Construct a new Sql::Sql object
 * 
 */
Sql::Sql()
{
    // Save the result of opening the file
    rc = sqlite3_open("crypto.sqlite", &db);

    if (rc)
    {
        // Show an error message
        ELOG(ERROR, "Can't open database: %s", sqlite3_errmsg(db));
        // Close the connection
        sqlite3_close(db);
        // Return an error
        exit(0);
    }


    /* Create USER Table */
    sql = "CREATE TABLE IF NOT EXISTS BOT("  \
        "ID                     INT PRIMARY KEY     NOT NULL," \
        "STATUS                 INT);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);

    if (rc != SQLITE_OK) 
    {
        ELOG(ERROR, "Can't create table: %s", sqlite3_errmsg(db));
    }


    /* Create USER Table */
    sql = "CREATE TABLE IF NOT EXISTS USER("  \
        "ID                     INT PRIMARY KEY     NOT NULL," \
        "STATUS                 TEXT," \
        "ENABLEREADING          INT," \
        "ENABLESPOTANDMARGIN    INT," \
        "ENABLETRANSFER         INT);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);

    if (rc != SQLITE_OK) 
    {
        ELOG(ERROR, "Can't create table: %s", sqlite3_errmsg(db));
    }


    /* Drop CLOSEDKLINES table*/
    sql = "DROP TABLE IF EXISTS CLOSEDKLINES";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        ELOG(ERROR, "Can't drop table: %s", sqlite3_errmsg(db));
    }


    /* Create CLOSEDKLINES Table */
    sql = "CREATE TABLE CLOSEDKLINES("  \
        "ID             INT PRIMARY KEY     NOT NULL," \
        "COINNAME       TEXT    NOT NULL," \
        "PRICE          REAL    NOT NULL," \
        "TIMESTAMP      DATETIME DEFAULT (datetime('now','localtime')));";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);

    if (rc != SQLITE_OK) 
    {
        ELOG(ERROR, "Can't create table: %s", sqlite3_errmsg(db));
    }


    // Close the SQL connection
    sqlite3_close(db);

    ELOG(INFO, "Sql constructor initialized.");
}


Sql::~Sql() {}


/**
 * @brief Sqlite callback
 * 
 * @param NotUsed 
 * @param argc 
 * @param argv 
 * @param azColName 
 * @return int 
 */
int Sql::sqliteCallback (void *NotUsed, int argc, char **argv, char **azColName)
{
    (void)NotUsed;
    (void)argc;
    (void)argv;
    (void)azColName;

    return 0;
}


bool Sql::addUserData(std::string status, bool read, bool spot, bool transfer)
{
    /* Open database */
    rc = sqlite3_open("crypto.sqlite", &db);
    
    if (rc) 
    {
        // Show an error message
        ELOG(ERROR, "Can't open database: %s", sqlite3_errmsg(db));
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return false;
    }

    /* Create SQL statement */
    sql = "INSERT INTO USER (ID, STATUS, ENABLEREADING, ENABLESPOTANDMARGIN, ENABLETRANSFER) "  \
            "VALUES (1, '" \
            + status + "', " \
            + std::to_string(read) + ", " \
            + std::to_string(spot) + ", " \
            + std::to_string(transfer) + ");";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        ELOG(ERROR, "SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return true;
}


/**
 * @brief Add closed kline price
 * 
 * @param candle 
 * @return true 
 * @return false 
 */
bool Sql::addClosedKlinePrice(float candle)
{
    /* Open database */
    rc = sqlite3_open("crypto.sqlite", &db);
    
    if (rc) 
    {
        // Show an error message
        ELOG(ERROR, "Can't open database: %s", sqlite3_errmsg(db));
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return false;
    }

    /* Create SQL statement */
    sql = "INSERT INTO CLOSEDKLINES (ID, COINNAME, PRICE) "  \
            "VALUES (" + std::to_string(mId++) + \
            ", 'SOLBUSD', " + std::to_string(candle) + ");";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqliteCallback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        ELOG(ERROR, "SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return true;
}