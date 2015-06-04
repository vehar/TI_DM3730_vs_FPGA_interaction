#include "CustThread.h"
//#include "CustDbgOutput.h"

/*********************************************Thread**************************************************/
CustThread::CustThread(CustThreadProc Proc, LPVOID lpParameter)
{
	ThreadHandle  = NULL;
	ThreadHandle = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)Proc, lpParameter, 0,&dwThreadId);   
}

CustThread::~CustThread()
{
	DWORD Status;
	if(ThreadHandle)
		do
		{
			GetExitCodeThread(ThreadHandle, &Status);
			Sleep(100);
		} while(Status == STILL_ACTIVE);
	//OutputDbgText(L"Exiting from thread destructor");
	CloseHandle(ThreadHandle);
}
HANDLE CustThread::GetHandle()
{
	return ThreadHandle;
}