/********************************************************************************
* File Name : spacecraft.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in the sun-centerd
* inertial coordinate and in the geo coord
*
********************************************************************************/

#ifndef SPACECRAFT_H_
#define SPACECRAFT_H_


#include "spacecraft.h"
#include "tf.h"
#include "planet.h"

#include <cmath>
#include <iostream>
using namespace std;

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;


class SpacecraftCalculator {
	public:
		SpacecraftCalculator (void);
		virtual ~SpacecraftCalculator (void);
		
		void setSpacecraftOrbitInfo (double epochMjd, const double (&posEci)[3], const double (&velEci)[3]);
		void getSpacecraftOrbitInfo (double* epochMjd, double* posEci, double* velEci) const;
		void setSpacecraftParams (double ballisticCoeff);
		void getSpacecraftParams (double* ballisticCoeff) const;
		
		void setObserverGeoCoord (double latitude, double longitude, double altitude);
		void getObserverGeoCoord (double* latitude, double* longitude, double* altitude) const;
		
		void setSpacecraftState (double secondsFromEpoch, const double (&posSci)[3], const double (&velSci)[3]);
		void setSpacecraftState (double secondsFromEpoch, Vector3d const& posSci, Vector3d const& velSci);
		double getSpacecraftState (double* posSci, double* velSci) const;
		double getSpacecraftState (Vector3d* posSci, Vector3d* velSci) const;
		
		void getEpochTime (double* epochMjd) const;
		void getSecondsFromEpoch (double* secondsFromEpoch) const;
		
		void getGeometryEarthCentered (double* declination, double* rightAscension, double decError = 0.0, double raError = 0.0) const;
		void getDopplerRatioEarthCentered (double* downlink, double* uplink) const;
		void getDistanceEarthCentered (double* distance, double error = 0.0) const;
		void getSpacecraftRelativeSpeed (double* speed) const;
		void getSpacecraftGeoCoord (double* latitude, double* longitude, double* altitude) const;
		void getSpacecraftDirection (double* elevation, double* azimuth, double eleError = 0.0, double aziError = 0.0) const;
		void getDopplerRatio (double* downlink, double* uplink) const;
        
        void getSpacecraftPosEci (double* scPosEci) const;
		void getSpacecraftVelEci (double* scVelEci) const;
        
        void getEarthPosSci (double* earthPosSci) const;
        void getEarthVelSci (double* earthVelSci) const;
		
		void resetSpacecraftState (void);
		double integrateSpacecraftState (double dt, double srpErrorRatio = 0.0);
		
		void test1 (int periodDay);
		void test2 (int periodDay);
						
	private:
		double epochMjd_;
		double secondsFromEpoch_;
		double initialPosEci_[3], initialVelEci_[3];
		double ballisticCoeff_;
		double observerGeoCoord_[3];
		Planet earth_;
		VectorXd spacecraftState_;
		
		void calcInitialScState (Vector3d* posSci, Vector3d* velSci) const;
		void calcSpacecraftStateDerivative (VectorXd* scStateDerivative, VectorXd const& scState, double srpErrorRatio) const;
		void calcSolarGravityAcc (Vector3d* acceleration, VectorXd const& scState) const;
		void calcSolarRadiationPressureAcc (Vector3d* acceleration, VectorXd const& scState, double srpErrorRatio) const;
		void calcEarthGravityAcc (Vector3d* acceleration, VectorXd const& scState) const;
		
		void calcObserverPosEci (Vector3d* obsPosEci) const;
		void calcObserverVelEci (Vector3d* obsVelEci) const;
		void calcSpacecraftPosEci (Vector3d* scPosEci) const;
		void calcSpacecraftVelEci (Vector3d* scVelEci) const;
		void calcSpacecraftPosEcef (Vector3d* scPosEcef) const;
		void calcSpacecraftRelativePosEnu (Vector3d* scPosEnu) const;
		void calcGeometry (double* lat, double* lon, Vector3d const& relativePos, double latError, double lonError) const;
		void calcDopplerRatio (double* downlink, double* uplink,  Vector3d const& relativePos, Vector3d const& relativeVel) const;
};


#endif