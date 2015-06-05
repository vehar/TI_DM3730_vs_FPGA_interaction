#include "CustControls.h"

//pervii parametr eto 0-down, 1 - up ... vtoroi et konstanta knopki takie kak :VK_F1,VK_HOME ...
void SendKbdMsg(int down,int vk)
{
	KEYBDINPUT kInp;
	INPUT x1;

	kInp.wVk=vk;
	if(down)
		kInp.dwFlags=0;
	else
		kInp.dwFlags=KEYEVENTF_KEYUP; //? надо ли
	x1.type=INPUT_KEYBOARD;
	x1.ki=kInp;
	SendInput(1,&x1,sizeof(INPUT));

}

int SendMouseMsg(DWORD flag, int data, int dx, int dy)
{
	MOUSEINPUT  mInp; 
	INPUT xIn;
	int err = 1;

	switch (flag)
	{
	case MOUSEEVENTF_ABSOLUTE:
	mInp.dwFlags = MOUSEEVENTF_ABSOLUTE;
	mInp.dx = dx;
	mInp.dy = dy;
		break;

	case MOUSEEVENTF_LEFTDOWN:
	mInp.dwFlags = MOUSEEVENTF_LEFTDOWN;
		break;
	case MOUSEEVENTF_LEFTUP:
	mInp.dwFlags = MOUSEEVENTF_LEFTUP;
		break;
	case MOUSEEVENTF_RIGHTDOWN:
	mInp.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		break;
	case MOUSEEVENTF_RIGHTUP:
	mInp.dwFlags = MOUSEEVENTF_RIGHTUP;
		break;
	case MOUSEEVENTF_MIDDLEDOWN:
	mInp.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
		break;
	case MOUSEEVENTF_MIDDLEUP:
	mInp.dwFlags = MOUSEEVENTF_MIDDLEUP;
		break;

	case MOUSEEVENTF_WHEEL:
	mInp.dwFlags = MOUSEEVENTF_WHEEL;
	mInp.mouseData = data;
		break;
	case MOUSEEVENTF_MOVE:
	mInp.dwFlags = MOUSEEVENTF_MOVE;
		break;
	default:
		return err;
		break;
	}
	
	xIn.type=INPUT_MOUSE;
	xIn.mi = mInp;
	SendInput(1,&xIn,sizeof(INPUT));
return 0;
}
