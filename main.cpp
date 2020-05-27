/*
 * Filename:		main.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
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
#include <iterator>
#include <pthread.h>
#include "cmdParse.h"
#include "readConfig.h"
#include "loggingFunc.h"
#include "signalHandling.h"
#include "commandFunc.h"
#include "helperFunc.h"
#include "mcServer.h"

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
int serverMenu();
int menuStart();
int menuLog();
int menuStop();

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

int serverMenu()
{
    char choice;
    string input;

    cout << "Welcome to Minecraft-Daemon." << endl;

    do
    {
        cout << "Choose an option:" << endl;
        cout << "[s] Start a server" << endl;
        cout << "[l] View a server's output" << endl;
        cout << "[k] Stop a running server" << endl;
        cout << "[0] Quit the daemon" << endl;

        getline(cin, input);

        choice = input[0];

        switch (choice)
        {
        case 's':
            menuStart();
            break;
        case 'l':
            menuLog();
            break;
        case 'k':
            menuStop();
            break;
        
        default:
            break;
        }
    }
    while (choice != '0');

    return 0;
}

int menuStart()
{
    string input;

    cout << "Enter a unique number for the server: ";
    getline(cin, input);

    int serverNum = stoi(input);

    bool duplicateID = false;

    for (auto i : serverIDs)
    {
        if (i == serverNum)
        {
            duplicateID = true;
        }
    }

    while (duplicateID)
    {
        cout << "That ID has already been used, please enter a different one: ";
        getline(cin, input);

        serverNum = stoi(input);

        duplicateID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                duplicateID = true;
            }
        }
    }

    serverIDs.push_back(serverNum);

    cout << "Enter the folder to run the server in: ";
    getline(cin, input);

    string serverDir = input;

    cout << "Enter the name of the server jar file: ";
    getline(cin, input);

    string serverJar = input;
    string serverPath = serverDir + "/" + serverJar;

    vector<char*> serverArgs;
    serverArgs.push_back((char*)"java");
    serverArgs.push_back((char*)"-jar");
    serverArgs.push_back((char*)serverPath.c_str());
    serverArgs.push_back((char*)"nogui");
    serverArgs.push_back((char*)nullptr);

    // Create a new server child process
    servers[serverNum] = new MCServer(serverArgs.data(), serverDir, serverNum);

    return 0;
}

int menuLog()
{
    string input;

    cout << "Enter the server number to view the log of: ";
    getline(cin, input);

    int serverNum = stoi(input);

    bool validID = false;

    for (auto i : serverIDs)
    {
        if (i == serverNum)
        {
            validID = true;
        }
    }

    while (!validID)
    {
        cout << "That ID does not exist, please enter a different one: ";
        getline(cin, input);

        serverNum = stoi(input);

        validID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                validID = true;
            }
        }
    }

    cout << servers[serverNum]->getLog() << endl;

    return 0;
}

int menuStop()
{
    string input;

    cout << "Enter the server number to stop: ";
    getline(cin, input);

    int serverNum = stoi(input);

    bool validID = false;

    for (auto i : serverIDs)
    {
        if (i == serverNum)
        {
            validID = true;
        }
    }

    while (!validID)
    {
        cout << "That ID does not exist, please enter a different one: ";
        getline(cin, input);

        serverNum = stoi(input);

        validID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                validID = true;
            }
        }
    }

    servers[serverNum]->stop();

    delete servers[serverNum];

    auto iter = serverIDs.begin();

    for (unsigned long i = 0; i < serverIDs.size(); i++)
    {
        if (serverIDs.at(i) == serverNum)
        {
            serverIDs.erase(iter);
        }

        iter++;
    }

    return 0;
}
