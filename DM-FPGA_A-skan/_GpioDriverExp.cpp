// GpioDriverExp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>

#include "GpioDriver.h"




int _tmain(int argc, _TCHAR* argv[])
{
	GPIODriver *gpio=new GPIODriver();

	gpio->SetGpioMode(DCLK_PIN, GPIO_DIR_OUTPUT);
//	for(int i=0;i<100;i++)
		while(1)
	{
		gpio->SetGpioBit(DCLK_PIN); 
		Sleep(50);
		gpio->ClrGpioBit(DCLK_PIN); 
		Sleep(10);
		//gpio->FPGA_DCLK(true);
		//Sleep(50);
		//gpio->FPGA_DCLK(false);
	}

	delete gpio;
	return 0;
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