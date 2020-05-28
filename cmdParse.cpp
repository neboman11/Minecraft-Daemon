/*
 * Filename:	cmdParse.cpp
 * Date:		  04/14/2020
 * Author:	  Michael Nesbitt
 *
 * Description:
 *
 *	Parses the arguments using tclap and returns a map of the found values.
 */

#include "cmdParse.h"

map<int, string> cmdParse(int argc, char** argv)
{
  // Map of all the read in flags and options
  map<int, string> optionMap;

  // Try in case TCLAP throws an exception while getting the command line info
  try
  {
    // The main command line object for parsing the command line
    TCLAP::CmdLine cmd("Minecraft Daemon", ' ', "0.1");

    // Whether or not to run as a daemon
    TCLAP::SwitchArg daemon("d", "daemon", "Run in daemon mode (forks to run as a daemon).", false);
    cmd.add(daemon);

    // The file name for the config file
    TCLAP::UnlabeledValueArg<string> confFile("config-filename", "The name of the configuration file. Defaults to minecraft-daemon.conf", false, "minecraft-daemon.conf", "config filename", false);
    cmd.add(confFile);

    // Reparse the command line arguments for the given flags
    cmd.parse(argc, argv);

    // Put the daemon state in the map
    if (daemon.getValue())
    {
      optionMap[DAEMON] = "true";
    }

    else
    {
      optionMap[DAEMON] = "false";
    }

    // Put the config file name into the map
    optionMap[CONFIG] = confFile.getValue();
  }

  // If tclap threw an exception
  catch (TCLAP::ArgException &e)
  {
    // Write an error message to the screen
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    // Exit the program
    exit(11);
  }

  // Return the built map
  return optionMap;
}
