#pragma once

#include "CustAverBuf.h"


enum AcousticThreadCommands
{
	Gen1_Ch0X = 0,	
};  


//#include "QueueTask.h"
#include "FPGACommunication.h"

#ifdef WINCE

#include "AnalogContrStm.h"
#include "..\CustThread\CustThread.h"
#include "AcousticSchemeProcessor.h"

//class FPGA_CustomPulser;
class FPGA_CustomTGC;

class AcousticTract35_2Ch
{
	
	AnalogContrStm	*stm;
	static DWORD WINAPI acousticProc(LPVOID lpParam);
	QueueTask acousticTasks;
	CustThread *acousticThread;
	static bool acousticThreadShouldExit;	
	
	
public:
	AcousticTract35_2Ch();
	~AcousticTract35_2Ch();

	FPGACommunication *fpgaCom;
	
	AcousticSchemeProcessor *schemeProcessor;//acousticDataScheme

	bool getAnalogPCBVerion(VerData_t* verData);
	bool setAnalogControlValue(UsedAnalogCmd vmd, UCHAR value = 0);
	void setFpgaChannelsMode(UsedChModes data);	
};

#else
//blank class

class AcousticTract35_2Ch
{
public:
	AcousticTract35_2Ch(){};
	~AcousticTract35_2Ch(){};

	bool getAnalogPCBVerion(void* verData){return true;};
	void getMajorVersion(USHORT &val){};
	void getSvnVersion(USHORT &val){};


	bool setAnalogControlValue(UsedAnalogCmd vmd, UCHAR value = 0){return true;};
	void setFpgaChannelsMode(UsedChModes data){};

	void setGenSel(USHORT data){};
	void setGenData(DWORD* Buff, int size){};

	void resetReadRamCounter(){};

	void setSyncSource(USHORT syncSource){};
	void setSyncFreq(UINT32 freqInSamples){};


	void getSignalData(USHORT *Buff, int size){};
	void setSignalDataLen(USHORT val){};
	void setSignalCompress(USHORT val){};
	void setSignalInversion(USHORT val){};
	void setSignalDetector(USHORT val){};
	void setSignalIntegration(USHORT val){};

	void setFilterEn(USHORT val){};
	void setFilterCoeffs(USHORT* Buff, int size){};

	void setAdcRdDelay(UINT32 val){};

	void setAttenuator(USHORT val){};

	void setAcoustContGainCode(USHORT val){}; //acoustic contact
	void getAcoustContSum(USHORT& val){};

	void setProbeDelay(USHORT val){};

	void setDACCh(USHORT val){};
	void setDACGain(USHORT val){};

	void setTGCState(USHORT val){};
	void setTGCData( DWORD* Buff, int size){};

	void setGateStart(int gateIdx, UINT32 val){};
	void setGateEnd(int gateIdx, UINT32 val){};
	void setGateLevel(int gateIdx, USHORT val){};
	void getGateMeasureTFirst(int gateIdx, DWORD &val){};
	void getGateMeasureTMax(int gateIdx, DWORD &val){};
	void getGateMeasureTCros0(int gateIdx, DWORD &val){};
};

#endif