/*
 * Filename:		main.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Includes and definitions necessary for interacting with the global variables defined in main.cpp.
 */

#ifndef __MAIN_H_
#define __MAIN_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <limits.h>
#include "serverLog.h"

using namespace std;

// Enumerator for readable map indexing
enum flagOptions { DAEMON, CONFIG };
enum configEnum { VERBOSE, LOG_FILE, JAVA_PATH };

// Global Variables
extern ofstream* logFile;
extern map<int, string> inputOptions;
extern map<int, string> configOptions;

#endif // __MAIN_H_
