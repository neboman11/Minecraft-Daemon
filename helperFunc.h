/*
 * Filename:		helperFunc.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Includes and prototypes for helperFunc.cpp.
 */

#ifndef __HELPERFUNC_H_
#define __HELPERFUNC_H_

#include <sys/stat.h>
#include "main.h"
#include "loggingFunc.h"
#include "commandFunc.h"

int verifyFolders();
void printConfig();
char* trimNewLine(char* s, int size);

#endif // __HELPERFUNC_H_
