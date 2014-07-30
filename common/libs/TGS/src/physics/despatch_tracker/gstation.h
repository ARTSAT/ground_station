/********************************************************************************
* File Name : gstation.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.3.24
* Language  : C++
*********************************************************************************
* class to calculate doppler, azimuth, and elevation from a specific ground 
* station to a spacecraft
*
********************************************************************************/
#ifndef GSTATION_H_
#define GSTATION_H_


#include "gstation.h"
#include "planet.h"
#include "spacecraft.h"
#include "stochastic.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;


class GStationCalculator
{
	public:
		// constructor
		GStationCalculator (void);

		// return spacecraft elevation in rad
		double elevationAt (double* gsLocation, VectorXd scState, Planet earth);

		// calculate azimuth, elevation and doppler shift
		//-- if (scState.size () > 6): calculation of sigma points (UKF)
		//-- else if (error = 1): add white noise
		//-- else: calculate true value
		void calcGeometry (Vector3d &ans, double* gsLocation, VectorXd scState, Planet earth, int error = 0);
		void calcDoppler (double& ans, double* gsLocation, VectorXd scState, Planet earth, int error = 0);
		
		void fileOut (char const* fileName ,double t, Vector3d trueGeometry);
		void test (double duration);


	private:
		void calcGsPositionInEci (Vector3d &pos, Vector3d &vel, double* gsLocation, Planet earth);
		void calcScPositionInEci (Vector3d &pos, Vector3d &vel, VectorXd scState, Planet earth);
		void calcScRelativePosInEci (Vector3d &pos, Vector3d &vel, double* gsLocation, VectorXd scState, Planet earth);
		void calcScPositionInEnu (Vector3d &pos, Vector3d relativeScPosEci, double* gsLocation, Planet earth);

		double azimuth (Vector3d relativeScPosEnu);
		double elevation (Vector3d relativeScPosEnu);
		double dopplerShift (Vector3d relativeScPosEci, Vector3d relativeScVelEci);

		Stochastic noise_;
		int firstWrite;
		
		static const double Pi;
		static const double SecondsPerDay;	// [sec]
		static const double LightSpeed;		// [m/s]
		
		static const double EarthSpinRate;	// [rad/s]
		static const double Obliquity;		// Earth's obliquity [rad]

		static const double EarthRadius;	// [m]
		static const double EarthF;			// Earth's oblateness
};


#endif