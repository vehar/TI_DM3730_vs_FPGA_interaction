class AnalogContrStm
{

public:
	AnalogContrStm();
	~AnalogContrStm();
	
	bool I2C_GenRecSetControlValue(UCHAR val);
	void getMajorVersion(USHORT& val);

	
};