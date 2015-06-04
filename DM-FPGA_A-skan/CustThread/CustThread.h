// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <windows.h>


typedef DWORD (*CustThreadProc)(LPVOID lpParameter);

//using namespace std;
class CustThread
{
public:
	CustThread(CustThreadProc, LPVOID lpParameter);
	~CustThread();
	HANDLE CustThread::GetHandle();
private:
	HANDLE ThreadHandle;
	DWORD dwThreadId;
	//static DWORD LocalProcFunc(LPVOID lpParameter);	
};

