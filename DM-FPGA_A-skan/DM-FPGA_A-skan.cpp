// DM-FPGA_A-skan.cpp : Defines the entry point for the console application.
//
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

//#include "UniDriver.h"
//#include "driverHelper.h"

#include "gpio.h"
#include "omap35xx_gpio.h"

#include <commctrl.h>

#include "GpioDriver.h"
//#include "GpioDriver.cpp"

#include "FPGA/FPGA.h"
//#include "FPGA/FPGA.cpp"

#include "ExtBusFunc.h" 

//#define DEBUG 
#define DCLK_PIN 130

//void ExtBusCs3Init();
//void SetCs3Value(DWORD val, DWORD addr);
//DWORD GetCs3Value(DWORD addr);
//DWORD ReadReg(RWRegData_t* readData);
//DWORD WriteReg(RWRegData_t* writeData);

int                SetupExtBus();
void			   SendKbdMsg(int vk);

HANDLE         hGPIO = 0;
HANDLE hCons;

char           lastError[1024];
DWORD		   err;


    DWORD addr = 0xF0;
	DWORD value = 0xff;

unsigned char CanExit; 
unsigned char Value;
bool Initialized = false;

DWORD ThreadProc(LPVOID lpParameter);
int FPGA_Programing();
int FPGA_Blast();
void ProgramByte( int one_byte );

int FPGA_DBUS_TEST();
int FPGA_ABUS_TEST();

int tmp_rd  = 0;

int koef_array[23];
/* 
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
*/

/////////////////////////////////////////////////////////////
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


