#include "AcousticTract35_2Ch.h"
#ifdef WINCE
#include "AcousticDefines.h"
#include "FPGA_CustTypes.h"

bool AcousticTract35_2Ch::acousticThreadShouldExit = false;

AcousticTract35_2Ch::AcousticTract35_2Ch()
{
   //fpgaScheme = new AcoustiFpgaScheme_t;
   schemeProcessor = new AcousticSchemeProcessor();
   stm = schemeProcessor->getStmPtr();
   fpgaCom = schemeProcessor->getFpgaComPtr();
   //acousticTasks.startThread();
   acousticThread = new CustThread(&AcousticTract35_2Ch::acousticProc, this);

}

AcousticTract35_2Ch::~AcousticTract35_2Ch()
{
	acousticThreadShouldExit = true;
	if(acousticThread)
		delete acousticThread;
	
	if(schemeProcessor)
		delete schemeProcessor;
}

bool AcousticTract35_2Ch::getAnalogPCBVerion(VerData_t* verData)
{
	return stm->I2C_GenRecGetVer(verData);
}

bool AcousticTract35_2Ch::setAnalogControlValue(UsedAnalogCmd cmd, UCHAR value)
{
	return schemeProcessor->setAnalogControlValue(cmd, value);
}

void AcousticTract35_2Ch::setFpgaChannelsMode(UsedChModes Data)
{
	schemeProcessor->setFpgaChannelsMode(Data);
}

DWORD WINAPI AcousticTract35_2Ch::acousticProc(LPVOID lpParam)
{
	AcousticTract35_2Ch *This = (AcousticTract35_2Ch *)lpParam;
	acousticThreadShouldExit = false;
	DWORD lastInqTime = 0;
	while(!acousticThreadShouldExit)
	{
		if(activeScheme)
		{
			if((activeScheme->dataChanged)&&(1))
			{
				This->schemeProcessor->loadAcousticScheme(*activeScheme);
				activeScheme->dataChanged = false;
				lastInqTime = GetTickCount();
			}
			if((activeScheme)&&(activeScheme->inquiryStarted)&&\
				(GetTickCount() -	lastInqTime >= This->schemeProcessor->inqTime))
			{
				lastInqTime = GetTickCount();
				This->schemeProcessor->collectAscan();
			}
		}
		//Sleep(100);
	}
	return 0;
}


#endif




/*if(This->acousticTasks.isEmpty())
		{
			Sleep(10);
		}
		else
		{
			This->acousticTasks.receiveElement();
			//ThreadCommandsFPGA Cmd = (ThreadCommandsFPGA)acousticTasks.getCmd();
		}*/