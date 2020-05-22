/*
 * Filename:	loggingFunc.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:	2020, All Rights Reserved
 *
 * Description:
 *
 *	Includes and prototypes for loggingFunc.cpp.
 */

#ifndef __STARTSERVER_H_
#define __STARTSERVER_H_

#include <unistd.h>
#include "main.h"
#include "loggingFunc.h"

int startServer(string command, char* arguments[], string workDir);

#endif // __STARTSERVER_H_