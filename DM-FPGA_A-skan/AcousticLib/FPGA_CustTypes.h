
#pragma once

/************************************************************************
 INCLUDES
************************************************************************/

#include "CustomTypes.h"
#include "AcousticDefines.h"
#include "QueueTask.h"

/************************************************************************
 DEFINES
************************************************************************/


#define  MAX_TGC_FPGA_BUFF		256

#define dB_FORMULA_PLUS			60//113
#define db_FORMULA_MULT			5.15//10.12
#define TGC_Bits				10
#define Pulser_Bits				3

#define InitialGainVal			0
#define InitialDelayVal			20
#define PulserDelayCoef			32

#define PulserFormDrawingSize	5

extern U16 PulserFormDrawingArray[PulserFormDrawingSize];
static const int dy_coeff = 4;

/************************************************************************
 CLASS TO WORK WITH CUSTOM PULSER
************************************************************************/
class FPGA_CustomPulser : public Lock_CriticalSection
{
	U32 *pulserBuffer;//меандр			
	U32 *customBuffer;//индивидуальная форма сигнала
	I32 cnt;
	double freq, delay;
	int pulserType;//0 - форма сигнала меандр, 1 - индивидуальная форма сигнала
	U32 convertOneValue(int ind);

public:
	U32 *delayArr; U16 *formArr; U16 *formForDrawingArr;
	I32 PulserBufferCount;		// number of elements in array
	FPGA_CustomPulser(I32 nbt);
	~FPGA_CustomPulser(void);
	void convert3ParamToFPGAFormat();
	void setNewPointValueX(int index, U32 value);
	void setNewPointValueY(int index, U16 value);
	void setDelay(double delay) {delay = delay; convert3ParamToFPGAFormat(); };
	void setImpCnt(I32 ImpCnt)	{ cnt = ImpCnt; convert3ParamToFPGAFormat(); };
	void setFreq(double _freq) { freq = _freq; convert3ParamToFPGAFormat(); };
	void setPulserType(int NewType) { lock(); pulserType = NewType; unlock(); };
	U32* getPtrToArrOfActivePulser() { return (pulserType ? customBuffer : pulserBuffer); };
	U32* getPtrToCustomBuffer() {return customBuffer;}
	void updateFPGACustomPulserBuffer(U32 *newBuffer);
};



/************************************************************************
 CLASS TO WORK WITH TGC
************************************************************************/

class CurveParamCalculation
{

protected:
	float coeff_a, coeff_b, Step;
	int dx, dy,
		interim_points_cnt;	// количество промежуточных точек ВРЧ кривой между двумя основными точками
	void calcTGCCurveParams();
	void calcInterimPointsAndStep(int x0, int x1, int y0, int y1);
	void calcCurveCoeffs(int x0, int x1, int y0, int y1);
	int calcYFromX(int x);
	int calcXFromY(int y);

public:
	int interpolateXPointOnLine(int x0, int x1, int y0, int y1, int x);
	int interpolateYPointOnLine(int x0, int x1, int y0, int y1, int y);
};

class FPGA_CustomTGC: public Lock_CriticalSection, public CurveParamCalculation
{
	int used_fpga_points;

	void convertTgcToFPGAFormat();
	U32 convertOneValue(int ind);
	//template<typename ParType> U32 convertOnePoint(ParType x, ParType y);
	template<typename ParType> U32 convertOnePoint(ParType x, ParType y) 
	{
		I32 coeff;
		I16 TempDb = I16(y*db_FORMULA_MULT)+dB_FORMULA_PLUS;
		if (TempDb < 0) coeff = 1;
		else coeff = 0;
		coeff = coeff << 31;
		return ((((U32)(x*COEFF_FREQ40MHZ)<<TGC_Bits))|((U16)(y*db_FORMULA_MULT)+dB_FORMULA_PLUS)) + coeff;
	};

	void fillOnePoint(float x, float y);
	void fillFpgaBuff(int x0, int x1, int y0, int y1);
	void convertAllValuesToFpgaArray();
	U32 calcAllInterimPointsBetween2Points(int x0, int x1, int y0, int y1, U32 used_range);
	U32 calcSumTilIndex(int index);
//	bool getMaxAndMinAndCheckThem(int coeff);
	void updateYArray();
	I16 *tgc_GainCoef;
	int refGainValue;
public:
	I16 *tgc_Gain; // gprah needd this buf

	U32 *fpga_Buffer,				// buffer is used to save/load data to/from database
		*fullFpgaBuff;				// buffer is used to write values to FPGA
	

	U32	*tgc_Delay;
	int activeElem;
	I32 tgcBufferCount;				// number of elements in array

	FPGA_CustomTGC(I32 nbt);
	~FPGA_CustomTGC(void);
	int  getRefGain() {return refGainValue;};
	void setRefGain(int newVal, bool updYArray = true);
	bool setNewPointValueX(int index, U32 value);
	bool setNewPointValueY(int index, I16 value);
	bool setNewPointValueXY(int index, U16 xValue, U16 yValue);
	void getCurrentXValues(int index, int &minValue, int &currentValue);
	void setFirstDelay(U16 newDelay);
	void updateFPGABuffer(U32 *newBuffer);
	//bool changeYArrayCoeff(int coeff);
	I16  getTGC_Gain(I32 index);
};
