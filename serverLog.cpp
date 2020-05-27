/*
 * Filename:	serverLog.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:	2020, All Rights Reserved
 *
 * Description:
 *
 *	Data structure for holding the log of a server.
 */

#include "serverLog.h"

ServerLog::ServerLog()
{
}

ServerLog::ServerLog(unsigned long logSize)
{
    this->maxLogSize = logSize;
}

ServerLog::~ServerLog()
{
}

void ServerLog::addLine(string s)
{
    log.push_back(s);

    if (log.size() > maxLogSize)
    {
        log.pop_front();
    }
}

void ServerLog::addLine(char* s)
{
    log.push_back(s);

    if (log.size() > maxLogSize)
    {
        log.pop_front();
    }
}


string ServerLog::getLog()
{
    stringstream logCat;

    for (auto s : log)
    {
        logCat << s << endl;
    }

    return logCat.str();
}
