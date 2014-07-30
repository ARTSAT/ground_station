/********************************************************************************
* File Name : tracker.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.7.29
* Language  : C++
*********************************************************************************
* class to calculate the direction and doppler-shift of the spacecraft at a 
* specific observation location
*
********************************************************************************/

#include "tracker.h"


const double SpacecraftTracker:: Pi = 3.14159265359;

const double SpacecraftTracker:: PeriodNearEarth = 24.0 * 3600.0;
const double SpacecraftTracker:: DtNearEarth = 60.0;
const double SpacecraftTracker:: DtFarEarth  = 3600.0;


SpacecraftTracker:: SpacecraftTracker (char const* fileName)
{
	// read the spacecraft's orbit information file
	if (!readOrbitInfo (fileName)) {
		exit (1);
	}
	departureUnixtime_ = mjdToUnixtime (departureMjd_);
	
	x_ = VectorXd:: Zero (6);
}


void SpacecraftTracker:: init (void) 
{
	earth_.init ("earth", departureMjd_);
	descal_.init (x_, initialPos_, initialVel_, earth_);
}


void SpacecraftTracker:: getOrbitInfo (char* scId, char* orbitId, double& departureMjd, double* posEcenterd, double* velEcenterd)
{
	strcpy (scId, scId_);
	strcpy (orbitId, orbitId_);
	
	departureMjd = departureMjd_;
	
	for (int i = 0; i < 3; i++) {
		posEcenterd[i] = initialPos_[i];
		velEcenterd[i] = initialVel_[i];
	}
}


double SpacecraftTracker:: getDepartureUnixtime (void)
{
	return departureUnixtime_;
}


int SpacecraftTracker:: getTarget (double unixtime, double lat, double lon, double alt, 
					   			   double& ele, double& azi, double& freq, double& distance, int& mode)
{
	double dt = unixtime - departureUnixtime_;
	if (dt < 0.0) {
		printf ("[ERROR] obs_unixtime must be bigger than departure in unixtime\n");
		printf ("[ERROR] departure in unixtime is: %9.2lf\n", departureUnixtime_);
		exit (1);
	}
	
	init ();
	
	integrate (0.0, dt);
	
	calcGeometry (lat, lon, alt, ele, azi, freq);
	distance = descal_.distanceEarthCenterd (x_, earth_);
	mode = descal_.mode (dt);
	
	return 1;
}


int SpacecraftTracker:: getTarget (double unixtime_s, double unixtime_e, double lat, double lon, double alt, 
								   vector<double>& unixtime, vector<double>& ele, vector<double>& azi, vector<double>& freq, 
								   vector<double>& distance, vector<int>& mode, double obs_dt)
{
	double dt1 = unixtime_e - departureUnixtime_;
	if (dt1 < 0.0) {
		printf ("[ERROR] obs_unixtime_end must be bigger than departure in unixtime\n");
		printf ("[ERROR] departure in unixtime is: %9.2lf\n", departureUnixtime_);
		exit (1);
	}
	
	double dt0 = unixtime_s - departureUnixtime_;
	if (dt0 < 0.0) {
		printf ("[ERROR] obs_unixtime_start must be bigger than departure in unixtime\n");
		printf ("[ERROR] departure in unixtime is: %9.2lf\n", departureUnixtime_);
		exit (1);
	}
	
	double t = 0.0;
	double dt;
	
	double ele_tmp, azi_tmp, freq_tmp, distance_tmp;
	int mode_tmp;
	
	init ();
	unixtime.clear (); ele.clear (); azi.clear (); freq.clear ();
	distance.clear (); mode.clear ();
	
	while (1) {
		
		if (t == 0.0) {
			dt = dt0;
		}
		else {
			dt = obs_dt;
		}
		
		if (t + dt > dt1) { break; }
		
		integrate (t, dt);
		t += dt;
		
		calcGeometry (lat, lon, alt, ele_tmp, azi_tmp, freq_tmp);
		distance_tmp = descal_.distanceEarthCenterd (x_, earth_);
		mode_tmp = descal_.mode (t);
		
		unixtime.push_back (t + departureUnixtime_);
		ele.push_back (ele_tmp); azi.push_back (azi_tmp); freq.push_back (freq_tmp);
		distance.push_back (distance_tmp); mode.push_back (mode_tmp); 
	};
	
	return unixtime.size ();
}


int SpacecraftTracker:: readOrbitInfo (char const* fileName)
{
	FILE *fp;
	
	fp = fopen (fileName, "r");
	if (fp == NULL) {
		printf ("[Error] Cannot open %s\n", fileName);
		return 0;
	}
	
	const int X = 0, Y = 1, Z = 2;
	
	fscanf (fp, "%[^,],%s", scId_, orbitId_);
	fscanf (fp, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", 
			&departureMjd_, &initialPos_[X], &initialPos_[Y], &initialPos_[Z], &initialVel_[X], &initialVel_[Y], &initialVel_[Z]);
	
	fclose (fp);
	
	return 1;
}


double SpacecraftTracker:: mjdToUnixtime (double mjd)
{
	double utime;
	const double mjdAtUnixtimeOrigin = 40587.0;
	
	utime = (mjd - mjdAtUnixtimeOrigin) * 24.0 * 3600.0;
	
	return utime;
}


void SpacecraftTracker:: integrate (double t0, double duration)
{
	double dt;
	double t = 0.0;
	
	while (1) {
		
		if (t0 < PeriodNearEarth) {
			dt = DtNearEarth;
		}
		else {
			dt = DtFarEarth;
		}
		
		if ((duration - t) < dt) {break;}
		
		t += dt;
		t0 += dt;
		
		earth_.calcOrbitAt (t0);
		descal_.integrate (dt, x_, earth_);
	};
	
	earth_.calcOrbitAt (t0 + (duration - t));
	descal_.integrate (duration - t, x_, earth_);
}


void SpacecraftTracker:: calcGeometry (double lat, double lon, double alt, double &ele, double &azi, double &freq)
{
	double gsLocation[3];
	gsLocation[0] = lon * Pi / 180.0;
	gsLocation[1] = lat * Pi / 180.0;
	gsLocation[2] = alt;
	
	Vector3d geometry;
	gscal_.calcGeometry (geometry, gsLocation, x_, earth_, 0);
	
	ele = geometry[1] * 180.0 / Pi;
	azi = geometry[0] * 180.0 / Pi;
	freq = SpacecraftCalculator::ScRadioFreq - geometry[2];
}
