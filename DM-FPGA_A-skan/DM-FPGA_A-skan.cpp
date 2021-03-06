// FPGA_Client.cpp : Defines the entry point for the console application.
//#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h> 
#include <string.h>
#include "locale.h"
#include <winbase.h>
#include <WinIoCtl.h>
#include <initguid.h>
#include "spi.h"
#include "omap35xx_mcspi.h" 

#include <winuser.h>   
#include <winuserm.h>
#include <pm.h>

#include "gpio.h"
#include "omap35xx_gpio.h"

#include <commctrl.h>

#include "GpioDriver.h"

#include "FPGA/FPGA.h"
#include "ExtBusFunc.h" 

#include "AcousticLib/FPGACommunication.h"
#include "AcousticLib/AcousticScheme.h"
//#include "AcousticLib/AcousticSchemeProcessor.h"


#include "CustThread/CustThread.h"
#include "Controls/CustControls.h"

//#define DEBUG 
	int DBUS_TEST_result = 0;
	int ABUS_TEST_result = 0;
	int error = 0;

int FPGA_ABUS_TEST();
int FPGA_DBUS_TEST();

int koef_array[23];
FPGACommunication FPGA;

//AcousticSchemeProcessor asp;

DWORD dwThreadId;
HANDLE hHandle;

 int DataRefreshed_Flag = 0;
 int KeyState = 0;
 int EncState = 0;


///////////////////Settings///////////////////////////////////
//FreqSync+			activeScheme->inqFreq;
 //SyncCtrl
 //Compress+
 //AnChSwich
 //AttenSw
 //AcousticContactGain!
 //GenBuff?
 //AScanEn
 //AdcDelay?
 //ProbeDelay?

//����������:
 //��������� ���������������� - ��������� ������������� (������� ����������� �������)
 //�H, �L  -�������� ������ ������� � ������� ���������

int compress_val;
int Channel;
bool AScanEn;
Sync SyncState = SyncStop;

/////////////////////////////////////////////////////////////
void AcousticScheme_DefaultInit(void)
{
activeScheme = &Rdm11Scheme;

activeScheme->inqFreq  = 1000;//FreqSync	
SyncState = SyncInt;//SyncCtrl
compress_val = 3;//Compress
Channel = 0;//AnChSwich
activeScheme->params.attenEn = 0; //AttenSw
activeScheme->params.gainDb = 10;

 //AcousticContactGain!
 //GenBuff?
 AScanEn = 1;//AScanEn

 activeScheme->signal.adcDelayUs = 500; //AdcDelay?
 activeScheme->probe.delayUs = 10;//ProbeDelay?
}


void _dellay(void)
{
for (int i = 10000; i>0; i--);
}


 
void draw_sine (void)
{
int cnt = 5;
while(cnt)
{	
	for(double x1 = -40; x1 < 5; x1 += 0.1)
	{
			int y1 = sin(x1)*3*cnt+100;

		FPGA_Write(98 ,y1+10+cnt/2);
		FPGA_Write(99 ,y1);
		_dellay();
	 }
		cnt --;
		/*if(cnt>=100)
		{
			cnt = 1;
			//FPGA_Write(51, 1);//? ram cnr rst
		}*/
}
}

int del_chk(int del)
{
	return del & 0xFFF8;
}
int t; 



