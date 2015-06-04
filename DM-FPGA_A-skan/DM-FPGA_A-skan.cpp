// FPGA_Client.cpp : Defines the entry point for the console application.
#include "stdafx.h"
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
//#define DEBUG 

int koef_array[23];

/////////////////////////////////////////////////////////////
void _dellay(void)
{
for (int i = 10000; i>0; i--);
}

FPGACommunication FPGA;

 
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
FPGA.setSyncFreq(1000);
FPGA.setSignalCompress(1);
FPGA.setSyncSource(SyncInt);//SyncCtrl - on

FPGA_Write(100 ,0);//SyncCtrl_nENABLE - on
//////System_init/////////////////////////////////////////////////////
}
void Ascan_init (void)
{
//////A-SCAN_init//////////////
//FPGA.
FPGA_Write(_AScanDrawMode ,2); //AScanDrawMode 0 или AScan№
FPGA_Write(_AScanEnAddr ,2); //Если не произвести эту запись (AScanEn) - изображение на экране не появится!!!!!
FPGA_Write(_AScanWrCS ,2);

FPGA_Write(_AScanStartAddrWr ,0x00); //а тут - ок
FPGA_Write(_AScanRamCntRdRst ,0x01); //не сбрасвается счётчик записи
//////A-SCAN_init//////////////
}
void Gen_init (void)
{
//////GEN_init//////////////////////////////////////////////////////////////////////////
FPGA_Write(_GenCSAddr ,1);//GenCS
FPGA_Write(_GenEn ,1);//GenEn
FPGA_Write(_GenStartAddrWr ,0);//GenStartAddrWr
//////////////////////////////////////
for (int i = 0; i < 256; ++i)
{
	FPGA_Write(_GenBuffAddr1 ,0);
	FPGA_Write(_GenBuffAddr2 ,0);
}
FPGA_Write(67 ,0);//GenStartAddrWr
//while(1)//for(int i = 0; i<10; i++)
{	
	FPGA_Write(_GenBuffAddr1 ,1 + del_chk(160));
	FPGA_Write(_GenBuffAddr2 ,0);

	FPGA_Write(_GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(_GenBuffAddr2 ,0);

	
    FPGA_Write(_GenBuffAddr1 ,2 + del_chk(16));
	FPGA_Write(_GenBuffAddr2 ,0);

	FPGA_Write(_GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(_GenBuffAddr2 ,0);

	Sleep(10);
}
//////GEN_init////////////////////////////////////////////////////////////////////
}

void Acust_init(void)
{
//////ANALOG_init start///////////////////////////////////////////////////////////


//FPGA_Write(IntegratorCoefAddr ,0);//IntegratorKoef
//FPGA_Write(9 ,0);//InversionSignal
//FPGA_Write(AttenSw ,1);//AttenSw  - при 0 - пропадает генерация
FPGA_Write(_TgcStartAddrWr ,0);//TgcStartAddrWr

FPGA_Write(_RamCntRdRst ,1); //RamCntRdRst - ok
FPGA_Write(_DetectorAddr ,0);//Detector = pos+neg 
FPGA_Write(_ReadBuffSize ,480); //ReadBuffSize

FPGA_Write(_AdcDelayAddr1 ,5); //130
FPGA_Write(_AdcDelayAddr2 ,0);

FPGA_Write(_ProbeDelay ,1);//10

FPGA_Write(_CompressAddr ,3);//Compress //3


	FPGA_Write(_TgcEnAddr ,1);//TgcEn - при 0 - пропадала генерация
{
		for(int i = 0; i< 479; i++)
		{
			FPGA_Write(_DacData1 ,20);//DacData[15:0]
			FPGA_Write(_DacData2 ,10);//DacData[31:16]
		}
	}

FPGA_Write(_AcousticContactGain ,10);

FPGA_Write(_DacCh ,0);//DacCh 0-1; 1-сильно шумит

FPGA_Write(_AttenSw ,0);
FPGA_Write(_AnChSwich ,0);//AnChSwich


FPGA_Write(_FilterEnAddr ,1);
FPGA_Write(_FilterCompressAddr ,1);
FPGA_Write(_FilterCoeffsRstAddrWr,1);
FPGA_Write(_FilterSwich,0);

for(int i=0; i<23; i++)
{
FPGA_Write(_FilterCoeffsAddr ,1/*koef_array[i]*/);
}
//////Analog init end/////////////////////////////////////////////////////////////////
}

void FPGA_Regs_deinit(void)//дополнить деинитом буфферов
{
	for (int i = 0; i<=127; i++){FPGA_Write(i ,0);}
FPGA_Write(0 ,0);

for (int i = 0; i<=127; i++){FPGA_Write(i ,0);}
FPGA_Write(0 ,0);	
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
	
FPGA_BUS_Init();

FPGA_Regs_deinit();

System_init();
Ascan_init();

//TEST_A-SCAN////////////////
draw_sine();
////////////////////////////

Gen_init();


Acust_init();


//////READ_ADC////////////////////////////////////////////////////////////////////

int arr[1010] = {0};
int adc_val = 0;
int adc_sample_cnt = 0;
int rd_index = 0;

int window_480 = 1;

//FPGA_Write(GenEn ,0);//GenEn

while(1)
{
	int t = 0;
	//while((t <= 10)||(t >= 950)){t = FPGA_Read(AdcBuffAddr);} //задержка 130
/*	
	while((t&(3<<9))==0) //Фильтр
	{
		t = FPGA_Read(AdcBuffAddr);
		rd_index++;
		if (rd_index>=479)
		{
			rd_index  = 0;
			FPGA_Write(RamCntRdRst ,0x01);
		}	
	} //задержка 0
*/	


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