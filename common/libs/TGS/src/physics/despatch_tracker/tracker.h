/********************************************************************************
* File Name : tracker.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the direction and doppler-shift of the spacecraft at a 
* specific observation location
*
********************************************************************************/
#ifndef TRACKER_H_
#define TRACKER_H_

#include "tracker.h"
#include "planet.h"
#include "spacecraft.h"
#include "despatch.h"
#include "gstation.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;

class SpacecraftTracker {

	public:
		
		SpacecraftTracker (char const* fileName);
		void init (void);
		
		void getOrbitInfo (char* scId, char* orbitId, double& departureMjd, double* posEcenterd, double* velEcenterd);
		double getDepartureUnixtime (void);
		
		// poit search at unixtime
		int getTarget (double unixtime, double lat, double lon, double alt, 
					   double& ele, double& azi, double& freq, 
					   double& distance, int& mode);
		
		// range search from unixtime_s to unixtime_e
		int getTarget (double unixtime_s, double unixtime_e, double lat, double lon, double alt, 
					   vector<double>& unixtime, vector<double>& ele, vector<double>& azi, vector<double>& freq, 
					   vector<double>& distance, vector<int>& mode, double obs_dt = 60.0);
		
	private:
		
		char scId_[64], orbitId_[64];
		Vector3d initialPos_, initialVel_;
		double departureMjd_;				// departure in Modifeid Julian Day
		double departureUnixtime_;			// departure in unix time
		
		Planet earth_;
		
		DespatchCalculator descal_;
		VectorXd x_;
		
		GStationCalculator gscal_;
		
		int readOrbitInfo (char const* fileName);
		double mjdToUnixtime (double mjd);
		
		void integrate (double t0, double duration);
		void calcGeometry (double lat, double lon, double alt, double &ele, double &azi, double &freq);
		
		static const double Pi;
		
		static const double PeriodNearEarth;	// [sec]
		static const double DtNearEarth;		// [sec]
		static const double DtFarEarth;			// [sec]
};

#endif