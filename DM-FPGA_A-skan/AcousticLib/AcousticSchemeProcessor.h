#pragma once
#include <windows.h>

#include "AnalogContrStm.h" 
#include "FPGACommunication.h"
//#include "CustAverBuf.h" //rdm11
#include "CustLimitType.h"
#include "FPGA_CustTypes.h"
#include "AcousticScheme.h"

//Acoustic Globals


//enum PulserVoltagesIdx;
enum UsedAnalogCmd;
enum UsedChModes;
typedef struct 
{
	UINT	start;
	UINT	end;
	USHORT	level;
	//gateLogicMode   mode;
} FpgaGates_t;

typedef struct dataGates_st 
{
	//bool isEnabled;
	bool isTriggered;
	UINT tAMax, tFirst, tZeroCros;
    USHORT valTAMax;
	dataGates_st() {/*isEnabled = false;*/ isTriggered = false;}
} DataGates_t;

typedef struct AcoustiFpgaScheme_st
{
	UsedChModes		  chMode;
	USHORT			  dacCode;
	
	USHORT   attenEn;
	UCHAR   impedance;

	USHORT            probeDelay;
	//pulser data
	struct 
	{
		UCHAR					voltage;
		bool					enEnhancedGen;
		float					freq;
		CustLimitType<USHORT>	pulseCount;
		UINT					pulserBuf[PULSER_BUF_SIZE];//custom buf signal
	} pulser;

	struct  
	{
		CustLimitType<USHORT> tgcEn;
		UINT   tgcFpgaBuf[TGC_BUF_SIZE+1];				// one byte ("+1") will be used to change delay before TGC
	} receiver;

	//signal data
	struct 
	{
		UINT   adcDelay;//16+3 bits, 1unit == 25ns 
		USHORT buffLen; 
		USHORT compressFactor;
		USHORT detector;
		
	} signal;

	struct
	{
		CustLimitType<USHORT> enabled;
		SHORT coeffs[FILTER_COEFF_COUNT];
	}filter;
	
	//gates
	FpgaGates_t gates[GATES_NUMBER];
	AcoustiFpgaScheme_st()
	{
		pulser.pulseCount.init(1,8,1);
		filter.enabled.init(0,1,0);
		receiver.tgcEn.init(0,1,0);
		pulser.enEnhancedGen = 0;
	}
} AcoustiFpgaScheme_t;


class AcousticSchemeProcessor
{
//	AnalogContrStm	*stm; rdm11
	FPGACommunication *fpgaCom;
	CustAverBuf<USHORT> *averBuf;

	UCHAR analogCtrlRegVal; // analog pcb control register value
	//AcousticScheme *currentScheme;
	
	
	USHORT signalFromFpga[MaxSignalBuffSize];
	USHORT signalMax[MaxSignalBuffSize];


	void initValuesRestrictions();
	void trimSignal(USHORT *readSignalBuf, USHORT bufSize);
	void gatherMaxSignal(USHORT *readSignalBuf, USHORT bufSize);
	bool compressSignal(USHORT *dataIn, USHORT dataInSize, UINT* dataOut, USHORT dataOutSize);
	void resetMaxSignal();
	void checkGatesBySoft(USHORT *signalBuf);
	void checkGatesByFpga();
	void useFpgaScheme();
	void calcFilterCoeffs(float &freqLow);
public:	
	AcousticSchemeProcessor();
	~AcousticSchemeProcessor();
//	AnalogContrStm* getStmPtr() {return stm;} rdm11
	FPGACommunication* getFpgaComPtr() {return fpgaCom;}
	bool setAnalogControlValue(UsedAnalogCmd vmd, UCHAR value = 0);
	void setFpgaChannelsMode(UsedChModes Data);
	
	void loadAcousticScheme(AcousticScheme &scheme);
	void collectAscan();	
 
	void setSignalDetectorType(DetectorTypeIdx &detIdx);
	void setSignalLenUs(USHORT& len);
	void setSignalAdcDelayUs(USHORT& delay);
    void setProbeDelayUs(float& delay);
	
	AcoustiFpgaScheme_t fpgaData;
	CustLimitType<USHORT> inqFreq;// acquisition frequency
	DWORD inqTime;
	struct 
    {
		CustLimitType<USHORT> adcDelayUs;
		CustLimitType<USHORT> lengthUs;
		CustLimitType<UCHAR>  averaging;//max 16
		float coefToUncompBuf;//coefficient from compressed buffer sample to uncompressed
		CustLimitType<UCHAR>  trimPercent;
    }signal;

	struct
	{
		CustLimitType<UCHAR> angle;
		CustLimitType<float> delayUs;
		CustLimitType<UCHAR> refGainDb; //probe refGainDb + gainDb = globalGain
	}probe;

    CustLimitType<CHAR> gainDb;
    
	FPGA_CustomPulser *pulserBuf;
	FPGA_CustomTGC	  *tgcBuf;

	DataGates_t gates[GATES_NUMBER];

};


static const UCHAR pulserVoltageValues[3] = {1, 3, 7}; //45, 90,160

enum UsedAnalogCmd
{
	aCmdGenVoltage,		// bits [2..0], 7 - maximum voltage, 0 - minimum
	aCmdEnAmplifier,	// bit 3,		1 - enable amplifiers, 0 - disable amplifiers
	aCmdFrontImpedance,	// bit 6,		0 = 1k, 1 = 50
	aCmdReset
};






