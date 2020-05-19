/*
 * Filename:		loggingFunc.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
 *
 * Description:
 *
 *	Writes the given message to the log file based on the message verbosity.
 */

#include "loggingFunc.h"

void writeLog(string message, bool messageVerbosity /*= false*/)
{
  // If the configuration is set to output verbose messages OR if the message is not verbose
  if (configOptions[VERBOSE] == "true" || !messageVerbosity)
  {
    // Write the message to the log file.
    *logFile << message << endl;
  }
}
