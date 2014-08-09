/********************************************************************************
* File Name : planet.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.2
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the planets around the sun
* in the sun-senterd inertial coordinate
*
********************************************************************************/
#ifndef PLANET_H_
#define PLANET_H_

#include "planet.h"
#include "tf.h"

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;

#include <cmath>
#include <string>
#include <iostream>
using namespace std;


class Planet {
	public:
		Planet (void);
		Planet (string planet, double epochMjd);
		~Planet (void);
		int setPlanetName (string planet);
		void getPlanetName (string* planet) const;
		
		void setEpoch (double epochMjd);
		void getEpoch (double *epochMjd) const;
		void setTargetTime (double time);
		void getTargetTime (double *time) const;
		
		void getPosition (double* position) const;
		void getPosition (Vector3d* position) const;
		void getVelocity (double* velocity) const;
		void getVelocity (Vector3d* velocity) const;
		void getGst (double *gst) const;
		
		void test (int periodDay);
	
	private:
		static const string NotDefined_;
		static const string Invalid_;
		
		string planet_;		// name of the planet
		double epochMjd_;	// epoch in Modified Julian Day
		
		double a_;			// semi major axis
		double e_;			// eccectricity
		double i_;			// inclination
		double w_;			// augument of perihelion
		double W_;			// longitude of the ascending node
		double M0_;			// mean anomaly
		
		double t0Mjd_;		// epoch at mean anomaly in Modified Julian Day
		double n_;			// mean motion
		
		Vector3d position_;	// planet position in Sun Centerd Inertial coordinate in meter
		Vector3d velocity_;	// planet velocity in Sun Centerd Inertial coordinate in m/s
		double gst_;		// Greenwich Sedial Time in rad
		
		double time_;		// target time from epoch in second
		
		int initOrbitElements (void);
		int calcPositionVelocity (void);
		void calcGst (void);
		void solveKeplerEq (double* ans) const;
		int planetNameIsValid (void) const;
};

#endif