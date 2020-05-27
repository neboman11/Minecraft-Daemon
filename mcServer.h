/*
 * Filename:	mcServer.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:	2020, All Rights Reserved
 *
 * Description:
 *
 *	Includes and prototypes for mcServer.cpp.
 */

#ifndef __MCSERVER_H_
#define __MCSERVER_H_

#include <unistd.h>
#include <pthread.h>
#include <thread>
#include "main.h"
#include "loggingFunc.h"
#include "helperFunc.h"

// Thread info struct
// Used for passing arguments to a thread
struct thread_info
{
    pthread_t   threadID;   // ID of the thread returned by pthread_create
    int         serverNum;  // Number of server to read log of
    FILE*       pipeFile;   // File pointer to the pipe for reading
    ServerLog*  logSpot;    // Spot in memory where the log for the MCServer object is located
};

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