int _tmain(int argc, _TCHAR* argv[])
{
GPIODriver *gpio=new GPIODriver();

//For ext bus debug
	int cnt = 1;
	int	A_skan = 0;
	int Ok = 1;
	u16 rd = 0;
	u8 AddrBusTest = 0;
	u16 DataBusTest = 0;

FPGA_Init();

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
	
for (int i = 0; i<=127; i++){FPGA_Write(i ,0);}
FPGA_Write(0 ,0);

//////System_init/////////////////////////////////////////////////////
FPGA_Write(SyncCtrlAddr ,0);//SyncCtrl - off
Sleep(1);
FPGA_Write(SyncCtrlAddr ,0);//SyncCtrl - off
FPGA_Write(FreqSync1Addr ,1000);//FreqSync - hight
FPGA_Write(FreqSync2Addr ,0);//FreqSync - low
FPGA_Write(CompressAddr ,1);//Compress
Sleep(1);
FPGA_Write(SyncCtrlAddr ,1);//SyncCtrl - on
Sleep(1);
FPGA_Write(SyncCtrlAddr ,1);//SyncCtrl - on
//////System_init/////////////////////////////////////////////////////

//////A-SCAN_init//////////////
FPGA_Write(AScanDrawMode ,2); //AScanDrawMode 0 или AScan№
FPGA_Write(AScanEnAddr ,2); //Если не произвести эту запись (AScanEn) - изображение на экране не появится!!!!!
FPGA_Write(AScanWrCS ,2);

FPGA_Write(AScanStartAddrWr ,0x00); //а тут - ок
FPGA_Write(AScanRamCntRdRst ,0x01); //не сбрасвается счётчик записи
//////A-SCAN_init//////////////

//TEST_A-SCAN////////////////
draw_sine();
////////////////////////////


//////GEN_init//////////////////////////////////////////////////////////////////////////
FPGA_Write(GenCSAddr ,1);//GenCS
FPGA_Write(GenEn ,1);//GenEn
FPGA_Write(GenStartAddrWr ,0);//GenStartAddrWr
//////////////////////////////////////
for (int i = 0; i < 256; ++i)
{
	FPGA_Write(GenBuffAddr1 ,0);
	FPGA_Write(GenBuffAddr2 ,0);
}
FPGA_Write(67 ,0);//GenStartAddrWr
//while(1)//for(int i = 0; i<10; i++)
{	
	FPGA_Write(GenBuffAddr1 ,1 + del_chk(160));
	FPGA_Write(GenBuffAddr2 ,0);

	FPGA_Write(GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(GenBuffAddr2 ,0);

	
    FPGA_Write(GenBuffAddr1 ,2 + del_chk(160));
	FPGA_Write(GenBuffAddr2 ,0);

	FPGA_Write(GenBuffAddr1 ,4 + del_chk(50));
	FPGA_Write(GenBuffAddr2 ,0);

	Sleep(10);
}
//////GEN_init////////////////////////////////////////////////////////////////////


//////ANALOG_init start///////////////////////////////////////////////////////////


//FPGA_Write(IntegratorCoefAddr ,0);//IntegratorKoef
//FPGA_Write(9 ,0);//InversionSignal
//FPGA_Write(AttenSw ,1);//AttenSw  - при 0 - пропадает генерация
FPGA_Write(TgcStartAddrWr ,0);//TgcStartAddrWr

FPGA_Write(RamCntRdRst ,1); //RamCntRdRst - ok
FPGA_Write(DetectorAddr ,0);//Detector = pos+neg 
FPGA_Write(ReadBuffSize ,480); //ReadBuffSize

FPGA_Write(AdcDelayAddr1 ,130);//130
FPGA_Write(AdcDelayAddr2 ,0);

FPGA_Write(ProbeDelay ,10);//10

FPGA_Write(CompressAddr ,3);//Compress //3


FPGA_Write(TgcEnAddr ,1);//TgcEn - при 0 - пропадала генерация
{
for(int i = 0; i< 479; i++)
{
	FPGA_Write(DacData1 ,0);//DacData[15:0]
	FPGA_Write(DacData2 ,0);//DacData[31:16]
}
}

FPGA_Write(AcousticContactGain ,1);

FPGA_Write(DacCh ,0);//DacCh 0-1; 1-сильно шумит

FPGA_Write(AttenSw ,0);
FPGA_Write(AnChSwich ,0);//AnChSwich


FPGA_Write(FilterEnAddr ,1);
FPGA_Write(FilterCompressAddr ,1);
FPGA_Write(FilterCoeffsRstAddrWr,1);
FPGA_Write(FilterSwich,0);

for(int i=0; i<23; i++)
{
FPGA_Write(FilterCoeffsAddr ,1/*koef_array[i]*/);
}

//FPGA_Write(SyncCtrlAddr ,1);//SyncCtrl 
/*FPGA_Write(AttenSw ,0);
FPGA_Write(AnChSwich ,0);//AnChSwich
FPGA_Write(AnChSwich ,1);//AnChSwich
FPGA_Write(AnChSwich ,2);//AnChSwich
FPGA_Write(AnChSwich ,3);//AnChSwich
*/
//////Analog init end/////////////////////////////////////////////////////////////////



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


FPGA_Write(RamCntRdRst ,0x01);
FPGA_Write(AScanStartAddrWr ,0x00);
FPGA_Write(AScanRamCntRdRst ,0x01);

for(int i = 0; i< 479; i++)
{	
	//arr[i] = FPGA_Read(AdcBuffAddr);
	//adc_val = arr[i];// / 6;

    adc_val = FPGA_Read(AdcBuffAddr);

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
	FPGA_Write(AScanBuffAddr1 ,adc_val+5);
	FPGA_Write(AScanBuffAddr2 ,adc_val);
	}
  }
}




/*for(int i = 0; i< 480; i++)
{
adc_val = 0;
		FPGA_Write(AScanBuffAddr1 ,adc_val+5);
		FPGA_Write(AScanBuffAddr2 ,adc_val);
}*/
//FPGA_Write(AScanStartAddrWr ,0x00);


/*
for(int i = 0; i< 480; i++)
{
	//if(!(i&1))
	{
	adc_val =  arr[i];

	if(adc_val>900){adc_val = adc_val/2;}
	if(adc_val>700){adc_val = adc_val/2;}
	if(adc_val>500){adc_val = adc_val/2;}
	if(adc_val>350){adc_val = adc_val/2;}

	adc_val -=150;
adc_val = 272 - adc_val;

	if(i&1)
	{
	  FPGA_Write(AScanBuffAddr1 ,adc_val+5);
	  //FPGA_Write(AScanBuffAddr2 ,adc_val);
	}
	//else			
	  FPGA_Write(AScanBuffAddr2 ,adc_val);
			//for(int i = 0; i<100000; i++){}
	}
}
*/
Sleep(50);
//Sleep(135);


/*
for(int i = 0; i< 100; i++)
{
adc_val = FPGA_Read(AdcBuffAddr);

if(adc_val>350){adc_val = adc_val/2;}
if(adc_val>500){adc_val = adc_val/3;}
if(adc_val>700){adc_val = adc_val/4;}
if(adc_val>900){adc_val = adc_val/5;}

//adc_val = adc_val/4;
//adc_val -=50;

		FPGA_Write(AScanBuffAddr1 ,adc_val+5);
		FPGA_Write(AScanBuffAddr2 ,adc_val);
	
		for(int i = 0; i<10000; i++){}
		adc_sample_cnt++;
}*/
//FPGA_Write(RamCntRdRst ,1); 
}
//////READ_ADC////////////////////////////////////////////////////////////////////

