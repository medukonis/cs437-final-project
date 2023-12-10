/*
 Name:		wcs.h and updated, cut down version of WinsonLib.h
 Created:	4/5/2023
 Author:	Mike Edukonis
 Editor:	Visual Studio Code
*/

#ifndef _wcs_h
#define _wcs_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//#include <SoftwareSerial.h>

// WCS Sensitivity (mV per Ampere)
// Built-in Conductor
//(Single)

#define _WCS1800  66


enum WType_t {
	AC,
	DC,
	AT,
	ACDC,
	Modbus,
	NaN
};

union BytesArrayConverter {
	byte array[4];
	int32_t ToInt32;
};

struct Wdata_t
{
	WType_t Sign = NaN;
	double Value = 0;
};

enum _mode
{
	SingleOutput,
	DifferentialOutput
};


class WCS
{
public:
	WCS(uint8_t analogPin, uint16_t mVperA);
	WCS(uint8_t analogPin, uint8_t analogPin2, uint16_t mVperA);
	void Reset();
	double A_DC();
	double A_AC();

	//uint32_t _start2;
	//uint32_t _end2;
private:
	uint32_t _start;

	_mode   _Mode;
	uint8_t _AIpin;
	uint8_t _AIpin2;
	int16_t _midPoint;
	uint16_t _sensitivity;
	int16_t _dataScaled[120];

	void ReadADCBuffer(int16_t* rawData);
};

#endif



