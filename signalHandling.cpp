/*
 * Filename:		signalHandling.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Signal handling functions for the INT and HUP signals.
 */

#include "signalHandling.h"

void interruptSignal(int signum)
{
  // Tell the user what's happening
  writeLog("Received interrupt signal. Exiting...");
  // Remove the PID file
  runCommand("rm -f cs3377dirmond.pid");
  // Close the logfile
  logFile->close();
  // Free memory
  delete logFile;
  // Exit the program
  exit(0);
}

void hangupSignal(int signum)
{
  // Store the current password
  string password = configOptions[PASSWORD];
  // Store the current watchDir
  string watchDir = configOptions[WATCH_DIR];
  // Store the current log file
  string oldLogFile = configOptions[LOG_FILE];
  // Tell the user what's happening
  writeLog("Received hangup signal.");
  // Tell the user what's happening
  writeLog("Reading config file: " + inputOptions[CONFIG]);
  // Read the config file
  configOptions = readConfig(inputOptions[CONFIG]);
  // Tell the user what's happening
  writeLog("Done reading config.");

  // If the read password is different from the old password
  if (password != configOptions[PASSWORD])
  {
    // Write a message to the log indicating the change
    writeLog("Cannot change password while daemon is running. Please restart the daemon to use the new password.");
    // Keep using the old password
    configOptions[PASSWORD] = password;
  }

  // If the read watch dir is different from the old watch dir
  if (watchDir != configOptions[WATCH_DIR])
  {
    // Write a message to the log indicating the change
    writeLog("Cannot change watch directory while daemon is running. Please restart the daemon to use the new watch directory.");
    // Keeping using the old watch dir
    configOptions[WATCH_DIR] = watchDir;
  }

  // If the read log file is different from the old log file
  if (oldLogFile != configOptions[LOG_FILE])
  {
    // Tell the user what's happening
    writeLog("Opening new log file: " + configOptions[LOG_FILE]);
    // Open a new file for logging
    ofstream* newLog = new ofstream(configOptions[LOG_FILE].c_str());

    // If the file failed to open
    if (!newLog)
    {
      // Write an error messsage to the old log
      writeLog("Failed to open new log file: " + configOptions[LOG_FILE]);
      // Exit the program
      exit(10);
    }

    // Tell the user what's happening in the new log file
    *newLog << "Closing old log file: " << oldLogFile << endl;

    // Close the old log file
    logFile->close();

    // Change the log file to the new one
    logFile = newLog;

    // Tell the user what's happening
    writeLog("Done moving to new log file.", true);
  }

  // Write the new config values to the log
  printConfig();
}
