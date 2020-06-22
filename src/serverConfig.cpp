/*
 * Filename:	serverConfig.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	
 */

#include "serverConfig.h"

void printServerSettings(string serverDir)
{
    // Create a rude config object
    rude::Config config;

    string configValue;

    // Load the given config file
    if(config.load((serverDir + "/server.properties").c_str()))
    {
        configValue = config.getStringValue("gamemode");

        cout << "Gamemode: " + configValue << endl;

        configValue = config.getStringValue("difficulty");

        cout << "Difficulty: " + configValue << endl;

        if(config.getBoolValue("pvp"))
        {
            cout << "PVP: true" << endl;
        }

        else
        {
            cout << "PVP: false" << endl;
        }

        if(config.getBoolValue("hardcore"))
        {
            cout << "Hardcore: true" << endl;
        }

        else
        {
            cout << "Hardcore: false" << endl;
        }

        if(config.getBoolValue("enable-command-block"))
        {
            cout << "Command Blocks enabled: true" << endl;
        }

        else
        {
            cout << "Command Blocks enabled: false" << endl;
        }

        configValue = to_string(config.getIntValue("max-players"));

        cout << "Max Players: " + configValue << endl;

        configValue = to_string(config.getIntValue("server-port"));

        cout << "Server Port: " + configValue << endl;
    }

    else
    {
        cout << "Failed to open " + serverDir + "/server.properties" << endl;
    }
}
