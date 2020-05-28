/*
 * Filename:	mcServer.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for mcServer.cpp.
 */

#ifndef __MCSERVER_H_
#define __MCSERVER_H_

#include <unistd.h>
#include <thread>
#include "loggingFunc.h"
#include "helperFunc.h"
#include "serverLog.h"

class MCServer
{
private:
    // The maximum number of lines of the log to hold
    int serverNum;
    // The lines of the log
    ServerLog* log;
    // Array of pipes for stdout and stdin for the child
    // Layout: row 0 - stdout pipe
    //         row 1 - stdin pipe
    //         col 0 - read end of pipe
    //         col 1 - write end of pipe
    int** serverPipe;
    thread childThread;

    int spawnLogWatcher();
    int spawnChild(char* arguments[], string workDir);

public:
    // Create a new serverLog with the given max size
    MCServer(char* arguments[], string workDir, int childNum);

    ~MCServer();
    // Get the log as a single string
    string getLog();
    // Stop the server
    void stop();
};

void readServerLog(FILE* pipeFile, ServerLog* log);

#endif // __MCSERVER_H_