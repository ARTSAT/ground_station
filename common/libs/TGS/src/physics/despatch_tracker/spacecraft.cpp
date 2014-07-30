/********************************************************************************
* File Name : spacecraft.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in the sun-senterd
* inertial coordinate
*
********************************************************************************/
#include "spacecraft.h"


#define SC_POSITION x.block (0, 0, 3, 1)	// position of the spacecraft
#define SC_VELOCITY x.block (3, 0, 3, 1)	// velocoty of the spacecraft

//***************************************************************
// Departure in Modified Julian Day
//***************************************************************
const double SpacecraftCalculator:: DepartureMjd = 56992.264444;

//***************************************************************
// Spacecraft initial condition
//***************************************************************
const double SpacecraftCalculator:: InitPosX = 10718921.0; 
const double SpacecraftCalculator:: InitPosY = 747344.0;
const double SpacecraftCalculator:: InitPosZ = -1050332.0;

const double SpacecraftCalculator:: InitVelX =  5616.853;
const double SpacecraftCalculator:: InitVelY =  6764.471;
const double SpacecraftCalculator:: InitVelZ =  -4193.746;

//***************************************************************
// Error of the soacecraft initial consition
//***************************************************************
const double SpacecraftCalculator:: ErrorPosX = 100000.0;
const double SpacecraftCalculator:: ErrorPosY = 100000.0;
const double SpacecraftCalculator:: ErrorPosZ = 100000.0;

const double SpacecraftCalculator:: ErrorVelX = 100.0;
const double SpacecraftCalculator:: ErrorVelY = 100.0;
const double SpacecraftCalculator:: ErrorVelZ = 100.0;

//***************************************************************
// Measurement noise
//***************************************************************
const double SpacecraftCalculator:: ObsDopplerDev = 1000.0;
const double SpacecraftCalculator:: ObsAngleDev = 10.0;

//***************************************************************
// Disturbances
//***************************************************************
const double SpacecraftCalculator:: SrpFlucRatioMax = 1.0;

//***************************************************************
// Spacecraft specifications
//***************************************************************
const double SpacecraftCalculator:: ScArea = 0.5 * 0.5;
const double SpacecraftCalculator:: ScMass = 35.0;
const double SpacecraftCalculator:: ScRadioFreq = 437.325e6;

// Other constants
const double SpacecraftCalculator:: SecondsPerDay = 24.0 * 3600.0;
const double SpacecraftCalculator:: Pi = 3.14159265359;
const double SpacecraftCalculator:: LightSpeed = 299792458.0;
const double SpacecraftCalculator:: Obliquity = -23.44 * (Pi / 180.0);
const double SpacecraftCalculator:: MueEarth = 3.986004418e14;
const double SpacecraftCalculator:: MueSun = 1.32712442099e20;

const double SpacecraftCalculator:: SrpG1 = 1.0e14;
const double SpacecraftCalculator:: SrpCoeff = 1.0;


SpacecraftCalculator:: SpacecraftCalculator (void)
{
	firstWriteStatus = 1;
	firstWriteMeasurements = 1;
}


void SpacecraftCalculator:: init (VectorXd& x, Planet earth, int error)
{
	// calculate init condition of the earth
	earth.calcOrbitAt (0.0);

	// spacecraft's initial condition
	const int X = 0, Y = 1, Z = 2;
	Vector3d posECenterd, velECenterd;
	posECenterd[X] = InitPosX;	posECenterd[Y] = InitPosY;	posECenterd[Z] = InitPosZ;
	velECenterd[X] = InitVelX;	velECenterd[Y] = InitVelY;	velECenterd[Z] = InitVelZ;

	// if error = 1, error is added to initial condition
	if (error) {
		posECenterd[X] += ErrorPosX;	posECenterd[Y] += ErrorPosY;	posECenterd[Z] += ErrorPosZ;
		velECenterd[X] += ErrorVelX;	velECenterd[Y] += ErrorVelY;	velECenterd[Z] += ErrorVelZ;
	}

	// transform to sun centerd inertial coordinate
	Matrix3d C; 
	C <<
		1.0,	0.0,				0.0,
		0.0,	cos (Obliquity),	-sin (Obliquity),
		0.0,	sin (Obliquity),	cos (Obliquity); 

	SC_POSITION = earth.pos_ + C * posECenterd;
	SC_VELOCITY = earth.vel_ + C * velECenterd;
}


