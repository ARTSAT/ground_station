/********************************************************************************
* File Name : despatch.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.7
* Language  : C++
*********************************************************************************
* class to calculate the phase of DESPATCH
*
********************************************************************************/
#include "despatch.h"

const double DespatchTracker:: SecondsMorseEnd_ = 9.0 * 3600.0;
const double DespatchTracker:: SecondsBaudotEnd_ = 108.0 * 3600.0;
const double DespatchTracker:: SecondsBeaconEnd_ = (10.0 * 24.0) * 3600.0;

DespatchTracker:: DespatchTracker (void) : SpacecraftTracker (), departureMjd_ (0.0)
{
	
}

DespatchTracker:: DespatchTracker (double departureMjd, SCDRec const& scd) : SpacecraftTracker (scd), departureMjd_ (departureMjd)
{
	
}

DespatchTracker:: ~DespatchTracker (void)
{
	
}

void DespatchTracker:: setDepartureTime (double departureMjd)
{
	departureMjd_ = departureMjd;
}

void DespatchTracker:: getDepartureTime (double* departureMjd) const
{
	*departureMjd = departureMjd_;
}

int DespatchTracker:: setTargetTime (double unixtime) 
{
	double departureUnixtime;
	tf:: convertMjdToUnixtime (&departureUnixtime, departureMjd_);
	
	if (unixtime < departureUnixtime) {
		cout << setprecision (10);
		cout << "[ERROR] Specified unixtime (" << unixtime << ") is too small" << endl;
		cout << "[ERROR] Unixtime must be bigger than one at departure: " << departureUnixtime << endl;
		return -1;
		
	}
	else if (unixtime > departureUnixtime + SecondsBeaconEnd_) {
		cout << setprecision (10);
		cout << "[ERROR] Specified unixtime (" << unixtime << ") is too big" << endl;
		cout << "[ERROR] Unixtime must be smaller than one at transmitter-off: " << departureUnixtime + SecondsBeaconEnd_ << endl;
		return 1;
	}
	else;
	
	return SpacecraftTracker:: setTargetTime (unixtime);
}

void DespatchTracker:: getDespatchPhase (string* phase) const
{	
	double secondsFromDeparture;
	calcSecondsFromDeparture (&secondsFromDeparture);
	
	if (secondsFromDeparture > SecondsBeaconEnd_) {
		*phase = "radio_stop";
	}
	else if (secondsFromDeparture > SecondsBaudotEnd_) {
		*phase = "3";
	}
	else if (secondsFromDeparture > SecondsMorseEnd_) {
		*phase = "2";
	}
	else if (secondsFromDeparture > 0.0) {
		*phase = "1";
	}
	else {
		*phase = "not_defined";
	}
}

void DespatchTracker:: test (double unixtime_s, double unixtime_e, double outputDt)
{
	if (unixtime_s > unixtime_e) {
		cout << "[ERROR] unixtime_e must be bigger than unixtime_s" << endl;
		return;
	}
	
	if (outputDt <= 0.0) {
		cout << "[ERROR] outputDt must bigger than zero." << endl;
		return;
	}
	
	// set departure time
	const double DepartureMjd = 56992.264444;
	setDepartureTime (DepartureMjd);
	
	// set spacecraft orbit information and parameter
	SCDRec scd;
	scd.orbitInfo.epochMjd = DepartureMjd;
	scd.orbitInfo.positionEci[0] = 10718921.0;
	scd.orbitInfo.positionEci[1] =   747344.0;
	scd.orbitInfo.positionEci[2] = -1050332.0;
	scd.orbitInfo.velocityEci[0] =  5616.853;
	scd.orbitInfo.velocityEci[1] =  6764.471;
	scd.orbitInfo.velocityEci[2] = -4193.746;
	scd.param.ballisticCoeff = 150.0;
	setSpacecraftInfo (scd);
	
	// variables
	double distance;
	string phase;
	
	double utime = unixtime_s;
	
	cout << "unixtime, distance, phase" << endl;
	do {
		setTargetTime (utime);
		
		getDistanceEarthCentered (&distance);
		getDespatchPhase (&phase);
		
		cout << setprecision (10);
		cout << utime << ",";
		cout << distance << ",";
		cout << phase;
		cout << endl;
		
		utime += outputDt;
		
	} while (utime < unixtime_e);
}

void DespatchTracker:: calcSecondsFromDeparture (double* secondsFromDeparture) const
{
	double epochMjd;
	getEpochTime (&epochMjd);
	double epochUnixtime;
	tf:: convertMjdToUnixtime (&epochUnixtime, epochMjd);
	
	double departureUnixtime;
	tf:: convertMjdToUnixtime (&departureUnixtime, departureMjd_);
	
	double secondsFromEpoch;
	getSecondsFromEpoch (&secondsFromEpoch);
	
	*secondsFromDeparture = (epochUnixtime - departureUnixtime) + secondsFromEpoch;
}