/*
 * Filename:	main.h
 * Author:		Michael Nesbitt
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

using namespace std;

// Enumerator for readable map indexing
enum flagOptions { DAEMON, CONFIG };
enum configEnum { VERBOSE, LOG_FILE, JAVA_PATH, DATABASE, DATABASE_FILE };

// Global Variables
// Pointer to ofstream object that is the log file
extern ofstream* logFile;
// Map of the command line options given
extern map<int, string> inputOptions;
// Map of the config file options
extern map<int, string> configOptions;

#endif // __MAIN_H_
