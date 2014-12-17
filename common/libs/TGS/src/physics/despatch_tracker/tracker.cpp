/********************************************************************************
* File Name : tracker.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to calculate the position & velocity of the spacecraft in geo coordinate
*
********************************************************************************/
#include "tracker.h"


SpacecraftTracker:: SpacecraftTracker (void) : SpacecraftCalculator (), unixtime_ (0.0)
{
	
}

SpacecraftTracker:: SpacecraftTracker (SCDRec const& scd) : SpacecraftCalculator (), unixtime_ (0.0)
{
	setSpacecraftInfo (scd);
}

SpacecraftTracker:: SpacecraftTracker (SerializedSCDRec const& scd) : SpacecraftCalculator (), unixtime_ (0.0)
{
    setSpacecraftInfo (scd);
}

SpacecraftTracker:: ~SpacecraftTracker (void)
{
	
}

int SpacecraftTracker:: setSpacecraftInfo (SCDRec const& scd)
{
	scd_ = scd;
	
	setSpacecraftOrbitInfo (scd_.orbitInfo.epochMjd, scd_.orbitInfo.positionEci, scd_.orbitInfo.velocityEci);
	setSpacecraftParams (scd_.param.ballisticCoeff);
    
    return 0;
}

int SpacecraftTracker:: setSpacecraftInfo (SerializedSCDRec const& scd)
{
    SCDRec temp;
    if (!convert (scd, &temp)) {
        cout << "[ERROR] Specified SCD is invalid." << endl;
        return 1;
    }
    
    scd_ = temp;
    setSpacecraftOrbitInfo (scd_.orbitInfo.epochMjd, scd_.orbitInfo.positionEci, scd_.orbitInfo.velocityEci);
	setSpacecraftParams (scd_.param.ballisticCoeff);
    
    return 0;
}

void SpacecraftTracker:: getSpacecraftInfo (SCDRec *scd) const
{
	*scd = scd_;
}

void SpacecraftTracker:: getSpacecraftInfo (SerializedSCDRec *scd) const
{
    convert(scd_, scd);
}

int SpacecraftTracker:: setTargetTime (double unixtime)
{
	unixtime_ = unixtime;
	
	updateSpacecraftState ();
	
	return 0;
}

void SpacecraftTracker:: getTargetTime (double* unixtime) const
{
	*unixtime = unixtime_;
}

void SpacecraftTracker:: test (double unixtime_s, double unixtime_e, double outputDt)
{
	if (unixtime_s > unixtime_e) {
		cout << "[ERROR] unixtime_e must be bigger than unixtime_s" << endl;
		return;
	}
	
	if (outputDt <= 0.0) {
		cout << "[ERROR] outputDt must bigger than zero." << endl;
		return;
	}
	
	// set spacecraft orbit information and parameter
	SCDRec scd;
	scd.orbitInfo.epochMjd = 56992.264444;
	scd.orbitInfo.positionEci[0] = 10718921.0;
	scd.orbitInfo.positionEci[1] =   747344.0;
	scd.orbitInfo.positionEci[2] = -1050332.0;
	scd.orbitInfo.velocityEci[0] =  5616.853;
	scd.orbitInfo.velocityEci[1] =  6764.471;
	scd.orbitInfo.velocityEci[2] = -4193.746;
	scd.param.ballisticCoeff = 150.0;
	setSpacecraftInfo (scd);
	
	// variables
	double declination, ra, doppler_down, doppler_up, distance, speed;
	
	double utime = unixtime_s;
	
    const double Frequency = 437.325e6f;
    
	cout << "unixtime, x, y, z, u, v, w, declination, RA, doppler, distance, speed" << endl;
	do {
		setTargetTime (utime);
		
		getGeometryEarthCentered (&declination, &ra);
		getDopplerRatioEarthCentered (&doppler_down, &doppler_up);
		getDistanceEarthCentered (&distance);
		getSpacecraftRelativeSpeed (&speed);
		
		cout << setprecision (10);
		cout << utime << ",";
		cout << declination << "," << ra << ",";
		cout << (doppler_down - 1.0) * Frequency << ",";
		cout << distance << ",";
		cout << speed;
		cout << endl;
		
		utime += outputDt;
		
	} while (utime < unixtime_e);
}

