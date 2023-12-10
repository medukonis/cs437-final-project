//##################################################
//# Name:         Michael Edukonis
//# UIN:          677141300
//# email:        meduk2@illinois.edu
//# class:        CS437
//# assignment:   Capstone Project
//# date:         4/22/2023
//##################################################
//wcs.cpp an updated, cut down version of the official WinsonLib.cpp
//original library had settings for many other sensors and softwareserial
//implementation that was not compatible with ESP boards.

#include "wcs.h"

// Setting:Single Output Current 
WCS::WCS(uint8_t analogPin, uint16_t mVperA)
{
	_AIpin = analogPin;
	_sensitivity = mVperA;
	_midPoint = 512;        //was 2790;
}

// Current Reset
void WCS::Reset()
{
	int32_t dataSum;

	ReadADCBuffer(_dataScaled);

	dataSum = 0;
	for (int i = 0; i < 120; i++)
	{
		dataSum += _dataScaled[i];
	}
	_midPoint = dataSum / 120;
}

// Measure AC Current
double WCS::A_AC()
{
	int16_t steps;
	double steps2;
	double dataSum;
  	double current;
  	double adjustment_value = 7.6217;   // was7.28

	ReadADCBuffer(_dataScaled);

	dataSum = 0.0;
	for (int i = 0; i < 120; i++)
	{
		//printf("_midpoint: %d\n", _midPoint);
		steps = _dataScaled[i] - _midPoint;
		steps2 = ((double)steps * 5 / 1023) / ((double)_sensitivity * 0.001);
		dataSum += sq(steps2);
		//printf("i: %d\t\t", i);
		//printf("datasum: %d\n", int(dataSum));
	}
  current = sqrt(dataSum / 120)/adjustment_value;

	return current;
}

void WCS::ReadADCBuffer(int16_t* rawData)
{
	analogRead(_AIpin); //  read analog pin to stabilize the ADC 

	for (int i = 0; i < 120; i++)
	{
		_start = micros();
		rawData[i] = analogRead(_AIpin);
		while (micros() - _start < 829); // Sample Rate = 1200Hz
	}
	
}