while(1)
{//STOP

}

while(1){
	
//FPGA_Write(99 ,cnt);		//0x2A
//FPGA_Write(112 ,cnt);
 /*rd = FPGA_Read(1); //0x3D

 rd = FPGA_Read(34); 

 rd = FPGA_Read(64); 

  rd = FPGA_Read(65);*/

	FPGA_Write(1 ,5);
	rd = FPGA_Read(1); 


for(int i=0;i<7;i++)
	{
		FPGA_Read( 1 << i );  
		rd = FPGA_Read(113);
		if(rd != 1<<i){	AddrBusTest |= 1<<i;} 
		else {Ok++;}
	}
AddrBusTest = 0; //12

for(int i=0;i<15;i++)
	{
		FPGA_Write(112 ,(u16)1<<i);
		FPGA_Write(127,(u16)~1<<i);
		rd = FPGA_Read(112);
		if(rd != 1<<i){	DataBusTest |= 1<<i;} 
		else {Ok++;}
	}
DataBusTest = 0;

 rd = FPGA_Read(64); 

  rd = FPGA_Read(65);

if(rd>1)
	{
		printf("regF\n"); 
		rd = 0;
	}
	Sleep(1);
	cnt = cnt*2;
} 
////////////////////



FPGA_Prg_Init();

