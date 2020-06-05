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
        cout << "[s] Start a server" << endl;
        cout << "[l] List all running servers" << endl;
        cout << "[o] View a server's output" << endl;
        cout << "[i] Interact with a running server" << endl;
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
            if (serverIDs.size() > 0)
            {
                menuList();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }

            break;
        case 'o':
            if (serverIDs.size() > 0)
            {
                menuLog();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }

            break;
        case 'i':
            if (serverIDs.size() > 0)
            {
                menuInteract();
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }

            break;
        case 'k':
            if (serverIDs.size() > 0)
            {
                menuStop(-1);
            }

            else
            {
                cout << "There are currently no running servers." << endl;
            }

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

    serverIDs.push_back(serverNum);

    cout << "Enter the folder to run the server in: ";
    getline(cin, input);

    string serverDir = input;

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
        getline(cin, input);

        serverDir = input;

        // Stat the PID file to see if it exists
        temp = stat(serverDir.c_str(), &fileStatus);

        // If stat returned anything other than -1, the directory exists
        if (temp != -1)
        {
            // Set validDir to true
            validDir = true;
        }
    }

    cout << "Enter the name of the server jar file: ";
    getline(cin, input);

    string serverJar = input;
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
        getline(cin, input);

        serverJar = input;
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

    vector<char*> serverArgs;
    serverArgs.push_back((char*)"java");
    serverArgs.push_back((char*)"-jar");
    serverArgs.push_back((char*)serverPath.c_str());
    serverArgs.push_back((char*)"nogui");
    serverArgs.push_back((char*)nullptr);

    writeLog("Creating server with ID: " + to_string(serverNum) + " in folder: " + serverDir + " with jar file: " + serverJar);

    // Create a new server child process
    servers[serverNum] = new MCServer(serverArgs.data(), serverDir, serverJar, serverNum);

    return 0;
}

void menuList()
{
    cout << "These are all the current running servers:" << endl;

    for (auto i : serverIDs)
    {
        cout << "  Server Number: " << to_string(servers[i]->getServerNum()) << ", Running in folder: " << servers[i]->getWorkingDir() << ", With server jar: " << servers[i]->getJarFile() << endl;
    }
}

int menuLog()
{
    string input;

    cout << "Enter the server number to view the log of: ";

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

    cout << servers[serverNum]->getLog() << endl;

    return 0;
}

int menuInteract()
{
    string input;

    cout << "Enter the server number to interact with: ";

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
        string input;

        cout << "Enter the server number to stop: ";

        serverNum = getNumberFromUser();

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
    }

    writeLog("Shutting down server: " + to_string(serverNum));

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
