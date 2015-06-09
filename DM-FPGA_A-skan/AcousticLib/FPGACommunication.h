#pragma once


//#include "CustomTypes.h"
#include <windows.h>
#ifdef WINCE
	//#include "GMI.h"
#include "../UniDriver.h"
#include "../GpioDriver.h"
#include "../ExtBusFunc.h" 
#endif

//#define WINCE

enum FPGA_Addresses
{
	FreqSync1Addr = 1, FreqSync2Addr, SyncCtrlAddr, CompressAddr, DetectorAddr, AnChSwich = 19, AttenSw = 7, IntegratorCoefAddr, InversionSignal, AcoustContSum,
	TgcEnAddr = 11, DacData1, DacData2, TgcStartAddrWr, DacCh = 49, AcousticContactGain = 95,
	GenBuffAddr1 = 21, GenBuffAddr2, GenCSAddr, GenStartAddrWr = 67, GenEn = 80,
	FilterEnAddr = 30, FilterCompressAddr, FilterCoeffsAddr, FilterCoeffsRstAddrWr,
	//AScanDrawMode = 39,
	//AScanEnAddr = 40, AScanBuffAddr1, AScanWrCS, AScanStartAddrWr, AScanRamCntRdRst,
	//AScanBuffAddr2 = 45, Cursor1CoordX, Cursor1CoordY, Cursor2CoordX, Cursor2CoordY,

	RamCntRdRst = 51, AdcDelayAddr1, AdcDelayAddr2, AdcBuffAddr, ReadBuffSize, ProbeDelayAddr,
	TestDataBus = 61, TestAddrBus, MajorVersion, SvnVersion,

	Gate1Start_1 = 70, Gate1Start_2, Gate1End_1, Gate1End_2, Gate1Level, Gate1TFirst_1, Gate1TFirst_2, Gate1TMax_1, Gate1TMax_2, Gate1T0_1, Gate1T0_2,
	Gate2Start_1	 , Gate2Start_2, Gate2End_1, Gate2End_2, Gate2Level, Gate2TFirst_1, Gate2TFirst_2, Gate2TMax_1, Gate2TMax_2, Gate2T0_1, Gate2T0_2

	//Gate2Start_1 = 70, Gate2Start_2, Gate2End_1, Gate2End_2, Gate2Level, Gate2TFirst_1, Gate2TFirst_2, Gate2TMax_1, Gate2TMax_2, Gate2T0_1, Gate2T0_2,
	//Gate1Start_1	 , Gate1Start_2, Gate1End_1, Gate1End_2, Gate1Level, Gate1TFirst_1, Gate1TFirst_2, Gate1TMax_1, Gate1TMax_2, Gate1T0_1, Gate1T0_2,

};

enum Sync{SyncStop = 0, SyncInt = 1, SyncExt = 2, SyncCont = 3};

#ifdef WINCE

class FPGACommunication
{
	//GMI_Interface *gmi;
   UniDriver *gmi;
public:
	FPGACommunication();
	~FPGACommunication();
	UCHAR ReadDWORD(DWORD addr, DWORD addr1, DWORD &val);
	UCHAR WriteDWORD(DWORD addr, DWORD addr1, DWORD val);
	UCHAR WriteBuf32(DWORD addr, DWORD addr1, DWORD *val, int size);
	
	void getMajorVersion(USHORT& val);
	void getSvnVersion(USHORT& val);

	void setSyncFreq(UINT freqInSamples);
	void setSyncSource(USHORT syncSource);

	void setSignalDelay(UINT val);
	void resetReadRamCounter();
	void getSignalData(USHORT* Buff, int size);

	void setSignalDataLen(USHORT val);
	void setSignalCompress(USHORT val);
	void setSignalInversion(USHORT val);
	void setSignalDetector(USHORT val);
	void setAnalogChSwich(USHORT val);
	void setSignalIntegration(USHORT val);
	void setAttenuator(USHORT val);

	void setFilterEn(USHORT val);
	void setFilterCoeffs(USHORT* Buff, int size);

	void setGenData(DWORD* Buff, int size);
	void setGenSel(USHORT val);
	void setGenOutCh(USHORT val);
	
	void setAcoustContGainCode(USHORT val);
	void getAcoustContSum( USHORT& val); //acoustic contact

	void setProbeDelay(USHORT val);
	
	void setDACCh(USHORT val);
	void setDACGain(USHORT val);

	void setTGCState(USHORT val);
	void setTGCData(DWORD* Buff, int size);
	
	void setGateStart(int gateIdx, UINT val);
	void setGateEnd(int gateIdx, UINT val);
	void setGateLevel(int gateIdx, USHORT val);
	void getGateMeasureTFirst(int gateIdx, DWORD &val);
	void getGateMeasureTMax(int gateIdx, DWORD &val);
	void getGateMeasureTCros0( int gateIdx, DWORD &val);
	
};

#endif