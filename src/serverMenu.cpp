/*
 * Filename:	serverMenu.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	
 */

#include "serverMenu.h"

// Function prototypes
void menuStart(int serverNum);
void menuListRunning();
int menuLog();
int menuInteract();
int menuStop(int serverID);
void menuCreate();
void menuList();
void menuListStopped();
void menuRemove();
void menuUpdate();
void menuPrint();

// TODO: Need a way to validate run memory and start memory input since they are in a "human readable" format

int serverMenu()
{
    char choice;
    string input;

    cout << "Welcome to Minecraft-Daemon." << endl;

    do
    {
        cout << "Choose an option:" << endl;
        cout << "[c] Create a new server" << endl;
        cout << "[s] Start an existing server" << endl;
        cout << "[l] List all servers" << endl;
        cout << "[m] List all running servers" << endl;
        cout << "[n] List all stopped servers" << endl;
        cout << "[o] View a server's output" << endl;
        cout << "[i] Interact with a running server" << endl;
        cout << "[u] Change a server's settings" << endl;
        cout << "[p] Print a server's current settings" << endl;
        cout << "[k] Stop a running server" << endl;
        cout << "[r] Remove a server" << endl;
        cout << "[0] Quit the daemon" << endl;

        getline(cin, input);

        choice = input[0];

        switch (choice)
        {
        case 'c':
            menuCreate();
            break;
        case 's':
            if (serverIDs.size() > 0 && serverIDs.size() > runningServers.size())
            {
                menuStart(-1);
            }

            else if (serverIDs.size() == 0)
            {
                cout << "No servers have been created. Create a server first." << endl;
            }

            else
            {
                cout << "All available servers are currently running." << endl;
            }
            break;
        case 'l':
            if (serverIDs.size() > 0)
            {
                menuList();
            }

            else
            {
                cout << "No servers have been created. Create a server first." << endl;
            }
            break;
        case 'm':
            if (runningServers.size() > 0)
            {
                menuListRunning();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }
            break;
        case 'n':
            if (serverIDs.size() > 0 && serverIDs.size() > runningServers.size())
            {
                menuListStopped();
            }

            else if (serverIDs.size() == 0)
            {
                cout << "No servers have been created. Create a server first." << endl;
            }

            else
            {
                cout << "All available servers are currently running." << endl;
            }
            break;
        case 'o':
            if (runningServers.size() > 0)
            {
                menuLog();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }
            break;
        case 'i':
            if (runningServers.size() > 0)
            {
                menuInteract();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }
            break;
        case 'u':
            if (serverIDs.size() > 0)
            {
                menuUpdate();
            }

            else
            {
                cout << "No servers have been created. Create a server first." << endl;
            }
            break;
        case 'p':
            if (serverIDs.size() > 0)
            {
                menuPrint();
            }

            else
            {
                cout << "No servers have been created. Create a server first." << endl;
            }
            break;
        case 'k':
            if (runningServers.size() > 0)
            {
                menuStop(-1);
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }
            break;
        case 'r':
            if (serverIDs.size() > 0)
            {
                menuRemove();
            }

            else
            {
                cout << "No servers have been created. Create a server first." << endl;
            }
            break;
        case '0':
            break;
        default:
            cout << "Invalid option, please choose an option:" << endl;
            break;
        }
    }
    while (choice != '0');

    return 0;
}

void menuStart(int serverNum)
{
    if (serverNum == -1)
    {
        cout << "Enter the server number to start: ";

        serverNum = getValidStoppedServer();

        if (serverNum == -1)
        {
            return;
        }
    }

    writeLog("Starting server " + serverNum);
    // Create a new server child process
    servers[serverNum] = new MCServer(serverNum);

    runningServers.push_back(serverNum);
}

void menuListRunning()
{
    cout << "These are all the currently running servers:" << endl;

    for (auto i : runningServers)
    {
        map<int, string> serverData;
            queryServerData(i, serverData);
            cout << "  " + serverData[NAME] + " - Server Number: " << serverData[ID] << ", Server folder: " << serverData[DIRECTORY] << ", Server jar: " << serverData[JARFILE] << endl;
    }
}

