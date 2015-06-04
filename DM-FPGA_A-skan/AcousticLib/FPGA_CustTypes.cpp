/********************************************************************
	author:		Victor Mariinschii
	created:	2012/11/08
	filename: 	CustomCurve.cpp
	purpose:	Work with custom curves (advanced pulser, tgc, etc..)
*********************************************************************/

#include "FPGA_CustTypes.h"
#include "math.h"
//#include "defines.h"


U16 PulserFormDrawingArray[PulserFormDrawingSize] = {45, 85, 5, 0, 50};	// 3rd element won't be used, related to FPGA... (see doc)

/************************************************************************
 CUSTOM PULSER
************************************************************************/
FPGA_CustomPulser::FPGA_CustomPulser(I32 nbt)
{
	PulserBufferCount = nbt;
	pulserBuffer = new U32[nbt];
	customBuffer = new U32[nbt];
	formArr = new U16[nbt];
	formForDrawingArr = new U16[nbt];
	delayArr = new U32[nbt];
	memset(pulserBuffer, 0, nbt*4);
	delay = PulserDelayCoef;	// default pulser delay
	freq = 2.5; cnt = 2;
	convert3ParamToFPGAFormat();
	pulserType = 0;
}

FPGA_CustomPulser::~FPGA_CustomPulser()
{
	delete []pulserBuffer;
	delete []customBuffer;
	delete []formArr;
	delete []delayArr;
	delete []formForDrawingArr;
}

U32 FPGA_CustomPulser::convertOneValue(int ind)
{
	return ((delayArr[ind]*COEFF_FREQ200MHZ)<<Pulser_Bits)+formArr[ind];
}
void FPGA_CustomPulser::convert3ParamToFPGAFormat()
{
	int i = 0;
	lock();
	int ImpParts = 2;		// 1 - positive, 2 - negative, 3 - delay between impulses (for example)
	pulserBuffer[i++] = ((U32)(COEFF_FREQ200MHZ*delay)<<3);	
	for(; i <= cnt*ImpParts; i+=ImpParts)
	{
		pulserBuffer[i] = (((U32)(100/freq)-1)<<3)|1;					// positive		
		pulserBuffer[i+ImpParts-1] = (((U32)(100/freq)-1)<<3)|2;		// negative
	}
	for(; i < PulserBufferCount; i++)
		pulserBuffer[i] = 0;
	unlock();
}
void FPGA_CustomPulser::setNewPointValueX(int Index, U32 Value)
{
	if (Index < 0)
		Index = 0;
	if (Index >= PulserBufferCount)
		Index = PulserBufferCount-1;
	delayArr[Index] = Value;
	//CustomBuffer[0] = PulserBuffer[0];
	lock();
	customBuffer[Index] = convertOneValue(Index);
	unlock();
}
void FPGA_CustomPulser::setNewPointValueY(int Index, U16 Value)
{
	if (Index < 0)
		Index = 0;
	if (Index >= PulserBufferCount)
		Index = PulserBufferCount-1;
	if (Value)
		formArr[Index] = 1<<(Value-1);
	else
		formArr[Index] = 0;
	formForDrawingArr[Index] = PulserFormDrawingArray[formArr[Index]];
	lock();
	customBuffer[Index] = convertOneValue(Index);
	unlock();
}


void FPGA_CustomPulser::updateFPGACustomPulserBuffer(U32 *NewBuffer)
{
	memcpy(customBuffer, NewBuffer, PulserBufferCount*4);
	int i;
	lock();
	for (i = 0; i < PulserBufferCount; i++)
	{
		delayArr[i] = (U32)((customBuffer[i]>>Pulser_Bits)/COEFF_FREQ200MHZ);
		formArr[i] = (U16)customBuffer[i]&((1<<Pulser_Bits)-1);
		formForDrawingArr[i] = PulserFormDrawingArray[formArr[i]];
	}
	unlock();
}

/************************************************************************
 CLASS TO WORK WITH TGC
************************************************************************/
U32 FPGA_CustomTGC::calcSumTilIndex(int index)
{
	U32 Sum = 0;
	for(int i = 0; i < index; i++)
		Sum += tgc_Delay[i];
	return Sum;
}

FPGA_CustomTGC::FPGA_CustomTGC(I32 nbt)
{
	tgcBufferCount = nbt;
	fpga_Buffer = new U32[nbt+1];
	tgc_Delay = new U32[nbt];
	tgc_Gain = new I16[nbt];
	tgc_GainCoef = new I16[nbt];
	fullFpgaBuff = new U32[MAX_TGC_FPGA_BUFF];

	int i;
	for(i = 0; i < nbt; i++)
	{
		tgc_Delay[i] = InitialDelayVal;
		tgc_Gain[i] = InitialGainVal;
		tgc_GainCoef[i] = InitialGainVal;
	}
	fpga_Buffer[0] = 0;
	convertTgcToFPGAFormat();
}

FPGA_CustomTGC::~FPGA_CustomTGC()
{
 	delete []fpga_Buffer;
  	delete []tgc_Delay;
	delete []tgc_Gain;
	delete []tgc_GainCoef;
	delete []fullFpgaBuff;
}



