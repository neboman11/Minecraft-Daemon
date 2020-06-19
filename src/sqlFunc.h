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
void updateServerNameDB(int serverNum, string serverName);
void updateServerFolderDB(int serverNum, string serverDir);
void updateServerJarDB(int serverNum, string serverJar);
void updateServerRunMemoryDB(int serverNum, string runMemory);
void updateServerStartMemoryDB(int serverNum, string startMemory);
void updateServerJavaArgsDB(int serverNum, string javaArgs);

#endif // __SQLFUNC_H_
