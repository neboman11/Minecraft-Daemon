/*
 * Filename:		helperFunc.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Functions that help for re-reading the config file.
 */

#include "helperFunc.h"

int verifyFolders()
{
  // Tell the user what's happening
  writeLog("Checking watch directory location.", true);

  // Stat struct for using stat on the directory
  struct stat fileStatus;
  // Stat the directory to see if it exists
  int temp = stat(configOptions[WATCH_DIR].c_str(), &fileStatus);

  // Check if it is a directory
  bool isdir = S_ISDIR(fileStatus.st_mode);

  // If it is not a directory or does not exists
  if (!isdir || temp == -1)
  {
    // Write an error message to the log
    writeLog("Watch directory (" + configOptions[WATCH_DIR] + ") is not a directory or does not exist.");
    // Exit the program
    return 5;
  }

  // Tell the user what's happening
  writeLog("Verified watch directory location.", true);

  // Tell the user what's happening
  writeLog("Checking for " +  configOptions[WATCH_DIR] + "/.version", true);

  // Stat the version folder to see if it exists
  temp = stat((configOptions[WATCH_DIR] + "/.version").c_str(), &fileStatus);

  // Check to make sure it is a directory
  isdir = S_ISDIR(fileStatus.st_mode);

  // If it doesn't exist
  if (temp == -1)
  {
    // Create it with mkdir
    runCommand("mkdir " + configOptions[WATCH_DIR] + "/.version");
    // Tell the user what's happening
    writeLog("Created .version subfolder.");
  }

  // If it is not a directory
  else if (!isdir)
  {
    // Write an error message to the log
    writeLog(configOptions[WATCH_DIR] + "/.version is not a directory.");
    // Exit the program
    return 5;
  }

  // Tell the user what's happening
  writeLog("Verified .version subfolder location.", true);

  // Tell the user what's happening
  writeLog("Checking for " + configOptions[WATCH_DIR] + "/.secure", true);

  // Stat the secure folder to see if it exists
  temp = stat((configOptions[WATCH_DIR] + "/.secure").c_str(), &fileStatus);

  // Check to make sure it is a directory
  isdir = S_ISDIR(fileStatus.st_mode);

  // If it doesn't exists
  if (temp == -1)
  {
    // Create it withh mkdir
    runCommand("mkdir " + configOptions[WATCH_DIR] + "/.secure");
    // Tell the user what's happening
    writeLog("Created .secure subfolder.");
  }

  // If it is not a directory
  else if (!isdir)
  {
    // Write an error message to the log
    writeLog(configOptions[WATCH_DIR] + "/.secure is not a directory.");
    // Exit the program
    return 5;
  }

  // Tell the user what's happening
  writeLog("Verified .secure subfolder location.", true);

  // Return to where it was called
  return 0;
}

void printConfig()
{
  // Write all of the read in config values to the log
  writeLog("Read config options:", true);
  writeLog("\tVerbose: " + configOptions[VERBOSE], true);
  writeLog("\tLogFile: " + configOptions[LOG_FILE], true);
  writeLog("\tPassword: " + configOptions[PASSWORD], true);
  writeLog("\tNumVersions: " + configOptions[NUM_VERSIONS], true);
  writeLog("\tWatchDir: " + configOptions[WATCH_DIR], true);
}
