/********************************************************************************
* File Name : stochastic.cpp
* Title     : 
* Programmer: Motoki Kimura
* Belonging : 
* Date      : 2014.2.22
* Language  : C++
*********************************************************************************
* class to calculate stochastic process
*
********************************************************************************/
#include "stochastic.h"

Stochastic:: Stochastic (void)
{
	srand ((unsigned int)time (NULL));
}

double Stochastic:: zeroTo (double n)
{
	return ( (double)rand () / (double)RAND_MAX ) * n;
}

double Stochastic:: plusMinus (double n_max)
{
	return 2.0 * (zeroTo (n_max) - 0.5 * n_max);
}

double Stochastic:: whiteNoise (double dev)
{
	const double Pi =  3.14159265359;
	double rnd;	// N (0.0, 1.0)

	// Box-Muller
	double r1 = zeroTo (1.0);
	double r2 = zeroTo (1.0);
	rnd =  sqrt(-2.0 * log (r1)) * cos(2.0 * Pi * r2);

	return rnd * dev;	// N (0.0, dev)
}
