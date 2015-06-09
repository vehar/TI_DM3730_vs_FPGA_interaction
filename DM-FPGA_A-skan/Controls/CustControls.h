// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <windows.h>

void SendKbdMsg(int down,int vk);
int SendMouseMsg(DWORD flag, int data, int dx, int dy);

extern int DataRefreshed_Flag;
extern int KeyState;
extern int EncState;

void KeypadInit(void);
DWORD WINAPI KeyPadInterruptThread(PVOID pvParam);
DWORD WINAPI ThreadKeybProc(LPVOID lpParameter);