void System_init (void) 
{
//////System_init/////////////////////////////////////////////////////
FPGA.setSyncSource(SyncStop);
FPGA.setSyncFreq(activeScheme->inqFreq);//1000 //>>IN_SET
FPGA.setSignalCompress(activeScheme->signal.compress);//1
FPGA.setSyncSource(SyncState);//SyncCtrl - on

FPGA_Write(100 ,0);//SyncCtrl_nENABLE - on
//////System_init/////////////////////////////////////////////////////
}
void Ascan_init (void)
{
//////A-SCAN_init//////////////
FPGA_Write(_AScanDrawMode ,2); //AScanDrawMode 0 ��� AScan�
FPGA_Write(_AScanEnAddr ,2); //���� �� ���������� ��� ������ (AScanEn) - ����������� �� ������ �� ��������!!!!!
FPGA_Write(_AScanWrCS ,2);

FPGA_Write(_AScanStartAddrWr ,0x00); //� ��� - ��
FPGA_Write(_AScanRamCntRdRst ,0x01); //�� ����������� ������� ������
//////A-SCAN_init//////////////
}
void Gen_init (void)
{
//////GEN_init//////////////////////////////////////////////////////////////////////////
FPGA.setGenSel(1);//GenCS //FPGA_Write(_GenCSAddr ,1);//GenCS
//FPGA.setGenData();

FPGA_Write(_GenStartAddrWr ,0);//GenStartAddrWr
//////////////////////////////////////
for (int i = 0; i < 256; ++i)
{
	FPGA_Write(_GenBuffAddr1 ,0);
	FPGA_Write(_GenBuffAddr2 ,0);
}
FPGA_Write(_GenStartAddrWr ,0);//GenStartAddrWr
//while(1)//for(int i = 0; i<10; i++) //>>IN_SET
{	
	FPGA_Write(_GenBuffAddr1 ,1 + del_chk(160));
	FPGA_Write(_GenBuffAddr2 ,0);

	FPGA_Write(_GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(_GenBuffAddr2 ,0);

	
    FPGA_Write(_GenBuffAddr1 ,2 + del_chk(16));
	FPGA_Write(_GenBuffAddr2 ,0);

	FPGA_Write(_GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(_GenBuffAddr2 ,0);

	//Sleep(10);
}
FPGA.setGenOutCh(1);//GenEn //FPGA_Write(_GenEn ,1);//GenEn
//////GEN_init////////////////////////////////////////////////////////////////////
}

void Acust_init(void)
{
//////ANALOG_init start///////////////////////////////////////////////////////////


//FPGA_Write(IntegratorCoefAddr ,0);//IntegratorKoef
//FPGA_Write(9 ,0);//InversionSignal
//FPGA_Write(AttenSw ,1);//AttenSw  - ��� 0 - ��������� ���������
FPGA_Write(_TgcStartAddrWr ,0);//TgcStartAddrWr

FPGA_Write(_RamCntRdRst ,1); //RamCntRdRst - ok
FPGA_Write(_DetectorAddr ,0);//Detector = pos+neg 
FPGA_Write(_ReadBuffSize ,480); //ReadBuffSize


FPGA.WriteDWORD(_AdcDelayAddr1,_AdcDelayAddr2,activeScheme->signal.adcDelayUs);//need test
//FPGA_Write(_AdcDelayAddr1 ,500); //130 //>>IN_SET
//FPGA_Write(_AdcDelayAddr2 ,0); //>>IN_SET

FPGA.setProbeDelay(activeScheme->probe.delayUs);//5-2000  //FPGA_Write(_ProbeDelay ,1);////>>IN_SET


FPGA_Write(_CompressAddr ,compress_val);//Compress //3 //>>IN_SET

FPGA.setTGCState(1);//TgcEn - ��� 0 - ��������� ��������� //FPGA_Write(_TgcEnAddr ,1); //>>IN_SET	
{
		for(int i = 0; i< 479; i++)
		{
			FPGA_Write(_DacData1 ,20);//DacData[15:0] //>>IN_SET
			FPGA_Write(_DacData2 ,10);//DacData[31:16] //>>IN_SET
		}
	}

FPGA.setAcoustContGainCode(10);//FPGA_Write(_AcousticContactGain ,10); //>>IN_SET

FPGA.setDACCh(0);//DacCh 0-1; 1-������ ����� FPGA_Write(_DacCh ,0);//DacCh 0-1; 1-������ �����

FPGA_Write(_AttenSw , activeScheme->params.attenEn);//>>IN_SET 

FPGA.setAnalogChSwich(Channel);//FPGA_Write(_AnChSwich ,0);//AnChSwich //>>IN_SET


FPGA_Write(_FilterEnAddr ,1); //>>IN_SET
FPGA_Write(_FilterCompressAddr ,1); //>>IN_SET
FPGA_Write(_FilterCoeffsRstAddrWr,1); //>>IN_SET
FPGA_Write(_FilterSwich,0); //>>IN_SET

for(int i=0; i<23; i++) //>>IN_SET
{
FPGA_Write(_FilterCoeffsAddr ,1/*koef_array[i]*/);
}
//////Analog init end/////////////////////////////////////////////////////////////////
}

void FPGA_Regs_deinit(void)//��������� �������� ��������
{
	for (int i = 0; i<=127; i++){FPGA_Write(i ,0);}
FPGA_Write(0 ,0);

for (int i = 0; i<=127; i++){FPGA_Write(i ,0);}
FPGA_Write(0 ,0);	
}

DWORD WINAPI ThreadAdcAskan(LPVOID lpParameter)
{
int arr[1010] = {0};
int adc_val = 0;
int adc_sample_cnt = 0;
int rd_index = 0;
int window_480 = 1;

	while(1)
	{
	Sleep(0);//Return quants to system must be 0
   //printf("while \n");
	int t = 0;

FPGA_Write(_RamCntRdRst ,0x01);
FPGA_Write(_AScanStartAddrWr ,0x00);
FPGA_Write(_AScanRamCntRdRst ,0x01);

for(int i = 0; i< 479; i++)
{	
	//arr[i] = FPGA_Read(AdcBuffAddr);
	//adc_val = arr[i];// / 6;

    adc_val = FPGA_Read(_AdcBuffAddr);

	if((i>window_480)&&(i<window_480+480))
  {
	if(adc_val>900){adc_val = adc_val/2;}
	if(adc_val>700){adc_val = adc_val/2;}
	if(adc_val>500){adc_val = adc_val/2;}
	if(adc_val>300){adc_val = adc_val/2;}

adc_val -=100;
adc_val = 272 - adc_val;

	//if(adc_val<255)
	{
	FPGA_Write(_AScanBuffAddr1 ,adc_val+5);
	FPGA_Write(_AScanBuffAddr2 ,adc_val);
	}
  }
}
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
koef_array	[	0	]	=	-75	;
koef_array	[	1	]	=	-113	;
koef_array	[	2	]	=	-127;	
koef_array	[	3	]	=	-113	;
koef_array	[	4	]	=	-75	;
koef_array	[	5	]	=	-20	;
koef_array	[	6	]	=	39	;
koef_array	[	7	]	=	90	;
koef_array	[	8	]	=	121	;
koef_array	[	9	]	=	126	;
koef_array	[	10	]	=	103	;
koef_array	[	11	]	=	58	;
koef_array	[	12	]	=	0	;
koef_array	[	13	]	=	-58	;
koef_array	[	14	]	=	-103	;
koef_array	[	15	]	=	-126	;
koef_array	[	16	]	=	-121	;
koef_array	[	17	]	=	-90	;
koef_array	[	18	]	=	-39	;
koef_array	[	19	]	=	20	;
koef_array	[	20	]	=	75	;
koef_array	[	21	]	=	114	;
koef_array	[	22	]	=	128	;

//--------------------Tests section start----------------------------------------
		DBUS_TEST_result = FPGA_DBUS_TEST();
		printf("FPGA_D_BUS_TEST %s ", (DBUS_TEST_result?"ERR = ":"OK\n"));
		if(DBUS_TEST_result){printf("%d\n",DBUS_TEST_result); error++;}
        
		ABUS_TEST_result = FPGA_ABUS_TEST();
		printf("FPGA_A_BUS_TEST %s ", (ABUS_TEST_result?"ERR = ":"OK\n"));
		if(ABUS_TEST_result){printf("%d\n",ABUS_TEST_result); error++;}
//--------------------Tests section end------------------------------------------



AcousticScheme_DefaultInit();

KeypadInit();

FPGA_BUS_Init();

FPGA_Regs_deinit(); 

System_init();
Ascan_init();

//TEST_A-SCAN////////////////
draw_sine();
////////////////////////////

Gen_init();

Acust_init(); 

//----------------------------Main threads started--------------------------------
//CustThread(ThreadKeybProc,NULL);
/*printf("Starting treads \n");

	 hHandle = CreateThread(NULL, 0, KeyPadInterruptThread, (LPVOID)1, 0, &dwThreadId);   
 
	 hHandle = CreateThread(NULL, 0, ThreadKeybProc, (LPVOID)2, 0, &dwThreadId);  
		//Sleep(1000);

	 printf("Leave treads \n");*/
	// SendMouseMsg(MOUSEEVENTF_ABSOLUTE, NULL,5000,500 );
//hHandle = CreateThread(NULL, 0, ThreadAdcAskan, (LPVOID)2, 0, &dwThreadId);  //Main adc_to_A-scan thread
//	CloseHandle(hHandle);
//--------------------------------------------------------------------------------

//////READ_ADC////////////////////////////////////////////////////////////////////

int arr[1010] = {0};
int adc_val = 0;
int adc_sample_cnt = 0;
int rd_index = 0;

int window_480 = 1;

//FPGA_Write(GenEn ,0);//GenEn


while(1)
{
	Sleep(0);//Return quants to system
//printf("while \n");


	int t = 0;

FPGA_Write(_RamCntRdRst ,0x01);
FPGA_Write(_AScanStartAddrWr ,0x00);
FPGA_Write(_AScanRamCntRdRst ,0x01);

for(int i = 0; i< 479; i++)
{	
	//arr[i] = FPGA_Read(AdcBuffAddr);
	//adc_val = arr[i];// / 6;

    adc_val = FPGA_Read(_AdcBuffAddr);

	if((i>window_480)&&(i<window_480+480))
  {
	if(adc_val>900){adc_val = adc_val/2;}
	if(adc_val>700){adc_val = adc_val/2;}
	if(adc_val>500){adc_val = adc_val/2;}
	if(adc_val>300){adc_val = adc_val/2;}

adc_val -=100;
adc_val = 272 - adc_val;

	//if(adc_val<255)
	{
	FPGA_Write(_AScanBuffAddr1 ,adc_val+5);
	FPGA_Write(_AScanBuffAddr2 ,adc_val);
	}
  }
}
}
//////READ_ADC////////////////////////////////////////////////////////////////////

    while(1)
    {//STOP

    }
}


//=======================================================================================
unsigned char   Value;

int FPGA_DBUS_TEST()
{
	int Ok = 0;
#ifdef WINCE
	unsigned short temp;
	int Value1;
	for(int i=0;i<15;i++)
	{
		Value1 =(int) (100.0 * (float) i/14);
		if(i==14) Value1 = 99;
		if(Value1 >= 100) 
		{
			Value = 99;
		}
		else
		{
			Value = Value1;
		}
		//InvalidateRect(MainW->GetHWnd(),0,0);
		FPGA_Write(17 ,(unsigned short)1<<i);
		FPGA_Write(127,(unsigned short)~1<<i);
		temp = FPGA_Read(17);
		if(temp != 1<<i) Ok |= 1<<i;
	}
#else
	for(int i=0;i<100;i++)
	{
		Sleep(20);
		Value = i;
		InvalidateRect(MainW->GetHWnd(), NULL, 0);
	}
	return 1;

#endif
	return Ok;
}

int FPGA_ABUS_TEST()
{
	int Ok = 0;

#ifdef WINCE
	unsigned short temp;
	int Value1;
	for(int i=0;i<7;i++)
	{
		Value1 =(int) (100.0 * (float) i/6);
		if(i==6) Value1 = 99;
		if(Value1 >= 100)
		{
          Value = 99;
		}
		else
		{
		  Value = Value1;
		}
//		InvalidateRect(MainW->GetHWnd(),0,0);
		FPGA_Read( 1 << i );  
		temp = FPGA_Read(18);
		if(temp != 1<<i) Ok |= 1<<i;
	}
#else
	for(int i=0;i<100;i++)
	{
		Sleep(20);
		Value = i;
		InvalidateRect(MainW->GetHWnd(), NULL, 0);
	}
	return 1;
#endif

	return Ok;
}