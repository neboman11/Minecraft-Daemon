/*
 * Filename:		commandFunc.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Functions for running commands from inside the daemon.
 */

#include "commandFunc.h"

void runCommand(string command)
{
  // Pointer to the pipe
  FILE* pipeTown;

  // Open a pipe using the given command
  pipeTown = popen(command.c_str(), "r");

  // Close the pipe
  pclose(pipeTown);
}