void FPGA_CustomTGC::setRefGain(int newVal, bool updYArray)
{
	refGainValue = newVal; 
	for (int i = 0; i < tgcBufferCount; i++)
	{
		tgc_Gain[i] = refGainValue + tgc_GainCoef[i]; 
	}
	if(updYArray) 
		updateYArray();
}

U32 FPGA_CustomTGC::convertOneValue(int idx)
{
	return (convertOnePoint((I32)tgc_Delay[idx], (I32)tgc_Gain[idx]));
}

void FPGA_CustomTGC::convertTgcToFPGAFormat()
{
	
	lock();
	for (int i = 0; i < tgcBufferCount; i++)
		fpga_Buffer[i+1] = convertOneValue(i);
	convertAllValuesToFpgaArray();
	unlock();
}

void FPGA_CustomTGC::setFirstDelay(U16 NewDelay)
{
	lock();
	fpga_Buffer[0] = (NewDelay<<10)*COEFF_FREQ40MHZ;
	convertAllValuesToFpgaArray();
	unlock();
}

bool FPGA_CustomTGC::setNewPointValueX(int Index, U32 Value)
{
	if (Index < 0)
		Index = 0;
	if (Index >= tgcBufferCount)
		Index = tgcBufferCount-1;
	tgc_Delay[Index] = Value-calcSumTilIndex(Index);
	lock();
	fpga_Buffer[Index+1] = convertOneValue(Index);
	convertAllValuesToFpgaArray();
	unlock();
	return (used_fpga_points > MAX_TGC_FPGA_BUFF);		// true == error
}
bool FPGA_CustomTGC::setNewPointValueY(int Index, I16 Value)
{
	if (Index < 0)
		Index = 0;
	if (Index >= tgcBufferCount)
		Index = tgcBufferCount-1;
	tgc_Gain[Index] = Value;
	tgc_GainCoef[Index] = Value-refGainValue;
	lock();
	fpga_Buffer[Index+1] = convertOneValue(Index);
	convertAllValuesToFpgaArray();
	unlock();
	return (used_fpga_points > MAX_TGC_FPGA_BUFF);		// true == error
}
bool FPGA_CustomTGC::setNewPointValueXY(int Index, U16 XValue,  U16 YValue)
{
	if (Index < 0)
		Index = 0;
	if (Index >= tgcBufferCount)
		Index = tgcBufferCount-1;
	tgc_Gain[Index] = YValue;
	tgc_GainCoef[Index] = YValue-refGainValue;
	tgc_Delay[Index] = XValue-calcSumTilIndex(Index);
	lock();
	fpga_Buffer[Index+1] = convertOneValue(Index);
	convertAllValuesToFpgaArray();
	unlock();
	return (used_fpga_points > MAX_TGC_FPGA_BUFF);		// true == error
}
/*
bool FPGA_CustomTGC::getMaxAndMinAndCheckThem(int coeff)
{
	int min, max;
	min = max = tgc_Gain[0];
	for (int i = 1; i < tgcBufferCount; i++)
	{
		if (tgc_Gain[i] > max)
			max = tgc_Gain[i];
		else if (tgc_Gain[i] < min)
			min = tgc_Gain[i];
	}
	if ((coeff > 0) && (max + coeff > DB_MAX_VALUE))
		return true;
	else if ((coeff < 0) && (min + coeff < 0))
		return true;
	else
		return false;
}
*/
/*
bool FPGA_CustomTGC::changeYArrayCoeff(int coeff)
{
	for(int i = 0; i < tgcBufferCount; i++)
	{
		tgc_Gain[i] += coeff; 
		fpga_Buffer[i+1] = convertOneValue(i);
	}
	lock();
	convertAllValuesToFpgaArray();
	unlock();
	return false;
}
*/
void FPGA_CustomTGC::updateYArray()
{
	for(int i = 0; i < tgcBufferCount; i++)
	{
//		tgc_Gain[i] += coeff; 
		fpga_Buffer[i+1] = convertOneValue(i);
	}
	lock();
	convertAllValuesToFpgaArray();
	unlock();
}

void FPGA_CustomTGC::updateFPGABuffer(U32 *new_fpga_Buffer)
{
	memcpy(fpga_Buffer, new_fpga_Buffer, (tgcBufferCount+1)*4);
	int i, coeff;
	lock();
	for (i = 0; i < tgcBufferCount; i++)
	{
		coeff = fpga_Buffer[i+1]>>31;			//sign of saved gain
		tgc_Delay[i] = (U32)(((fpga_Buffer[i+1]-(coeff<<31))>>TGC_Bits)/COEFF_FREQ40MHZ);		//(FPGA_Buffer[i+1]-coeff<<31) delete sign of saved gain
		if (coeff) coeff = -1;
		else coeff = 1;
		tgc_Gain[i] = (I16)floor((((I16)coeff*(I16)(fpga_Buffer[i+1]&((1<<TGC_Bits)-1))-dB_FORMULA_PLUS)/db_FORMULA_MULT)+0.5);
		tgc_GainCoef[i] = tgc_Gain[i] - refGainValue;
	}
	convertAllValuesToFpgaArray();
	unlock();
}

