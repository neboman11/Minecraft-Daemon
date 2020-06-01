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