int menuLog()
{
    cout << "Enter the server number to view the log of: ";

    int serverNum = getValidRunningServer();

    if (serverNum == -1)
    {
        return 0;
    }

    cout << servers[serverNum]->getLog() << endl;

    return 0;
}

int menuInteract()
{
    cout << "Enter the server number to interact with: ";

    int serverNum = getValidRunningServer();

    if (serverNum == -1)
    {
        return 0;
    }

    int status = servers[serverNum]->interact(&cout);

    if (status == 1)
    {
        menuStop(serverNum);
    }

    return 0;
}

int menuStop(int serverNum)
{
    if (serverNum == -1)
    {
        cout << "Enter the server number to stop: ";

        serverNum = getValidRunningServer();

        if (serverNum == -1)
        {
            return 0;
        }
    }

    map<int, string> serverData;

    // Query server data from database
    queryServerData(serverNum, serverData);

    writeLog("Shutting down server: " + serverData[NAME]);

    servers[serverNum]->stop();

    delete servers[serverNum];

    auto iter = runningServers.begin();

    for (unsigned long i = 0; i < runningServers.size(); i++)
    {
        if (runningServers.at(i) == serverNum)
        {
            runningServers.erase(iter);
        }

        iter++;
    }

    return 0;
}

void menuCreate()
{
    cout << "Enter a name for the server: ";

    string serverName;

    getline(cin, serverName);

    cout << "Enter a unique number for the server: ";

    int serverNum = getNumberFromUser();

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

        serverNum = getNumberFromUser();

        duplicateID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                duplicateID = true;
            }
        }
    }

    string serverDir;

    cout << "Enter the folder to run the server in: ";
    getline(cin, serverDir);

    // Stat struct for using the stat function to determine if the file exists
    struct stat fileStatus;
    // Stat the PID file to see if it exists
    int temp = stat(serverDir.c_str(), &fileStatus);
    // Whether or not the user entered a valid directory
    bool validDir = false;

    // If stat returned anything other than -1, the directory exists
    if (temp != -1)
    {
        // Set validDir to true
        validDir = true;
    }

    while (!validDir)
    {
        cout << "That folder does not exist, please enter a different directory: ";
        getline(cin, serverDir);

        // Stat the PID file to see if it exists
        temp = stat(serverDir.c_str(), &fileStatus);

        // If stat returned anything other than -1, the directory exists
        if (temp != -1)
        {
            // Set validDir to true
            validDir = true;
        }
    }

    string serverJar;

    cout << "Enter the name of the server jar file: ";
    getline(cin, serverJar);

    string serverPath = serverDir + "/" + serverJar;

    // Stat the PID file to see if it exists
    temp = stat(serverPath.c_str(), &fileStatus);
    // Whether or not the user entered a valid directory
    validDir = false;

    // If stat returned anything other than -1, the directory exists
    if (temp != -1)
    {
        // Set validDir to true
        validDir = true;
    }

    while (!validDir)
    {
        cout << "That file does not exist, please enter a valid server jar file: ";
        getline(cin, serverJar);

        serverPath = serverDir + "/" + serverJar;

        // Stat the PID file to see if it exists
        temp = stat(serverPath.c_str(), &fileStatus);

        // If stat returned anything other than -1, the directory exists
        if (temp != -1)
        {
            // Set validDir to true
            validDir = true;
        }
    }

    cout << "Enter the maximum amount of RAM to allocate for the server [1G]: ";

    string runRAM;
    getline(cin, runRAM);

    if (runRAM == "")
    {
        runRAM = "1G";
    }

    cout << "Enter the amount of RAM to start the server with [256M]: ";

    string startRAM;
    getline(cin, startRAM);

    if (startRAM == "")
    {
        startRAM = "256M";
    }

    cout << "Would you like to enter addition arguments for Java? (y/n): ";

    bool extraArgs = getYesOrNo();
    string javaArgs = "";

    if (extraArgs)
    {
        cout << "Enter all the arguments on a single line: ";
        getline(cin, javaArgs);
    }

    writeLog("Adding server to the database");

    addServerDB(serverNum, serverName, serverDir, serverJar, runRAM, startRAM, javaArgs);

    serverIDs.push_back(serverNum);

    bool startServer = false;

    cout << "Server was successfully created. Would you like to start it now? (y/n): ";

    startServer = getYesOrNo();

    if (startServer)
    {
        menuStart(serverNum);
    }
}

