/*
 * Filename:	mcServer.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Class representing a running server instance.
 */

#include "mcServer.h"

MCServer::MCServer(char* arguments[], string workDir, int childNum)
{
    this->serverNum = childNum;

    // Server log object to hold output of child
    log = new ServerLog();

    int status;

    status = spawnChild(arguments, workDir);

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        //return status;
    }

    status = spawnLogWatcher();

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        //return status;
    }

    //return 0;
}

MCServer::~MCServer()
{
    // TODO free used memory
    delete log;

    for (int i = 0; i < 2; i++)
    {
        delete[] serverPipe[i];
    }

    delete[] serverPipe;
}

int MCServer::spawnLogWatcher()
{
    // Open the pipe as a file for fgets (so it blocks)
    FILE* pipeOutFile = fdopen(serverPipe[0][0], "r");

    // Spawn a new thread to watch the log
    childThread = thread(readServerLog, pipeOutFile, log);

    return 0;
}

int MCServer::spawnChild(char* arguments[], string workDir)
{
    // PID type for storing the process's PID
    pid_t mypid;
    // Pipe for writing data to the child. This will become stdin in the child
    int wpipefd[2];
    // Pipe for reading data from the child. This will become stdout in the child
    int rpipefd[2];
    // Status for keeping track of errors
    int status;

    writeLog("Opening pipe for stdin", true);
    // Open the write pipe
    status = pipe(wpipefd);

    // If there was a failure opening the pipe
    if (status == -1)
    {
            perror("pipe");
            return 20;
    }

    writeLog("Opening pipe for stdout", true);
    // Open the read pipe
    status = pipe(rpipefd);

    // If there was a failure opening the pipe
    if (status == -1)
    {
            perror("pipe");
            return 20;
    }

    // Create 2D array from pipe arrays
    serverPipe = new int*[2];

    for (int i = 0; i < 2; i++)
    {
        serverPipe[i] = new int[2];
    }

    serverPipe[0][0] = wpipefd[0];
    serverPipe[0][1] = wpipefd[1];
    serverPipe[1][0] = rpipefd[0];
    serverPipe[1][1] = rpipefd[1];

    writeLog("Added pipes to global vector", true);
    // Add 2D array to global array of server pipes
    //serverPipes[childNum] = serverPipe;

    writeLog("Forking", true);
    // Fork the process and store the return value
    mypid = fork();

    // If fork returned a negative value
    if (mypid < 0)
    {
        // Write an error message to the screen
        writeLog("There was an error creating the fork. No child process was created. Exiting...");
        // Exit the program
        return 2;
    }

    // If fork returned a positive number
    else if (mypid > 0)
    {
        // This process is the parent
        writeLog("Child's process ID is " + to_string(mypid), true);
        // Close the write end of what will become stdout for the child
        close(serverPipe[0][1]);
        // Close the read end of what will become stdin for the child
        close(serverPipe[1][0]);
    }

    // If fork returned 0
    else
    {
        // This process is the child
        // Close the read end of what will become stdout
        close(serverPipe[0][0]);
        // Close the write end of what will become stdin
        close(serverPipe[1][1]);

        // Stdout and stdin should already be closed in main.cpp/initialize()
        // Duplicate the read pipe to stdin
        dup2(serverPipe[1][0], STDIN_FILENO);
        // Duplicate the write pipe to stdout
        dup2(serverPipe[0][1], STDOUT_FILENO);
        // Duplicate the write pipe to stderr
        dup2(serverPipe[0][1], STDERR_FILENO);

        // Int for keeping track of errors
        int status;

        // Change working directory of child
        status = chdir(workDir.c_str());

        if (status < 0)
        {
            exit(EXIT_FAILURE);
        }

        status = execv(configOptions[JAVA_PATH].c_str(), arguments);

        if (status < 0)
        {
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void MCServer::stop()
{
    // Message to send to child server
    const char* message = "stop\n";

    // Write the message to stdin of the child
    write(serverPipe[1][1], message, 6);

    // Wait for the thread to finish
    childThread.join();
}

string MCServer::getLog()
{
    return log->getLog();
}

int MCServer::interact()
{
    string input;

    cout << this->getLog();
    getline(cin, input);

    while (input != "exit")
    {
        if (input == "stop")
        {
            return 1;
        }
        
        // Write the message to stdin of the child
        write(serverPipe[1][1], (input + '\n').c_str(), input.length() + 1);

        sleep(5);

        cout << this->getLog();
        getline(cin, input);
    }

    return 0;
}

void readServerLog(FILE* pipeFile, ServerLog* log)
{
    // Array of characters to use as a buffer for reading from the pipe
    char buffer[BUFSIZ];
    // Pointer to the currently read in line from the pipe
    char* workingLine;

    // Read in the first buffer space of the pipe output
    workingLine = fgets(buffer, BUFSIZ, pipeFile);

    // Loop until there is nothing more to be read from the buffer
    while (workingLine != nullptr)
    {
        // Remove newline from workingLine
        trimNewLine(workingLine, BUFSIZ);
        // Add the contents of the current working line to the string stream
        log->addLine(workingLine);
        // Read in the next buffer space of the pipe output
        workingLine = fgets(buffer, BUFSIZ, pipeFile);
    }

    fclose(pipeFile);

    //return 0;
}
