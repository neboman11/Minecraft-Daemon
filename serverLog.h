/*
 * Filename:	serverLog.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:	2020, All Rights Reserved
 *
 * Description:
 *
 *	Header for serverLog class.
 */

#ifndef __SERVERLOG_H_
#define __SERVERLOG_H_

#include <list>
#include <string>
#include <sstream>

using namespace std;

class ServerLog
{
private:
    // The maximum number of lines of the log to hold
    unsigned long maxLogSize = 100;
    // The lines of the log
    list<string> log;
public:
    // Create a new serverLog with the default max size of 100
    ServerLog();
    // Create a new serverLog with the given max size
    ServerLog(unsigned long logSize);

    ~ServerLog();
    // Add a line to the log
    void addLine(string s);
    // Add a line to the log
    void addLine(char* s);
    // Get the log as a single string
    string getLog();
};

#endif // __SERVERLOG_H_
