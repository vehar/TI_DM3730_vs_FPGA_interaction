#include "FPGACommunication.h"

#ifdef WINCE
#define GMI_Interface UniDriver //RDM_11

FPGACommunication::FPGACommunication()
{
	gmi = new GMI_Interface();
}

FPGACommunication::~FPGACommunication()
{
	delete gmi;
}

void FPGACommunication::setSyncFreq(UINT freqInSamples)
{
	WriteDWORD(FreqSync1Addr, FreqSync2Addr, freqInSamples);
}

void FPGACommunication::setSyncSource(USHORT syncSource)
{
	gmi->WriteWORD(SyncCtrlAddr, syncSource);
}

void FPGACommunication::setSignalCompress(USHORT val)
{
	gmi->WriteWORD(CompressAddr, val);
}

void FPGACommunication::setSignalDetector(USHORT val )
{
	gmi->WriteWORD(DetectorAddr, val);
}

void FPGACommunication::setAnalogChSwich( USHORT val )
{
	gmi->WriteWORD(AnChSwich, val);
}

void FPGACommunication::setAttenuator( USHORT val )
{
	gmi->WriteWORD(AttenSw, val);
}

void FPGACommunication::setSignalIntegration( USHORT val )
{
	gmi->WriteWORD(IntegratorCoefAddr, val);
}

void FPGACommunication::setSignalInversion( USHORT val )
{
	gmi->WriteWORD(InversionSignal, val);
}

void FPGACommunication::getAcoustContSum( USHORT& val )
{
	gmi->ReadWORD(AcoustContSum, val);
}

void FPGACommunication::setTGCState( USHORT val )
{
	gmi->WriteWORD(TgcEnAddr, val);
}

void FPGACommunication::setTGCData( DWORD* Buff, int size)
{
	gmi->WriteWORD(TgcStartAddrWr, 0);
	WriteBuf32(DacData1, DacData2, Buff, size);
}

void FPGACommunication::setDACCh( USHORT val )
{
	gmi->WriteWORD(DacCh, val);
}

void FPGACommunication::setDACGain( USHORT val )
{
	gmi->WriteWORD(DacData1, val);
}

void FPGACommunication::setAcoustContGainCode( USHORT val )
{
	gmi->WriteWORD(AcousticContactGain, val);
}

void FPGACommunication::setGenData( DWORD* Buff, int size )
{
	gmi->WriteWORD(GenStartAddrWr, 0);
	WriteBuf32(GenBuffAddr1, GenBuffAddr2, Buff, size);
}

void FPGACommunication::setGenSel(USHORT val ) //выбор рам активного генератора
{
	gmi->WriteWORD(GenCSAddr, val);
}

void FPGACommunication::setGenOutCh( USHORT val )
{
	gmi->WriteWORD(GenEn, val);
}

void FPGACommunication::setFilterEn( USHORT val )
{
	gmi->WriteWORD(FilterEnAddr, val);
}

void FPGACommunication::setFilterCoeffs( USHORT* Buff, int size )
{
	gmi->WriteWORD(FilterCoeffsRstAddrWr, 0);
	gmi->WriteBuf(FilterCoeffsAddr, Buff, size);
}

void FPGACommunication::resetReadRamCounter()
{
	gmi->WriteWORD(RamCntRdRst, 0);
}

void FPGACommunication::setSignalDelay(UINT val)
{
	WriteDWORD(AdcDelayAddr1, AdcDelayAddr2, val);
}

void FPGACommunication::getSignalData( USHORT *Buff, int size )
{
	//gmi->ReadWORD(AdcBuffAddr, Buff[0]);
	//gmi->ReadWORD(AdcBuffAddr, Buff[0]);
	gmi->ReadBuf(AdcBuffAddr, Buff, size+1);
}

void FPGACommunication::setSignalDataLen( USHORT val )
{
	gmi->WriteWORD(ReadBuffSize, val);
}

void FPGACommunication::setProbeDelay( USHORT val )
{
	gmi->WriteWORD(ProbeDelayAddr, val);
}

void FPGACommunication::getMajorVersion( USHORT &val )
{
	gmi->ReadWORD(MajorVersion, val);
}

void FPGACommunication::getSvnVersion(USHORT &val)
{
	gmi->ReadWORD(SvnVersion, val);
}

void FPGACommunication::setGateStart( int gateIdx, UINT val )
{
 	DWORD StartAddr = (gateIdx ? Gate2Start_1 : Gate1Start_1);
	WriteDWORD(StartAddr, StartAddr+1, val);
}

void FPGACommunication::setGateEnd( int gateIdx, UINT val )
{
	DWORD StartAddr = (gateIdx ? Gate2End_1 : Gate1End_1);
	WriteDWORD(StartAddr, StartAddr+1, val);
}

void FPGACommunication::setGateLevel( int gateIdx, USHORT val )
{
	DWORD StartAddr = (gateIdx ? Gate2Level : Gate1Level);
	gmi->WriteWORD(StartAddr, val);
}

void FPGACommunication::getGateMeasureTFirst( int gateIdx, DWORD &val )
{
	DWORD StartAddr = (gateIdx ? Gate2TFirst_1 : Gate1TFirst_1);
	ReadDWORD(StartAddr, StartAddr+1, val);
}

void FPGACommunication::getGateMeasureTMax( int gateIdx, DWORD &val )
{
	DWORD StartAddr = (gateIdx ? Gate2TMax_1 : Gate1TMax_1);
	ReadDWORD(StartAddr, StartAddr+1, val);
}

void FPGACommunication::getGateMeasureTCros0( int gateIdx, DWORD &val )
{
	DWORD StartAddr = (gateIdx ? Gate2T0_1 : Gate1T0_1);
	ReadDWORD(StartAddr, StartAddr+1, val);
}

UCHAR FPGACommunication::WriteBuf32(DWORD addr, DWORD addr1, DWORD *val, int size)
{
	for(int i=0;i<size;i++)
	{
		if(!gmi->WriteWORD(addr, val[i]& 0xffff))  return 0;
		if(!gmi->WriteWORD(addr1, (val[i]>>16))) return 0;
	}
	return 1;
}

UCHAR FPGACommunication::WriteDWORD(DWORD addr, DWORD addr1, DWORD val)
{
	if(!gmi->WriteWORD(addr, val & 0xffff)) return 0;
	if(!gmi->WriteWORD(addr1, (val>>16))) return 0;
	return 1;
}

UCHAR FPGACommunication::ReadDWORD(DWORD addr, DWORD addr1, DWORD &val)
{
	WORD d1,d2;
	if(!gmi->ReadWORD(addr, d1)) return 0;
	if(!gmi->ReadWORD(addr1, d2)) return 0;
	val = d2<<16 | d1;
	return 1;
}

#endif