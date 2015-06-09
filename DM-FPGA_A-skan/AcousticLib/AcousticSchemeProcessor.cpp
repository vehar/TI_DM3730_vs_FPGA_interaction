#include "AcousticSchemeProcessor.h"

AcousticSchemeProcessor::AcousticSchemeProcessor()
{
	//stm = new AnalogContrStm(); rdm11
	fpgaCom = new FPGACommunication();
	analogCtrlRegVal = 0;

	averBuf = new CustAverBuf<USHORT>(1, AVERAGE_SIGNAL_MAX_VALUE, MaxSignalBuffSize, MaxSignalBuffSize);
	pulserBuf = new FPGA_CustomPulser(PULSER_BUF_SIZE);
	tgcBuf = new FPGA_CustomTGC(TGC_BUF_SIZE);
	
	activeScheme = NULL;
	inqTime = 100;
	initValuesRestrictions();
	setAnalogControlValue(aCmdReset);
}

AcousticSchemeProcessor::~AcousticSchemeProcessor()
{
	delete pulserBuf;
	delete tgcBuf;
	delete averBuf;

	delete fpgaCom;
//	delete stm; rdm11
}

void AcousticSchemeProcessor::initValuesRestrictions()
{
	inqFreq.init(1,500,1);
	signal.adcDelayUs.init(0,3000,0);
	signal.lengthUs.init(8,4000,100);
	signal.averaging.init(1,16,1);
	signal.trimPercent.init(0,99,0);
	
	probe.delayUs.init(0.0f, 99.0f, 0.0f);
	probe.angle.init(0,80,0);
	probe.delayUs.init(0.0f, 90.0f, 0.0f);
	probe.refGainDb.init(0, 80, 0);
	gainDb.init(-80, 80,0);
}

void AcousticSchemeProcessor::collectAscan()
{
	USHORT bufSize = fpgaData.signal.buffLen;
	if(!bufSize) return;
	averBuf->setBufSize(bufSize);
	averBuf->setAverSize(signal.averaging);

	bool isCheckGatesByFpga = true;
	if((signal.averaging-1)||(signal.trimPercent))
		isCheckGatesByFpga = false;
	
	
	fpgaCom->resetReadRamCounter();//ram idx->0
	fpgaCom->getSignalData(averBuf->getInsBuf(), bufSize);
	averBuf->getInsBuf()[1] = averBuf->getInsBuf()[2]; 
	averBuf->getInsBuf()[0] = averBuf->getInsBuf()[1]; // TODO: remove this crazy defense later (FPGA problem)
	averBuf->useInsBuf();
	memcpy(signalFromFpga, averBuf->getResBuf(), bufSize<<1);//BuffSize*2 size in bytes
	trimSignal(signalFromFpga, bufSize);//couldn't use  averBuf->getResBuf(),will affect averaging process
	gatherMaxSignal(signalFromFpga, bufSize);
	if(isCheckGatesByFpga)
		checkGatesByFpga(); 
	else
		checkGatesBySoft(signalFromFpga);	
		
	//if(!activeScheme->displaySignalFilled)
	{
		compressSignal(signalFromFpga, bufSize, activeScheme->resultData.displaySignal, DISPLAY_SIGNAL_LEN);
	//	activeScheme->displaySignalFilled = true;
	}
}

//signal Operations
void AcousticSchemeProcessor::trimSignal(USHORT *signalBuf, USHORT bufSize)
{
	if(signal.trimPercent)
	{
		if(fpgaData.signal.detector)
		{
			USHORT thresVal= (U16)(signal.trimPercent*(ADC_MAX_VALUE/100.0f));
			for(int i=0;i<bufSize;i++)
			{
				if(signalBuf[i] < thresVal)
					signalBuf[i] = 0;
			}
		}	
		else
		{
			USHORT thresVal= (U16)(signal.trimPercent*(ADC_MID_VALUE/100.0f));
			for(int i=0;i<bufSize;i++)
			{
				if ((signalBuf[i]< ADC_MID_VALUE+thresVal) && (signalBuf[i]>(ADC_MID_VALUE -thresVal)))
					signalBuf[i] = ADC_MID_VALUE;
			}
		}
	}
}

void AcousticSchemeProcessor::gatherMaxSignal(USHORT *signalBuf, USHORT bufSize)
{
	for(int i=0;i<bufSize;i++)
	{
		if(signalBuf[i]>signalMax[i])
			signalMax[i] = signalBuf[i];
	}
}

