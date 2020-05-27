/*
 * Filename:		commandFunc.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Includes and prototypes for commandFunc.cpp.
 */

#ifndef __COMMANDFUNC_H_
#define __COMMANDFUNC_H_

#include <string>
#include <stdio.h>
#include <sstream>

#define BUFFER_SIZE 512

using namespace std;

void runCommand(string command);

#endif // __COMMANDFUNC_H_
