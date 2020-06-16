/*
 * Filename:	sqlFunc.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for signalHandling.cpp
 */

#ifndef __SQLFUNC_H_
#define __SQLFUNC_H_

#include <sqlite3.h>
#include <vector>
#include "loggingFunc.h"

extern std::vector<int> serverIDs;

enum TableFields { ID, NAME, DIRECTORY, JARFILE };

int createDB(string dbFile);
int addServerDB(int serverNum, string serverName, string serverDir, string serverJar);
int loadIDs();
void queryServerData(int serverNum, map<int, string>& serverData);

// TODO: Write a function and callback that will query the database for a given server ID and place the data in a vector, map, etc

#endif // __SQLFUNC_H_