void AcousticSchemeProcessor::resetMaxSignal()
{
	memset(signalMax, 0, fpgaData.signal.buffLen*2);
}

bool AcousticSchemeProcessor::compressSignal(USHORT *dataIn, USHORT dataInLen, UINT *dataOut, USHORT dataOutLen)
{
	if((dataIn == 0)||(dataInLen == 0)||(dataOut==NULL)||(dataOutLen == 0)||(dataOutLen > dataInLen))
		return false;

	//TimeCheck();
	float factor =  dataInLen / (float)(dataOutLen+1); 
	int i, j, begin, end;
	float fBegin, fEnd = 0.0f;

	bool needMin = !fpgaData.signal.detector;
	for(i=0; i<dataOutLen; i++)
	{
		fBegin = fEnd;
		fEnd += factor;
		begin = (int)fBegin;
		end = (int)fEnd;

		if(end >= dataInLen)
			end = dataInLen - 1;
      
		USHORT &dataOutMax = *(USHORT*) &dataOut[i];
		USHORT &dataOutMin = *(USHORT*)((USHORT*)&dataOut[i]+1);
		
		dataOutMax = dataIn[begin];

		if(needMin) 
			dataOutMin = dataIn[begin];

		for(j = begin; j<=end; j++)
		{
			if(dataIn[j] > dataOutMax)
				dataOutMax = dataIn[j];

			if((needMin)&&(dataIn[j] < dataOutMin))
				dataOutMin = dataIn[j];
		}
	}
	return true;
}


void AcousticSchemeProcessor::checkGatesBySoft(USHORT *signalBuf)
{
	FpgaGates_t* fpgaGate;
	DataGates_t* dataGate;
	for(UCHAR gateIdx = 0; gateIdx<2; gateIdx++)
	{
		dataGate = &gates[gateIdx];
		fpgaGate = &fpgaData.gates[gateIdx];
		if(activeScheme->gates[gateIdx].mode != gateOff)
		{
			dataGate->isTriggered = false;
			dataGate->tFirst = 0;
			dataGate->tAMax = 0;
			dataGate->valTAMax = 0;
			dataGate->tZeroCros = 0;
			for(UINT i=fpgaGate->start; i<fpgaGate->end; i++)
			{
				if((!dataGate->isTriggered) && (signalBuf[i]>fpgaGate->level))
				{
					dataGate->tFirst = i;
					dataGate->isTriggered = true;
				}

				if(dataGate->valTAMax < signalBuf[i])
				{
					dataGate->valTAMax = signalBuf[i];
					dataGate->tAMax = i;
				}

				if((!fpgaData.signal.detector)&&(dataGate->isTriggered)&&(!dataGate->tZeroCros))
				{
					if(((signalBuf[i+1] > ADC_MID_VALUE)&&(signalBuf[i] <= ADC_MID_VALUE))||
						(signalBuf[i+1] < ADC_MID_VALUE)&&(signalBuf[i] >= ADC_MID_VALUE))
					{
						dataGate->tZeroCros = i;
					}
				}
			}

			if(dataGate->isTriggered)
			{	
				dataGate->tFirst	= (UINT) (dataGate->tFirst * signal.coefToUncompBuf)+(fpgaData.signal.adcDelay<<1);//fpgaScheme->signal.adcDelay<<1 (40Mhz<<1 == 80 Mhz)
				dataGate->tAMax		= (UINT) (dataGate->tAMax *  signal.coefToUncompBuf)+(fpgaData.signal.adcDelay<<1);
				dataGate->tZeroCros = (UINT) (dataGate->tZeroCros * signal.coefToUncompBuf)+(fpgaData.signal.adcDelay<<1);
			}
		}
	}
}


