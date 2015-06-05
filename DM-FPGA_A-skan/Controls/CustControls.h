// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <windows.h>

void SendKbdMsg(int down,int vk);
int SendMouseMsg(DWORD flag, int data, int dx, int dy);