void CurveParamCalculation::calcCurveCoeffs(int x0, int x1, int y0, int y1)
{
	coeff_a = (float)dy/dx;
	if (fabs(coeff_a) < 0.01)
		coeff_a = 0;
	coeff_b = y1 - coeff_a*x1;
	if (fabs(coeff_b) < 0.01)
		coeff_b = 0;
}

void CurveParamCalculation::calcInterimPointsAndStep(int x0, int x1, int y0, int y1)
{
	dy = y1 - y0;
	dx = x1 - x0;
	interim_points_cnt = (int)ceil((float)abs(dy)/dy_coeff);
	if (interim_points_cnt < 1)
		interim_points_cnt = 1;
	if (dx <= interim_points_cnt)
	{
		interim_points_cnt = dx;
		Step = 1.0;
	}
	else
		Step = (float)dx/interim_points_cnt;
}

int CurveParamCalculation::calcYFromX(int x)
{
	return (int)floor((coeff_a*x+coeff_b) + 0.5);
}

int CurveParamCalculation::calcXFromY(int y)
{
	return (int)floor((y-coeff_b)/coeff_a + 0.5);
}
//get y from coord x (line)
int CurveParamCalculation::interpolateXPointOnLine(int x0, int x1, int y0, int y1, int x)
{
	if ((x1 > x0) && ((x > x1) || (x < x0)))
		return -INT_MAX;
	else if ((x1 < x0) && ((x < x1) || (x > x0)))
		return -INT_MAX;
	else {
		calcInterimPointsAndStep(x0, x1, y0, y1);
		calcCurveCoeffs(x0, x1, y0, y1);
		return calcYFromX(x);
	}
}
//get y from coord x (line)
int CurveParamCalculation::interpolateYPointOnLine(int x0, int x1, int y0, int y1, int y)
{
	if ((y1 > y0) && ((y > y1) || (y < y0)))
		return -INT_MAX;
	else if ((y1 < y0) && ((y < y1) || (y > y0)))
		return -INT_MAX;
	else {
		calcInterimPointsAndStep(x0, x1, y0, y1);
		calcCurveCoeffs(x0, x1, y0, y1);
		return calcXFromY(y);
	}
}

void FPGA_CustomTGC::fillFpgaBuff(int x0, int x1, int y0, int y1)
{
	float x, y;
	int i = 0;
	while(i < interim_points_cnt)
	{
		x = x0 + Step*(i+1);
		y = coeff_a*x+coeff_b;
		if (x > (float)x1)
		{
			x = (float)x1; y = (float)y1;
			i = interim_points_cnt;
		}
		if (y > DB_MAX_VALUE) 
			y = DB_MAX_VALUE;
		else 
			if (y < 0) y = 0;
		fillOnePoint(Step, y);
		i++;
	}
}

inline void FPGA_CustomTGC::fillOnePoint(float x, float y)
{
	if (used_fpga_points < MAX_TGC_FPGA_BUFF)
	{
		fullFpgaBuff[used_fpga_points] = convertOnePoint(x, y);
	}
	used_fpga_points++;
}

void FPGA_CustomTGC::convertAllValuesToFpgaArray()
{
	U32 used_range = 0;
	int i = 0;
	used_fpga_points = 0;
	used_range = calcAllInterimPointsBetween2Points(0, tgc_Delay[i], 0, tgc_Gain[i], used_range);
	while((i < (tgcBufferCount-1))&&(used_fpga_points < MAX_TGC_FPGA_BUFF))
	{
		used_range = calcAllInterimPointsBetween2Points(tgc_Delay[i], tgc_Delay[i+1], tgc_Gain[i], tgc_Gain[i+1], used_range);	
		i++;
	}
	while(used_fpga_points < MAX_TGC_FPGA_BUFF) {
		fullFpgaBuff[used_fpga_points] = convertOnePoint(5, 0);
		used_fpga_points++;
	}
}

U32 FPGA_CustomTGC::calcAllInterimPointsBetween2Points(int x0, int x1, int y0, int y1, U32 used_range) 
{
	calcInterimPointsAndStep(used_range, used_range+x1, y0, y1);
	calcCurveCoeffs(used_range, used_range+x1, y0, y1);
	fillFpgaBuff(used_range, used_range+x1, y0, y1);
	return (used_range+x1);
}

void FPGA_CustomTGC::getCurrentXValues(int index, int &MinValue, int &CurrentValue)
{
	U32 Sum = calcSumTilIndex(index-1);
	MinValue = Sum+1;
	CurrentValue = Sum+tgc_Delay[index-1];
}

I16 FPGA_CustomTGC::getTGC_Gain(I32 index)
{
	if (tgc_Gain[index] < -DB_MAX_VALUE) return -DB_MAX_VALUE;
	else if (tgc_Gain[index] > DB_MAX_VALUE) return DB_MAX_VALUE;
	else return tgc_Gain[index];
}