void SpacecraftCalculator:: init (VectorXd& x, Vector3d posECenterd, Vector3d velECenterd, Planet earth)
{
	// calculate init condition of the earth
	earth.calcOrbitAt (0.0);
	
	// transform to sun centerd inertial coordinate
	Matrix3d C; 
	C <<
		1.0,	0.0,				0.0,
		0.0,	cos (Obliquity),	-sin (Obliquity),
		0.0,	sin (Obliquity),	cos (Obliquity); 

	SC_POSITION = earth.pos_ + C * posECenterd;
	SC_VELOCITY = earth.vel_ + C * velECenterd;
}


void SpacecraftCalculator:: integrate (double dt, VectorXd &x, Planet earth)
{
	Vector3d gravityEarth;
	Vector3d gravitySun;
	Vector3d solarPressure;

	calcGravityFromEarth (gravityEarth, x, earth);
	calcGravityFromSun (gravitySun, x);
	calcSolarPressure (solarPressure, x);
	
	// total accelaration
	Vector3d acceleration;
	acceleration = gravityEarth + gravitySun + solarPressure;

	SC_POSITION += SC_VELOCITY * dt;
	SC_VELOCITY += acceleration * dt;
}


void SpacecraftCalculator:: calcGravityFromEarth (Vector3d& ans, VectorXd x, Planet earth)
{
	Vector3d relativePos = SC_POSITION - earth.pos_;
	double distance = relativePos.norm ();

	double gravityEarth = MueEarth / (distance * distance);

	ans = -1.0 * gravityEarth * (relativePos / distance);
}


void SpacecraftCalculator:: calcGravityFromSun (Vector3d& ans, VectorXd x)
{
	double distance = SC_POSITION.norm ();

	double gravitySun = MueSun / (distance * distance);

	ans = -1.0 * gravitySun * (SC_POSITION / distance);
}


void SpacecraftCalculator:: calcSolarPressure (Vector3d& ans, VectorXd x)
{
	double distance = SC_POSITION.norm ();

	// cannon-ball model
	double solarPressure = SrpCoeff * SrpG1 * (ScArea / ScMass) / (distance * distance);

	double ratio;	// fluctuation ratio

	// calculate sigma points (UKF)
	if(x.size() > 6) {
		ratio = x[6];
	}
	// calculate true value
	else {
		ratio = noise_.plusMinus (SrpFlucRatioMax);
	}

	solarPressure = (1.0 + ratio) * solarPressure;

	ans = -1.0 * solarPressure * (SC_POSITION / distance);
}


void SpacecraftCalculator:: calcGeometry (Vector3d &ans, VectorXd x, Planet earth, int error)
{
	Vector3d relativePos, relativeVel;
	relativePos = SC_POSITION - earth.pos_;
	relativeVel = SC_VELOCITY - earth.vel_;

	const int X = 0, Y = 1, Z = 2;
	const int RightAscension = 0, Declination = 1, Doppler = 2;

	ans[RightAscension] = atan2 (relativePos[Y], relativePos[X]);
	ans[Declination] = atan2 (relativePos[Z], sqrt (relativePos[X] * relativePos[X] + relativePos[Y] * relativePos[Y]));
	
	double radialVel = relativeVel.dot (relativePos / relativePos.norm ());	// ray velocity
	ans[Doppler] = ScRadioFreq * (radialVel / LightSpeed);
	
	// measurement noise
	Vector3d n;
	// calculate sigma points (UKF)
	if (x.size () > 6) {
		n[RightAscension] = x[6];
		n[Declination] = x[7];
		n[Doppler] = x[8];
	}
	// calculate white noise
	else if (error) {
		n[RightAscension] = noise_.whiteNoise (ObsAngleDev * (Pi / 180.0));
		n[Declination] = noise_.whiteNoise (ObsAngleDev * (Pi / 180.0));
		n[Doppler] = noise_.whiteNoise (ObsDopplerDev);
	}
	// calculate true value
	else {
		n[RightAscension] = 0.0;
		n[Declination] = 0.0;
		n[Doppler] = 0.0;
	}

	ans = ans + n;
}

