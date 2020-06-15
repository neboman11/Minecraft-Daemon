/*
 * Filename:	sqlFunc.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for signalHandling.cpp
 */

#include "sqlFunc.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i = 0; i<argc; i++)
   {
      writeLog(string(azColName[i]) + " = " + (argv[i] ? argv[i] : "NULL"));
   }
   writeLog("\n", ((long long)NotUsed && false));
   return 0;
}

// TODO: when does this need to be called?
// Stat the database file in initialization in main and create it if it doesn't exist?
// Installer program/script?
int createDB()
{
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    string sql;

    rc = sqlite3_open("test.db", &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
        return(1);
    }
    else
    {
        writeLog("Opened database successfully");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE Servers( \
        ID INT PRIMARY KEY     NOT NULL, \
        NAME           TEXT    NOT NULL, \
        DIRECTORY      TEXT    NOT NULL, \
        JARFILE        TEXT    NOT NULL \
        );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        writeLog(string("SQL error: ") + zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        writeLog("Table created successfully");
    }

    sqlite3_close(db);

    return 0;
}

int addServerDB(int serverNum, string serverName, string serverDir, string serverJar)
{
    sqlite3* db;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        writeLog("Opened database successfully");
    }

    string sql = "INSERT INTO Servers (ID, NAME, DIRECTORY, JARFILE) "
                 "VALUES (" + to_string(serverNum) + ", \"" + serverName + "\", \"" + serverDir + "\", \"" + serverJar + "\");";
    char* zErrMsg = 0;

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        writeLog("Error Inserting into database: " + string(zErrMsg));
        sqlite3_free(zErrMsg);
        return 1;
    } 
    else
    {
        writeLog("Record created Successfully!");
    }

    sqlite3_close(db);

    return 0;
}