void AcousticSchemeProcessor::checkGatesByFpga()
{
	DataGates_t* dataGate;
	for(UCHAR gateIdx = 0; gateIdx<2; gateIdx++)
	{
		dataGate = &gates[gateIdx];
		gatesResult &gateInfo = activeScheme->resultData.gatesInfo[gateIdx];
		if(activeScheme->gates[gateIdx].mode != gateOff)
		{
			dataGate->tFirst = 0;
			fpgaCom->getGateMeasureTFirst(gateIdx, (DWORD&)dataGate->tFirst); 
			if(dataGate->tFirst)
			{
				switch(activeScheme->gates[gateIdx].mode)
				{
				case gateOn_First:
					gateInfo.tFirst = dataGate->tFirst;	
					break;
				case gateOn_Max:
					fpgaCom->getGateMeasureTMax(gateIdx, (DWORD&) dataGate->tAMax); 
					gateInfo.tAMax = dataGate->tAMax;
					break;
				case gateOn_Zero:
					if(activeScheme->signal.detectorIdx == radioSignal)
					{
						fpgaCom->getGateMeasureTCros0(gateIdx, (DWORD&)dataGate->tZeroCros); 
						gateInfo.tZeroCros = dataGate->tZeroCros;
					}
					break;
				}
				gateInfo.isProcessed = false;
				gateInfo.isTriggered = true;
			}
			else
			{
				if(gateInfo.isProcessed)
				{
					gateInfo.isTriggered = false;
					if(gateInfo.tFirst.getCount())
					{
						gateInfo.tFirst.reset();
						gateInfo.tAMax.reset();
						gateInfo.tZeroCros.reset();
					}
				}
			}
		}
	}
}

bool AcousticSchemeProcessor::setAnalogControlValue(UsedAnalogCmd cmd, UCHAR value)
{
	switch(cmd)
	{
	case aCmdGenVoltage:
		analogCtrlRegVal &= 0xF8;					// bits[2..0] = 0
		analogCtrlRegVal |= value;					// set bits[2..0]
		break;
	case aCmdEnAmplifier:
		analogCtrlRegVal &= 0xF7;					// bit#3 = 0
		analogCtrlRegVal |= (value<<3);				// set bit#3
		break;
	case aCmdFrontImpedance:
		analogCtrlRegVal &= 0xBF;					// bit#6 = 0
		analogCtrlRegVal |= (value<<6);				// set bit#6
		break;
	case aCmdReset:
		analogCtrlRegVal = 0;
		break;
	default:
		break;
	}
//	return  stm->I2C_GenRecSetControlValue(analogCtrlRegVal);  rdm11
	return 0;
}


void AcousticSchemeProcessor::setFpgaChannelsMode(UsedChModes Data)
{
	switch(Data)
	{
	case Gen0_Ch0://1 סמגל
		fpgaCom->setGenOutCh(1);
		fpgaCom->setAnalogChSwich(0);
		break;
	case Gen0_Ch3://1 נאחה
		fpgaCom->setGenOutCh(1);
		fpgaCom->setAnalogChSwich(3);
		break; 
	case Gen1_Ch2://2 סמגל
		fpgaCom->setGenOutCh(2);
		fpgaCom->setAnalogChSwich(1);
		break;
	case Gen1_Ch1://2 נאחה
		fpgaCom->setGenOutCh(2);
		fpgaCom->setAnalogChSwich(2);
		break;
	default: break;
	}

	fpgaCom->setGenSel(Data > 1 ? 2 : 1);
}


void AcousticSchemeProcessor::setSignalDetectorType(DetectorTypeIdx &detIdx)
{
	switch(detIdx)
	{
	case radioSignal:
		fpgaCom->setSignalDetector(0);
		fpgaCom->setSignalIntegration(1);
		break;
	case videoSignalTau1:
		fpgaCom->setSignalDetector(3);
		fpgaCom->setSignalIntegration(1);
		break;
	case videoSignalTau2:
		fpgaCom->setSignalDetector(3);
		fpgaCom->setSignalIntegration(5);
		break;
	case videoSignalTau3:
		fpgaCom->setSignalDetector(3);
		fpgaCom->setSignalIntegration(6);
		break;
	case videoSignalTau4:
		fpgaCom->setSignalDetector(3);
		fpgaCom->setSignalIntegration(7);
		break;
	case videoSignalTau5:
		fpgaCom->setSignalDetector(3);
		fpgaCom->setSignalIntegration(9);
		break;
	}
}
void AcousticSchemeProcessor::setSignalLenUs(USHORT& len)
{
	signal.lengthUs = len;
	//calculate compress factor  us = MaxBuffSize*(2^Compress)/SampleFrequency
	fpgaData.signal.compressFactor = 0;
	while((MaxSignalBuffSize*(1<<fpgaData.signal.compressFactor) / COEFF_FREQ80MHZ) < signal.lengthUs)
		fpgaData.signal.compressFactor++;

	//calculate fpga Buff len
	fpgaData.signal.buffLen =(USHORT) (signal.lengthUs * COEFF_FREQ80MHZ / (1<<fpgaData.signal.compressFactor));
	signal.coefToUncompBuf = ((1<<fpgaData.signal.compressFactor)/COEFF_FREQ80MHZ);
	resetMaxSignal();
}

