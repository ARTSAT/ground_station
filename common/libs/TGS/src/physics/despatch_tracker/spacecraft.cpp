/********************************************************************************
* File Name : spacecraft.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in the sun-centerd
* inertial coordinate
*
********************************************************************************/
#include "spacecraft.h"

#define	SC_POSITION	block (0, 0, 3, 1)
#define	SC_VELOCITY	block (3, 0, 3, 1)

SpacecraftCalculator:: SpacecraftCalculator (void): epochMjd_ (0.0), secondsFromEpoch_ (0.0), 
													ballisticCoeff_ (100.0), txFrequency_ (0.0), 
													earth_ ("earth", epochMjd_), spacecraftState_ (VectorXd:: Zero (6))
{
	for (int i = 0; i < 3; i++) {
		initialPosEci_[i] = 0.0;
		initialVelEci_[i] = 0.0;
	}
	
	for (int i = 0; i < 3; i++) {
		observerGeoCoord_[i] = 0.0;
	}
}

SpacecraftCalculator:: ~SpacecraftCalculator (void)
{
	
}

void SpacecraftCalculator:: setSpacecraftOrbitInfo (double epochMjd, const double (&posEci)[3], const double (&velEci)[3])
{
	epochMjd_ = epochMjd;
	earth_.setEpoch (epochMjd_);
	
	for (int i = 0; i < 3; i++) {
		initialPosEci_[i] = posEci[i];
		initialVelEci_[i] = velEci[i];
	}
	
	Vector3d posSci, velSci;
	calcInitialScState (&posSci, &velSci);
	
	setSpacecraftState (0.0, posSci, velSci);
}

void SpacecraftCalculator:: getSpacecraftOrbitInfo	(double* epochMjd, double* posEci, double* velEci) const
{
	*epochMjd = epochMjd_;
	
	for (int i = 0; i < 3; i++) {
		posEci[i] = initialPosEci_[i];
		velEci[i] = initialVelEci_[i];
	}	
}

void SpacecraftCalculator:: setSpacecraftParams	(double ballisticCoeff, double txFrequency)
{
	ballisticCoeff_ = ballisticCoeff;
	txFrequency_ = txFrequency;
}

void SpacecraftCalculator:: getSpacecraftParams	(double* ballisticCoeff, double* txFrequency) const
{
	*ballisticCoeff = ballisticCoeff_;
	*txFrequency = txFrequency_;
}

void SpacecraftCalculator:: setSpacecraftState (double secondsFromEpoch, const double (&posSci)[3], const double (&velSci)[3])
{
	secondsFromEpoch_ = secondsFromEpoch;
	earth_.setTargetTime (secondsFromEpoch_);
	
	Vector3d posSciVec (posSci);
	Vector3d velSciVec (velSci);
	
	spacecraftState_.SC_POSITION = posSciVec;
	spacecraftState_.SC_VELOCITY = velSciVec;
}

void SpacecraftCalculator:: setSpacecraftState	(double secondsFromEpoch, Vector3d const& posSci, Vector3d const& velSci)
{
	secondsFromEpoch_ = secondsFromEpoch;
	earth_.setTargetTime (secondsFromEpoch_);
	
	spacecraftState_.SC_POSITION = posSci;
	spacecraftState_.SC_VELOCITY = velSci;
}

double SpacecraftCalculator:: getSpacecraftState (double* posSci, double* velSci) const
{
	for (int i = 0; i < 3; i++) {
		posSci[i] = (spacecraftState_.SC_POSITION) [i];
		velSci[i] = (spacecraftState_.SC_VELOCITY) [i];
	}
	
	return secondsFromEpoch_;
}

double SpacecraftCalculator:: getSpacecraftState (Vector3d* posSci, Vector3d* velSci) const
{
	*posSci = spacecraftState_.SC_POSITION;
	*velSci = spacecraftState_.SC_VELOCITY;
	
	return secondsFromEpoch_;
}

void SpacecraftCalculator:: getEpochTime (double* epochMjd) const
{
	*epochMjd = epochMjd_;
}

void SpacecraftCalculator:: getSecondsFromEpoch (double* secondsFromEpoch) const
{
	*secondsFromEpoch = secondsFromEpoch_;
}

void SpacecraftCalculator:: resetSpacecraftState (void)
{
	Vector3d posSci, velSci;
	calcInitialScState (&posSci, &velSci);
	
	setSpacecraftState (0.0, posSci, velSci);
}

double SpacecraftCalculator:: integrateSpacecraftState (double dt, double srpErrorRatio)
{
	//Rrunge-Kutta
	VectorXd k1 (6), k2 (6), k3 (6), k4 (6);
	
	calcSpacecraftStateDerivative (&k1, spacecraftState_, srpErrorRatio);
	calcSpacecraftStateDerivative (&k2, spacecraftState_ + k1 * 0.5 * dt, srpErrorRatio);
	calcSpacecraftStateDerivative (&k3, spacecraftState_ + k2 * 0.5 * dt, srpErrorRatio);
	calcSpacecraftStateDerivative (&k4, spacecraftState_ + k3 * dt, srpErrorRatio);
	
	spacecraftState_ = spacecraftState_ + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0 * dt;
	
	secondsFromEpoch_ += dt;
	earth_.setTargetTime (secondsFromEpoch_);
	
	return secondsFromEpoch_;
}

