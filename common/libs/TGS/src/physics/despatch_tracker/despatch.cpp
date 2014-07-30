/********************************************************************************
* File Name : despatch.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the mode of DESPATCH
*
********************************************************************************/
#include "despatch.h"


const double DespatchCalculator:: PeriodMorse = 8.0 * 3600.0;
const double DespatchCalculator:: PeriodBaudot = 77.0 * 3600.0;
const double DespatchCalculator:: PeriodBeacon = (7.0 * 24.0) * 3600.0;

const int DespatchCalculator:: NotDefined = 0;
const int DespatchCalculator:: Morse = 1;
const int DespatchCalculator:: Baudot = 2;
const int DespatchCalculator:: Beacon = 3;
const int DespatchCalculator:: RadioStop = 4;


int DespatchCalculator:: mode (double periodFromDeparture)
{
	
	double period = periodFromDeparture;
	
	if (period > PeriodBeacon) {
		return RadioStop;
	}
	else if (period > PeriodBaudot) {
		return Beacon;
	}
	else if (period > PeriodMorse) {
		return Baudot;
	}
	else if (period > 0.0) {
		return Morse;
	}
	
	return NotDefined;	// not defined
}