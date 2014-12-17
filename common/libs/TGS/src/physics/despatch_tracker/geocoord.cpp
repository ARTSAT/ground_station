/********************************************************************************
* File Name : geocoord.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.4
* Language  : C++
*********************************************************************************
* class to calculate doppler frequency, azimuth, and elevation of 
* the spacecraft at a specific observer 
*
********************************************************************************/
#include "spacecraft.h"

#define	SC_POSITION	block (0, 0, 3, 1)
#define	SC_VELOCITY	block (3, 0, 3, 1)

void SpacecraftCalculator:: setObserverGeoCoord (double latitude, double longitude, double altitude)
{
	observerGeoCoord_[0] = latitude;
	observerGeoCoord_[1] = longitude;
	observerGeoCoord_[2] = altitude;
}

void SpacecraftCalculator:: getObserverGeoCoord (double* latitude, double* longitude, double* altitude) const
{
	*latitude = observerGeoCoord_[0];
	*longitude = observerGeoCoord_[1];
	*altitude = observerGeoCoord_[2];
}

void SpacecraftCalculator:: getGeometryEarthCentered (double *declination, double* rightAscension, double decError, double raError) const
{	
	Vector3d scPosEci;
	calcSpacecraftPosEci (&scPosEci);
    
    const int X  = 0, Y = 1, Z = 2;
	const double Pi = M_PI;
	
	*declination = atan2 (scPosEci[Z], sqrt (scPosEci[X] * scPosEci[X] + scPosEci[Y] * scPosEci[Y])) + decError;
	*rightAscension = atan2 (scPosEci[Y], scPosEci[X]) + raError;
	
	tf:: normalizeRadian (declination, -Pi);
	tf:: normalizeRadian (rightAscension);
}

void SpacecraftCalculator:: getDopplerRatioEarthCentered (double* downlink, double* uplink) const
{
	Vector3d earthPos, earthVel;
	earth_.getPosition (&earthPos);
	earth_.getVelocity (&earthVel);
	
	Vector3d relativePos = spacecraftState_.SC_POSITION - earthPos;
	Vector3d relativeVel = spacecraftState_.SC_VELOCITY - earthVel;
	
	calcDopplerRatio (downlink, uplink, relativePos, relativeVel);
}

void SpacecraftCalculator:: getDistanceEarthCentered (double* distance, double error) const
{
	Vector3d earthPos;
	earth_.getPosition (&earthPos);
	
	*distance = (spacecraftState_.SC_POSITION - earthPos).norm () + error;
}

void SpacecraftCalculator:: getSpacecraftRelativeSpeed (double* speed) const
{
	Vector3d earthVel;
	earth_.getVelocity (&earthVel);
	
	Vector3d relativeVel;
	relativeVel = spacecraftState_.SC_VELOCITY - earthVel;
	
	*speed = relativeVel.norm ();
}

void SpacecraftCalculator:: getSpacecraftGeoCoord (double* latitude, double* longitude, double* altitude) const
{
	Vector3d scPosEcef;
	calcSpacecraftPosEcef (&scPosEcef);
	
	const double EarthOblatness = 1.0 / 298.257223563;
	const double EarthRadius = 6.378137e6f;
	double a = EarthRadius;
	double b = a * (1.0 - EarthOblatness);
	double e = sqrt (2.0 * EarthOblatness - EarthOblatness * EarthOblatness);
	double n = a * a - b * b;
	
	const int X = 0, Y = 1, Z = 2;
	double p = sqrt (scPosEcef[X] * scPosEcef[X] + scPosEcef[Y] * scPosEcef[Y]);
	double t = atan2 (scPosEcef[Z] * a, p * b);
	double sint = sin (t);
	double cost = cos (t);
	
	double lat = atan2 (scPosEcef[Z] + n/b * sint*sint*sint, p - n/a *cost*cost*cost);
	double h = a / sqrt (1.0 - e * e * sin (lat) * sin (lat));
	*longitude = atan2 (scPosEcef[Y], scPosEcef[X]);
	*altitude = (p / cos (lat)) - h;
	*latitude = lat;
	
	const double Pi = M_PI;
	tf:: normalizeRadian (latitude, -Pi);
	tf:: normalizeRadian (longitude, -Pi);
}

