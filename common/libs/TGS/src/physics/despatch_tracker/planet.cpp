/********************************************************************************
* File Name : planet.cpp
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
#include "planet.h"

const double Planet:: SecondsPerDay = 24.0 * 3600.0;
const double Planet:: Pi = 3.14159265359;
const double Planet:: AU =  149597870700.0;
const double Planet:: MueSun = 1.32712442099e20;


Planet:: Planet (char const* planetName, double t0Mjd)
{
	init (planetName, t0Mjd);
}


Planet:: Planet (void)
{
	return;
}


void Planet:: init (char const* planetName, double t0Mjd)
{
	t0Mjd_ = t0Mjd;

	strcpy (planetName_, planetName);

	// Earth (reference: Rika-Nenpyo H.26)
	if (strcmp (planetName, "earth") == 0) {
		a_ = 1.0 * AU;
		e_ = 0.0167;
		i_ = 0.002 * Pi / 180.0;
		w_ = 102.985 * Pi / 180.0;
		W_ = 174.838 * Pi / 180.0 + Pi;
		M0_ = 137.350 * Pi /180.0;

		epoch_ = (2456800.5 - 2400000.5);	// MJD = JD - 2 400 000.5
		n_ = sqrt (MueSun / (a_ * a_ * a_));
	}

	// 1999JU3 (reference: 2014/3/20 http://ssd.jpl.nasa.gov/sbdb.cgi?sstr=162173)
	else if (strcmp (planetName, "1999ju3") == 0) {
		a_ = 1.1895 * AU;
		e_ = 0.1902;
		i_ = 5.883 * Pi / 180.0;
		w_ = 211.437 * Pi / 180.0;
		W_ = 251.614 * Pi / 180.0;
		M0_ = 322.370 * Pi /180.0;

		epoch_ = (2456800.5 - 2400000.5);	// MJD = JD - 2 400 000.5
		n_ = sqrt (MueSun / (a_ * a_ * a_));
	}
	// ...
	//else if (strcmp (planetName, "???") == 0) {
	//
	//}
	
	// Not defined
	else {
		printf ("[ERROR] Cannot init planet %s\n", planetName);
		printf ("[ERROR] Program stopped\n");
		exit (1);
	}

	firstWrite = 1;

	calcOrbitAt (0.0);
}


void Planet:: calcOrbitAt (double t)
{
	calcPosVelAt (t);
	calcThetaGAt (t);
}


void Planet:: calcPosVelAt (double t)
{
	double E = solveKeplerEq (t + (t0Mjd_ - epoch_) * SecondsPerDay);
	double dotE = n_ / (1.0 - e_ * cos (E));

	const int X = 0, Y = 1, Z = 2;
	Vector3d posPerigee, velPerigee;	// from center body to perigee: x-axis, orbit up: z-axis

	posPerigee[X] = a_ * (cos (E) - e_);
	posPerigee[Y] = a_ * sqrt (1.0 - e_ * e_) * sin (E);
	posPerigee[Z] = 0.0;

	velPerigee[X] = -1.0 * a_ * dotE * sin (E);
	velPerigee[Y] = a_ * sqrt (1.0 - e_ * e_) * dotE * cos (E);
	velPerigee[Z] = 0.0;
	
	Matrix3d C1;
	calcDcm (Z, -w_, C1);
	Matrix3d C2;
	calcDcm (X, -i_, C2);
	Matrix3d C3;
	calcDcm (Z, -W_, C3);

	// transform to the value in Sun Centerd Inertial coordinate
	pos_ = C3 * C2 * C1 * posPerigee;
	vel_ = C3 * C2 * C1 * velPerigee;
}


void Planet:: calcThetaGAt (double t)
{
	// reference: http://ja.wikipedia.org/wiki/%E6%81%92%E6%98%9F%E6%99%82
	if (strcmp (planetName_, "earth") == 0) {
		double jd = t0Mjd_ + 2400000.5;	// JD = MJD + 2 400 000.5
		double tjd = jd - 2440000.5;	// Truncated JD = JD - 2 440 000.5;
		tjd += t / SecondsPerDay;

		thetaG_ = (0.671262 + 1.0027379094 * tjd) * 2.0 * Pi;
		thetaG_ = fmod (thetaG_, 2.0 * Pi);
	}
	else {
		thetaG_ = 0.0;
	}
}


double Planet:: solveKeplerEq (double secondsFromEpoch)
{
	double M = M0_ + n_ * secondsFromEpoch;

	double E;	// Eccentric anomaly
	double tmpE = M;
	
	for (int i = 0; i < 500; i++) {
		E = M + e_ * sin (tmpE);
		
		if (abs (E - tmpE) < 1.0e-9) {
			break;
		}

		tmpE = E;
		
		if (i == 500 - 1) {
			printf ("[ERROR] Cannot solve Kepler Eq\n");
			printf ("[ERROR] Program stopped\n");
			exit (1);
		}
	}

	return E;
}


void Planet:: calcDcm (int axis, double angle, Matrix3d &ans)
{
	const int X = 0, Z = 2;

	switch (axis) {

		// angle[rad] around x-axis
		case X:
			ans <<
				1.0,          0.0,         0.0,
				0.0,  cos (angle), sin (angle),
				0.0, -sin (angle), cos (angle);
			break;
		
		// angle[rad] around z-axis
		case Z:
			ans << 
				 cos (angle), sin (angle), 0.0,
				-sin (angle), cos (angle), 0.0,
				         0.0,         0.0, 1.0;
			break;
		
		default:
			break;
	}
}


void Planet:: fileOut (double t)
{
	char fileName[32];
	sprintf (fileName, "%s.csv", planetName_);

	ofstream fout (fileName, ios:: out | ios:: app);
	if (!fout) {
		printf ("[ERROR] Cannot open %s\n", fileName);
		printf ("[ERROR] Program stopped\n");
		exit (1);
	}

	if(firstWrite) {
		fout << "MJD," << "day," << "t[s]," 
			 << "x[m]," << "y[m]," << "z[m]," 
			 << "u[m/s]," << "v[m/s]," << "w[m/s],"
			 << "thetaG[deg]"
			 << endl;

		firstWrite = 0;
	}

	fout << t0Mjd_ + (t / SecondsPerDay) << ",";
	fout << (t / SecondsPerDay) << ",";
	fout << t << ",";
	for(int i = 0; i < 3; i++) { fout << pos_[i] << ","; }
	for(int i = 0; i < 3; i++) { fout << vel_[i] << ","; }
	fout << thetaG_ * (180.0 / Pi) << endl;

	fout.close();
}


void Planet:: test (double period, double dt)
{
	cout << planetName_ << endl;

	double t = 0.0;

	do {
		this->calcOrbitAt (t);
		this->fileOut (t);

		t += dt;
	}
	while (t < period);
}