void SpacecraftCalculator:: calcDoppler (double &ans, VectorXd x, Planet earth, int error)
{
	Vector3d relativePos, relativeVel;
	relativePos = SC_POSITION - earth.pos_;
	relativeVel = SC_VELOCITY - earth.vel_;
	
	double radialVel = relativeVel.dot (relativePos / relativePos.norm ());	// ray velocity
	ans = ScRadioFreq * (radialVel / LightSpeed);

	// measurement noise
	double n;
	// calculate sigma points (UKF)
	if (x.size () > 6) {
		n = x[6];
	}
	// calculate white noise
	else if (error) {
		n = noise_.whiteNoise (ObsDopplerDev);
	}
	// calculate true value
	else {
		n = 0.0;
	}

	ans = ans + n;
}

double SpacecraftCalculator:: distanceEarthCenterd (VectorXd x, Planet earth)
{
	Vector3d relativePos;
	relativePos = SC_POSITION - earth.pos_;
	
	double d = relativePos.norm ();
	
	return d;
}


void SpacecraftCalculator:: fileOutStatus (double t, VectorXd x, Planet earth)
{
	ofstream fout ("spacecraft.csv",ios::out|ios::app);
	if (!fout) {
		printf ("[ERROR] Cannot open spacecraft.csv\n");
		printf ("[ERROR] Program stopped\n");
		exit (1);
	}

	if (firstWriteStatus) {
		fout << "day," << "t[s],"
			 << "x[m]," << "y[m]," << "z[m]," 
			 << "u[m/s]," << "v[m/s]," << "w[m/s],"
			 << "x_e[m]," << "y_e[m]," << "z_e[m]," 
			 << "u_e[m/s]," << "v_e[m/s]," << "w_e[m/s]," 
			 << "thetaG[deg]"
			 << endl;

		firstWriteStatus = 0;
	}

	fout << t / SecondsPerDay << ",";
	fout << t << ",";
	
	for(int i=0; i<6; i++)	fout << x[i] << ",";
	
	for(int i=0; i<3; i++)	fout << earth.pos_[i] << ",";
	for(int i=0; i<3; i++)	fout << earth.vel_[i] << ",";
	fout << earth.thetaG_ * (180.0 / Pi) << endl;

	fout.close();
}


void SpacecraftCalculator:: fileOutMeasurements (double t, Vector3d trueGeometry, Vector3d measuredGeometry)
{
	ofstream fout ("measurements.csv",ios::out|ios::app);
	if (!fout) {
		printf ("[ERROR] Cannot open measurements.csv\n");
		printf ("[ERROR] Program stopped\n");
		exit (1);
	}

	if (firstWriteMeasurements) {
		fout << "day," << "t[s],"
			 << "lon[deg]," << "lat[deg]," << "doppler[Hz]," 
			 << "lon_m[deg]," << "lat_m[deg]," << "doppler_m[Hz]"
			 << endl;

		firstWriteMeasurements = 0;
	}

	fout << t / SecondsPerDay << ",";
	fout << t << ",";
	
	fout << trueGeometry[0] * (180.0 / Pi) << ",";
	fout << trueGeometry[1] * (180.0 / Pi) << ",";
	fout << trueGeometry[2]  << ",";

	fout << measuredGeometry[0] * (180.0 / Pi) << ",";
	fout << measuredGeometry[1] * (180.0 / Pi) << ",";
	fout << measuredGeometry[2];

	fout << endl;

	fout.close();
}


void SpacecraftCalculator::test (double duration)
{
	Planet earth ("earth", DepartureMjd);
	
	VectorXd x (6);
	this->init (x, earth, 0);

	Vector3d trueZ, measuredZ;
	this->calcGeometry (trueZ, x, earth, 0);
	this->calcGeometry (measuredZ, x, earth, 1);

	double dt;
	double t = 0.0;

	do {

		this->fileOutStatus (t, x, earth);
		this->fileOutMeasurements (t, trueZ, measuredZ);

		if (t < 1.0 * 24.0 * 3600.0) {
			dt = 60.0;
		}
		else {
			dt = 3600.0;
		}

		t += dt;
		earth.calcOrbitAt (t);
		this->integrate (dt, x, earth);

		this->calcGeometry (trueZ, x, earth);
		this->calcGeometry (measuredZ, x, earth, 1);
		
	}
	while (t < duration);
}