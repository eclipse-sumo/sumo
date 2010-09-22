/****************************************************************************/
/// @file    AGDataAndStatistics.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Contains various data, statistical values and functions from input used
// by various objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "AGDataAndStatistics.h"
#include <utils/common/RandHelper.h>
#include <cmath>
#include <iomanip>
#define LIMIT_CHILDREN_NUMBER 3


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

/*int
AGDataAndStatistics::getRandom()
{
	srand(randomNbr);
	randomNbr = rand();
	return randomNbr;
}*/

// ===========================================================================
// method definitions
// ===========================================================================
AGDataAndStatistics&
AGDataAndStatistics::getDataAndStatistics() throw()
{
	static AGDataAndStatistics ds;
	return ds;
}

int
AGDataAndStatistics::getRandom(int n, int m)
{
	if(m<n)
		return 0;
	int num = RandHelper::rand(m - n);
	num += n;
	return num;
}

int
AGDataAndStatistics::getRandomPopDistributed(int n, int m)
{
	if(m<n || n>= limitEndAge)
		return -1;
	if(m>limitEndAge)
		m=limitEndAge;
	float alea = (float)(RandHelper::rand());
	float beginProp = getPropYoungerThan(n);
	float total = getPropYoungerThan(m) - beginProp;
	if(total <= 0)
		return -1;
	/**
	 * alea = alea * total + beginProp =====> easier test
	 * than: alea < (getPropYoungerThan(a+1)-beginProp)/total
	 */
	alea = alea * total + beginProp;
	for(int a=n ; a<m ; ++a)
	{
		if(alea < getPropYoungerThan(a+1))
			return a;
	}
	return -1;
}

int
AGDataAndStatistics::getPoissonsNumberOfChildren(float mean)
{
	float alea = (float)(RandHelper::rand());
	float cumul = 0;
	for(int nbr = 0 ; nbr < LIMIT_CHILDREN_NUMBER ; ++nbr)
	{
		cumul += poisson(mean, nbr);
		if(cumul > alea)
			return nbr;
	}
	return LIMIT_CHILDREN_NUMBER;
}

float
AGDataAndStatistics::poisson(float mean, int occ)
{
	double proba = exp(-1*(double)mean);
	proba *= pow(mean, occ);
	proba /= (double)factorial(occ);
	return (float)proba;
}

int
AGDataAndStatistics::factorial(int fact)
{
	if(fact > 0)
		return fact * factorial(fact-1);
	return 1;
}

void
AGDataAndStatistics::consolidateStat()
{
	normalizeMapProb(&beginWorkHours);
	normalizeMapProb(&endWorkHours);
	normalizeMapProb(&population);
	limitEndAge = population.rbegin()->first;

	oldAgeHhProb = (float)getPeopleOlderThan(limitAgeRetirement) / (float)getPeopleOlderThan(limitAgeChildren);
	secondPersProb = (float)(getPeopleOlderThan(limitAgeChildren) - households) / (float)households;
	meanNbrChildren = (float)getPeopleYoungerThan(limitAgeChildren) / ((1 - oldAgeHhProb) * (float)households);
	//cout << " --> oldAgeHhProb = " << setprecision(3) << oldAgeHhProb << "  - retAge? " << getPeopleOlderThan(limitAgeRetirement) << " adAge? " << getPeopleOlderThan(limitAgeChildren) << endl;
	//cout << " --> secondPersProb = " << setprecision(3) << secondPersProb << "  - adAge? " << getPeopleOlderThan(limitAgeChildren) << " hh?" << households << endl;
	//cout << " --> meanNbrChildren = " << setprecision(3) << meanNbrChildren << "  - chAge? " << getPeopleYoungerThan(limitAgeChildren) << endl;
}

float
AGDataAndStatistics::getPropYoungerThan(int age)
{
	map<int, float>::iterator it;
	float sum = 0;
	int previousAge = 0;
	float prop = 0;

	for(it = population.begin() ; it != population.end() ; ++it)
	{
		if(it->first < age)
		{
			sum += it->second;
		}
		else if(it->first >= age && previousAge < age)
		{
			prop = ((float)(age - previousAge) / (float)(it->first - previousAge));
			sum += prop * it->second;
			break;
		}
		previousAge = it->first;
	}
	return sum;
}

int
AGDataAndStatistics::getPeopleYoungerThan(int age)
{
	return (int)((float)inhabitants * getPropYoungerThan(age));
}

int
AGDataAndStatistics::getPeopleOlderThan(int age)
{
	return (inhabitants - getPeopleYoungerThan(age));
}

void
AGDataAndStatistics::normalizeMapProb(map<int, float> *myMap)
{
	//cout << "- Normalization: ";
	float sum = 0;
	map<int, float>::iterator it;
	for(it = myMap->begin() ; it != myMap->end() ; ++it)
	{
		sum += it->second;
	}
	if(sum == 0)
		return;
	for(it = myMap->begin() ; it != myMap->end() ; ++it)
	{
		it->second = it->second / sum;
		//cout << " + " << it->second;
	}
	//cout << " = 1.00" << endl;
}

float
AGDataAndStatistics::getInverseExpRandomValue(float mean, float maxVar)
{
	if(maxVar <= 0)
		return mean;
	SUMOReal p = RandHelper::rand(static_cast<SUMOReal>(0.0001), static_cast<SUMOReal>(1));
	//we have to scale the distribution because maxVar is different from INF
	float scale = exp((-1)*maxVar);
	//new p: scaled
	p = p * (1-scale) + scale; // p = [scale ; 1) ==> (1-p) = (0 ; 1-scale]

	float variation = (-1)*log(p);
	//decide the side of the mean value
	if(RandHelper::rand(1000) < 500)
		return mean + variation;
	else
		return mean - variation;

}

/****************************************************************************/
