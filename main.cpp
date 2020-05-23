/*
 * Filename:		main.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Daemon process for watching a directory specified in a config file. It monitors for modify events in the directory and creates copies of files when they are modified.
 */

#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <pthread.h>
#include "cmdParse.h"
#include "readConfig.h"
#include "loggingFunc.h"
#include "signalHandling.h"
#include "commandFunc.h"
#include "helperFunc.h"
#include "startServer.h"
#include "serverLog.h"

// Pointer to ofstream object that is the log file
ofstream* logFile;
// Map of the command line options given
map<int, string> inputOptions;
// Map of the config file options
map<int, string> configOptions;
// Vector of arrays of pipes for each running server
// Each entry in the vector is a 2x2 array of file descriptors for pipes.
// The [0][1] and [1][0] pipes will always be closed from this end
vector<int**> serverPipes;
// Vector of child log objects
vector<ServerLog*> childLogs;

struct thread_info
{
    pthread_t   threadID;   // ID of the thread returned by pthread_create
    int         serverNum;  // Number of server to read log of
    FILE*       pipeFile;   // File pointer to the pipe for reading
};

// Function prototypes
void createPIDFile(pid_t mypid);
int initialize();
static void * readServerLog(void* arg);

int main (int argc, char** argv)
{
    // Register signal SIGINT and signal handler
    signal(SIGINT, interruptSignal);

    // Register signal SIGHUP and signal handler
    signal(SIGHUP, hangupSignal);

    // Parse the command line options
    inputOptions = cmdParse(argc, argv);

    // Initialize the program
    int status = initialize();

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        return status;
    }

    // Arguments to be passed to java for running the server
    char* testArgs[2];
    testArgs[0] = (char*)"java";
    testArgs[1] = (char*)"-version";

    // Directory to run the server in
    string workDir = "/home/nesbitt/Downloads/testserver/";

    // Create a new server child process
    // This will push_back a 2D array of pipes to serverPipes
    status = startServer(configOptions[JAVA_PATH], testArgs, workDir);

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        return status;
    }

    writeLog("Printing stdout of child to log", true);

    // Child number to watch the log for
    int childNum = 0;
    // Thread info struct to hold the arguments to be passed to the created thread
    thread_info* tinfo = new thread_info;
    // Attributes for the thread
    pthread_attr_t attr;

    // Initialize the thread attributes
    status = pthread_attr_init(&attr);

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        return status;
    }

    // Set the child number to watch the log of
    tinfo->serverNum = childNum;

    // Open the pipe as a file for fgets (so it blocks)
    FILE* pipeOutFile = fdopen(serverPipes.at(tinfo->serverNum)[0][0], "r");

    // Set the pipefile in the struct to pass to the thread
    tinfo->pipeFile = pipeOutFile;

    // Spawn a new thread to watch the log
    status = pthread_create(&tinfo->threadID, &attr, &readServerLog, &tinfo);

    // If an error occurred during initialization
    if (status != 0)
    {
        // Return the error value
        return status;
    }

    // Wait for the server to finish starting up
    sleep(15);

    // Write the child log to the daemon log
    writeLog(childLogs.at(childNum)->getLog());

    // Message to send to child server
    const char* message = "stop\n";

    // Write the message to stdin of the child
    write(serverPipes.at(childNum)[1][1], message, 6);

    // Wait for the server to stop
    sleep(5);

    // Write the child log to the daemon log
    writeLog(childLogs.at(childNum)->getLog());

    // Tell the user what's happening
    writeLog("Exiting...");

    // // Close the files used for reading from the child pipes
    // for (auto file : pipeFiles)
    // {
    //     fclose(file);
    // }

    // Close the log file just in case
    logFile->close();

    // Free memory just in case
    delete logFile;
    for (auto pipe : serverPipes)
    {
        for (int i = 0; i < 2; i++)
        {
            delete[] pipe[i];
        }
        delete[] pipe;
    }

    // Remove the PID file
    runCommand("rm -f minecraft-daemon.pid");

    // Exit
    return 0;
}

void createPIDFile(pid_t mypid)
{
    // Stream for creating and writing to the file
    ofstream PIDFile("minecraft-daemon.pid");

    // Write the PID to the file
    PIDFile << mypid << endl;

    // Close the file
    PIDFile.close();
}

int initialize()
{
    // PID type for storing the process's PID
    pid_t mypid;

    // Tell the user what's happening
    cout << "Checking for existing PID file." << endl;

    // Stat struct for using the stat function to determine if the file exists
    struct stat fileStatus;
    // Stat the PID file to see if it exists
    int temp = stat("minecraft-daemon.pid", &fileStatus);

    // If stat returned anything other than -1, the file already exists
    if (temp != -1)
    {
        // Write an error message to the screen
        cerr << "PID file already exists." << endl;
        // Exit the program
        return 6;
    }

    // If the process is to run as a daemon
    if (inputOptions[DAEMON] == "true")
    {
        // Fork the process and store the return value
        mypid = fork();

        // If fork returned a negative value
        if (mypid < 0)
        {
            // Write an error message to the screen
            cerr << "There was an error creating the fork. No child process was created. Exiting..." << endl;
            // Exit the program
            return 2;
        }

        // If fork returned a positive number
        else if (mypid > 0)
        {
            // This process is the parent so it exits
            exit(0);
        }
    }

    // Get the current process's PID
    mypid = getpid();

    // Tell the user what's happening
    cout << "Reading the config file at " << inputOptions[CONFIG] << endl;

    // Read the config file
    configOptions = readConfig(inputOptions[CONFIG]);

    // Tell the user what's happening
    cout << "Read the config file." << endl;

    // Open the logfile
    logFile = new ofstream(configOptions[LOG_FILE].c_str());

    // If the logfile failed to open
    if (!logFile)
    {
        // Write an error message to the screen
        cerr << "Failed to open log file." << endl;
        // Exit the program
        return 1;
    }

    // Tell the user what's happening
    cout << "Opened the log file." << endl;

    // Close standard inputs and outputs
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // String stream for converting and integer to a string
    stringstream itos;
    // Put the integer into the ss
    itos << mypid;

    // Tell the user what's happening
    writeLog("Creating PID file with PID: " + itos.str(), true);

    // Create the PID file
    createPIDFile(mypid);

    // Write the read in config settings to the log
    printConfig();

    // Return to where it was called
    return 0;
}

void* readServerLog(void* arg)
{
    thread_info* tinfo = (thread_info*)arg;

    // PIPE STDOUT OF CHILD
    // Server log object to hold output of child
    ServerLog* childLog = new ServerLog();
    // Add the childLog to the global vector
    childLogs.push_back(childLog);
    // Array of characters to use as a buffer for reading from the pipe
    char buffer[BUFSIZ];
    // Pointer to the currently read in line from the pipe
    char* workingLine;

    // Read in the first buffer space of the pipe output
    workingLine = fgets(buffer, BUFSIZ, tinfo->pipeFile);

    // Loop until there is nothing more to be read from the buffer
    while (workingLine > 0)
    {
        // Remove newline from workingLine
        trimNewLine(workingLine, BUFSIZ);
        // Add the contents of the current working line to the string stream
        childLog->addLine(workingLine);
        // Read in the next buffer space of the pipe output
        workingLine = fgets(buffer, BUFSIZ, tinfo->pipeFile);

        // TODO fgets can't return a negative value
        // If fgets returns a negative value, an error occured while reading from the pipe
        if (workingLine < 0)
        {
            writeLog("Error reading from pipe");
            break;
        }
    }

    fclose(tinfo->pipeFile);

    return 0;
}
