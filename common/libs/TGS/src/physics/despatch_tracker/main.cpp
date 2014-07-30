#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
using namespace std;

#include "tracker.h"

/***************************************************
Cammand Line Auguments:
 
arg[1]: orbitinfo_filename
arg[2]: obs_unixtime[sec]
arg[3]: obs_latitude [deg]
arg[4]: obs_longitude [deg]
arg[5]: obs_altitude [m]

OR

arg[1]: orbitinfo_filename
arg[2]: obs_unixtime_start[sec]
arg[3]: obs_unixtime_end[sec]
arg[4]: obs_latitude [deg]
arg[5]: obs_longitude [deg]
arg[6]: obs_altitude [m]
arg[7]: dt [sec] (optional, default:60)

****************************************************/

void testPointSearch (SpacecraftTracker sc_tracker, int argc, char* argv[]);
void testRangeSearch (SpacecraftTracker sc_tracker, int argc, char* argv[]);

int argIsValid (int argc, char* argv[]);

void showOrbitInfo (char* scId, char* orbitId, double departureMjd, double initPos[3], double initVel[3]);

int main (int argc, char* argv[])
{
	// check the command line auguments (if necessary)
	if (!argIsValid (argc, argv)) {
		return 1;
	}
	
	
	// [1] init spacecraft orbit using orbit information file
	SpacecraftTracker sc_tracker (argv[1]);
	// ----- end of [1]
	
	
	// check the spacecraft orbit configuration (if necessary)
	char scId[64], orbitId[64];
	double departureMjd, initPos[3], initVel[3];
	sc_tracker.getOrbitInfo (scId, orbitId, departureMjd, initPos, initVel);
	showOrbitInfo (scId, orbitId, departureMjd, initPos, initVel);
	
	
	// [2] track the spacecraft
	
	// (2.a) case of Point search
	if (argc == 6) { testPointSearch (sc_tracker, argc, argv); }
	
	// (2.b) case of Range search
	if (argc == 7 || argc == 8) { testRangeSearch (sc_tracker, argc, argv); }
	
	// ----- end of [2]
	
	return 0;
}


void testPointSearch (SpacecraftTracker sc_tracker, int argc, char* argv[])
{
	double unixtime;
	double latitude, longitude, altitude;
	
	double ele, azi, freq, distance;
	int mode;
	
	unixtime = atof (argv[2]);
	latitude = atof (argv[3]);
	longitude = atof (argv[4]);
	altitude = atof (argv[5]);
	
	sc_tracker.getTarget (unixtime, latitude, longitude, altitude,
					      ele, azi, freq, distance, mode);
	
	printf ("\n");
	printf ("----- Point Search Results -----\n");
	printf ("unixtime, elevation[deg], azimuth[deg], frequency[Hz], distance[m], mode\n");
	printf ("%.2lf, %.1lf, %.1lf, %.0lf, %.0lf, %d\n", unixtime, ele, azi, freq, distance, mode);
	printf ("\n");
}


void testRangeSearch (SpacecraftTracker sc_tracker, int argc, char* argv[])
{
	double unixtime_s, unixtime_e, dt;
	double latitude, longitude, altitude;
	
	vector<double> unixtime, ele, azi, freq, distance;
	vector<int> mode;
	
	unixtime_s = atof (argv[2]);
	unixtime_e = atof (argv[3]);
	latitude = atof (argv[4]);
	longitude = atof (argv[5]);
	altitude = atof (argv[6]);
	
	switch (argc) {
	
	case 7:
		sc_tracker.getTarget (unixtime_s, unixtime_e, latitude, longitude, altitude,
					          unixtime, ele, azi, freq, distance, mode);
		break;
		
	case 8:
		dt = atof (argv[7]);
		sc_tracker.getTarget (unixtime_s, unixtime_e, latitude, longitude, altitude,
					          unixtime, ele, azi, freq, distance, mode, dt);
		break;
	
	default: 
		break;
	}
	
	printf ("\n");
	printf ("----- Range Search Results -----\n");
	printf ("unixtime, elevation[deg], azimuth[deg], frequency[Hz], distance[m], mode\n");
	for (int i = 0; i < unixtime.size (); i++) {
		printf ("%.2lf, %.1lf, %.1lf, %.0lf, %.0lf, %d\n", unixtime[i], ele[i], azi[i], freq[i], distance[i], mode[i]);
	}
	printf ("\n");
}


int argIsValid (int argc, char* argv[])
{

	switch (argc) {
		
	case 6:
		cout << endl;
		cout << "----- Command Line Auguments -----" << endl;
		cout << "orbitinfo_filename: " << argv[1] << endl;
		cout << "obs_unixtime: " << argv[2] << endl;
		cout << "obs_latitude: " << argv[3] << endl;
		cout << "obs_longitude: " << argv[4] << endl;
		cout << "obs_altitude: " << argv[5] << endl;
		break;
		
	case 7:
		cout << endl;
		cout << "----- Command Line Auguments -----" << endl;
		cout << "orbitinfo_filename: " << argv[1] << endl;
		cout << "obs_unixtime_start: " << argv[2] << endl;
		cout << "obs_unixtime_end: " << argv[3] << endl;
		cout << "obs_latitude: " << argv[4] << endl;
		cout << "obs_longitude: " << argv[5] << endl;
		cout << "obs_altitude: " << argv[6] << endl;
		break;
	
	case 8:
		cout << endl;
		cout << "----- Command Line Auguments -----" << endl;
		cout << "orbitinfo_filename: " << argv[1] << endl;
		cout << "obs_unixtime_start: " << argv[2] << endl;
		cout << "obs_unixtime_end: " << argv[3] << endl;
		cout << "obs_latitude: " << argv[4] << endl;
		cout << "obs_longitude: " << argv[5] << endl;
		cout << "obs_altitude: " << argv[6] << endl;
		cout << "dt: " << argv[7] << endl;
		break;
		
	default: 
		cout << "[Error] Please input auguments." << endl;
	
		cout << "-----" << endl;
		cout << "arg[1]: orbitinfo_filename" << endl;
		cout << "arg[2]: obs_unixtime[sec]" << endl;
		cout << "arg[3]: obs_latitude [deg]" << endl;
		cout << "arg[4]: obs_longitude [deg]" << endl;
		cout << "arg[5]: obs_altitude [m]" << endl;
		
		cout << "---or---" << endl;
		cout << "arg[1]: orbitinfo_filename" << endl;
		cout << "arg[2]: obs_unixtime_start[sec]" << endl;
		cout << "arg[3]: obs_unixtime_end[sec]" << endl;
		cout << "arg[4]: obs_latitude [deg]" << endl;
		cout << "arg[5]: obs_longitude [deg]" << endl;
		cout << "arg[6]: obs_altitude [m]" << endl;
		cout << "arg[7]: dt [sec] (optional, default:60)" << endl;
		return 0;
	}		
	
	return 1;
}


void showOrbitInfo (char* scId, char* orbitId, double departureMjd, double initPos[3], double initVel[3])
{	
	printf ("\n");
	printf ("----- Orbit Information -----\n");
	printf ("Spacecraft id: %s\n", scId);
	printf ("Orbit id: %s\n", orbitId);
	printf ("Departure MJD: %.6lf\n", departureMjd);
	printf ("Initial Position: %.1lf, %.1lf, %.1lf\n", initPos[0], initPos[1], initPos[2]);
	printf ("Initial Velocity: %.3lf, %.3lf, %.3lf\n", initVel[0], initVel[1], initVel[2]);
}
