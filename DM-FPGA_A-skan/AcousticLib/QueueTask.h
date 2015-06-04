#pragma once

/************************************************************************
 INCLUDES
************************************************************************/
//#include "stdafx.h"
//#include "Data_DAQLoop.h"
#include <windows.h>
#include <queue>
using namespace std;

/************************************************************************
 DEFINES
************************************************************************/
#define QUEUE_MAX_SIZE	30

struct tTransfer
{
	int Cmd;
	int Data;
};

static DWORD ThreadID = 0;

/************************************************************************
 CLASS LOCK
************************************************************************/
class Lock_CriticalSection
{
public:
	Lock_CriticalSection(void) { InitializeCriticalSection(&cs); };
	~Lock_CriticalSection(void){ DeleteCriticalSection(&cs); };
	void lock() { EnterCriticalSection(&cs); };
	void unlock() { LeaveCriticalSection(&cs); };

private:
	CRITICAL_SECTION cs; 
};

/************************************************************************
 CLASS SLAVE LOOP SYNCHRONIZATION
************************************************************************/
class QueueTask : public Lock_CriticalSection
{
public:
	QueueTask(void) { hThread = 0; };
	~QueueTask(void) {if (hThread) CloseHandle(hThread); };

private:
	HANDLE hThread;
	tTransfer tx, rx;
	queue <tTransfer> qTie;

public:
	bool isThreadCreated() { return ((hThread == 0) ? false : true); };
	bool isEmpty() { return qTie.empty(); };
	void setElement(int Cmd, const int Data = -1);	// -1 --> No Data
	void receiveElement();
	int getCmd() { return rx.Cmd; } ;
	int getData() { return rx.Data; };
	void finishThread(int Cmd) { setElement(Cmd); Sleep(30); };
	void startThread(PTHREAD_START_ROUTINE MFunc) { hThread = CreateThread(NULL, 0, MFunc, 0, 0, &ThreadID); /*Sleep(100);*/ };
};
