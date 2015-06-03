//#include "stdafx.h"
#include <windows.h>
#include "../UniDriver.h"
//#include "ClockMgr.h"
//#include "GPIO.h"
#include "../GpioDriver.h"
#include "../ExtBusFunc.h"
#include "FPGA.h"

//#include <iostream>
//using namespace std;

//GPIO_REGS *GPIO_Temp;// = (GPIO_REGS*) malloc (sizeof(GPIO_REGS));

GPIODriver *FPGA_prg_pio=new GPIODriver();
UniDriver gmi;


void FPGA_Prg_Init() 
{
	bool IsError = false;
/*	
    GPIO_REGS *GPIO = (GPIO_REGS*) malloc (sizeof(GPIO_REGS));
    WriteReg(PXA_GPIO_BASE, (void*)GPIO, GPIO->gpsr1, 1, 1<<18, IsError);
	if(IsError) goto FExit;
	GPIO->gpdr0 = ReadReg(PXA_GPIO_BASE,(void*)GPIO,GPIO->gpdr0,1, IsError); 
	if(IsError) goto FExit;
	GPIO->gpdr1 = ReadReg(PXA_GPIO_BASE,(void*)GPIO,GPIO->gpdr1,1, IsError); 
	if(IsError) goto FExit;

	GPIO->gpdr0 |= 1<<27; //gpio27 output
	GPIO->gpdr0 |= 1<<2; //gpio2 output
//	GPIO->gpdr1 |= 1<<18; //gpio50 output ---------v
	GPIO->gpdr0 |= 1<<5; //gpio5 output       <----| 
	GPIO->gpdr0 &= (u32)~(1<<6); //gpio6 input
	GPIO->gpdr1 &= (u32)~(1<<22); //gpio54 input
*/
//GPIODriver *FPGA_prg_pio=new GPIODriver();	
FPGA_prg_pio->SetGpioMode(FPGA_DCLKPIO,     GPIO_DIR_OUTPUT); //output
FPGA_prg_pio->SetGpioMode(FPGA_DATA0PIO,    GPIO_DIR_OUTPUT);//output
FPGA_prg_pio->SetGpioMode(FPGA_NSTATUSPIO,  GPIO_DIR_INPUT);//input
FPGA_prg_pio->SetGpioMode(FPGA_CDONEPIO,    GPIO_DIR_INPUT);//input
FPGA_prg_pio->SetGpioMode(FPGA_NCONFIGPIO,  GPIO_DIR_OUTPUT);//output


	//WriteReg(PXA_GPIO_BASE,(void*)GPIO,GPIO->gpdr0,1,GPIO->gpdr0, IsError);
	//if(IsError) goto FExit;
	//WriteReg(PXA_GPIO_BASE,(void*)GPIO,GPIO->gpdr1,1,GPIO->gpdr1, IsError);
	
//FExit:
    //free((void *)GPIO);
}

void FPGA_Init()
{
	bool IsError = false;
    HANDLE hDrv=NULL;
/*
	// for 48 mhz clk - begin
	volatile u32 PinsClk[]={1, GPIO59};
	const u8 PinsClk_Af1[]={1,2};
	const u8 PinsClk_PP1[]={1,0};
	GPIOConfig(PinsClk,PinsClk_Af1,PinsClk_PP1, IsError);
	if(IsError) return;
*/	

   // for 26 mhz clk - begin
//FPGA_prg_pio->SetGpioMode(10,GPIO_DIR_OUTPUT); //GPIO10
//Generates 26mhz default!

	/*
	if(hDrv==NULL)
		hDrv=UniDriverOpen();
	if(hDrv == INVALID_HANDLE_VALUE) return;
	*/
//	DeviceIoControl(hDrv,IOCTL_EXTCS0,(PBYTE)NULL,1,NULL,0,NULL,NULL);// ?_?
ExtBusCs1Init();


//need to read and write GPMC_CONFIG2_1
DWORD tmp = 0;
tmp = GetRegVal(1);

//TODO �������� ����������� ����� ��������� � ������� � ������!


/*    
	//init cpld (colibri)
	SMEM_REGS *SMem = (SMEM_REGS*) malloc(sizeof(SMEM_REGS));
	SMem->MSC1 = ReadReg(PXA_SMEM_BASE,(void*)SMem,SMem->MSC1,1, IsError);
	if(IsError) goto FExit;
	SMem->MSC1 &= ~(0xf<<20);
       //SMem->MSC1 |= 0x1<<20; //rdm-35
	   //SMem->MSC1 |= 0x3<<20; //SD-sample
	SMem->MSC1 &= ~(0xf<<20);
	//SMem->MSC1 |= 0x1<<20;	//rdm-35
	//SMem->MSC1 |= 0x3<<20;	//SD-sample
	//SMem->MSC1 |= 0x7<<20;	//rdm-35
	SMem->MSC1  |= 0x7<<20;		//rdm-35
	SMem->MSC1 &= ~(0xf<<4);
	SMem->MSC1 |= 0x7<<4;
	  
	WriteReg(PXA_SMEM_BASE, (void*)SMem,SMem->MSC1,1,SMem->MSC1, IsError);
	if(IsError) goto FExit;
	SMem->MSC1 = ReadReg(PXA_SMEM_BASE,(void*)SMem,SMem->MSC1,1, IsError);
	if(IsError) goto FExit;
FExit:
	free((void*)SMem);
*/
}

