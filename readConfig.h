/*
 * Filename:		readConfig.h
 * Date:		04/14/2020
 * Author:		Michael Nesbitt
 * Email:		nesbitt@treeman.nesbitt.rocks
 * Version:		1.0
 * Copyright:		2020, All Rights Reserved
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
