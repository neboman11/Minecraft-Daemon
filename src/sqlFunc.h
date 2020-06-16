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

int createDB(string dbFile);
int addServerDB(int serverNum, string serverName, string serverDir, string serverJar);
int loadIDs();

#endif // __SQLFUNC_H_
