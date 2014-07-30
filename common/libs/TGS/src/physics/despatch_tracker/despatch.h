/********************************************************************************
* File Name : despatch.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the mode of DESPATCH
*
********************************************************************************/
#ifndef DESPATCH_H_
#define DESPATCH_H_

#include "despatch.h"
#include "spacecraft.h"

class DespatchCalculator : public SpacecraftCalculator {

	public:
		
		int mode (double periodFromDeparture);
	
	private:
		
		static const double PeriodMorse;
		static const double PeriodBaudot;
		static const double PeriodBeacon;
		
		static const int NotDefined;
		static const int Morse;
		static const int Baudot;
		static const int Beacon;
		static const int RadioStop;
};

#endif