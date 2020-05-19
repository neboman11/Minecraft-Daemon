/*
 * Filename:		commandFunc.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
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

string getFormattedDate()
{
  // FILE object for reading from the pipe for the gawk command
  FILE* pipeTown;
  // Array of characters to use as a buffer for reading from the pipe
  char buffer[BUFFER_SIZE];
  // Pointer to the currently read in line from the pipe
  char* workingLine;
  // String stream for collecting the command output
  stringstream output;

  // Open the pipe with the given command in read mode
  pipeTown = popen("date +%Y.%m.%d-%T", "r");

  // If the pipe failed to open
  if (!pipeTown)
  {
    // Let what called the function know
    return string();
  }

  // Read in the first buffer space of the pipe output
  workingLine = fgets(buffer, BUFFER_SIZE, pipeTown);

  // Loop until there is nothing more to be read from the buffer
  while (workingLine != NULL)
  {
    // Add the contents of the current working line to the string stream
    output << workingLine;
    // Read in the next buffer space of the pipe output
    workingLine = fgets(buffer, BUFFER_SIZE, pipeTown);
  }

  // Close the pipe
  pclose(pipeTown);

  // Return the date string without the newline at the end
  return output.str().substr(0, output.str().length() - 1);
}
