#include "CustControls.h"

 /*DataRefreshed_Flag = 0;
 KeyState = 0;
 EncState = 0;*/

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

//---------KeyPad_proc-------------------

//Dummy
void KeypadInit(void){};

DWORD WINAPI KeyPadInterruptThread(PVOID pvParam)
{
	while(1)
	{
	 DataRefreshed_Flag = rand()%2;
	 KeyState = rand()%18;
	 EncState ++;
	 Sleep(100);
	}
	 return 0;
} 
//---------KeyPad_proc-------------------


DWORD WINAPI ThreadKeybProc(LPVOID lpParameter)
{
	int i = 0;
	while(1)
	{
		if(DataRefreshed_Flag)
		{
			DataRefreshed_Flag = 0;
			printf("Key =  %i, Enc = %i\n",KeyState, EncState);
			//SendMouseMsg(MOUSEEVENTF_ABSOLUTE, NULL,KeyState*5000,KeyState*5000 );
			SendKbdMsg(1,KeyState+40);
		}
	Sleep(1);
	}

printf("Exiting tread %i \n",lpParameter);

//VOID ExitThread();
//	CloseHandle(hHandle);
	return 0;
}
