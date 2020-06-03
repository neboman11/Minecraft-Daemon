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
#include <iostream>
#include "loggingFunc.h"
#include "helperFunc.h"
#include "serverLog.h"

class MCServer
{
private:
    // The unique identifier of the server
    int serverNum;
    // The server's log
    ServerLog* log;
    // Array of pipes for stdout and stdin for the child
    // Layout: row 0 - stdout pipe
    //         row 1 - stdin pipe
    //         col 0 - read end of pipe
    //         col 1 - write end of pipe
    int** serverPipe;
    // The thread for watching the server's log
    thread childThread;
    // Pointer to ostream for actively updating the server output to that stream
    ostream* activeOutput = nullptr;

    int spawnLogWatcher();
    int spawnChild(char* arguments[], string workDir);

public:
    // Create a new serverLog with the given max size
    MCServer(char* arguments[], string workDir, int childNum);

    ~MCServer();
    // Get the log as a single string
    string getLog();
    // Add a new line to the server log and write it to the activeOutput
    void addLogLine(string s);
    void addLogLine(char* s);
    // Stop the server
    void stop();
    // Create an interactive console for the server
    int interact(ostream* outputStream);
};

void readServerLog(FILE* pipeFile, MCServer* server);

#endif // __MCSERVER_H_