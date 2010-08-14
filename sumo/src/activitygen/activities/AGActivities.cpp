/****************************************************************************/
/// @file    AGActivities.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Main object managing activities taken in account and generating the city
// inhabitants' trip list.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2010 TUM (Technische Universität München, http://www.tum.de/)
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

#include "AGActivities.h"
#include "AGWorkAndSchool.h"
#include "AGFreeTime.h"
#include "../city/AGTime.h"
#include <sstream>
#include <utils/common/RandHelper.h>

#define REBUILD_ITERATION_LIMIT 2


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGActivities::addTrip(AGTrip t, list<AGTrip> *tripSet)
{
	tripSet->push_back(t);
}

void
AGActivities::addTrips(list<AGTrip> t, list<AGTrip> *tripSet)
{
	list<AGTrip>::iterator it;
	for(it=t.begin() ; it!=t.end() ; ++it)
	{
		tripSet->push_back(*it);
	}
}

void
AGActivities::generateActivityTrips()
{
	cout << "beginning: " << trips.size() << endl;
	int numbErr;
	/**
	 * trips due to public transportation
	 */
	numbErr = 0;
	list<AGBusLine>::iterator itBL;
	for(itBL=myCity->busLines.begin() ; itBL!=myCity->busLines.end() ; ++itBL)
	{
		if( ! generateBusTraffic(*itBL) )
			++numbErr;
	}
	if(numbErr != 0)
		cerr << "ERROR: " << numbErr << " bus lines couldn't been generated ( " << (float)numbErr*100.0/(float)myCity->busLines.size() << "% )..." << endl;
	else
		cout << "no problem during bus line trip generation..." << endl;

	cout << "after public transportation: " << trips.size() << endl;
	/**
	 * trips due to activities in the city
	 * @NOTICE: includes people working in work positions out of the city
	 */
	numbErr = 0;
	list<AGHousehold>::iterator itHH;
	for(itHH=myCity->households.begin() ; itHH!=myCity->households.end() ; ++itHH)
	{
		if( ! generateTrips(*itHH) )
			++numbErr;
	}
	if(numbErr != 0)
		cout << "WARNING: " << numbErr << " ( " << (float)numbErr*100.0/(float)myCity->households.size() << "% ) households' trips haven't been generated: would probably need more iterations for rebuilding..." << endl;
	else
		cout << "no problem during households' trips generation..." << endl;

	cout << "after household activities: " << trips.size() << endl;
	/**
	 * trips due to incoming and outgoing traffic
	 * @WARNING: the outgoing traffic is already done: households in which someone works on a work position that is out of the city.
	 */
	if( ! generateInOutTraffic() )
		cerr << "ERROR while generating in/Out traffic..." << endl;
	else
		cout << "no problem during in/out traffic generation..." << endl;

	cout << "after incoming/outgoing traffic: " << trips.size() << endl;
	/**
	 * random traffic trips
	 * @NOTICE: this includes uniform and proportional random traffic
	 */
	if( ! generateRandomTraffic() )
		cerr << "ERROR while generating random traffic..." << endl;
	else
		cout << "no problem during random traffic generation..." << endl;

	cout << "after random traffic: " << trips.size() << endl;
}

bool
AGActivities::generateTrips(AGHousehold &hh)
{
	int iteration = 0;
	bool generated = false;
	list<AGTrip> temporaTrips;
	while(!generated && iteration < REBUILD_ITERATION_LIMIT)
	{
		if(!temporaTrips.empty())
			temporaTrips.clear();
		// Work and school activities
		AGWorkAndSchool ws(&hh, &(myCity->statData), &temporaTrips);
		generated = ws.generateTrips();
		if(!generated)
		{
			hh.regenerate();
			++iteration;
			continue;
		}
		addTrips(ws.partialActivityTrips, &temporaTrips);

		// free time activities
		AGFreeTime ft(&hh, &(myCity->statData), &temporaTrips, nbrDays);
		generated = ft.generateTrips();
		if(!generated)
		{
			hh.regenerate();
			++iteration;
			continue;
		}
		addTrips(ft.partialActivityTrips, &temporaTrips);
		//cout << "after this hh: " << temporaTrips.size() << " we have: " << trips.size() << endl;
		//trips of all activities generated:
		addTrips(temporaTrips, &trips);
	}
	return generated;
}

