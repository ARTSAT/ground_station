/********************************************************************************
* File Name : planet.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.3.20
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the planets around the sun
* in the sun-senterd inertial coordinate
*
********************************************************************************/
#ifndef PLANET_H_
#define PLANET_H_

#include "planet.h"

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

class Planet 
{
	public:
		Vector3d pos_, vel_;	// Earth position and velocity in Sun Cernerd Inertial coordinate [m], [m/s]
		double thetaG_;			// Greenwich Sideral Time (only for Earth) [rad]


		Planet (char const* planetName, double t0Mjd);	// constructor
		Planet (void);
		void init (char const* planetName, double t0Mjd);	// initialization

		void calcOrbitAt (double t);				// calculate the planet orbit
		
		void fileOut (double t);
		void test (double perido, double dt);


	private:

		double t0Mjd_;			// epoch in Modified Julian Day
		char planetName_[32];	// planet nema

		void calcPosVelAt (double t);							// calculate planet velocity and position
		void calcThetaGAt (double t);							// calculate Greenwich Sideral Time (only for Earth) [rad]
		double solveKeplerEq (double secondsFromEpoch);			// solve Kepler equation
		void calcDcm (int axis, double angle, Matrix3d &ans);	// calculate Directional Cosine Matrix


		// six elements f the planet oebit
		double a_;	// semi major axis [m]
		double e_;	// eccentricity
		double i_;	// inclination [rad]
		double w_;	// argument of perihelion [rad]
		double W_;	// longitude of the ascending node [rad]
		double M0_;	// mean anomaly [rad]

		double epoch_;	// epoch at mean anomary [MJD]
		double n_;		// mean motion [rad/s]

		int firstWrite;
		
		static const double SecondsPerDay;	// [sec]
		static const double Pi;
		static const double AU;				// astronomical unit[m]
		static const double MueSun;			// Heliocentric gravitational constant [m^3 s^(-2)]
};

#endif
