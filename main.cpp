/*
 * Filename:	main.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Daemon process for running Minecraft servers.
 */

#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "cmdParse.h"
#include "readConfig.h"
#include "loggingFunc.h"
#include "signalHandling.h"
#include "commandFunc.h"
#include "helperFunc.h"
#include "mcServer.h"
#include "serverMenu.h"

// Pointer to ofstream object that is the log file
ofstream* logFile;
// Map of the command line options given
map<int, string> inputOptions;
// Map of the config file options
map<int, string> configOptions;
// Map of child servers
map<int, MCServer*> servers;
// List of server IDs
vector<int> serverIDs;

// Function prototypes
void createPIDFile(pid_t mypid);
int initialize();

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

    serverMenu();

    // Tell the user what's happening
    writeLog("Exiting...");

    // Close the log file just in case
    logFile->close();

    // Free memory just in case
    delete logFile;

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

    // // If the process is to run as a daemon
    // if (inputOptions[DAEMON] == "true")
    // {
    //     // Fork the process and store the return value
    //     mypid = fork();

    //     // If fork returned a negative value
    //     if (mypid < 0)
    //     {
    //         // Write an error message to the screen
    //         cerr << "There was an error creating the fork. No child process was created. Exiting..." << endl;
    //         // Exit the program
    //         return 2;
    //     }

    //     // If fork returned a positive number
    //     else if (mypid > 0)
    //     {
    //         // This process is the parent so it exits
    //         exit(0);
    //     }
    // }

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
    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);

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
