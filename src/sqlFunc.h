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
#include "loggingFunc.h"

int createDB();
int addServerDB(int serverNum, string serverName, string serverDir, string serverJar);

#endif // __SQLFUNC_H_