/********************************************************************************
* File Name : gstation.cpp
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
#include "gstation.h"


#define GS_LONGITUDE gsLocation[0]	// longitude of the ground station
#define GS_LATITUDE  gsLocation[1]	// latitude of the ground station
#define GS_HEIGHT    gsLocation[2]	// altitude of the ground station

#define SC_POSITION  scState.block (0, 0, 3, 1)	// spacecraft position
#define SC_VELOCITY  scState.block (3, 0, 3, 1)	// spacecraft velocity


const double GStationCalculator:: LightSpeed = 299792458.0;
const double GStationCalculator:: SecondsPerDay = 24.0 * 3600.0;
const double GStationCalculator:: Pi = 3.14159265359;

const double GStationCalculator:: EarthSpinRate = 2.0 * Pi / 23.9345 / 3600.0;
const double GStationCalculator:: Obliquity = -23.45 * (Pi / 180.0);
const double GStationCalculator:: EarthRadius = 6.378137e6;
const double GStationCalculator:: EarthF = 1.0 / 298.257223563;


GStationCalculator:: GStationCalculator (void)
{
	firstWrite = 1;
}


double GStationCalculator:: elevationAt (double* gsLocation, VectorXd scState, Planet earth)
{
	Vector3d relativePosEci;	// spacecraft position from ground station in Earth Centerd Inertial coordinate
	Vector3d relativeVelEci;	// spacecraft velocity ground station in Earth Centerd Inertial coordinate
	calcScRelativePosInEci (relativePosEci, relativeVelEci, gsLocation, scState, earth);

	Vector3d relativePosEnu;	// spacecraft position from ground station in East North Up coordinate
	calcScPositionInEnu (relativePosEnu, relativePosEci, gsLocation, earth);

	return elevation (relativePosEnu);
}


void GStationCalculator:: calcGeometry (Vector3d &ans, double* gsLocation, VectorXd scState, Planet earth, int error)
{
	Vector3d relativePosEci;	// spacecraft position from ground station in Earth Centerd Inertial coordinate
	Vector3d relativeVelEci;	// spacecraft velocity ground station in Earth Centerd Inertial coordinate
	calcScRelativePosInEci (relativePosEci, relativeVelEci, gsLocation, scState, earth);

	Vector3d relativePosEnu;	// spacecraft position from ground station in East North Up coordinate
	calcScPositionInEnu (relativePosEnu, relativePosEci, gsLocation, earth);


	const int Azimuth = 0, Elevation = 1, Doppler = 2;

	// calculation of the soacecraft direction and doppler shift
	ans[Azimuth]   = azimuth (relativePosEnu);
	ans[Elevation] = elevation (relativePosEnu);
	ans[Doppler]   = dopplerShift (relativePosEci, relativeVelEci);
	

	// mesurement error
	Vector3d n;
	// calculation of the sigma points (UKF)
	if (scState.size () > 6) {
		n[Azimuth]   = scState[6];
		n[Elevation] = scState[7];
		n[Doppler]   = scState[8];
	}
	// calculation of the white noise
	else if (error) {
		n[Azimuth]   = noise_.whiteNoise (SpacecraftCalculator:: ObsAngleDev * (Pi / 180.0));
		n[Elevation] = noise_.whiteNoise (SpacecraftCalculator:: ObsAngleDev * (Pi / 180.0));
		n[Doppler]   = noise_.whiteNoise (SpacecraftCalculator:: ObsDopplerDev);
	}
	// true value (without the error)
	else {
		n[Azimuth]   = 0.0;
		n[Elevation] = 0.0;
		n[Doppler]   = 0.0;
	}

	ans = ans + n;
}


void GStationCalculator:: calcDoppler (double &ans, double* gsLocation, VectorXd scState, Planet earth, int error)
{
	Vector3d relativePosEci;	// spacecraft position from ground station in Earth Centerd Inertial coordinate
	Vector3d relativeVelEci;	// spacecraft velocity ground station in Earth Centerd Inertial coordinate
	calcScRelativePosInEci (relativePosEci, relativeVelEci, gsLocation, scState, earth);

	// calculation of the doppler shift
	ans = dopplerShift (relativePosEci, relativeVelEci);

	// mesurement error
	double n;
	// calculation of the sigma points (UKF)
	if (scState.size () > 6) {
		n = scState[6];
	}
	// calculation of the white noise
	else if (error) {
		n = noise_.whiteNoise (SpacecraftCalculator:: ObsDopplerDev);
	}
	// true value (without the error)
	else {
		n = 0.0;
	}

	ans = ans + n;
}


void GStationCalculator:: calcGsPositionInEci (Vector3d &pos, Vector3d &vel, double* gsLocation, Planet earth)
{
	// reference: http://www.enri.go.jp/~fks442/K_MUSEN/1st/1st021118.pdf
	
	double e = sqrt (2.0 * EarthF - EarthF * EarthF);
	double N = EarthRadius / sqrt (1.0 - e * e * sin (GS_LATITUDE) * sin (GS_LATITUDE));
	
	const int X = 0, Y = 1, Z = 2;

	double theta = earth.thetaG_;		// Greenwich Sideral Time

	// lat, lon, alt -> position in Earth Centerd Inertial coordinate
	pos[X] = (N + GS_HEIGHT) * cos (GS_LATITUDE) * cos (GS_LONGITUDE + theta);
	pos[Y] = (N + GS_HEIGHT) * cos (GS_LATITUDE) * sin (GS_LONGITUDE + theta);
	pos[Z] = (N * (1.0 - e * e) + GS_HEIGHT) * sin (GS_LATITUDE);

	vel[X] = -(N + GS_HEIGHT) * EarthSpinRate * cos (GS_LATITUDE) * sin (GS_LONGITUDE + theta);
	vel[Y] =  (N + GS_HEIGHT) * EarthSpinRate * cos (GS_LATITUDE) * cos (GS_LONGITUDE + theta);
	vel[Z] =  0.0;
}


void GStationCalculator:: calcScPositionInEci (Vector3d &pos, Vector3d &vel, VectorXd scState, Planet earth)
{
	Vector3d relativePos, relativeVel;	// spacecraft position from ground station in Earth Centerd Inertial coordinate

	relativePos = SC_POSITION - earth.pos_;
	relativeVel = SC_VELOCITY - earth.vel_;

	Matrix3d C;	// Sun Centerd Inertial coodinate -> Earth Centerd Inertial coordinate
	C <<
		1.0,	             0.0,				0.0,
		0.0,	 cos (Obliquity),	sin (Obliquity),
		0.0,	-sin (Obliquity),	cos (Obliquity);

	pos = C * relativePos;
	vel = C * relativeVel;
}


void GStationCalculator:: calcScRelativePosInEci (Vector3d &pos, Vector3d &vel, double* gsLocation, VectorXd scState, Planet earth)
{
	Vector3d gsPosEci, gsVelEci;	// ground station position and velocity in Earth Centerd Inertial coordinate
	calcGsPositionInEci (gsPosEci, gsVelEci, gsLocation, earth);

	Vector3d scPosEci, scVelEci;	// spacecraft position and velocity in Earth Centerd Inertial coordinate
	calcScPositionInEci (scPosEci, scVelEci, scState, earth);

	pos = scPosEci - gsPosEci;
	vel = scVelEci - gsVelEci;
}


void GStationCalculator:: calcScPositionInEnu (Vector3d &pos, Vector3d relativeScPosEci, double* gsLocation, Planet earth)
{
	double theta = earth.thetaG_;	// Greenwich Sideral Time

	double lon = GS_LONGITUDE;
	double lat = GS_LATITUDE;

	Matrix3d C;	// 地球中心慣性座標->局所地表座標への座標変換行列
	C << 
		            -sin (lon + theta),              cos (lon + theta),      0.0,
		-cos (lon + theta) * sin (lat), -sin (lon + theta) * sin (lat),	cos(lat),
		 cos (lon + theta) * cos (lat),	 sin (lon + theta) * cos (lat),	sin(lat);

	pos = C * relativeScPosEci;
}


double GStationCalculator:: azimuth (Vector3d relativeScPosEnu)
{
	return atan2 (relativeScPosEnu[0], relativeScPosEnu[1]);	// angle from North to East
}


double GStationCalculator:: elevation (Vector3d relativeScPosEnu)
{
	return atan2 (relativeScPosEnu[2], sqrt(relativeScPosEnu[0] * relativeScPosEnu[0] + relativeScPosEnu[1] * relativeScPosEnu[1]));
}


double GStationCalculator:: dopplerShift (Vector3d relativeScPosEci, Vector3d relativeScVelEci)
{
	double radialVelocity;	// spacecraft ray velocity
	radialVelocity = relativeScVelEci.dot (relativeScPosEci / relativeScPosEci.norm ());

	double freq0 = SpacecraftCalculator:: ScRadioFreq;

	return freq0 * (radialVelocity / LightSpeed);
}


void GStationCalculator:: fileOut (char const* gsName, double t, Vector3d trueGeometry)
{
	char fileName[64];
	sprintf (fileName, "%s.csv", gsName);

	ofstream fout (fileName,ios::out|ios::app);
	if (!fout) {
		printf ("[ERROR] Cannot open gstation.csv\n");
		printf ("[ERROR] Program stopped\n");
		exit (1);
	}

	if (firstWrite) {
		fout << "day," << "t[s],"
			 << "azi[deg]," << "ele[deg]," << "doppler[Hz]"
			 << endl;

		firstWrite = 0;
	}

	fout << t / SecondsPerDay << ",";
	fout << t << ",";
	
	fout << trueGeometry[0] * (180.0 / Pi) << ",";
	fout << trueGeometry[1] * (180.0 / Pi) << ",";
	fout << trueGeometry[2];

	fout << endl;

	fout.close();
}


void GStationCalculator:: test (double duration)
{
	double gsLocation[3];

	// Tama Art University
	gsLocation[0] = 139.350433 * Pi / 180.0;
	gsLocation[1] =  35.611946 * Pi / 180.0;
	gsLocation[2] = 146.0;

	/*
	// Usuda
	gsLocation[0] = (138.0 + 21.0 / 60.0 + 54.0 / 3600.0) * Pi / 180.0;
	gsLocation[1] = (36.0 + 7.0 / 60.0 + 44.0 / 3600.0) * Pi / 180.0;
	gsLocation[2] = 1456.0;
	
	// Berlin
	gsLocation[0] = (13.0 + 22.0 / 60.0 + 39.0 / 3600.0) * Pi / 180.0;
	gsLocation[1] = (52.0 + 30.0 / 60.0 + 59.0 / 3600.0) * Pi / 180.0;
	gsLocation[2] = 0.0;
	
	// Boston
	gsLocation[0] = -(71.0 + 3.0 / 60.0 + 42.0 / 3600.0) * Pi / 180.0;
	gsLocation[1] = (42.0 + 21.0 / 60.0 + 28.0 / 3600.0) * Pi / 180.0;
	gsLocation[2] = 0.0;
	
	// Sydney
	gsLocation[0] = (151.0 + 11.0 / 60.0 + 32.0 / 3600.0) * Pi / 180.0;
	gsLocation[1] = -(33.0 + 52.0 / 60.0 + 6.0 / 3600.0) * Pi / 180.0;
	gsLocation[2] = 0.0;
	
	// Buenos Aires
	gsLocation[0] = -(58.0 + 22.0 / 60.0 + 54.0 / 3600.0) * Pi / 180.0;
	gsLocation[1] = -(34.0 + 36.0 / 60.0 + 13.0 / 3600.0) * Pi / 180.0;
	gsLocation[2] = 0.0;
	*/

	Planet earth ("earth", SpacecraftCalculator:: DepartureMjd);
	
	SpacecraftCalculator sccal;
	VectorXd x (6);
	sccal.init (x, earth, 0);

	Vector3d trueGeo;
	this->calcGeometry (trueGeo, gsLocation, x, earth, 0);

	double dt;
	double t = 0.0;

	do {

		sccal.fileOutStatus (t, x, earth);
		this->fileOut ("gs", t, trueGeo);

		if (t < 1.0 * 24.0 * 3600.0) {
			dt = 60.0;
		}
		else {
			dt = 3600.0;
		}

		t += dt;
		earth.calcOrbitAt (t);
		sccal.integrate (dt, x, earth);
		
		this->calcGeometry (trueGeo, gsLocation, x, earth, 0);
		
	}
	while (t < duration);
}
