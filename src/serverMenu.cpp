/*
 * Filename:	serverMenu.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	
 */

#include "serverMenu.h"

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
                menuStart();
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

void menuStart()
{
    cout << "Enter the server number to start: ";

    int serverNum = getValidStoppedServer();

    map<int, string> serverData;

    // Query server data from database
    queryServerData(serverNum, serverData);

    writeLog("Starting server " + serverData[NAME]);
    // Create a new server child process
    servers[serverNum] = new MCServer(serverData[RUN_MEMORY], serverData[START_MEMORY], serverData[JAVA_ARGS], serverData[DIRECTORY], serverData[JARFILE], serverNum);

    runningServers.push_back(serverNum);
}

void menuListRunning()
{
    // TODO: query data from database
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

    cout << servers[serverNum]->getLog() << endl;

    return 0;
}

int menuInteract()
{
    cout << "Enter the server number to interact with: ";

    int serverNum = getValidRunningServer();

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
        writeLog("Creating server with ID: " + to_string(serverNum) + " in folder: " + serverDir + " with jar file: " + serverJar + " with " + runRAM + " of RAM");

        // Create a new server child process
        servers[serverNum] = new MCServer(runRAM, startRAM, javaArgs, serverDir, serverJar, serverNum);

        runningServers.push_back(serverNum);
    }
}

void menuList()
{
    menuListRunning();
    menuListStopped();
}

void menuListStopped()
{
    // TODO: query data from database
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
        cout << "WARNING: Server is currently running!!" << endl;
        cout << "It will be stopped before it is removed." << endl;
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

// TODO: return on stopped server number
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
