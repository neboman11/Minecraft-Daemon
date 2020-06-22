/*
 * Filename:	serverConfig.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	
 */

#ifndef __SERVERCONFIG_H_
#define __SERVERCONFIG_H_

#include <rude/config.h>
#include <string>
#include <iostream>

using namespace std;

// Display current server settings (in server.properties)
void printServerSettings(string serverDir);

#endif // __SERVERCONFIG_H_
