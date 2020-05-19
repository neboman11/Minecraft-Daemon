/*
 * Filename:		cmdParse.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		man170530@utdallas.edu
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
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