/*
HANDLE hGPIOtest = GPIOOpen();
//GPIOSetMode(hGPIOtest,FPGA_NSTATUSPIO,GPIO_DIR_OUTPUT);
GPIOSetMode(hGPIOtest,FPGA_NSTATUSPIO,GPIO_DIR_INPUT);
//GPIOClrBit(hGPIOtest,FPGA_NSTATUSPIO);
//GPIOSetBit(hGPIOtest,FPGA_NSTATUSPIO);
//DWORD tpud = GPIOPullup(hGPIOtest, FPGA_NSTATUSPIO,1);
//DWORD tbd_ = GPIOGetBit(hGPIOtest, FPGA_NSTATUSPIO);
//DWORD tpdd = GPIOPulldown(hGPIOtest, FPGA_NSTATUSPIO,1);
DWORD tbd = GPIOGetBit(hGPIOtest, FPGA_NSTATUSPIO);
DWORD tmd = GPIOGetMode(hGPIOtest, FPGA_NSTATUSPIO);
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//GPIO_read_DbugSection
//FPGA_DATA0(1);
//gpio->PullUpGpioBit(FPGA_DATA0PIO);
//gpio->PullDownGpioBit(FPGA_DATA0PIO);
/*
while(FPGA_GETDATA0()==1);
	{
          printf("FPGA_GETDATA0 1\n");
	}

 while(FPGA_GETDATA0()==0);
	{
          printf("FPGA_GETDATA0 0\n");
	}


 while(FPGA_GETNSTATUS()==0);
	{
          printf("FPGA_GETNSTATUS 0 to 1\n");
	}

while(FPGA_GETNSTATUS()==1);
	{
          printf("FPGA_GETNSTATUS 1 to 0\n");
	}

 while(FPGA_GETCDONE()==0);
	{
          printf("FPGA_GETNCDONE 0 to 1\n");
	}

 while(FPGA_GETCDONE()==1);
	{
          printf("FPGA_GETNCDONE 1 to 0\n");
	}

	printf("\n All Gpio_Input tests Ok!\n\n");

	while(FPGA_GETNSTATUS()==1)
	{
		Sleep(5);
		FPGA_DATA0(0);
		Sleep(15);
		FPGA_DATA0(1);

	}

gpio->PullUpGpioBit(FPGA_NSTATUSPIO);
gpio->PullUpGpioBit(FPGA_CDONEPIO);


while(FPGA_GETNSTATUS()==1);
	{
          printf("FPGA_GETNSTATUS 1\n");
	}

 while(FPGA_GETNSTATUS()==0);
	{
          printf("FPGA_GETNSTATUS 0\n");
	}

 while(FPGA_GETCDONE()==1);
	{
          printf("FPGA_GETNCDONE 1\n");
	}
 while(FPGA_GETCDONE()==0);
	{
          printf("FPGA_GETNCDONE 0\n");
	}

gpio->PullDownGpioBit(FPGA_NSTATUSPIO );
gpio->PullDownGpioBit(FPGA_CDONEPIO  );


while(FPGA_GETNSTATUS()==1);
	{
          printf("FPGA_GETNSTATUS 1\n");
	}

 while(FPGA_GETNSTATUS()==0);
	{
          printf("FPGA_GETNSTATUS 0\n");
	}

 while(FPGA_GETCDONE()==1);
	{
          printf("FPGA_GETNCDONE 1\n");
	}
 while(FPGA_GETCDONE()==0);
	{
          printf("FPGA_GETNCDONE 0\n");
	}
	*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//DWORD ThreadProc(LPVOID lpParameter)
//{
	static char programmed = 0;
	int result;

		//result = FPGA_Programing();
		result = FPGA_Blast();

		//SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGAPRG, result, 0);
		//FPGA_Init();
		//PrgFPGATestAddr->Show();
		 printf("FPGA_Programing resultn %i\n",result);
        
		result = FPGA_DBUS_TEST();
		//SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGAD TST, result, 0);
		//PrgFPGATestData->Show();
		printf("FPGA_DBUS_TEST resultn %i\n",result);
        
		result = FPGA_ABUS_TEST();
		//SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGAATST, result, 0);
		printf("FPGA_ABUS_TEST resultn %i\n",result);
		programmed = 1;
//}

/*
FPGA_DCLK(0); 
FPGA_NCONFIG(0);
while(FPGA_GETNSTATUS()==1);
FPGA_NCONFIG(1);
while(FPGA_GETNSTATUS()==0);

//cout<<"NStatus "<<((int)FPGA_GETNSTATUS())<<endl;
//cout<<"CDONE "<<((int)FPGA_GETCDONE())<<endl;
printf("NStatus = %d\n",((int)FPGA_GETNSTATUS()));
printf("CDONE = %d\n",((int)FPGA_GETCDONE()));

HANDLE hFile;
hFile = CreateFile (TEXT("\\FlashDisk\\RDM-35_FPGA.rbf"),
GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);                 

if (hFile == INVALID_HANDLE_VALUE)	
{
//cout<<"Could not open RDM-35_FPGA.rbf"<<endl; 
printf("Could not open RDM-35_FPGA.rbf\n");
}
else
{
printf("Ok\n");
//cout<<"Ok"<<endl;
DWORD nr = 16384;
DWORD count;
DWORD size = 0;
u8 *buf =(u8*) malloc (sizeof(u8)*nr);
DWORD dwBytesRead= 0;
int i,j;
while(count = (ReadFile (hFile, buf, nr, &dwBytesRead, NULL)))
{	 
size+=nr;
// for(i=0;i<dwBytesRead;i++)
// {
//	 for(j=0;j<8;j++)
//	 {
//    FPGA_DATA0((buf[i]>>j)&1);
//	  FPGA_DCLK(1);
//	  FPGA_DCLK(0);
//	 }
// }
FPGA_Write_Prg_Buf((unsigned short*)buf, dwBytesRead);

if(dwBytesRead != nr) {size += dwBytesRead; break;};
printf("size = %d\n",size);
//cout<<size<<endl;
}

printf("size = %d\n",size);
printf("Readed = %d\n",dwBytesRead);
//cout<<"Size "<<size<<endl;
//cout<<"Readed "<<dwBytesRead<<endl;


free(buf);
CloseHandle (hFile);
}

printf("NStatus = %d\n",((int)FPGA_GETNSTATUS()));
printf("CDONE = %d\n",((int)FPGA_GETCDONE()));
//cout<<"NStatus "<<((int)FPGA_GETNSTATUS())<<endl;
//cout<<"CDONE "<<((int)FPGA_GETCDONE())<<endl;

*/
/*FpgaProgramming_END*/
//=========================================================




//GPIO_TEST_BENCH START
//printf("GPIO section\n");
/*hGPIO = GPIOOpen();  
if(!hGPIO)
{
	printf("GPIOOpen() error\n");
	//return 0;
};
*/ 
//GPIOSetMode(hGPIO, DCLK_PIN, GPIO_DIR_OUTPUT);
#define CUST_PIN 130 //181
/*
GPIOSetMode(hGPIO, CUST_PIN, GPIO_DIR_INPUT);
printf("PIN mode in= %d\n",GPIOGetMode(hGPIO, CUST_PIN));
GPIOSetMode(hGPIO, CUST_PIN, GPIO_DIR_OUTPUT);
printf("PIN mode out= %d\n",GPIOGetMode(hGPIO, CUST_PIN));
*/
//GPIOPullup(hGPIO, CUST_PIN, 1);
//printf("PIN mode pUp= %d\n",GPIOGetMode(hGPIO, CUST_PIN));

