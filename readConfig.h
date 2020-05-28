/*
 * Filename:	readConfig.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for readConfig.cpp.
 */

#ifndef __READCONFIG_H_
#define __READCONFIG_H_

// #include <tao/config.hpp>
#include <rude/config.h>
#include <iostream>
#include <cstdlib>
#include "main.h"

map<int, string> readConfig(string confFile);

#endif // __READCONFIG_H_
