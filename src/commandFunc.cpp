/*
 * Filename:	commandFunc.cpp
 * Author:		Michael Nesbitt
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