void SpacecraftCalculator:: getSpacecraftDirection (double* elevation, double* azimuth, double eleError, double aziError) const
{
	Vector3d scRelativePosEnu;
	calcSpacecraftRelativePosEnu (&scRelativePosEnu);
	
    const int X  = 0, Y = 1, Z = 2;
	const double Pi = M_PI;
	
	*elevation = atan2 (scRelativePosEnu[Z], sqrt (scRelativePosEnu[X] * scRelativePosEnu[X] + scRelativePosEnu[Y] * scRelativePosEnu[Y])) + eleError;
	*azimuth = atan2 (scRelativePosEnu[X], scRelativePosEnu[Y]) + aziError;
	
	tf:: normalizeRadian (elevation, -Pi);
	tf:: normalizeRadian (azimuth);
}

void SpacecraftCalculator:: getDopplerRatio (double* downlink, double* uplink) const
{
	Vector3d scPosEci, scVelEci;
	calcSpacecraftPosEci (&scPosEci);
	calcSpacecraftVelEci (&scVelEci);
	
	Vector3d obsPosEci, obsVelEci;
	calcObserverPosEci (&obsPosEci);
	calcObserverVelEci (&obsVelEci);
	
	Vector3d scRelativePosEci, scRelativeVelEci;
	scRelativePosEci = scPosEci - obsPosEci;
	scRelativeVelEci = scVelEci - obsVelEci;
	
	calcDopplerRatio (downlink, uplink, scRelativePosEci, scRelativeVelEci);
}

void SpacecraftCalculator:: test2 (int periodDay)
{
	// set orbit information
	const double EpochMjd = 56992.264444;
	const double PosEci[3] = {10718921.0, 747344.0, -1050332.0};
	const double VelEci[3] = {5616.853  , 6764.471,  -4193.746};
	setSpacecraftOrbitInfo (EpochMjd, PosEci, VelEci);
	
	// set parameters
	const double BallisticCoeff = 150.0;
	setSpacecraftParams (BallisticCoeff);
	
	// set observer position
	const double Pi = M_PI;
	const double observer[3] = {35.611946 * Pi / 180.0, 139.350433 * Pi / 180.0, 146.0};
	setObserverGeoCoord (observer[0], observer[1], observer[2]);
	
	// variables
	Vector3d scPos, scVel;
	double ele, azi, doppler_down, doppler_up;
	double lat, lon, alt;
	
	const double Dt = 60.0;
	const double SecondsDay = 3600.0 * 24.0;
	const double SecondsHour = 3600.0;
	const int NHour = static_cast<int> (SecondsHour / Dt); 
	double t = 0.0;
	int n = 0;
	
	const double Frequency = 437.325e6f;
    
	cout << "hour, elevation, azimuth, doppler, latitude, longitude, altitude" << endl;
	
	do {
		
		getSpacecraftDirection (&ele, &azi);
		getDopplerRatio (&doppler_down, &doppler_up);
		getSpacecraftGeoCoord (&lat, &lon, &alt);
		
		if (n % NHour == 0) {
			cout << n / NHour + 1 << ",";
			cout << ele << "," << azi << ",";
			cout << (doppler_down - 1.0) * Frequency << ",";
			cout << lat << "," << lon << "," << alt;
			cout << endl;
		}
		
		t = integrateSpacecraftState (Dt);
		
		n++;
		
	} while (t < SecondsDay * static_cast<double> (periodDay));
}

void SpacecraftCalculator:: calcObserverPosEci (Vector3d* obsPosEci) const
{
	const double EarthOblatness = 1.0 / 298.257223563;
	const double EarthRadius = 6.378137e6f;
	
	const double lat = observerGeoCoord_ [0];
	const double lon = observerGeoCoord_ [1];
	const double alt = observerGeoCoord_ [2];
	
	double e = sqrt (2.0 * EarthOblatness - EarthOblatness * EarthOblatness);
	double h = EarthRadius / sqrt (1.0 - e * e * sin (lat) * sin (lat));
	
	double gst;
	earth_.getGst (&gst);
	
	const int X = 0, Y = 1, Z = 2;
	Vector3d observerPosEci;
	
	// http://www.enri.go.jp/~fks442/K_MUSEN/1st/1st021118.pdf
	observerPosEci[X] = (h + alt) * cos (lat) * cos (lon + gst);
	observerPosEci[Y] = (h + alt) * cos (lat) * sin (lon + gst);
	observerPosEci[Z] = (h * (1.0 - e * e) + alt) * sin (lat);
	
	*obsPosEci = observerPosEci;
}

