/****************************************************************************/
/// @file    AGHousehold.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// A household contains the people and cars of the city: roughly represents
// families with their address, cars, adults and possibly children
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

#include <utils/common/RandHelper.h>
#include "AGHousehold.h"
#include "AGCar.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGHousehold::generatePeople()
{
	AGDataAndStatistics *ds = &(myCity->statData);
	//the first adult
	AGAdult pers(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitEndAge));
	adults.push_back(pers);

	//the second adult
	if(decisionProba(ds->secondPersProb))
	{
		if(pers.getAge() < ds->limitAgeRetirement)
		{
			AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitAgeRetirement));
			adults.push_back(pers2);
		}
		else
		{
			AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeRetirement, ds->limitEndAge));
			adults.push_back(pers2);
		}
	}

	//Children
	if(pers.getAge() < ds->limitAgeRetirement)
	{
		int numChild = ds->getPoissonsNumberOfChildren(ds->meanNbrChildren);
		while(numChild > 0)
		{
			AGChild chl(ds->getRandomPopDistributed(0, ds->limitAgeChildren));
			children.push_back(chl);
			--numChild;
		}
	}
}

void
AGHousehold::generateCars(float rate)
{
	int peopleInNeed = adults.size() - cars.size();
	while(peopleInNeed > 0)
	{
		if(decisionProba(rate))
		{
			addACar();
		}
		--peopleInNeed;
	}
}

void
AGHousehold::addACar()
{
	int numCar = cars.size() + 1;
	cars.push_back(AGCar(idHH, numCar));
}

int
AGHousehold::getCarNbr()
{
	return cars.size();
}

int
AGHousehold::getPeopleNbr()
{
	return adults.size() + children.size();
}

int
AGHousehold::getAdultNbr()
{
	return adults.size();
}

bool
AGHousehold::isCloseFromPubTransport(list<AGPosition> *pubTransport)
{
	float distToPT = location.minDistanceTo(*pubTransport);
	if(distToPT > myCity->statData.maxFootDistance)
		return false;
	return true;
}

bool
AGHousehold::isCloseFromPubTransport(map<int, AGPosition> *pubTransport)
{
	float distToPT = location.minDistanceTo(*pubTransport);
	if(distToPT > myCity->statData.maxFootDistance)
		return false;
	return true;
}

void
AGHousehold::regenerate()
{
	//only allocation of work or school to people will change
	list<AGChild>::iterator itC;
	list<AGAdult>::iterator itA;
	for(itC=children.begin() ; itC != children.end() ; ++itC)
	{
		if(itC->haveASchool())
		{
			if(itC->leaveSchool())
				itC->alocateASchool(&(myCity->schools), getPosition());
		}
		else
			itC->alocateASchool(&(myCity->schools), getPosition());
	}
	for(itA=adults.begin() ; itA!=adults.end() ; ++itA)
	{
		if(itA->isWorking())
		{
			if(itA->quiteHisJob())
				itA->assocWork(1-myCity->statData.unemployement, &(myCity->workPositions), myCity->statData.workPositions);
		}
		else
			itA->assocWork(1-myCity->statData.unemployement, &(myCity->workPositions), myCity->statData.workPositions);
	}
}

bool
AGHousehold::allocateChildrenSchool()
{
	list<AGChild>::iterator it;
	bool oneRemainsAtHome = false;

	for( it = children.begin() ; it != children.end() ; ++it)
	{
		if(!it->alocateASchool(&(myCity->schools), location))
			oneRemainsAtHome = true;
	}
	return !oneRemainsAtHome;
}

bool
AGHousehold::allocateAdultsWork()
{
	list<AGAdult>::iterator it;
	for(it=adults.begin() ; it!=adults.end() ; ++it)
	{
		if(myCity->statData.workPositions <= 0)
			return false;
		if( it->assocWork(1-myCity->statData.unemployement, &(myCity->workPositions), myCity->statData.workPositions) )
			myCity->statData.workingPeople++;
	}
	return true;
}

bool
AGHousehold::decisionProba(float p)
{
	double q = RandHelper::rand(); //(double)(rand()%10000)/(double)10000;
	//cout << " p=" << p << " q=" << q;
	return (q<p);
}

AGPosition
AGHousehold::getPosition()
{
	return location;
}

AGCity*
AGHousehold::getTheCity()
{
	return myCity;
}

bool
AGHousehold::retiredHouseholders()
{
	return (adults.front().getAge() >= myCity->statData.limitAgeRetirement);
}

/****************************************************************************/
