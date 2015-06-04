#include "AcousticScheme.h"

/* //so3r 0 grade pep
AcousticScheme sampleScheme =
{
	{Gen0_Ch3, 12, false, false, true},
	{videoSignalTau2, 0, 33, 1, 0, false, true},
	{true, 2.5f},
	{pulserVoltage_90V, 2.5f, 2},
	{0, 4.39f, 16},						//probe
	{{gateOn_First, 8.0f, 11.6f, 24}, {gateOff, 0, 0, 0}},	//gates
	{true, 5893, 2},//toMMConversion
	0, false, 50, 
};	*/

//N725
AcousticScheme sampleScheme =
{
	{Gen0_Ch3, 12, false, false, true},
	{videoSignalTau2, 0, 33, 1, 0, false, true},
	{true, 2.5f},
	{pulserVoltage_90V, 2.5f, 2},
	{0, 5.70f, 16},						//probe
	{{gateOn_First, 8.0f, 11.6f, 24}, {gateOff, 0, 0, 0}},	//gates
	{true, 2830, 2},//toMMConversion
	0, false, 50, 
};

AcousticScheme *activeScheme = NULL;