void menuList()
{
    menuListRunning();
    menuListStopped();
}

void menuListStopped()
{
    cout << "These are all the current stopped servers:" << endl;

    for (auto i : serverIDs)
    {
        bool runningID = false;
        for (auto j : runningServers)
        {
            if (i == j)
            {
                runningID = true;
            }
        }

        if (!runningID)
        {
            map<int, string> serverData;
            queryServerData(i, serverData);
            cout << "  " + serverData[NAME] + " - Server Number: " << serverData[ID] << ", Server folder: " << serverData[DIRECTORY] << ", Server jar: " << serverData[JARFILE] << endl;
        }
    }
}

void menuRemove()
{
    cout << "Enter the server number to remove: ";

    int serverNum = getValidServer();

    bool runningID = false;
    for (auto i : runningServers)
    {
        if (i == serverNum)
        {
            runningID = true;
        }
    }

    if (runningID)
    {
        cout << endl << "WARNING: Server is currently running!!" << endl;
        cout << "It will be stopped before it is removed." << endl << endl;
    }

    cout << "Are you sure you want to remove this server? (y/n): ";

    bool choice = getYesOrNo();

    if (choice)
    {
        if (runningID)
        {
            menuStop(serverNum);
        }

        removeServerDB(serverNum);

        auto iter = serverIDs.begin();

        for (unsigned long i = 0; i < serverIDs.size(); i++)
        {
            if (serverIDs.at(i) == serverNum)
            {
                serverIDs.erase(iter);
            }

            iter++;
        }
    }
}

void menuUpdate()
{
    cout << "Enter the server number to update: ";

    int serverNum = getValidServer();

    bool runningID = false;
    for (auto i : runningServers)
    {
        if (i == serverNum)
        {
            runningID = true;
        }
    }

    char choice;
    string input;

    // Determine which setting to update
    cout << "Which setting would you like to change?" << endl;
    cout << "[n] Server name" << endl;
    cout << "[f] Server folder" << endl;
    cout << "[j] Server jar file" << endl;
    cout << "[r] Server run memory" << endl;
    cout << "[s] Server start memory" << endl;
    cout << "[a] Server java args" << endl;

    getline(cin, input);

    choice = input[0];

    switch (choice)
    {
    case 'n':
        cout << "Enter the new name for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "NAME", input);
        break;
    case 'f':
        cout << "Enter the new folder for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "DIRECTORY", input);
        break;
    case 'j':
        cout << "Enter the new jar file for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "JARFILE", input);
        break;
    case 'r':
        cout << "Enter the new run memory for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "RUN_MEMORY", input);
        break;
    case 's':
        cout << "Enter the new start memory for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "START_MEMORY", input);
        break;
    case 'a':
        cout << "Enter the new java args for the server: ";
        getline(cin, input);

        updateServerFieldDB(serverNum, "JAVA_ARGS", input);
        break;
    default:
        cout << "Invalid input, returning to main menu." << endl;
        break;
    }

    if (runningID)
    {
        cout << endl << "WARNING: Server is currently running!!" << endl;
        cout << "Would you like to restart the server with the new settings? (y/n): ";

        bool choice = getYesOrNo();

        if (choice)
        {
            if (runningID)
            {
                menuStop(serverNum);
            }

            menuStart(serverNum);
        }
    }

    
}

