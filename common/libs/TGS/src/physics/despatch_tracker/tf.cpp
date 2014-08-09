/********************************************************************************
* File Name : tf.cpp
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to transform the coordinate
*
********************************************************************************/
#include "tf.h"

void tf:: calcDcm (Matrix3d* ans, int axis, double angle)
{
	const int X = 0, Y = 1, Z = 2;

	switch (axis) {
		
		case X:
			*ans <<
				1.0,          0.0,         0.0,
				0.0,  cos (angle), sin (angle),
				0.0, -sin (angle), cos (angle);
			break;
		
		case Y:
			*ans <<
				cos (angle), 0.0, -sin (angle),
				        0.0, 1.0,          0.0,
				sin (angle), 0.0,  cos (angle);
			break;
			
		case Z:
			*ans << 
				 cos (angle), sin (angle), 0.0,
				-sin (angle), cos (angle), 0.0,
				         0.0,         0.0, 1.0;
			break;
		
		default:
			*ans = Matrix3d:: Identity ();
			break;
	}
}

void tf:: normalizeRadian (double* radian, double startPointRadian)
{
	const double TwoPi = 2.0 * M_PI;
	
	double rad = *radian;
	
	if (rad < 0.0 || rad > TwoPi) {
		
		int div = static_cast<int> ( (rad - startPointRadian) / TwoPi);
		rad -= static_cast<double> (div) * TwoPi;
		
		if (rad < startPointRadian) {
			rad += TwoPi;
		}
	}
	
	*radian = rad;
	
	// startPointRadian <= rad < startPointRadian + 2.0 Pi
}

void tf:: diffRadians (double *ans, double rad1, double rad2)
{
	const double Pi = M_PI;
	
	double diff = rad1 - rad2;
	normalizeRadian (&diff);
	
	if (diff > Pi) {
		diff -= 2.0 * Pi;
	}
	
	*ans = diff;
}

void tf:: convertMjdToJd (double* julianDay, double modifiedJd)
{
	*julianDay = modifiedJd + 2400000.5;
}

void tf:: convertJdToTjd (double* truncatedJd, double julianDay)
{
	*truncatedJd = julianDay - 2440000.5;
}

void tf:: convertMjdToUnixtime (double* unixtime, double modifiedJd)
{
	const double mjdAtUnixtimeOrigin = 40587.0;
	const double secondsDay = 24.0 * 3600.0;
	
	*unixtime = (modifiedJd - mjdAtUnixtimeOrigin) * secondsDay;
}

