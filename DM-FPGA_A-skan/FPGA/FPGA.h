#pragma once
//#include "PXA320_Types.h"
//#include "pxaUniLib.h"
//#include "PXA320_LIB.h"

#define FPGA_DCLKPIO     130
#define FPGA_DATA0PIO    135
#define FPGA_NSTATUSPIO  134
#define FPGA_CDONEPIO    133
#define FPGA_NCONFIGPIO  131


typedef char           u8;
typedef unsigned short u16;
typedef unsigned int   u32;

void FPGA_Init(); //Inits clock and ext_cs0(CPLD)
void FPGA_Write(DWORD Addr,unsigned short Value);
void FPGA_Write (DWORD Addr, unsigned short Value, DWORD Count) ;
u16 FPGA_Read(DWORD Addr);
void FPGA_Write_Buf(DWORD Addr, unsigned short *Buf, unsigned short Nr);
bool FPGA_Write_Prg_Buf(unsigned short *Buf, unsigned short Nr);
void FPGA_Write_Buf (DWORD fAddr,  DWORD sAddr, unsigned int *Buf, unsigned short Nr);
void FPGA_Read_Buf(DWORD Addr, unsigned short *Buf, unsigned short Nr);
void FPGA_Write_Buf_signed(DWORD Addr, short *Buf, unsigned short Nr);
void FPGA_Prg_Init();
void FPGA_DCLK(u8 val);
u8 FPGA_GETNSTATUS();
u8 FPGA_GETCDONE();
u8 FPGA_GETDATA0(); //for debug
void FPGA_NCONFIG(u8 x);
void FPGA_DATA0(u8 x);

void ExtBusCs1Init();


/************************************************************************
 TABLE WITH ALL FPGA ADDRESSES
************************************************************************/
enum FPGA_Addresses
{
	FreqSync1Addr = 1, FreqSync2Addr, SyncCtrlAddr, CompressAddr, DetectorAddr, AnChSwich, AttenSw, IntegratorCoefAddr, InversionSignal, AcoustContSum,
	TgcEnAddr = 11, DacData1, DacData2, TgcStartAddrWr, DacCh, AcousticContactGain,
    
	GenBuffAddr1 = 21, GenBuffAddr2, GenCSAddr, GenStartAddrWr, GenEn,
	FilterEnAddr = 30, FilterCompressAddr, FilterCoeffsAddr, FilterCoeffsRstAddrWr,
	AScanDrawMode = 39,
	AScanEnAddr = 40, AScanBuffAddr1, AScanWrCS, AScanStartAddrWr, AScanRamCntRdRst,
	AScanBuffAddr2 = 45, Cursor1CoordX, Cursor1CoordY, Cursor2CoordX, Cursor2CoordY,

	RamCntRdRst = 51, AdcDelayAddr1, AdcDelayAddr2, AdcBuffAddr, ReadBuffSize, ProbeDelay,
	TestDataBus = 61, TestAddrBus, MajorVersion, SvnVersion,

