/********************************************************************************
* File Name : stochastic.h
* Title     : 
* Programmer: Motoki Kimura
* Belonging : 
* Date      : 2014.2.22
* Language  : C++
*********************************************************************************
* class to calculate stochastic process
*
********************************************************************************/
#ifndef	STOCHASTIC_H_
#define	STOCHASTIC_H_


#include "stochastic.h"

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

	
class Stochastic
{
	public:
		Stochastic (void);					// constructor
		double zeroTo (double n);			// uniform randoms [0.0, n]
		double plusMinus (double n_mux);	// uniform randoms [-n_max, +n_max]
		double whiteNoise (double dev=1.0);	// white noise N (0.0, dev)

	private:

};

#endif
