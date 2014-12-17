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
#include "pass.h"
#include "tf.h"

#include <iostream>
#include <fstream>
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
	
	// [1] init the departure time (necessary to get DESPATCH phase)
	const double DepartureMjd = 56994.264074;//56994.264023;   //2014/12/3
	tracker.setDepartureTime (DepartureMjd);
	// ---end of [1]
	
	// [2] init spacecraft orbit and parameters, observer geo-coord
	DespatchTracker:: SCDRec scd;// SpaceCraft Description
	scd.orbitInfo.epochMjd = DepartureMjd;
	scd.orbitInfo.positionEci[0] = 10697006.0;
	scd.orbitInfo.positionEci[1] =  1094554.0;
	scd.orbitInfo.positionEci[2] = -1056032.0;
	scd.orbitInfo.velocityEci[0] =  5395.222;
	scd.orbitInfo.velocityEci[1] =  6937.623;
	scd.orbitInfo.velocityEci[2] = -4193.223;
	scd.param.ballisticCoeff = 150.0;
	tracker.setSpacecraftInfo (scd);
    
    { // added by HORIGUCHI J.
        DespatchTracker::SerializedSCDRec txt;
        tracker.getSpacecraftInfo(&txt);
        ofstream ofs("despatch.scd");
        ofs << "DESPATCH" << endl;
        ofs << "SCD00001" << txt.info << endl;
        ofs << txt.param << endl;
    }
    
    /*
    DespatchTracker:: SerializedSCDRec sscd =
    {
        "despatch",
        "SCD1-DESPATCH-02SCD1-DESPATCH-02SCD1-DESPATCH-02SCD1-DESPATCH-02_ 56991.265921 10731986.0 669676.0 -1056032.0 5665.982 6718.310 -4193.223",
        "150.0"
    };
    tracker.setSpacecraftInfo (sscd);
    */
	tracker.setObserverGeoCoord (DEG_TO_RAD(atof (argv[1])), DEG_TO_RAD(atof (argv[2])), atof (argv[3]));
	// ---end of [2]
	
	double unixtime;
	double elevation, azimuth, doppler_down, doppler_up, distance;
	double latitude, longitude, altitude;
	double declination, rightascension;
	string phase;
	
	double unixtime_s = atof (argv[4]);
	double unixtime_e;
	argc >5 ? unixtime_e = atof (argv[5]) : unixtime_e = unixtime_s;
	
	// [3] set time at first, then get values
	const double outputDt = 3600.0;
	double t = unixtime_s;
    
	const double TxFrequency = 437.325e6f;
    
	cout << endl;
	cout << "----- Results -----" << endl;
	cout << "unixtime, elevation[deg], azimuth[deg], frequency[Hz], distance[m], latitude[deg], longitude[deg], altitude[m], declination[deg], right ascension[deg], phase" << endl;
	
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
		tracker.getDopplerRatio (&doppler_down, &doppler_up);
		tracker.getDistanceEarthCentered (&distance);
		tracker.getSpacecraftGeoCoord (&latitude, &longitude, &altitude);
		tracker.getGeometryEarthCentered (&declination, &rightascension);
		tracker.getDespatchPhase (&phase);
	    
        //if (elevation > 0.0) {
		cout << setprecision (10);
		cout << unixtime << ",";
		cout << RAD_TO_DEG(elevation) << "," << RAD_TO_DEG(azimuth) << ",";
		cout << doppler_down * TxFrequency << ",";
		cout << distance << ",";
		cout << RAD_TO_DEG(latitude) << "," << RAD_TO_DEG(longitude) << ",";
		cout << altitude << ",";
		cout << RAD_TO_DEG(declination) << "," << RAD_TO_DEG(rightascension) << ",";
		cout << phase;
		cout << endl;
        //}
        
		t += outputDt;
	}
	
	// revert spacecraft position and velocity to the ones of the epoch (for next iteration)
	tracker.resetSpacecraftState ();
	
	// ---end of [3]

	// [4] find next pass

	PassFinder finder;
    
    double departureUnixtime;
    tf:: convertMjdToUnixtime (&departureUnixtime, DepartureMjd);
    finder.setDepartureTime (departureUnixtime);
    
	vector<Pass> passes = finder.findAll(&tracker, unixtime_s, unixtime_e, 60);

	cout << endl;
	cout << "----- Results -----" << endl;
	cout << "AOS, LOS, TCA" << endl;
	for (vector<Pass>::iterator it = passes.begin(); it != passes.end(); ++it) {
		cout << it->AOS << "," << it->LOS << "," << it->TCA << endl;
	}
	cout << endl;

	// ---end of [4]
    
    /*
    // [5] calculate spacecraft position in ICRF frame
    
    const double SecondsPerDay = 3600.0 * 24.0;
    
    double pos_sc[3], vel_sc[3];
    double pos_earth[3], vel_earth[3];
    
    tracker.resetSpacecraftState ();
    tf:: convertMjdToUnixtime (&unixtime_s, DepartureMjd);
    
    cout << endl;
	cout << "----- Results -----" << endl;
	cout << "unixtime, x_sc[km], y_sc[km], z_sc[km], x_e[km], y_e[km], z_e[km], u_sc[m/s], v_sc[m/s], w_sc[m/s], u_e[m/s], v_e[m/s], w_e[m/s], distance[km]" << endl;
	
    t = unixtime_s;
    unixtime_e = unixtime_s + 400.0 * SecondsPerDay; // 400 Days
    
	while (1) {
		if (t > unixtime_e) {
			cout << "finished" << endl;
			break;
		}
		
		if (tracker.setTargetTime (t) != 0) {
			cout << "Range error, exit." << endl;
			break;
		}
		
		tracker.getSpacecraftState (pos_sc, vel_sc);
        tracker.getEarthPosSci (pos_earth);
        tracker.getEarthVelSci (vel_earth);
        tracker.getDistanceEarthCentered (&distance);
	    
		cout << setprecision (10);
		cout << t << ",";
		cout << pos_sc[0] / 1000.0 << "," << pos_sc[1] / 1000.0 << "," << pos_sc[2] / 1000.0 << ",";
        cout << pos_earth[0] / 1000.0 << "," << pos_earth[1] / 1000.0 << "," << pos_earth[2] / 1000.0 << ",";
        cout << vel_sc[0] << "," << vel_sc[1] << "," << vel_sc[2] << ",";
        cout << vel_earth[0] << "," << vel_earth[1] << "," << vel_earth[2] << ",";
        cout << distance / 1000.0;
		cout << endl;
        
		t += SecondsPerDay;
	}
    
    // --end of [5]
    */
	
	return 0;
}
