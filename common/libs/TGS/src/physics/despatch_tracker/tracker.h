/********************************************************************************
* File Name : tracker.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in geo coordinate
*
********************************************************************************/
#ifndef TRACKER_H_
#define TRACKER_H_

#include "tracker.h"
#include "tf.h"
#include "spacecraft.h"

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;

#include <cmath>
#include <iostream>
#include <iomanip>

class SpacecraftTracker : public SpacecraftCalculator {
	public: 
		typedef struct {
			char scId[64];
			
			struct OrbitInfo {
				char id[64];
				double epochMjd;
				double positionEci[3];
				double velocityEci[3];
			} orbitInfo;
			
			struct Param {
				double ballisticCoeff;
				double transmitterFrequency;
			} param;
		} SCDRec;
						
		SpacecraftTracker (void);
		SpacecraftTracker (SCDRec const& scd);
		virtual ~SpacecraftTracker (void);
		
		void setSpacecraftInfo (SCDRec const& scd);
		void getSpacecraftInfo (SCDRec* scd) const;
		virtual int setTargetTime (double unixtime);
		void getTargetTime (double *unixtime) const;
		
		void test (double unixtime_s, double unixtime_e, double outputDt);
		
	private:
		SCDRec scd_;
		double unixtime_;
		
		void updateSpacecraftState (void);
		void calcIntegrationPeriod (double* period) const;
};


#endif