	Gate1Start_1 = 70, Gate1Start_2, Gate1End_1, Gate1End_2, Gate1Level, Gate1TFirst_1, Gate1TFirst_2, Gate1TMax_1, Gate1TMax_2, Gate1T0_1, Gate1T0_2,
	Gate2Start_1	 , Gate2Start_2, Gate2End_1, Gate2End_2, Gate2Level, Gate2TFirst_1, Gate2TFirst_2, Gate2TMax_1, Gate2TMax_2, Gate2T0_1, Gate2T0_2
};
/* DEFAULT
#define FreqSync1Addr 1
#define FreqSync2Addr 2
#define SyncCtrlAddr 3
#define CompressAddr 4
#define DetectorAddr 5
#define AnChSwich 6
#define AttenSw 7
#define IntegratorCoefAddr 8
#define InversionSignal 9
#define AcoustContSum 10

#define TgcEnAddr 11
#define DacData1 12
#define DacData2 13
#define TgcStartAddrWr 14
#define DacCh 15
#define AcousticContactGain 16

#define GenBuffAddr1 21
#define GenBuffAddr2 22
#define GenCSAddr 23
#define GenStartAddrWr 24
#define GenEn 25

#define FilterEnAddr 30
#define FilterCompressAddr 31
#define FilterCoeffsAddr 32
#define FilterCoeffsRstAddrWr 33
#define AScanDrawMode 39

#define AScanEnAddr 40
#define AScanBuffAddr1 41
#define AScanWrCS 42
#define AScanStartAddrWr 43
#define AScanRamCntRdRst 44
#define AScanBuffAddr2 45

#define Cursor1CoordX 46
#define Cursor1CoordY 47
#define Cursor2CoordX 48
#define Cursor2CoordY 49

#define RamCntRdRst 51
#define AdcDelayAddr1 52
#define AdcDelayAddr2 53
#define AdcBuffAddr 54
#define ReadBuffSize 55
#define ProbeDelay 56
#define TestDataBus 61
#define TestAddrBus 62
#define MajorVersion 63
#define SvnVersion 64

#define Gate1Start_1 70
#define Gate1Start_2 71
#define Gate1End_1 71
#define Gate1End_2 73
#define Gate1Level 74
#define Gate1TFirst_1 75
#define Gate1TFirst_2 76
#define Gate1TMax_1 77
#define Gate1TMax_2 78
#define Gate1T0_1 79
#define Gate1T0_2 80

#define Gate2Start_1 81
#define Gate2Start_2 82
#define Gate2End_1 83
#define Gate2End_2 84
#define Gate2Level 85
#define Gate2TFirst_1 86
#define Gate2TFirst_2 87
#define Gate2TMax_1 88
#define Gate2TMax_2 89
#define Gate2T0_1 90
#define Gate2T0_2 91
*/

// New
#define FreqSync1Addr 1
#define FreqSync2Addr 2
#define SyncCtrlAddr 3
#define CompressAddr 4
#define DetectorAddr 5
#define AnChSwich 19
#define AttenSw 16
#define IntegratorCoefAddr 8
#define InversionSignal 81
#define AcoustContSum 10    //RD

#define TgcEnAddr 11
#define DacData1 48
#define DacData2 50
#define TgcStartAddrWr 14
#define DacCh 49
#define AcousticContactGain 95

#define GenBuffAddr1 64
#define GenBuffAddr2 65
#define GenCSAddr 66
#define GenStartAddrWr 67
#define GenEn 80

#define FilterEnAddr 30
#define FilterCompressAddr 31
#define FilterCoeffsAddr 32
#define FilterCoeffsRstAddrWr 33
#define FilterSwich 34
#define AScanDrawMode 96

#define AScanEnAddr 97
#define AScanBuffAddr1 98
#define AScanWrCS 112
#define AScanStartAddrWr 113
#define AScanRamCntRdRst 114
#define AScanBuffAddr2 99

#define Cursor1CoordX 115
//#define Cursor1CoordY 47
//#define Cursor2CoordX 48
//#define Cursor2CoordY 49

#define RamCntRdRst 51
#define AdcDelayAddr1 52
#define AdcDelayAddr2 53
#define AdcBuffAddr 82      //RD
#define ReadBuffSize 55
#define ProbeDelay 56
#define TestDataBus 17  //R/W
#define TestAddrBus 18
//#define MajorVersion 63
//#define SvnVersion 64
/*
#define Gate1Start_1 70
#define Gate1Start_2 71
#define Gate1End_1 71
#define Gate1End_2 73
#define Gate1Level 74
#define Gate1TFirst_1 75
#define Gate1TFirst_2 76
#define Gate1TMax_1 77
#define Gate1TMax_2 78
#define Gate1T0_1 79
#define Gate1T0_2 80

#define Gate2Start_1 81
#define Gate2Start_2 82
#define Gate2End_1 83
#define Gate2End_2 84
#define Gate2Level 85
#define Gate2TFirst_1 86
#define Gate2TFirst_2 87
#define Gate2TMax_1 88
#define Gate2TMax_2 89
#define Gate2T0_1 90
#define Gate2T0_2 91
*/