void menuPrint()
{
    cout << "Enter the server number to print the settings for: ";

    int serverNum = getValidServer();

    map<int, string> serverData;

    // Query server data from database
    queryServerData(serverNum, serverData);

    cout << "Name: " << serverData[NAME] << endl;
    cout << "Directory: " << serverData[DIRECTORY] << endl;
    cout << "Jar File: " << serverData[JARFILE] << endl;
    cout << "Run Memory: " << serverData[RUN_MEMORY] << endl;
    cout << "Start Memory: " << serverData[START_MEMORY] << endl;
    cout << "Java Args: " << serverData[JAVA_ARGS] << endl;

    printServerSettings(serverData[DIRECTORY]);
}

int getNumberFromUser()
{
    int serverNum;
    bool validNum = true;
    string input;

    getline(cin, input);

    try
    {
        serverNum = stoi(input);
    }
    catch(const invalid_argument& e)
    {
        validNum = false;
    }

    while (!validNum)
    {
        cout << "Invalid input, please enter a number: ";
        getline(cin, input);

        validNum = true;

        try
        {
            serverNum = stoi(input);
        }
        catch(const invalid_argument& e)
        {
            validNum = false;
        }
    }

    return serverNum;
}

int getValidServer()
{
    int serverNum = getNumberFromUser();
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

        serverNum = getNumberFromUser();

        validID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                validID = true;
            }
        }
    }

    return serverNum;
}

int getValidRunningServer()
{
    int serverNum = getNumberFromUser();

    bool validID = false;

    for (auto i : runningServers)
    {
        if (i == serverNum)
        {
            validID = true;
        }
    }

    if (!validID)
    {
        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                cout << "That server is currently stopped." << endl;
                return -1;
            }
        }
    }

    while (!validID)
    {
        cout << "That ID does not exist, please enter a different one: ";

        serverNum = getNumberFromUser();

        validID = false;

        for (auto i : runningServers)
        {
            if (i == serverNum)
            {
                validID = true;
            }
        }

        if (!validID)
        {
            for (auto i : serverIDs)
            {
                if (i == serverNum)
                {
                    cout << "That server is currently stopped." << endl;
                    return -1;
                }
            }
        }
    }

    return serverNum;
}

int getValidStoppedServer()
{
    int serverNum = getNumberFromUser();

    bool validID = false;
    bool runningID = false;

    for (auto i : serverIDs)
    {
        if (i == serverNum)
        {
            validID = true;
        }
    }

    if (validID)
    {
        for (auto i : runningServers)
        {
            if (i == serverNum)
            {
                runningID = true;
            }
        }
    }

    while (!validID || runningID)
    {
        if (!validID)
        {
            cout << "That ID does not exist, please enter a different one: ";
        }
        else if (runningID)
        {
            cout << "That server is already running, please enter a differnt one: ";
        }

        serverNum = getNumberFromUser();

        validID = false;

        for (auto i : serverIDs)
        {
            if (i == serverNum)
            {
                validID = true;
            }
        }

        if (validID)
        {
            runningID = false;

            for (auto i : runningServers)
            {
                if (i == serverNum)
                {
                    runningID = true;
                }
            }
        }
    }

    return serverNum;
}

bool getYesOrNo()
{
    bool answer = false;
    bool validAnswer = false;
    string input;

    getline(cin, input);

    if (input == "y" || input == "Y" || input == "yes" || input == "Yes" || input == "YES")
    {
        answer = true;
        validAnswer = true;
    }
    else if (input == "n" || input == "N" || input == "no" || input == "No" || input == "NO")
    {
        answer = false;
        validAnswer = true;
    }
    else
    {
        validAnswer = false;
    }

    while (!validAnswer)
    {
        cout << "Invalid input, please enter yes or no: ";
        getline(cin, input);

        if (input == "y" || input == "Y" || input == "yes" || input == "Yes" || input == "YES")
        {
            answer = true;
            validAnswer = true;
        }
        else if (input == "n" || input == "N" || input == "no" || input == "No" || input == "NO")
        {
            answer = false;
            validAnswer = true;
        }
        else
        {
            validAnswer = false;
        }
    }

    return answer;
}
