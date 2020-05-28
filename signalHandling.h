/*
 * Filename:	signalHandling.h
 * Author:		Michael Nesbitt
 *
 * Description:
 *
 *	Includes and prototypes for signalHandling.cpp
 */

#ifndef __SIGNALHANDLING_H_
#define __SIGNALHANDLING_H_

#include <csignal>
#include <stdlib.h>
#include "loggingFunc.h"
#include "commandFunc.h"
#include "main.h"
#include "readConfig.h"
#include "helperFunc.h"

using namespace std;

void interruptSignal(int signum);
void hangupSignal(int signum);

#endif // __SIGNALHANDLING_H_
