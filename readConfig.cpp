/*
 * Filename:		readConfig.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
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

  // // Create a rude config object
  // rude::Config config;

  // // Load the given config file
  // if(config.load(confFile.c_str()))
  // {
  //   // Try to change to the parameters section, if it does not exists, this returns false.
  //   if (config.setSection("Parameters", false))
  //   {
  //     // If the verbose flag is present in the config file
  //     if (config.exists("Verbose"))
  //     {
  //       // If the Verbose value was true.
  //       if (config.getBoolValue("Verbose"))
  //       {
  //         // Set the config verbosity to true.
  //         configOptions[VERBOSE] = "true";
  //       }

  //       // If the Verbose value was false
  //       else
  //       {
  //         // Set the config verbosity to false
  //         configOptions[VERBOSE] = "false";
  //       }
  //     }

  //     // If the verbose flag was not present in the config file
  //     else
  //     {
  //       // Write an error message to the screen
  //       cerr << "Verbose flag missing from config file." << endl;
  //       // Exit the program
  //       exit(8);
  //     }

  //     // If the log file flag is preset in the config file
  //     if (config.exists("LogFile"))
  //     {
  //       // Set the log file value
  //       configOptions[LOG_FILE] = config.getStringValue("LogFile");
  //     }

  //     // If the log file flag was not present
  //     else
  //     {
  //       // Write an error message to the screen
  //       cerr << "LogFile flag missing from config file." << endl;
  //       // Exit the program
  //       exit(8);
  //     }

  //     // If the password flag is preset in the config file
  //     if (config.exists("Password"))
  //     {
  //       // Set the passowrd value
  //       configOptions[PASSWORD] = config.getStringValue("Password");
  //     }

  //     // If the password flag was not present
  //     else
  //     {
  //       // Write an error message to the screen
  //       cerr << "Password flag missing from config file." << endl;
  //       // Exit the program
  //       exit(8);
  //     }

  //     // If the num versions flag is preset in the config file
  //     if (config.exists("NumVersions"))
  //     {
  //       // Set the num versions value
  //       configOptions[NUM_VERSIONS] = config.getStringValue("NumVersions");
  //     }

  //     // If the num versions flag was not present
  //     else
  //     {
  //       // Write an error message to the screen
  //       cerr << "NumVersions flag missing from config file." << endl;
  //       // Exit the program
  //       exit(8);
  //     }

  //     // If the watch dir flag is preset in the config file
  //     if (config.exists("WatchDir"))
  //     {
  //       // Set the watch dir value
  //       configOptions[WATCH_DIR] = config.getStringValue("WatchDir");
  //     }

  //     // If the watch dir flag was not present
  //     else
  //     {
  //       // Write an error message to the screen
  //       cerr << "WatchDir flag missing from config file." << endl;
  //       // Exit the program
  //       exit(8);
  //     }
  //   }

  //   // If the parameters section is missing
  //   else
  //   {
  //     // Write an error message to the screen
  //     cerr << "Parameters section missing from config file." << endl;
  //     // Exit the program
  //     exit(9);
  //   }
  // }

  // // If there was an error opening the config file
  // else
  // {
  //   // Write an error message to the screen
  //   cerr << "Error opening config file: " <<  config.getError() << endl;
  //   // Exit the program
  //   exit(7);
  // }

  // Return the built map
  return configOptions;
}