void AcousticSchemeProcessor::setSignalAdcDelayUs(USHORT& delay)
{
	signal.adcDelayUs = delay;
    fpgaData.signal.adcDelay = COEFF_FREQ40MHZ * signal.adcDelayUs;
	resetMaxSignal();
}

void AcousticSchemeProcessor::setProbeDelayUs(float& _delayUs)
{
	probe.delayUs = _delayUs;
	fpgaData.probeDelay = (U16) (floor(COEFF_FREQ80MHZ * _delayUs + 0.5)+FILTER_DELAY_CNT);
	resetMaxSignal();
}

void AcousticSchemeProcessor::useFpgaScheme()
{
	//sync
	fpgaCom->setSyncSource(3);
	fpgaCom->setDACCh(1);

	setAnalogControlValue(aCmdEnAmplifier, 0);
	setAnalogControlValue(aCmdFrontImpedance, fpgaData.impedance);

	//filter
	fpgaCom->setFilterEn(fpgaData.filter.enabled);
	if(fpgaData.filter.enabled)
		fpgaCom->setFilterCoeffs((USHORT*)fpgaData.filter.coeffs, FILTER_COEFF_COUNT);

	//channel mode
	setFpgaChannelsMode(fpgaData.chMode);
	//pulser voltage
	setAnalogControlValue(aCmdGenVoltage, fpgaData.pulser.voltage);


	//pulser enhanced gen
	pulserBuf->setPulserType(fpgaData.pulser.enEnhancedGen);
	if(fpgaData.pulser.enEnhancedGen)
	{
		pulserBuf->updateFPGACustomPulserBuffer(fpgaData.pulser.pulserBuf);	
	}
	else
	{
		//pulser frequency
		pulserBuf->setFreq(fpgaData.pulser.freq); 
		//pulser pulse count
		pulserBuf->setImpCnt(fpgaData.pulser.pulseCount);
	}

	//probe 
	fpgaCom->setProbeDelay(fpgaData.probeDelay);

	//signal data
	fpgaCom->setSignalCompress(fpgaData.signal.compressFactor);
	fpgaCom->setSignalDataLen(fpgaData.signal.buffLen);
	fpgaCom->setAttenuator(fpgaData.attenEn);
	fpgaCom->setAcoustContGainCode((U16)((ACOUST_CONT_GAIN + ATTEN_LEVEL*fpgaData.attenEn)*db_FORMULA_MULT + dB_FORMULA_PLUS));
	fpgaCom->setSignalDelay(fpgaData.signal.adcDelay);
	fpgaCom->setSignalInversion(0);// <--------
	
	//gates
	for(int gIdx=0; gIdx < GATES_NUMBER; gIdx++)
	{
		if(activeScheme->gates[gIdx].mode == gateOff)  continue;

		fpgaCom->setGateStart(gIdx, fpgaData.gates[gIdx].start);
		fpgaCom->setGateEnd(gIdx, fpgaData.gates[gIdx].end);
		fpgaCom->setGateLevel(gIdx, fpgaData.gates[gIdx].level);

	}

	//pulser frequency, pulse count, enableEnhancedGen,			needs SetFullGen, 
	pulserBuf->lock();
	fpgaCom->setGenData((DWORD*) pulserBuf->getPtrToArrOfActivePulser(), pulserBuf->PulserBufferCount);
	pulserBuf->unlock();

	//acoustic tract
	fpgaCom->setTGCState(fpgaData.receiver.tgcEn);
	if(fpgaData.receiver.tgcEn)
	{
		tgcBuf->setRefGain(gainDb + probe.refGainDb);
		tgcBuf->updateFPGABuffer(fpgaData.receiver.tgcFpgaBuf);

		tgcBuf->lock();// fullTGC
		fpgaCom->setTGCData((DWORD*)tgcBuf->fullFpgaBuff, MAX_TGC_FPGA_BUFF);
		tgcBuf->unlock();
	}
	else
		fpgaCom->setDACGain(fpgaData.dacCode);
}


