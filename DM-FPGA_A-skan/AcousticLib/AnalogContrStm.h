#pragma once
#include <windows.h>
#include "AcousticSchemeProcessor.h"

class AnalogContrStm
{

public:
	AnalogContrStm();
	~AnalogContrStm();
	
	bool I2C_GenRecSetControlValue(UCHAR val);
	void getMajorVersion(USHORT& val);

	
}; 