void SpacecraftTracker:: updateSpacecraftState (void)
{
	double period;
	calcIntegrationPeriod (&period);
    
    double sgn = 1.0;
    if (period < 0.0) {
        sgn = -1.0;
    }
    
    double distance;
    const double RangeNearEarth = 100000000.0;
    
    double dt;
    const double DtNearEarth = 1.0;
    const double DtFarFromEarth = 1.0;
	
	double t = 0.0;
	
    while (1) {
        
		getDistanceEarthCentered (&distance);
        if (distance < RangeNearEarth) {
            dt = sgn * DtNearEarth;   // dt should be small enough when the spacecraft is near Earth for the accuracy
        }
        else {
            dt  = sgn * DtFarFromEarth;
        }
    
		if (abs (t + dt) > abs (period)) {
			break;
		}
        
		integrateSpacecraftState (dt);
		t += dt;
	};
	integrateSpacecraftState (period - t);
}

void SpacecraftTracker:: calcIntegrationPeriod (double* period) const
{
	double epochMjd;
	getEpochTime (&epochMjd);
	double epochUnixtime;
	tf:: convertMjdToUnixtime (&epochUnixtime, epochMjd);
	
	double secondsFromEpoch;
	getSecondsFromEpoch (&secondsFromEpoch);
	
	double tFinal = unixtime_ - epochUnixtime;
	if (tFinal < 0.0) {
		cout << "[WARNING] Specified unixtime is smaller than the unixtime of the epoch" << endl;
		cout << "[WARNING] Specified unixtime: " << unixtime_ << ", Unixtime of the epoch: " << epochUnixtime << endl;
	}
	
	*period = tFinal - secondsFromEpoch;	// period of the numerical integration
}

bool SpacecraftTracker:: convert(SerializedSCDRec const& in, SCDRec* out)
{
    SpacecraftTracker::SCDRec temp;
    std::string parse;
    size_t index;
    int i;
    bool result(false);
    
    if (out != NULL) {
        strncpy(temp.scId, in.name, sizeof(temp.scId));
        parse = in.info;
        if ((index = parse.find(' ')) > 0) {
            parse.copy(temp.orbitInfo.id, (index < sizeof(temp.orbitInfo.id)) ? (index) : (sizeof(temp.orbitInfo.id)));
            parse.erase(0, index + 1);
            if ((index = parse.find(' ')) > 0) {
                if (toDouble(parse.c_str(), ' ', &temp.orbitInfo.epochMjd)) {
                    parse.erase(0, index + 1);
                    for (i = 0; i < 3; ++i) {
                        if ((index = parse.find(' ')) > 0) {
                            if (toDouble(parse.c_str(), ' ', &temp.orbitInfo.positionEci[i])) {
                                parse.erase(0, index + 1);
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (i >= 3) {
                        for (i = 0; i < 2; ++i) {
                            if ((index = parse.find(' ')) > 0) {
                                if (toDouble(parse.c_str(), ' ', &temp.orbitInfo.velocityEci[i])) {
                                    parse.erase(0, index + 1);
                                }
                                else {
                                    break;
                                }
                            }
                            else {
                                break;
                            }
                        }
                        if (i >= 2) {
                            if (parse.find(' ') == std::string::npos) {
                                if (toDouble(parse.c_str(), '\0', &temp.orbitInfo.velocityEci[i])) {
                                    parse = in.param;
                                    if (parse.find(' ') == std::string::npos) {
                                        if (toDouble(parse.c_str(), '\0', &temp.param.ballisticCoeff)) {
                                            *out = temp;
                                            result = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

bool SpacecraftTracker:: convert(SCDRec const& in, SerializedSCDRec* out)
{
    SerializedSCDRec temp;
    char buffer[256];
    int size;
    bool result(false);
    
    if (out != NULL) {
        strncpy(temp.name, in.scId, sizeof(temp.name));
        size = snprintf(buffer, sizeof(buffer), "%.64s %.8f %.8f %.8f %.8f %.8f %.8f %.8f",
                        in.orbitInfo.id,
                        in.orbitInfo.epochMjd,
                        in.orbitInfo.positionEci[0], in.orbitInfo.positionEci[1], in.orbitInfo.positionEci[2],
                        in.orbitInfo.velocityEci[0], in.orbitInfo.velocityEci[1], in.orbitInfo.velocityEci[2]);
        if (0 < size && size < sizeof(buffer)) {
            temp.info = buffer;
            size = snprintf(buffer, sizeof(buffer), "%.8f", in.param.ballisticCoeff);
            if (0 < size && size < sizeof(buffer)) {
                temp.param = buffer;
                *out = temp;
                result = true;
            }
        }
    }
    return result;
}

bool SpacecraftTracker:: toDouble(char const* start, char end, double* value)
{
    double temp;
    char* error;
    bool result(false);
    
    temp = strtod(start, &error);
    if (*error == end) {
        if (!(temp == HUGE_VAL || (temp == 0 && errno == ERANGE))) {
            *value = temp;
            result = true;
        }
    }
    return result;
}