void AcousticSchemeProcessor::loadAcousticScheme(AcousticScheme &scheme)
{
	activeScheme = &scheme;
    
	inqFreq = scheme.inqFreq;
	inqTime = (DWORD)((1.0f/inqFreq)*1000);

	fpgaData.chMode		= scheme.params.chMode;
	
	fpgaData.attenEn	= scheme.params.attenEn;
	fpgaData.impedance	= scheme.params.impedance;

	fpgaData.pulser.voltage		= pulserVoltageValues[scheme.pulser.voltageIdx];
	fpgaData.pulser.freq		= scheme.pulser.freqLow;
	fpgaData.pulser.pulseCount	= scheme.pulser.pulseCount;
	fpgaData.filter.enabled		= (USHORT&)scheme.filter.enabled;

	if(fpgaData.filter.enabled)
		calcFilterCoeffs(scheme.filter.freqLow);

	fpgaData.receiver.tgcEn = (USHORT&)scheme.params.tgcEn;
	probe.angle = scheme.probe.angle;
	setSignalDetectorType(scheme.signal.detectorIdx);
	setProbeDelayUs(scheme.probe.delayUs);

	probe.refGainDb = scheme.probe.refGainDb;

	gainDb = scheme.params.gainDb;
	fpgaData.dacCode = (USHORT) ((gainDb + probe.refGainDb) * db_FORMULA_MULT  + dB_FORMULA_PLUS);// Calculate code for DAC
	
	setSignalAdcDelayUs(scheme.signal.adcDelayUs);
	setSignalLenUs(scheme.signal.lengthUs);


	float gBegin, gEnd;
	for(int gIdx = 0; gIdx < GATES_NUMBER; gIdx++)
	{
		if(activeScheme->gates[gIdx].mode == gateOff) continue;
		if( (activeScheme->gates[gIdx].startUs + activeScheme->gates[gIdx].lengthUs < activeScheme->signal.adcDelayUs)|| 
			((int)activeScheme->gates[gIdx].startUs > activeScheme->signal.adcDelayUs+activeScheme->signal.lengthUs))
		continue;
		
		gBegin = activeScheme->gates[gIdx].startUs;
		if(gBegin < activeScheme->signal.adcDelayUs)
			gBegin = activeScheme->signal.adcDelayUs;
		
		gEnd = activeScheme->gates[gIdx].startUs + activeScheme->gates[gIdx].lengthUs;
		if((int)gEnd > activeScheme->signal.adcDelayUs + activeScheme->signal.lengthUs)
		{
			gEnd = (float)activeScheme->signal.adcDelayUs + activeScheme->signal.lengthUs;
		}
		
		fpgaData.gates[gIdx].start = (U32)floor(gBegin * COEFF_FREQ80MHZ);
		fpgaData.gates[gIdx].end = (U32)floor(gEnd * COEFF_FREQ80MHZ);
		fpgaData.gates[gIdx].level = (USHORT) floor((activeScheme->gates[gIdx].levelPercent*ADC_MAX_VALUE/100.0f)+0.5f);
	}
	useFpgaScheme();
};


void AcousticSchemeProcessor::calcFilterCoeffs(float &freqLow)
{
	int i, c;
	if(freqLow > 10.0f) freqLow = 10.0f;
	const double freqHigh = 11.0f;
	float tempFilterCoeffs[FILTER_COEFF_COUNT], hammingWidow[FILTER_COEFF_COUNT], hanningWidow[FILTER_COEFF_COUNT],
		maxCoeff, fMin, fMax;
	int fSamp = (int) (COEFF_FREQ80MHZ*1000);
	fMin = (float)(1000*freqLow/fSamp);
	fMax = (float)(1000*freqHigh/fSamp);

	c = FILTER_COEFF_COUNT - 1;		// max coeff index
	maxCoeff = tempFilterCoeffs[c] = (float) (2*100*(fMax - fMin));	
	for (i = 1; i <= c; i++)		// calc coeffs
	{
		tempFilterCoeffs[c - i] = (float) ( (100*(sin(2*M_PI*fMax*i)-sin(2*M_PI*fMin*i)) / (M_PI*(i))) ); //Rectangle window
		hammingWidow[c - i] = (float) ( 0.54-0.46*cos(2*(c-i)*M_PI/(FILTER_COEFF_COUNT*2-1)) );// Hamming window
		hanningWidow[c - i] = (float) ( 0.5*(1-cos(2*(c-i)*M_PI/(FILTER_COEFF_COUNT*2-1))) );// Hanning window
		tempFilterCoeffs[c - i] *= hanningWidow[c - i];							// apply window
		if (abs((int)tempFilterCoeffs[c-i]) > maxCoeff)
			maxCoeff = (float)abs((int)tempFilterCoeffs[c-i]);
	}
	for(i = 0; i < FILTER_COEFF_COUNT; i++)		// convert coeffs to FPGA format
		fpgaData.filter.coeffs[i] = (I16) (tempFilterCoeffs[i]*5);
};