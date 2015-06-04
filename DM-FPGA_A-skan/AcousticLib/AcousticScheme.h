#pragma once
#include <windows.h>
#include "AcousticDefines.h"
#include "CustAccumType.h"
//#include "AscanCoordLink.h"

enum PulserVoltagesIdx;
enum UsedChModes;
enum DetectorTypeIdx;
enum gateLogicMode;

typedef struct  
{
	bool isTriggered;
	bool isProcessed;
    CustAccumType<UINT>tAMax;
	CustAccumType<UINT>tFirst;
	CustAccumType<UINT>tZeroCros;
}gatesResult;

typedef struct 
{
	struct 
	{
		UsedChModes chMode;
		CHAR		gainDb;
		BOOL		tgcEn;
		bool		attenEn;
		bool		impedance;
	} params;
	
	struct 
	{
		DetectorTypeIdx detectorIdx;
		USHORT			adcDelayUs;
		USHORT			lengthUs;
		UCHAR			averaging;
		UCHAR			trimPercent;
		bool			invertEn;
		bool			drawFilled;
	}signal;

	struct  
	{
		BOOL		enabled;
		float		freqLow;
	}filter;

	struct 
	{
		PulserVoltagesIdx voltageIdx;
		float			  freqLow;
		USHORT			  pulseCount;
	} pulser;
  
	struct
	{
		UCHAR angle;
		float delayUs;
		UCHAR refGainDb; 
	}probe;
	
	struct
	{
		gateLogicMode mode;
		float startUs;
		float lengthUs;
		UCHAR levelPercent;
	}gates[GATES_NUMBER];

	struct
	{
		bool mmUnitType;
		USHORT speedMpS;
		UCHAR calcModeCoef;	
	}toMMConv;
	
	bool dataChanged;
    bool inquiryStarted;
	USHORT inqFreq;
	
	struct  
	{
		gatesResult gatesInfo[GATES_NUMBER];
		UINT displaySignal[DISPLAY_SIGNAL_LEN];
//		AscanCoordLink *linkData;
	}resultData;
	
	//bool displaySignalFilled;
}AcousticScheme;


#define convToMM(us) (((us) / activeScheme->toMMConv.calcModeCoef)*\
	activeScheme->toMMConv.speedMpS/1000.0f)

enum DetectorTypeIdx
{
	radioSignal = 0,
	videoSignalTau1,
	videoSignalTau2,
	videoSignalTau3,
	videoSignalTau4,
	videoSignalTau5
};

enum PulserVoltagesIdx
{
	pulserVoltage_45V = 0,
	pulserVoltage_90V,
	pulserVoltage_160V
};

enum UsedChModes
{
	Gen0_Ch0 = 0,		// combined mode 1
	Gen0_Ch1 = 101,		// separate mode 1a
	Gen0_Ch2 = 100,		// doesn't exist
	Gen0_Ch3 = 1,		// separate mode 1b
	Gen1_Ch1 = 2,		// combined mode 2
	Gen1_Ch3 = 103,		// separate mode 2a
	Gen1_Ch2 = 3,		// separate mode 2b
	Gen1_Ch0 = 103		// doesn't exist
};
enum gateLogicMode {gateOff, gateOn_Max, gateOn_First, gateOn_Zero};

extern AcousticScheme sampleScheme;
extern AcousticScheme *activeScheme;
