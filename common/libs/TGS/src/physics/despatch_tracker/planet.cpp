/********************************************************************************
* File Name : planet.cpp
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
#include "planet.h"

const string Planet:: NotDefined_ = "not defined";
const string Planet:: Invalid_ = "invalid";

Planet:: Planet (void): planet_ (NotDefined_), epochMjd_ (0.0), 
						position_ (Vector3d:: Zero (3)), velocity_ (Vector3d:: Zero (3)), 
						gst_ (0.0), time_ (0.0)
{
	
}

Planet:: Planet (string planet, double epochMjd): planet_ (planet), epochMjd_ (epochMjd), 
												  position_ (Vector3d:: Zero (3)), velocity_ (Vector3d:: Zero (3)), 
												  gst_ (0.0), time_ (0.0)
{
	initOrbitElements ();
}

Planet:: ~Planet (void) 
{
	
}

int Planet:: setPlanetName (string planet)
{
	planet_ = planet;
	
	const int Failed = 1;
	if (initOrbitElements () == Failed) {
		return 1;
	}
	
	return 0;
}

void Planet:: getPlanetName (string *planet) const
{
	*planet = planet_;
}

void Planet:: setEpoch (double epochMjd)
{
	epochMjd_ = epochMjd;
}

void Planet:: getEpoch (double *epochMjd) const
{
	*epochMjd = epochMjd_;
}

void Planet:: setTargetTime (double time)
{	
	time_ = time;
	
	calcPositionVelocity ();
	calcGst ();
}

void Planet:: getTargetTime (double *time) const
{
	*time = time_;
}

void Planet:: getPosition (double* position) const
{
	for (int i = 0; i < 3; i++) {
		position[i] = position_[i];
	}
}

void Planet:: getPosition (Vector3d* position) const
{
	*position = position_;
}

void Planet:: getVelocity (double* velocity) const
{
	for (int i = 0; i < 3; i++) {
		velocity[i] = velocity_[i];
	}
}

void Planet:: getVelocity (Vector3d* velocity) const
{
	*velocity = velocity_;
}

void Planet:: getGst (double *gst) const
{
	*gst = gst_;
}

void Planet:: test (int periodDay)
{
	const double SecondsPerDay = 24.0 * 3600.0;
	
	// init
	setPlanetName ("earth");
	setEpoch (56992.264444);
	
	double pos[3], vel[3], gst;
	
	cout << "day, x, y, z, u, v, w, gst" << endl;
	
	for (int day = 0; day < periodDay; day++) {
		
		double t = static_cast<double> (day) * SecondsPerDay;
		
		setTargetTime (t);
		
		getPosition (pos);
		getVelocity (vel);
		getGst (&gst);
		
		// output
		cout << day+1 << ",";
		cout << pos[0] << "," << pos[1] << "," << pos[2] << ",";
		cout << vel[0] << "," << vel[1] << "," << vel[2] << ",";
		cout << gst << endl;
	}
}

int Planet:: initOrbitElements (void)
{
	const double Pi = M_PI;
	const double AU =  149597870700.0;
	const double MueSun = 1.32712442099e20;
	
	// Earth (Rika-nenpyo 2014)
	if (planet_ == "earth") {
		a_ = 1.0 * AU;
		e_ = 0.0167;
		i_ = 0.002 * Pi / 180.0;
		w_ = 102.985 * Pi / 180.0;
		W_ = 174.838 * Pi / 180.0 + Pi;
		M0_ = 137.350 * Pi /180.0;

		t0Mjd_ = (2456800.5 - 2400000.5);	// MJD = JD - 2 400 000.5
		n_ = sqrt (MueSun / (a_ * a_ * a_));
	}
	
	// 1999JU3 (http://ssd.jpl.nasa.gov/sbdb.cgi?sstr=162173, 20/3/2014)
	else if (planet_ == "1999ju3") {
		a_ = 1.1895 * AU;
		e_ = 0.1902;
		i_ = 5.883 * Pi / 180.0;
		w_ = 211.437 * Pi / 180.0;
		W_ = 251.614 * Pi / 180.0;
		M0_ = 322.370 * Pi /180.0;

		t0Mjd_ = (2456800.5 - 2400000.5);	// MJD = JD - 2 400 000.5
		n_ = sqrt (MueSun / (a_ * a_ * a_));
	}
	
	// ...
	//else if (planet == ???) {
	//
	//}
	
	else if (planet_ == NotDefined_) {
		a_ = e_ = i_ = w_ = W_ = M0_ = 0.0;
		t0Mjd_ = n_ = 0.0;
	}
	
	else {
		planet_ = Invalid_;
		
		a_ = e_ = i_ = w_ = W_ = M0_ = 0.0;
		t0Mjd_ = n_ = 0.0;
	}
	
	if (!planetNameIsValid ()) {
		return 1;
	}
	
	return 0;
}

int Planet:: calcPositionVelocity (void)
{	
	if (!planetNameIsValid ()) {
		position_ << Vector3d:: Zero (3);
		velocity_ << Vector3d:: Zero (3);
		
		return 1;
	}
	
	double E;
	solveKeplerEq (&E);
	
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
	tf:: calcDcm (&C1, Z, -w_);
	Matrix3d C2;
	tf:: calcDcm (&C2, X, -i_);
	Matrix3d C3;
	tf:: calcDcm (&C3, Z, -W_);

	// transform to the value in Sun Centerd Inertial coordinate
	position_ = C3 * C2 * C1 * posPerigee;
	velocity_ = C3 * C2 * C1 * velPerigee;
	
	return 0;
}

void Planet:: calcGst (void)
{
	const double Pi = M_PI;
	const double SecondsPerDay = 24.0 * 3600.0;
	
	double jd;
	tf:: convertMjdToJd (&jd, epochMjd_);
	
	double tjd;
	tf:: convertJdToTjd (&tjd, jd);
	tjd += time_ / SecondsPerDay;

	gst_ = (0.671262 + 1.0027379094 * tjd) * 2.0 * Pi;	// http://ja.wikipedia.org/wiki/%E6%81%92%E6%98%9F%E6%99%82
	tf:: normalizeRadian (&gst_);
}

void Planet:: solveKeplerEq (double* ans) const
{
	const double SecondsPerDay = 24.0 * 3600.0;
	
	double secondsFromT0 = time_ + (epochMjd_ - t0Mjd_) * SecondsPerDay;
	double M = M0_ + n_ * secondsFromT0;

	double E;	// Eccentric anomaly
	double tmpE = M;
	
	const int Loop = 500;
	const double Res = 1.0e-9;
	
	int i = 0;
	do {
		
		E = M + e_ * sin (tmpE);
		tmpE = E;
		
		try {
			if (i > Loop) {
				throw ("[ERROR] Cannot solve Kepler eq.");
			}
		}
		catch (char const* e) {
			cout << e << endl;
			
			*ans = 0.0;
			return;
		}
		
		i++;
	} while (abs (E - tmpE) > Res);
	
	*ans = E;
}

int Planet:: planetNameIsValid (void) const
{
	try {
		if (planet_ == Invalid_) {
			throw ("[ERROR] Specified planet name is not valid");
		}
		else if (planet_ == NotDefined_) {
			throw ("[ERROR] Planet name is not defined");
		}
	} 
	catch (char const* e) {
		cout << e << endl;
		cout << "[ERROR] Use setPlanetName method with a valid planet name" << endl;
		
		return 0;
	}
	
	return 1;
}