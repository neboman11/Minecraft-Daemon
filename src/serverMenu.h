/*
 * Filename:	serverMenu.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	
 */

#ifndef __SERVERMENU_H_
#define __SERVERMENU_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sys/stat.h>
#include "mcServer.h"
#include "sqlFunc.h"
#include "serverConfig.h"

using namespace std;

// Map of child servers
extern map<int, MCServer*> servers;
// List of server IDs
extern vector<int> serverIDs;
// List of running servers
extern vector<int> runningServers;

int serverMenu();
int getNumberFromUser();
int getValidServer();
int getValidRunningServer();
int getValidStoppedServer();
bool getYesOrNo();

#endif // __SERVERMENU_H_
