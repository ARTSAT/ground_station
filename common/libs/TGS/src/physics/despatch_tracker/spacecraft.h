/********************************************************************************
* File Name : spacecraft.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in the sun-centerd
* inertial coordinate
*
********************************************************************************/
#ifndef SPACECRAFT_H_
#define SPACECRAFT_H_


#include "spacecraft.h"
#include "planet.h"
#include "stochastic.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
using namespace std;

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;


class SpacecraftCalculator 
{
	public:
		
		SpacecraftCalculator (void);							// constructer
		
		void init (VectorXd& x, Planet earth, int error = 0);	// init spacecraft using constants (if error=1, error is added)
		void init (VectorXd& x, 
				  Vector3d posECenterd, Vector3d velECenterd, 
				  Planet earth);								// init spacecraft state using variables
				  
		void integrate (double dt, VectorXd& x, Planet earth);	// integrate spacecraft state: x+ = x + x'*dt
		
		// calculate right ascension, declination and doppler shift observed at Earth center
		//-- if (scState.size () > 6): calculation of sigma points (UKF)
		//-- else if (error = 1): add white noise
		//-- else: calculate true value
		void calcGeometry (Vector3d& ans, VectorXd x, Planet earth, int error = 0);
		void calcDoppler (double &ans, VectorXd x, Planet earth, int error = 0);
		
		double distanceEarthCenterd (VectorXd x, Planet earth);	// calculate the distance the spacecraft and earth

		void fileOutStatus (double t, VectorXd x, Planet earth);
		void fileOutMeasurements (double t, Vector3d trueGeometry, Vector3d measuredGeometry);
		void test (double duration);
		
		static const double ObsAngleDev;	// mesurement error dev of doppler shift [Hz] 
		static const double ObsDopplerDev;	// mesurement error dev of angles [deg]
		
		static const double ScRadioFreq;	// spacecraft radio frequency [Hz]
		
		static const double DepartureMjd;	// departure in Modified Julian day [day]
	

	private:
		
		Stochastic noise_;

		void calcGravityFromEarth (Vector3d& ans, VectorXd x, Planet earth);
		void calcGravityFromSun (Vector3d& ans, VectorXd x);
		void calcSolarPressure (Vector3d& ans, VectorXd x);
		
		int firstWriteStatus;
		int firstWriteMeasurements;	
		
		// spacecraft initial position and velocity in Earth Centerd Inertial coordinate [m] [m/s]
		static const double InitPosX;
		static const double InitPosY;
		static const double InitPosZ;
		static const double InitVelX;
		static const double InitVelY;
		static const double InitVelZ;
		
		// error of the spacecraft initial position and velocity in Earth Centerd Inertial coordinate [m] [m/s]
		static const double ErrorPosX;
		static const double ErrorPosY;
		static const double ErrorPosZ;
		static const double ErrorVelX;
		static const double ErrorVelY;
		static const double ErrorVelZ;
		
		static const double SrpFlucRatioMax;// fluctuation of Solar Radiation Pressure: [-FlucRatio, +FlucRatio]
		
		static const double ScMass;			// [kg]
		static const double ScArea;			// [m^s]
		
		static const double SecondsPerDay;	// [sec]
		static const double Pi;	
		static const double LightSpeed;		// [m/s]
		static const double Obliquity;		// Earth obliquity [rad]
		static const double MueEarth;		// Geocentric gravitational constant [m^3 s^(-2)]
		static const double MueSun;			// Heliocentric gravitational constant [m^3 s^(-2)]
		
		static const double SrpG1;			// parameter for SRP (Solar Radiation Pressure)
		static const double SrpCoeff;		// parameter for SRP
};

#endif