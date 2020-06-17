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

enum TableFields { ID, NAME, DIRECTORY, JARFILE, RUN_MEMORY, START_MEMORY, JAVA_ARGS };

int createDB(string dbFile);
int addServerDB(int serverNum, string serverName, string serverDir, string serverJar, string runRAM, string startRAM, string javaArgs);
int loadIDs();
void queryServerData(int serverNum, map<int, string>& serverData);
void removeServerDB(int serverNum);

#endif // __SQLFUNC_H_