bool
AGActivities::generateBusTraffic(AGBusLine bl)
{
	list<AGBus>::iterator itB;
	list<AGPosition>::iterator itS;
	for(itB=bl.buses.begin() ; itB!=bl.buses.end() ; ++itB)
	{
		if(bl.stations.size() < 1)
			return false;
		AGTrip t(bl.stations.front(), bl.stations.back(), *itB, itB->getDeparture());
		for(itS=bl.stations.begin() ; itS!=bl.stations.end() ; ++itS)
		{
			if(*itS == t.getDep() || *itS == t.getArr())
				continue;
			t.addLayOver(*itS);
		}
		trips.push_back(t);
	}
	return true;
}

bool
AGActivities::generateInOutTraffic()
{
	/**
	 * outgoing traffic already done by generateTrips():
	 * people who work out of the city.
	 * Here are people from outside the city coming to work.
	 */
	if(myCity->peopleIncoming.empty())
		return true;
	if(myCity->cityGates.empty())
		return false;

	int num = 1;
	bool employed;
	list<AGPosition>::iterator itP = myCity->cityGates.begin();
	list<AGAdult>::iterator itA;

	for(itA=myCity->peopleIncoming.begin() ; itA!=myCity->peopleIncoming.end() ; ++itA)
	{
		if(itP == myCity->cityGates.end())
			itP = myCity->cityGates.begin();
		string nom(generateName(num, "carIn"));
		if(! itA->assocWork(1, &(myCity->workPositions), myCity->statData.workPositions))
		{
			//shouldn't happen
			cout << "not enough work for incoming people..." << endl;
		}
		AGTrip wayTrip(*itP, itA->getWorkLocation(), nom, itA->getWorkOpening());
		//now we put the estimated time of entrance in the city.
		wayTrip.setDepTime( wayTrip.estimateDepTime(wayTrip.getTime(), myCity->statData.speedTimePerKm) );
		AGTrip retTrip(itA->getWorkLocation(), *itP, nom, itA->getWorkClosing());
		trips.push_back(wayTrip);
		trips.push_back(retTrip);
		++num;
	}
	return true;
}

string
AGActivities::generateName(int i, string prefix)
{
	std::ostringstream os;
	os << i;
	return prefix + os.str();
}

bool
AGActivities::generateRandomTraffic()
{
	//total number of trips during the whole simulation
	int totalTrips = 0, ttOneDayTrips = 0, ttDailyTrips = 0;
	list<AGTrip>::iterator it;
	for(it = trips.begin() ; it != trips.end() ; ++it)
	{
		if(it->isDaily())
			++ttDailyTrips;
		else
			++ttOneDayTrips;
	}
	totalTrips = ttOneDayTrips + ttDailyTrips * nbrDays;
	//TESTS
	cout << "Before Random traffic generation (days are still entire):" << endl;
	cout << "- Total number of trips: " << totalTrips << endl;
	cout << "- Total daily trips: " << ttDailyTrips << endl;
	cout << "- Total one-day trips: " << ttOneDayTrips << endl;
	//END OF TESTS

	//random uniform distribution:
	int nbrRandUni = (int)((float)totalTrips * myCity->statData.uniformRandomTrafficRate / (1.0f - myCity->statData.uniformRandomTrafficRate));
	//TESTS
	cout << "added uniform random trips: " << nbrRandUni << endl;
	//END OF TESTS
	for(int i=0 ; i<nbrRandUni ; ++i)
	{
		AGPosition dep(myCity->getRandomStreet());
		AGPosition arr(myCity->getRandomStreet());
		AGTime depTime(RandHelper::rand(nbrDays*86400));
		AGTrip rdtr(dep, arr, generateName(i, "randUni"), depTime.getTime()%86400, depTime.getDay()+1);
		rdtr.setType("random");
		trips.push_back(rdtr);
	}

	//random proportional distribution:
	float proportionalPercentage = 0.05f;
	//TODO generate a proportionally distributed random traffic

	return true;
}

/****************************************************************************/
