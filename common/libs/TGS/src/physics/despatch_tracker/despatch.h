/********************************************************************************
* File Name : despatch.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.7
* Language  : C++
*********************************************************************************
* class to calculate the mode of DESPATCH
*
********************************************************************************/
#ifndef DESPATCH_H_
#define DESPATCH_H_

#include "tf.h"
#include "tracker.h"

#include <string>

class DespatchTracker : public SpacecraftTracker {
	public:
		DespatchTracker (void);
		DespatchTracker (double departureMjd ,SCDRec const& scd);
		~DespatchTracker (void);
		
		void setDepartureTime (double departureMjd);
		void getDepartureTime (double* departureMjd) const;
		
		int setTargetTime (double unixtime);
		void getDespatchMode (string* mode) const;
		
		void test (double unixtime_s, double unixtime_e, double outputDt);
	
	private:
		static const double SecondsBeaconEnd_;
		
		double departureMjd_;		
		void calcSecondsFromDeparture (double* secondsFromDeparture) const;
};

#endif