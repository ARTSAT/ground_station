/********************************************************************************
* File Name : tf.h
* Title     : 
* Programmer: Motki Kimura
* Belonging : 
* Date      : 2014.8.3
* Language  : C++
*********************************************************************************
* class to transform the coordinate
*
********************************************************************************/
#ifndef TF_H_
#define TF_H_

#include "tf.h"

#include "eigen-eigen-6b38706d90a9/eigen/Core"
using namespace Eigen;

#include <cmath>
#include <iostream>
using namespace std;

namespace tf {
	void calcDcm (Matrix3d* ans, int axis, double angle);
	
	void normalizeRadian (double* radian, double startPointRadian = 0.0);
	void diffRadians (double* ans, double rad1, double rad2);
	
	void convertMjdToJd (double* julianDay, double modifiedJd);
	void convertJdToTjd (double* truncatedJd, double julianDay);
	void convertMjdToUnixtime (double* unixtime, double modifiedJd);
};

#endif