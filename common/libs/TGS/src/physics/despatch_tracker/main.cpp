/***************************************************
Cammand Line Auguments:
 
arg[1]: latitude  [deg]
arg[2]: longitude [deg]
arg[3]: altitude  [m]
arg[4]: unixtime  [sec]

OR

arg[1]: latitude       [deg]
arg[2]: longitude      [deg]
arg[3]: altitude       [m]
arg[4]: unixtime_start [sec]
arg[5]: unixtime_end   [sec]

****************************************************/
#include "despatch.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
using namespace std;

#define	DEG_TO_RAD(deg)	deg*M_PI/180.0
#define RAD_TO_DEG(rad)	rad*180.0/M_PI

int argIsValid (int argc, char* argv[])
{
	switch (argc) {
		
	case 5:
		cout << endl;
		cout << "----- Command Line Auguments -----" << endl;
		cout << "latitude: "  << argv[1] << endl;
		cout << "longitude: " << argv[2] << endl;
		cout << "altitude: "  << argv[3] << endl;
		cout << "unixtime: "  << argv[4] << endl;
		return 1;
		
	case 6:
		cout << endl;
		cout << "----- Command Line Auguments -----" << endl;
		cout << "latitude: "       << argv[1] << endl;
		cout << "longitude: "      << argv[2] << endl;
		cout << "altitude: "       << argv[3] << endl;
		cout << "unixtime_start: " << argv[4] << endl;
		cout << "unixtime_end: "   << argv[5] << endl;
		return 1;
		
	default: 
		cout << "[Error] Please input auguments correctly" << endl;
	
		cout << "-----" << endl;
		cout << "arg[1]: latitude  [deg]" << endl;
		cout << "arg[2]: longitude [deg]" << endl;
		cout << "arg[3]: altitude  [m]"   << endl;
		cout << "arg[4]: unixtime  [sec]" << endl;
		
		cout << "---or---" << endl;
		cout << "arg[1]: latitude       [deg]" << endl;
		cout << "arg[2]: longitude      [deg]" << endl;
		cout << "arg[3]: altitude       [m]"   << endl;
		cout << "arg[4]: unixtime_start [sec]" << endl;
		cout << "arg[5]: unixtime_end   [sec]" << endl;
		break;
	}
		
	return 0;
}

int main (int argc, char* argv[])
{
	// check the command line auguments
	if (!argIsValid (argc, argv)) {
		return 1;
	}
	
	DespatchTracker tracker;	// based on :public SpacecraftTracker, :public SpacecraftCalculator
	
	// [1] init the departure time (necessary to get DESPATCH mode)
	const double DepartureMjd = 56992.264444;
	tracker.setDepartureTime (DepartureMjd);
	// ---end of [1]
	
	// [2] init spacecraft orbit and parameters, observer geo-coord
	DespatchTracker:: SCDRec scd;// SpaceCraft Description
	scd.orbitInfo.epochMjd = DepartureMjd;
	scd.orbitInfo.positionEci[0] = 10718921.0;
	scd.orbitInfo.positionEci[1] =   747344.0;
	scd.orbitInfo.positionEci[2] = -1050332.0;
	scd.orbitInfo.velocityEci[0] =  5616.853;
	scd.orbitInfo.velocityEci[1] =  6764.471;
	scd.orbitInfo.velocityEci[2] = -4193.746;
	const double TxFrequency = 437.325e6f;
	scd.param.transmitterFrequency = TxFrequency;
	scd.param.ballisticCoeff = 150.0;
	
	tracker.setSpacecraftInfo (scd);
	tracker.setObserverGeoCoord (DEG_TO_RAD(atof (argv[1])), DEG_TO_RAD(atof (argv[2])), atof (argv[3]));
	// ---end of [2]
	
	double unixtime;
	double elevation, azimuth, doppler, distance;
	double latitude, longitude, altitude;
	string mode;
	
	double unixtime_s = atof (argv[4]);
	double unixtime_e;
	argc >5 ? unixtime_e = atof (argv[5]) : unixtime_e = unixtime_s;
	
	// [3] set time at first, then get values
	const double outputDt = 3600.0;
	double t = unixtime_s;
	
	cout << endl;
	cout << "----- Results -----" << endl;
	cout << "unixtime, elevation[deg], azimuth[deg], frequency[Hz], distance[m], latitude[deg], longitude[deg], altitude[m], mode" << endl;
	
	while (1) {
		if (t > unixtime_e) {
			cout << "finished" << endl;
			break;
		}
		
		if (tracker.setTargetTime (t) != 0) {
			cout << "Range error, exit." << endl;
			break;
		}
		
		tracker.getTargetTime (&unixtime);
		tracker.getSpacecraftDirection (&elevation, &azimuth);
		tracker.getDopplerFrequency (&doppler);
		tracker.getDistanceEarthCentered (&distance);
		tracker.getSpacecraftGeoCoord (&latitude, &longitude, &altitude);
		tracker.getDespatchMode (&mode);
		
		cout << setprecision (10);
		cout << unixtime << ",";
		cout << RAD_TO_DEG(elevation) << "," << RAD_TO_DEG(azimuth) << ",";
		cout << TxFrequency + doppler << ",";
		cout << distance << ",";
		cout << RAD_TO_DEG(latitude) << "," << RAD_TO_DEG(longitude) << ",";
		cout << altitude << ",";
		cout << mode;
		cout << endl;
		
		t += outputDt;
	}
	
	// revert spacecraft position and velocity to the ones of the epoch (for next iteration)
	tracker.resetSpacecraftState ();
	
	// ---end of [3]
	
	return 0;
}