/*
void FPGA_DCLK(u8 x)
{
	bool IsError = false;
	if(x)
	{
      WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpsr0, 1, 1<<27, IsError);
	}else
	{
      WriteReg(PXA_GPIO_BASE, (void*)GPIO_Temp, GPIO_Temp->gpcr0, 1, 1<<27, IsError);
	}
}
*/
//#error check : after define!
#define PIN_HI(pin_name) FPGA_prg_pio->SetGpioBit(pin_name); 
#define PIN_LO(pin_name) FPGA_prg_pio->ClrGpioBit(pin_name); 
//#define PIN_GET(pin_name, val) val=FPGA_prg_pio->GetGpioBit(pin_name);

void FPGA_DCLK(u8 val)
{
	if(val){PIN_HI(FPGA_DCLKPIO); }
	else 	{PIN_LO(FPGA_DCLKPIO);} 
}

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
*/
void FPGA_DATA0(u8 x)
{
	if(x){PIN_HI(FPGA_DATA0PIO); }
	else {PIN_LO(FPGA_DATA0PIO);} 
}
/*
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
*/

void FPGA_NCONFIG(u8 x)
{
	if(x){PIN_HI(FPGA_NCONFIGPIO); }
	else {PIN_LO(FPGA_NCONFIGPIO);} 
}
/*
u8 FPGA_GETNSTATUS()
{
	bool IsError = false;
	u32 GPLR1 = ReadReg(PXA_GPIO_BASE,(void*)GPIO_Temp,GPIO_Temp->gplr1,1, IsError);
	if(IsError) return 0;
    return (u8)((GPLR1>>22) & 1);
}
*/

u8 FPGA_GETNSTATUS()
{
return (u8)FPGA_prg_pio->GetGpioBit(FPGA_NSTATUSPIO);
}
/*
u8 FPGA_GETCDONE()
{
	bool IsError = false;
	u32 GPLR0 = ReadReg(PXA_GPIO_BASE,(void*)GPIO_Temp,GPIO_Temp->gplr0,1, IsError); 
	if(IsError) return 0;
	return (u8)((GPLR0>>6) & 1);
}
*/


u8 FPGA_GETCDONE()
{
return (u8)FPGA_prg_pio->GetGpioBit(FPGA_CDONEPIO);
}

u8 FPGA_GETDATA0()//for debug
{
return (u8)FPGA_prg_pio->GetGpioBit(FPGA_DATA0PIO);
}


void FPGA_Write(DWORD addr,unsigned short val)
{/*
  bool IsError = false;
  volatile u32 Offset = Addr*2;
  WriteReg(PXA_EXTCS0_BASE,(void *)&Offset,Offset,0,Value, IsError);
*/
	volatile u32 Offset = addr*2;
  	RWRegData_t regData = {GPMC_CS1_BASE, 0, 0};
	regData.value = val;
	regData.offset = Offset;
	gmi.WriteReg(&regData);	
}

u16 FPGA_Read(DWORD addr)
{
	u16 result;
	/*
  bool IsError = false;
  volatile u32 Offset = Addr*2;
  return ReadReg(PXA_EXTCS0_BASE,(void *)&Offset,Offset,0, IsError);
  */
	volatile u32 Offset = addr*2;
	RWRegData_t regData = {GPMC_CS1_BASE, 0, 0};
	regData.offset = Offset;
	result = gmi.ReadReg(&regData);
result = regData.value;
	return result;
}
unsigned int FPGA_Read32(DWORD fAddr, DWORD sAddr)
{
	unsigned int Result = FPGA_Read(sAddr);
	Result = (Result>>16) + FPGA_Read(fAddr);
	return Result;
}

void FPGA_Write_Buf(DWORD Addr, unsigned short *Buf, unsigned short Nr)
{
	bool IsError = false;
	volatile u32 Offset = Addr*2;
//	WriteReg_Buf(PXA_EXTCS0_BASE,(void *)&Offset,Offset,0,Buf,Nr, IsError);
//	SetCs1Value(Buf, Offset);
}

void FPGA_Read_Buf(DWORD Addr, unsigned short *Buf, unsigned short Nr)
{
	bool IsError = false;
	volatile u32 Offset = Addr*2;
// ReadReg_Buf(PXA_EXTCS0_BASE,(void *)&Offset,Offset,0,Buf,Nr, IsError);
}