/*
///===========================================================
GPIODriver *gpio=new GPIODriver();
	gpio->SetGpioMode(DCLK_PIN, GPIO_DIR_OUTPUT);
	for(int i=0;i<100;i++)
		while(1)
	{
		gpio->SetGpioBit(DCLK_PIN); 
		Sleep(5);
		gpio->ClrGpioBit(DCLK_PIN); 
		Sleep(1);
		gpio->FPGA_DCLK(true);
		Sleep(50);
		gpio->FPGA_DCLK(false);
		Sleep(50);
	}
delete gpio;
///============================================================
*/

/*
printf("hGPIO %x\n", hGPIO);
while(1) 
{
printf("PIN val= %d\n", GPIOGetBit(hGPIO, CUST_PIN));
Sleep(10); 

GPIOSetBit(hGPIO, CUST_PIN);

 printf("PIN val= %d\n", GPIOGetBit(hGPIO, CUST_PIN));
 Sleep(1); 

GPIOClrBit(hGPIO, CUST_PIN);
}
GPIOClose(hGPIO);
//GPIO_TEST_BENCH END
*/

//printf("SPI section\n");
/*
while(1)
{
// printf("OUT %d\n",  SPI_Tx_buf[0]);
// printf("IN %d\n", ( SPI_Rx_buf[0])); //if Signed short ->&0xFFFF logic and to distinguish 16 bit frame
 //DeviceIoControl(hPortSPI,IOCTL_SPI_WRITEREAD,SPI_Rx_buf,sizeof(SPI_Rx_buf),SPI_Tx_buf,sizeof(SPI_Tx_buf),NULL,NULL );
 //SendKbdMsg(0x70+SPI_Rx_buf[0]);
 //Sleep(5); 
Sleep(100); 
 } 
//CloseSPI(); 
*/



//printf("Stay in infinite loop\n");
//while(1){Sleep(1000); };
//FPGA_Write(61 ,(unsigned short)1<<i);
int major_version = FPGA_Read(63);
int svn_version = FPGA_Read(64);
printf("major_version = %i\n svn_version = %i\n",major_version,svn_version);
FPGA_Write(1 ,0xFF); //Регистр частоты запуска генераторов (генерации синхроимпульсов). 
FPGA_Write(2 ,0xFF); //Регистр частоты запуска генераторов (генерации синхроимпульсов). 
FPGA_Write(3 ,1); //внутренний таймер
FPGA_Write(5 ,3); //отрицательный + положительный сигнал
int regF = FPGA_Read(1);
int gen  = FPGA_Read(5);
printf("regF = %i\n gen = %i\n",regF,gen);
Sleep(60000);

return 0;
}

/*
void SendKbdMsg(int vk)
{
	KEYBDINPUT kInp; 	//char down;
	INPUT x1;

	kInp.wVk=vk;
	//if(down)  kInp.dwFlags=0; 
	//else
	// kInp.dwFlags=KEYEVENTF_KEYUP;//? надо ли
	x1.type=INPUT_KEYBOARD;
	x1.ki=kInp;
	SendInput(1,&x1,sizeof(INPUT));
}
//pervii parametr eto 0-down, 1 - up ... vtoroi et konstanta knopki takie kak :VK_F1,VK_HOME ...
*/

/* //программирование фпга
		result = FPGA_Programing();
		SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGAPRG, result, 0);
		//FPGA_Init();
		PrgFPGATestAddr->Show();
        
		result = FPGA_DBUS_TEST();
		SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGADTST, result, 0);
		PrgFPGATestData->Show();
        
		result = FPGA_ABUS_TEST();
		SendMessage(MainW->GetHWnd(), WM_MYMSG_FPGAATST, result, 0);
		programmed = 1;
*/