void SpacecraftCalculator:: calcObserverVelEci (Vector3d* obsVelEci) const
{
	const double Pi = M_PI;
	const double EarthOblatness = 1.0 / 298.257223563;
	const double EarthRadius = 6.378137e6f;
	const double EarthSpinRate = 2.0 * Pi / 23.9345 / 3600.0;
	
	const double lat = observerGeoCoord_ [0];
	const double lon = observerGeoCoord_ [1];
	const double alt = observerGeoCoord_ [2];
	
	double e = sqrt (2.0 * EarthOblatness - EarthOblatness * EarthOblatness);
	double h = EarthRadius / sqrt (1.0 - e * e * sin (lat) * sin (lat));
	
	double gst;
	earth_.getGst (&gst);
	
	const int X = 0, Y = 1, Z = 2;
	Vector3d observerVelEci;
	
	// http://www.enri.go.jp/~fks442/K_MUSEN/1st/1st021118.pdf
	observerVelEci[X] = -(h + alt) * EarthSpinRate * cos (lat) * sin (lon + gst);
	observerVelEci[Y] =  (h + alt) * EarthSpinRate * cos (lat) * cos (lon + gst);
	observerVelEci[Z] =  0.0;
	
	*obsVelEci = observerVelEci;
}


void SpacecraftCalculator:: calcSpacecraftPosEci (Vector3d* scPosEci) const
{
	const double Pi = M_PI;
	const double Obliquity = - 23.44 * Pi / 180.0;
	
	Vector3d earthPos;
	earth_.getPosition (&earthPos);
	
	Vector3d scPosECentered;
	scPosECentered = spacecraftState_.SC_POSITION - earthPos;

	Matrix3d C;
	tf:: calcDcm (&C, 0, Obliquity);

	*scPosEci = C * scPosECentered;
}

void SpacecraftCalculator:: calcSpacecraftVelEci (Vector3d* scVelEci) const
{
	const double Pi = M_PI;
	const double Obliquity = - 23.44 * Pi / 180.0;
	
	Vector3d earthVel;
	earth_.getVelocity (&earthVel);
	
	Vector3d scVelECentered;
	scVelECentered = spacecraftState_.SC_VELOCITY - earthVel;

	Matrix3d C;
	tf:: calcDcm (&C, 0, Obliquity);
	
	*scVelEci = C * scVelECentered;
}

void SpacecraftCalculator:: calcSpacecraftPosEcef (Vector3d* scPosEcef) const
{
	Vector3d scPosEci;
	calcSpacecraftPosEci (&scPosEci);
	
	double gst;
	earth_.getGst (&gst);
	
	Matrix3d C;
	tf:: calcDcm (&C, 2, gst);
	
	*scPosEcef = C * scPosEci;
}

void SpacecraftCalculator:: calcSpacecraftRelativePosEnu (Vector3d* scPosEnu) const
{
	const double Pi = M_PI;
	
	Vector3d scPosEci;
	calcSpacecraftPosEci (&scPosEci);
	
	Vector3d obsPosEci;
	calcObserverPosEci (&obsPosEci);
	
	Vector3d scRelativePosEci = scPosEci - obsPosEci;
	
	double gst;
	earth_.getGst (&gst);
	
	double lat = observerGeoCoord_[0];
	double lon = observerGeoCoord_[1];
	Matrix3d C1, C2, C3;
	
	tf:: calcDcm (&C1, 2, gst + lon);
	tf:: calcDcm (&C2, 1, Pi / 2.0 - lat);
	tf:: calcDcm (&C3, 2, Pi / 2.0);
	
	*scPosEnu = C3 * C2 * C1 * scRelativePosEci;
}

void SpacecraftCalculator:: calcDopplerRatio (double* downlink, double* uplink, Vector3d const& relativePos, Vector3d const& relativeVel) const
{
	const double LightSpeed = 299792458.0;
	
	double scRaySpeed = -1.0 * relativeVel.dot (relativePos / relativePos.norm ());
    double ratio = scRaySpeed / LightSpeed;
	*downlink = 1.0 + ratio;
    *uplink = 1.0 - ratio;
}
