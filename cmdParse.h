/*
 * Filename:	cmdParse.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for cmdParse.cpp.
 */

#ifndef __CMDPARSE_H_
#define __CMDPARSE_H_

#include <tclap/CmdLine.h>
#include "main.h"

map<int, string> cmdParse(int argc, char** argv);

#endif // __CMDPARSE_H_