void FPGA_Write_Buf (DWORD fAddr,  DWORD sAddr, unsigned int *Buf, unsigned short Nr)
{
	bool IsError = false;
	volatile u32 Offset = fAddr*2;
//	WriteReg_Buf(PXA_EXTCS0_BASE, (void *)&Offset, Offset, 0,(unsigned short*)Buf,Nr,sAddr*2, IsError);
}

void FPGA_Write32(DWORD fAddr, DWORD sAddr, unsigned int Value)
{
	FPGA_Write_Buf(fAddr, sAddr, &Value, 1);
}

void FPGA_Write_Buf_signed(DWORD Addr, short *Buf, unsigned short Nr)
{
	bool IsError = false;
	volatile u32 Offset = Addr*2;
//	WriteReg_Buf(PXA_EXTCS0_BASE,(void *)&Offset,Offset,0,(unsigned short*)Buf,Nr, IsError);
}

void FPGA_Write (DWORD Addr, unsigned short Value, DWORD Count) 
{
	bool IsError = false;
	volatile u32 Offset = Addr*2;
	if(Count == 1){SetCs1Value(Value, Offset);}
		//gmi.WriteReg(GPMC_CS1_BASE,(void *)&Offset,Offset,0,Value,IsError);
	else{SetCs1Value(Value, Offset);} //?_?
//		WriteReg(PXA_EXTCS0_BASE,(void *)&Offset,Offset,Count,Value,IsError);
}


bool FPGA_Write_Prg_Buf(unsigned short *Buf, unsigned short nr)
{
   bool IsError = false;
   volatile u32 Offset = 0;
 //  printf("Programming Fpga... ");
  // OutputDebugString(L"Programming Fpga ... \n");
//   WriteReg_Buf(PXA_FPGAPRG_BASE, (void *)&Offset, Offset, 0, Buf, Nr, IsError );
  // OutputDebugString(L"End Programming Fpga ... \n");
//============================================================

//DWORD nr = 16384;
DWORD count;
DWORD size = 0;
//u8 *buf =(u8*) malloc (sizeof(u8)*nr);
DWORD dwBytesRead= 0;
int i,j;
//while(count = (ReadFile (hFile, buf, nr, &dwBytesRead, NULL)))
//{	 
size+=nr;
 //for(i=0;i<dwBytesRead;i++)
 for(i=0;i<nr;i++)
 {
	 for(j=0;j<8;j++)
	 {
      FPGA_DATA0((Buf[i]>>j)&1);
	  FPGA_DCLK(1);
	  Sleep(0.1);
	  FPGA_DCLK(0);
	  Sleep(0.1);
	 }
 //}
//FPGA_Write_Prg_Buf((unsigned short*)buf, dwBytesRead);

if(dwBytesRead != nr) {size += dwBytesRead; break;};
}
 printf("size = %d",size);
//printf("End Programming Fpga.\n");
//===========================================================
   return !IsError;
}

/* 



/*programming fpga*/

/*
#define FPGA_DCLKPIO     GPIO59
#define FPGA_DATA0PIO    GPIO2
#define FPGA_NSTATUSPIO  GPIO54
#define FPGA_NCONFIGPIO     GPIO50
#define FPGA_CDONEPIO       GPIO6

volatile u32 FPGAPins[]={5, FPGA_DCLKPIO, FPGA_DATA0PIO,FPGA_NSTATUSPIO, FPGA_NCONFIGPIO, FPGA_CDONEPIO};
const u8 FPGAPins_Af1[]={5,0,0,0,0,0};
const u8 FPGAPins_PP1[]={5,3,3,3,3,3};

GPIOConfig(FPGAPins,FPGAPins_Af1,FPGAPins_PP1);   

GPIO_REGS *GPIO = (GPIO_REGS*) malloc (sizeof(GPIO_REGS));

FPGA_Prg_Init();
FPGA_DCLK(0);
FPGA_NCONFIG(0);
while(FPGA_GETNSTATUS()==1);
FPGA_NCONFIG(1);
while(FPGA_GETNSTATUS()==0);


cout<<"NStatus "<<((int)FPGA_GETNSTATUS())<<endl;
cout<<"CDONE "<<((int)FPGA_GETCDONE())<<endl;

HANDLE hFile;
hFile = CreateFile (TEXT("\\FlashDisk\\RDM-35_FPGA.rbf"),
GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);                 

if (hFile == INVALID_HANDLE_VALUE)	
{
cout<<"Could not open RDM-35_FPGA.rbf"<<endl; 
}
else
{
cout<<"Ok"<<endl;
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
cout<<size<<endl;
}

//cout<<"Size "<<size<<endl;
//cout<<"Readed "<<dwBytesRead<<endl;


free(buf);
CloseHandle (hFile);
}

cout<<"NStatus "<<((int)FPGA_GETNSTATUS())<<endl;
cout<<"CDONE "<<((int)FPGA_GETCDONE())<<endl;
*/