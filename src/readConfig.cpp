/*
 * Filename:    readConfig.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Reads the given configuration file and returns a map of the read data.
 */

#include "readConfig.h"

map<int, string> readConfig(string confFile)
{
    // Map for holding all the read config options.
    map<int, string> configOptions;

    // Create a rude config object
    rude::Config config;

    // Load the given config file
    if(config.load(confFile.c_str()))
    {
        // Try to change to the daemon section, if it does not exists, this returns false.
        if (config.setSection("Daemon", false))
        {
            // If the verbose flag is present in the config file
            if (config.exists("Verbose"))
            {
                // If the Verbose value was true.
                if (config.getBoolValue("Verbose"))
                {
                    // Set the config verbosity to true.
                    configOptions[VERBOSE] = "true";
                }

                // If the Verbose value was false
                else
                {
                    // Set the config verbosity to false
                    configOptions[VERBOSE] = "false";
                }
            }

            // If the verbose flag was not present in the config file
            else
            {
                // Write an error message to the screen
                cerr << "Verbose flag missing from config file." << endl;
                // Exit the program
                exit(8);
            }

            // If the log file flag is preset in the config file
            if (config.exists("LogFile"))
            {
                // Set the log file value
                configOptions[LOG_FILE] = config.getStringValue("LogFile");
            }

            // If the log file flag was not present
            else
            {
                // Write an error message to the screen
                cerr << "LogFile flag missing from config file." << endl;
                // Exit the program
                exit(8);
            }

            // If the java-path flag is preset in the config file
            if (config.exists("Java-Path"))
            {
                // Set the java-path value
                configOptions[JAVA_PATH] = config.getStringValue("Java-Path");
            }

            // If the java-path flag was not present
            else
            {
                // Write an error message to the screen
                cerr << "Password flag missing from config file." << endl;
                // Exit the program
                exit(8);
            }
        }

        // If the parameters section is missing
        else
        {
            // Write an error message to the screen
            cerr << "Daemon section missing from config file." << endl;
            // Exit the program
            exit(9);
        }

        // Try to change to the database section, if it does not exists, this returns false.
        if (config.setSection("Database", false))
        {
            // If the database flag is preset in the config file
            if (config.exists("Database"))
            {
                // Set the database value
                configOptions[DATABASE] = config.getStringValue("Database");
            }

            // If the database flag was not present
            else
            {
                // Write an error message to the screen
                cerr << "Database flag missing from config file." << endl;
                // Exit the program
                exit(8);
            }

            // If the database file flag is preset in the config file
            if (config.exists("Database-File"))
            {
                // Set the database file value
                configOptions[DATABASE_FILE] = config.getStringValue("Database-File");
            }

            // If the database file flag was not present
            else
            {
                // Write an error message to the screen
                cerr << "Database file flag missing from config file." << endl;
                // Exit the program
                exit(8);
            }
        }

        // If the database section is missing
        else
        {
            // Write an error message to the screen
            cerr << "Database section missing from config file." << endl;
            // Exit the program
            exit(9);
        }
    }

    // If there was an error opening the config file
    else
    {
        // Write an error message to the screen
        cerr << "Error opening config file: " <<  config.getError() << endl;
        // Exit the program
        exit(7);
    }

    // Return the built map
    return configOptions;
}
