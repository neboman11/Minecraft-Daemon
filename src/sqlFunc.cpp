/*
 * Filename:	sqlFunc.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for signalHandling.cpp
 */

#include "sqlFunc.h"

static int loadIDsCallback(void* NotUsed, int numCols, char** rowData, char** azColName)
{
   serverIDs.push_back(stoi(rowData[0]));
   writeLog(string("Found server: ") + rowData[1] + ", ID: " + rowData[0], ((long long)NotUsed || numCols || azColName || true));
   return 0;
}

static int queryServerDataCallback(void* _serverData, int numCols, char** rowData, char** azColName)
{
    map<int, string>* serverData = (map<int, string>*)_serverData;

    serverData->operator[](ID) = rowData[0];
    serverData->operator[](NAME) = rowData[1];
    serverData->operator[](DIRECTORY) = rowData[2];
    serverData->operator[](JARFILE) = rowData[3];
    serverData->operator[](RUN_MEMORY) = rowData[4];
    serverData->operator[](START_MEMORY) = rowData[5];

    if (rowData[6] == nullptr)
    {
        serverData->operator[](JAVA_ARGS) = "";
    }
    else
    {
        serverData->operator[](JAVA_ARGS) = rowData[6];
    }

    return 0;
}

int createDB(string dbFile)
{
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    string sql;

    rc = sqlite3_open(dbFile.c_str(), &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        writeLog("Created database successfully");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE Servers( \
        ID INT PRIMARY KEY     NOT NULL, \
        NAME           TEXT    NOT NULL, \
        DIRECTORY      TEXT    NOT NULL, \
        JARFILE        TEXT    NOT NULL, \
        RUN_MEMORY     TEXT    NOT NULL, \
        START_MEMORY   TEXT    NOT NULL, \
        JAVA_ARGS      TEXT \
        );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    
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

int addServerDB(int serverNum, string serverName, string serverDir, string serverJar, string runRAM, string startRAM, string javaArgs)
{
    sqlite3* db;
    int rc;

    rc = sqlite3_open(configOptions[DATABASE_FILE].c_str(), &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        writeLog("Opened database successfully", true);
    }

    string sql = "INSERT INTO Servers (ID, NAME, DIRECTORY, JARFILE, RUN_MEMORY, START_MEMORY, JAVA_ARGS) "
                 "VALUES (" + to_string(serverNum) + ", \"" + serverName + "\", \"" + serverDir + "\", \"" + serverJar + "\", \"" + runRAM + "\", \"" + startRAM + "\", " + ((javaArgs == "") ? "null" : ("\"" + javaArgs + "\"")) + ");";
    char* zErrMsg = 0;

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        writeLog("Error inserting into database: " + string(zErrMsg));
        sqlite3_free(zErrMsg);
        return 1;
    } 
    else
    {
        writeLog("Record created successfully!");
    }

    sqlite3_close(db);

    return 0;
}

int loadIDs()
{
    sqlite3* db;
    int rc;

    rc = sqlite3_open(configOptions[DATABASE_FILE].c_str(), &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        writeLog("Opened database successfully", true);
    }

    string sql = "SELECT * FROM Servers;";
    char* zErrMsg = 0;

    rc = sqlite3_exec(db, sql.c_str(), loadIDsCallback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        writeLog("Error querying database: " + string(zErrMsg));
        sqlite3_free(zErrMsg);
        return 1;
    } 
    else
    {
        writeLog("Records loaded successfully!", true);
    }

    sqlite3_close(db);

    return 0;
}

void queryServerData(int serverNum, map<int, string>& serverData)
{
    sqlite3* db;
    int rc;

    rc = sqlite3_open(configOptions[DATABASE_FILE].c_str(), &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
    }
    else
    {
        writeLog("Opened database successfully", true);
    }

    string sql = "SELECT * FROM Servers WHERE ID = " + to_string(serverNum) + ";";
    char* zErrMsg = 0;

    rc = sqlite3_exec(db, sql.c_str(), queryServerDataCallback, (void*)&serverData, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        writeLog("Error querying database: " + string(zErrMsg));
        sqlite3_free(zErrMsg);
    } 
    else
    {
        writeLog("Records loaded successfully!", true);
    }

    sqlite3_close(db);
}

void removeServerDB(int serverNum)
{
    sqlite3* db;
    int rc;

    rc = sqlite3_open(configOptions[DATABASE_FILE].c_str(), &db);

    if( rc )
    {
        writeLog(string("Can't open database: ") + sqlite3_errmsg(db));
    }
    else
    {
        writeLog("Opened database successfully", true);
    }

    string sql = "DELETE from Servers where ID = " + to_string(serverNum) + ";";
    char* zErrMsg = 0;

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        writeLog("Error querying database: " + string(zErrMsg));
        sqlite3_free(zErrMsg);
    } 
    else
    {
        writeLog("Records removed successfully!");
    }

    sqlite3_close(db);
}

// TODO: add function that changes the info for a server in the database
