/*
 * Filename:	helperFunc.cpp
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:	2020, All Rights Reserved
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