void SpacecraftCalculator:: test1 (int periodDay)
{
	// set orbit information
	const double EpochMjd = 56992.264444;
	const double PosEci[3] = {10718921.0, 747344.0, -1050332.0};
	const double VelEci[3] = {5616.853  , 6764.471,  -4193.746};
	setSpacecraftOrbitInfo (EpochMjd, PosEci, VelEci);
	
	// set parameters
	const double Frequency = 437.325e6f;
	const double BallisticCoeff = 150.0;
	setSpacecraftParams (BallisticCoeff, Frequency);
	
	// variables
	Vector3d scPos, scVel;
	double declination, ra, doppler, distance, speed;
	
	const double Dt = 60.0;
	const double SecondsDay = 3600.0 * 24.0;
	const int NDay = static_cast<int> (SecondsDay / Dt); 
	double t = 0.0;
	int n = 0;
	
	cout << "day, x, y, z, u, v, w, declination, RA, doppler, distance, speed" << endl;
	
	do {
		
		getSpacecraftState (&scPos, &scVel);
		getGeometryEarthCentered (&declination, &ra);
		getDopplerFreqEarthCentered (&doppler);
		getDistanceEarthCentered (&distance);
		getSpacecraftRelativeSpeed (&speed);
		
		if (n % NDay == 0) {
			cout << n / NDay + 1 << ",";
			cout << scPos[0] << "," << scPos[1] << "," << scPos[2] << ",";
			cout << scVel[0] << "," << scVel[1] << "," << scVel[2] << ",";
			cout << declination << "," << ra << ",";
			cout << doppler << ",";
			cout << distance << ",";
			cout << speed;
			cout << endl;
		}
		
		t = integrateSpacecraftState (Dt);
		
		n++;
		
	} while (t < SecondsDay * static_cast<double> (periodDay));
}

void SpacecraftCalculator:: calcInitialScState (Vector3d* posSci, Vector3d* velSci) const
{
	const double Pi = M_PI;
	const double Obliquity = - 23.44 * Pi / 180.0;
	
	Planet earth ("earth", epochMjd_);
	earth.setTargetTime (0.0);
	Vector3d earthPos, earthVel;
	earth.getPosition (&earthPos);
	earth.getVelocity (&earthVel);
	
	Vector3d posEciVec (initialPosEci_);
	Vector3d velEciVec (initialVelEci_);
	
	Matrix3d C;
	tf:: calcDcm (&C, 0, -1.0 * Obliquity);
	
	*posSci = earthPos + C * posEciVec;		// convert from ECI to SCI
	*velSci = earthVel + C * velEciVec;		// convert from ECI to SCI
}

void SpacecraftCalculator:: calcSpacecraftStateDerivative (VectorXd* scStateDerivative, VectorXd const& scState, double srpErrorRatio) const
{
	scStateDerivative->SC_POSITION = scState.SC_VELOCITY;
	
	Vector3d solarGravityAcc, srpAcc, earthGravityAcc;
	calcSolarGravityAcc (&solarGravityAcc, scState);
	calcSolarRadiationPressureAcc (&srpAcc, scState, srpErrorRatio);
	calcEarthGravityAcc (&earthGravityAcc, scState);
	
	scStateDerivative->SC_VELOCITY = solarGravityAcc + srpAcc + earthGravityAcc;
}

void SpacecraftCalculator:: calcSolarGravityAcc (Vector3d* acceleration, VectorXd const& scState) const
{
	const double MueSun = 1.32712442099e20;
	
	double distance = (scState.SC_POSITION).norm ();
	double solarGravity = MueSun / (distance * distance);

	*acceleration = -1.0 * solarGravity * (scState.SC_POSITION / distance);
}

void SpacecraftCalculator:: calcSolarRadiationPressureAcc (Vector3d* acceleration, VectorXd const& scState, double srpErrorRatio) const
{
	const double SrpG1 = 1.0e14;
	const double SrpCoeff = 1.0;
	
	double distance = (scState.SC_POSITION).norm ();
	
	// cannon-ball SRP model
	double srpForce = SrpCoeff * SrpG1 / ballisticCoeff_ / (distance * distance);
	srpForce = srpForce * (1.0 + srpErrorRatio);
	
	*acceleration = -1.0 * srpForce * (scState.SC_POSITION / distance);
}

void SpacecraftCalculator:: calcEarthGravityAcc (Vector3d* acceleration, VectorXd const& scState) const
{
	const double MueEarth = 3.986004418e14;
	
	Vector3d earthPos;
	earth_.getPosition (&earthPos);
	
	Vector3d relativePos = scState.SC_POSITION - earthPos;
	double distance = relativePos.norm ();
	double earthGravity = MueEarth / (distance * distance);
	
	*acceleration = -1.0 * earthGravity * (relativePos / distance);
}
