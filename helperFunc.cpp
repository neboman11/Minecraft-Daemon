/*
 * Filename:	helperFunc.cpp
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Functions that help for re-reading the config file.
 */

#include "helperFunc.h"


void printConfig()
{
    // Write all of the read in config values to the log
    writeLog("Read config options:", true);
    writeLog("\tVerbose: " + configOptions[VERBOSE], true);
    writeLog("\tLogFile: " + configOptions[LOG_FILE], true);
    writeLog("\tJava-Path: " + configOptions[JAVA_PATH], true);
}

char* trimNewLine(char* s, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (s[i] == '\n')
        {
            s[i] = '\0';
            break;
        }
    }

    return s;
}
