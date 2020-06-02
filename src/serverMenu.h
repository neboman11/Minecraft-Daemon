/*
 * Filename:	serverMen.h
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
#include "mcServer.h"

using namespace std;

// Map of child servers
extern map<int, MCServer*> servers;
// List of server IDs
extern vector<int> serverIDs;

int serverMenu();
int menuStart();
int menuLog();
int menuInteract();
int menuStop();

#endif // __SERVERMENU_H_