/*
void FPGA_DATA0(u8 x)
{
	bool IsError = false;
	if(x)
	{
		WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpsr0, 1, 1<<2, IsError);
	}else
	{
		WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpcr0, 1, 1<<2, IsError);
	}
}
void FPGA_NCONFIG(u8 x)
{
	bool IsError = false;
	if(x)
	{
		//WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpsr1, 1, 1<<18);
		WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpsr0, 1, 1<<5, IsError);
	}else
	{
		//WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpcr1, 1, 1<<18);
		WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpcr0, 1, 1<<5, IsError);
	}
}

u8 FPGA_GETNSTATUS()
{
	bool IsError = false;
	u32 GPLR1 = ReadReg(PXA_GPIO_BASE,(void*)GPIO_Temp,GPIO_Temp->gplr1,1, IsError);
	if(IsError) return 0;
	return (u8)((GPLR1>>22) & 1);
}

u8 FPGA_GETCDONE()
{
	bool IsError = false;
	u32 GPLR0 = ReadReg(PXA_GPIO_BASE,(void*)GPIO_Temp,GPIO_Temp->gplr0,1, IsError); 
	if(IsError) return 0;
	return (u8)((GPLR0>>6) & 1);
}
*/


int FPGA_Programing()
{
#ifdef WINCE
int result = 0;
//#define FPGA_DCLKPIO     GPIO59 //rdm35
/*
#define FPGA_DCLKPIO		GPIO27 
#define FPGA_DATA0PIO		GPIO2
#define FPGA_NSTATUSPIO		GPIO54
#define FPGA_NCONFIGPIO     GPIO5
#define FPGA_CDONEPIO       GPIO6
*/ 

#define FPGA_DCLKPIO     130
#define FPGA_DATA0PIO    135
#define FPGA_NSTATUSPIO  134
#define FPGA_CDONEPIO    133
#define FPGA_NCONFIGPIO  131

	bool IsError = false;
	int counter = 0;

	FPGA_Prg_Init();

	FPGA_DCLK(0);
	FPGA_NCONFIG(0);
/*
while((FPGA_GETNSTATUS()==1)&&(counter++<1000));
	if(counter>=1000)
	{
          printf("No FPGA_GETNSTATUS 1\n");
          return -1;
	}
*/
	FPGA_NCONFIG(1);

 /*while((FPGA_GETNSTATUS()==0)&&(counter++<1000));
	if(counter>=1000)
	{
          printf("No FPGA_GETNSTATUS 0\n");
          return -1;
	}*/


printf("NStatus = %d ",((int)FPGA_GETNSTATUS()));
printf("CDONE = %d\n",((int)FPGA_GETCDONE()));
	//	cout<<"NStatus "<<((int)FPGA_GETNSTATUS())<<endl;
	//	cout<<"CDONE "<<((int)FPGA_GETCDONE())<<endl;
	HANDLE hFile; 
	hFile = CreateFile (TEXT("YAFFS_PART1/RDM-35_FPGA.rbf"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	//hFile = CreateFile (FPGAFileCMD, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);                 

	if (hFile == INVALID_HANDLE_VALUE)	
	{
		printf("RDM-35_FPGA.rbf not found\n");
		return -2;
	}
	else
	{
		//cout<<"Ok"<<endl;
		printf("Ok\n");
		DWORD nr = 32000;
//		DWORD count;
		DWORD size = 0;
		DWORD estimatedSize = 0;
		u8 *buf = new u8[nr];//(u8*) malloc (sizeof(u8)*nr);
		DWORD dwBytesRead= 0;
//		int i,j;
		int Value1;
		SetFilePointer (hFile, 0, NULL, FILE_BEGIN); 
		while(1)
		{
			ReadFile (hFile, buf, nr, &dwBytesRead, NULL);
			estimatedSize +=dwBytesRead;
			if(nr!=dwBytesRead) 
				break;
		}
		//TimeCheck();
		
		SetFilePointer (hFile, 0, NULL, FILE_BEGIN) ; 
		size = 0;
		while(size < estimatedSize)
		{	 
			ReadFile (hFile, buf, nr, &dwBytesRead, NULL);
			size+=dwBytesRead;
			
			if(FPGA_Write_Prg_Buf((unsigned short*)buf, (unsigned short)dwBytesRead))
			{
				Value1 = (100 * size) / estimatedSize;
				if(Value1 >= 100) 
				{
					Value = 99;
					result  = true;
				}
				else
				{
					Value = Value1;
				}
				//InvalidateRect(MainW->GetHWnd(), NULL, 0);
				if(dwBytesRead != nr) 
				{
					size = estimatedSize;
				};
			}
			else
			{
				size = estimatedSize; 
				break;
			}
		}
		delete []buf;
		CloseHandle (hFile);
		return result;
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
	return -3;
}

#define INIT_CYCLE			50		/* extra DCLK cycles while initialization is in progress */
#define RECONF_COUNT_MAX	5		/* #auto-reconfiguration when error found */
#define	CHECK_EVERY_X_BYTE  10240	/* check NSTATUS pin for error for every X bytes */
									/* DO NOT put '0' */
#define CLOCK_X_CYCLE		0		/* clock another 'x' cycles during user mode (not necessary, 
									   for debug purpose only) */

int FPGA_Blast()
{
	char FileName []={"YAFFS_PART1/rdm.rbf"};
    char TestFileName []={"YAFFS_PART1/Test.rbf"};

	bool IsError = false;
	long int counter = 0;
	int	program_done = 0;			/* programming process (configuration and initialization) done = 1 */
	int	configuration_count = RECONF_COUNT_MAX;	/* # reprogramming after error */	
	int			confdone_ok = 1;			/* CONF_DONE pin. '1' - error */
	int			nstatus_ok = 0;				/* NSTATUS pin. '0' - error */
long int	i = 0;						/* standard counter variable */
	int			one_byte = 0;				/* the byte to program */


/* Open programming file as READ and in BINARY */
	FILE * ptrFile = fopen(FileName, "rb" );
	//FILE * ptrFile = fopen(TestFileName, "rb" );

	  if (ptrFile == NULL) { printf("fopen error\n");return -1;}
	/* Get file size */
	int seek_position = fseek(ptrFile, 0, SEEK_END );
		if (seek_position != NULL) { printf("fseek error = %i \n",seek_position );return -2;}
	int	file_size	= ftell( ptrFile );
	fprintf( stdout, "Info: Programming file size: %ld\n", file_size );//181945


//PINS_INIT---------------------------------------------------------------------------
	FPGA_Prg_Init();
//PINS_INIT---------------------------------------------------------------------------

//------------------------------------------------------------------------
fprintf( stdout, "-- Start configuration process...\n" );
		// Start configuration 
	while ( !program_done && (configuration_count>0) )
	{
		// Reset file pointer and parallel port registers 
		fseek(ptrFile, 0, SEEK_SET );

		// Drive a transition of 0 to 1 to NCONFIG to indicate start of configuration 
	FPGA_DCLK(0);
	FPGA_NCONFIG(0);
while((FPGA_GETNSTATUS()==1)&&(counter++<100000));
	if(counter>=1000){
          printf("No STATUS 1->0\n"); //return -1;
	}
	FPGA_NCONFIG(1);
while((FPGA_GETNSTATUS()==0)&&(counter++<100000));
	if(counter>=1000){
          printf("No STATUS 0->1\n");  //return -1;
	}
printf("NStatus = %d ",((int)FPGA_GETNSTATUS()));//normal state must be 1
printf("CDONE = %d\n",((int)FPGA_GETCDONE()));//normal state must be 0

		// Loop through every single byte 
		for ( i = 0; i < file_size; i++ )
		{
			one_byte = fgetc(ptrFile);

			// Progaram a byte 
			ProgramByte( one_byte );

			// Check for error through NSTATUS for every 10KB programmed and the last byte 
			if ( !(i % CHECK_EVERY_X_BYTE) || (i == file_size - 1) )
			{
				nstatus_ok = FPGA_GETNSTATUS();

				if ( !nstatus_ok )
				{
					printf("Conf error = %d\n",configuration_count-1);

					program_done = 0;
					break;
				}
				else 
					program_done = 1;
			}
		}//end for

		configuration_count--;
printf("Attempt- %i",configuration_count);
		if ( !program_done )
			continue;

		//Configuration end 
		// Check CONF_DONE that indicates end of configuration 
		confdone_ok = FPGA_GETCDONE();

		if ( confdone_ok )
		{
			printf("Error: Configuration done CONF_DONE is %i\n", confdone_ok  );
			program_done = 0;
			printf("configuration_count is %i\n",configuration_count);	
			if ( configuration_count == 0 )
				break;
		}
	
		// if contain error during configuration, restart configuration 
		if ( !program_done )
			continue;

		 program_done = 1; 

		//Start initialization 
		// Clock another extra DCLK cycles while initialization is in progress
		 //  through internal oscillator or driving clock cycles into CLKUSR pin 
		// These extra DCLK cycles do not initialize the device into USER MODE 
		// It is not required to drive extra DCLK cycles at the end of
		//   configuration													   
		// The purpose of driving extra DCLK cycles here is to insert some delay
		 //  while waiting for the initialization of the device to complete before
		 //  checking the CONFDONE and NSTATUS signals at the end of whole 
		 //  configuration cycle 											       
		for ( i = 0; i < INIT_CYCLE; i++ )
		{
			FPGA_DCLK(0);
			FPGA_DCLK(1);
		}
		// Initialization end 

		nstatus_ok = FPGA_GETNSTATUS();
		confdone_ok = FPGA_GETCDONE();

		if ( !nstatus_ok || confdone_ok )
		{
			printf("Error: Initialization finish but contains error: NSTATUS is %s and CONF_DONE is %s. Exiting...", (nstatus_ok?"HIGH":"LOW"), (confdone_ok?"LOW":"HIGH") );
			program_done = 0; 
			configuration_count = 0; // No reconfiguration 
		}
	}

	Sleep(100);//for gebug
	// Add another 'x' clock cycles while the device is in user mode.
	// This is not necessary and optional. Only used for debugging purposes 
	if ( CLOCK_X_CYCLE > 0 )
	{
		printf( "Info: Clock another %d cycles in while device is in user mode...\n", CLOCK_X_CYCLE );
		for ( i = 0; i < CLOCK_X_CYCLE; i++ )
		{
			FPGA_DCLK(0);
			FPGA_DCLK(1);
		}
	}

	if ( !program_done )
	{
		printf( "\nError: Configuration not successful! Error encountered...\n" );
		/*if (BBII)
			SetPortMode(PM_RESET_BBII);
		else if (BBMV)
			SetPortMode(PM_RESET_BBMV);
		return;*/
	}

	fprintf( stdout, "\nInfo: Configuration successful!\n" );
//------------------------------------------------------------------------

return 0;
}
/********************************************************************************/
/*	Name:			Dump2Port													*/
/*																				*/
/*	Parameters:		int signal, int data, int clk	 							*/
/*					- name of the signal (SIG_*).								*/
/*					- value to be dumped to the signal.							*/
/*					- assert a LOW to HIGH transition to SIG_DCLK togther with	*/
/*					  [signal].													*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump [data] to [signal]. If [clk] is '1', a clock pulse is	*/
/*					generated after the [data] is dumped to [signal].			*/
/*																				*/
/********************************************************************************/
/*
void Dump2Port( int signal, int data, int clk )
{
	//Get signal port number  
	int port = sig_port_maskbit[signal][0];

	//Get signal mask bit 
	int mask;
	
	//If clk == 1, drive signal with [data] and drive SIG_DCLK with '0' together. Then drive SIG_DCLK with '1' 
	//That is to create a positive edge pulse  
	if ( clk )
		mask = sig_port_maskbit[signal][1] | sig_port_maskbit[SIG_DCLK][1];
	else
		mask = sig_port_maskbit[signal][1];
	
	  // AND signal bit with '0', then OR with [data] 
	mask = ~mask;
	port_data[port] = ( port_data[port] & mask ) | ( data * sig_port_maskbit[signal][1] );
	
	WriteByteBlaster( port, port_data[port], 0 );

	if ( clk )
	{
		WriteByteBlaster( port, ( port_data[port] | sig_port_maskbit[SIG_DCLK][1] ), 0 );
	}
}
*/
/********************************************************************************/
/*	Name:			ProgramByte													*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump to parallel port bit by bit, from least significant	*/
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted.											*/
/*																				*/
/********************************************************************************/

void ProgramByte( int one_byte )
{
	int	bit = 0;
	int i = 0;

	//write from LSb to MSb  
	for ( i = 0; i < 8; i++ )
	{
		bit = one_byte >> i;
		bit = bit & 0x1;
		
		//Dump to DATA0 and insert a positive edge pulse at the same time  
		//Dump2Port( SIG_DATA0, bit, 1 );
	  FPGA_DATA0(bit);
	  FPGA_DCLK(1);
	  //Sleep(1);
	  FPGA_DCLK(0);
	  //Sleep(1);
	}
}




int FPGA_DBUS_TEST()
{
	char Ok = 1;
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
		FPGA_Write(61 ,(unsigned short)1<<i);
		FPGA_Write(127,(unsigned short)~1<<i);
		temp = FPGA_Read(61);
		if(temp != 1<<i)
			Ok ++;
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
	char Ok = 1;

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
		temp = FPGA_Read(62);
		if(temp != 1<<i) 
			Ok ++;
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
