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
#include <sys/stat.h>
#include "mcServer.h"
#include "sqlFunc.h"

using namespace std;

// Map of child servers
extern map<int, MCServer*> servers;
// List of server IDs
extern vector<int> serverIDs;

int serverMenu();
int menuStart();
void menuList();
int menuLog();
int menuInteract();
int menuStop(int serverID);
int getNumberFromUser();

#endif // __SERVERMENU_H_
