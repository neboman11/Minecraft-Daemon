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

// Check if the current build is through CMake
#ifdef CMAKE_BUILD
#include "versionConfig.h"
#endif

// Check if the version has been set
#ifndef __VERSION_MAJOR_
#define __VERSION_MAJOR_ '?'
#endif

#ifndef __VERSION_MINOR_
#define __VERSION_MINOR_ '?'
#endif

#include "main.h"

map<int, string> cmdParse(int argc, char** argv);

#endif // __CMDPARSE_H_
