/*
 * Filename:		main.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
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
#include <limits.h>

using namespace std;

// Constants for inotify
#define BUFFER_LENGTH (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

// Enumerator for readable map indexing
enum flagOptions { DAEMON, CONFIG };
enum configEnum { VERBOSE, LOG_FILE, PASSWORD, NUM_VERSIONS, WATCH_DIR };

// Global Variables
extern ofstream* logFile;
extern map<int, string> inputOptions;
extern map<int, string> configOptions;

#endif // __MAIN_H_
