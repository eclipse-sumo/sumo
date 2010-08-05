/****************************************************************************/
/// @file    AGActivityGen.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGActivityGen.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
// Central object handling City, Activities and Trips
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <sstream>
#include "AGActivityGen.h"
#include "AGActivityGenHandler.h"
#include "city/AGPosition.h"
#include "activities/AGActivities.h"
#include "AGActivityTripWriter.h"
#include "city/AGTime.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGActivityGen::importInfoCity()
{
	AGActivityGenHandler handler(city, net);
	handler.setFileName(inputFile);
	XMLSubSys::init(false);
	MsgHandler::initOutputOptions();
	if (!XMLSubSys::runParser(handler, inputFile)) {
		MsgHandler::getMessageInstance()->endProcessMsg("failed.");
		throw ProcessError();
	} else {
		MsgHandler::getMessageInstance()->endProcessMsg("done.");
	}

	cout << "\n\n\n\n\n/______| lecture XML: fait |______\\\n\n\n" << endl;
	cout << "\n\n\n\n\n/______| --consolidation-- |______\\\n\n\n" << endl;
	city.statData.consolidateStat();
	cout << "\n\n\n\n\n/______| consolidation: fait |______\\\n\n\n" << endl;
	city.completeStreets();
	cout << "\n\n\n\n\n/______| streets: fait |______\\\n\n\n" << endl;
	cout << " cou " << city.streets.size() << endl;
	city.generateWorkPositions();
	cout << "\n\n\n\n\n/______| workpositions: fait |______\\\n\n\n" << endl;
	city.completeBusLines();
	//test:
	//city.busLines.front().printBuses();
	//end test

	cout << "\n\n\n\n\n/______| Manque plus que la population |______\\\n\n\n" << endl;
	city.generatePopulation();
	cout << "\n\n\n\n\n/______| VILLE TERMINEE |______\\\n\n\n" << endl;

	//ALOCATIONS
	cout << "\n\n\n\n\n/______| ALLOCATIONS |______\\\n\n\n" << endl;
	city.schoolAllocation();
	cout << "\n\n\n\n\n/______| schools: fait |______\\\n\n\n" << endl;
	city.workAllocation();
	cout << "\n\n\n\n\n/______| work positions: fait |______\\\n\n\n" << endl;
	city.carAllocation();
	cout << "\n\n\n\n\n/______| cars: fait |______\\\n\n\n" << endl;
	cout << "\n\n\n\n\n/______| Fin de la construction de la ville |______\\\n\n\n" << endl;

	/*cout << "streets: " << city.streets.size() << endl;
	if(city.streets.size() >= 2)
	{
		city.streets.front().print();
		city.streets.back().print();
		cout << " distance entre les deux extremes: " << endl;
		cout << " ---> " << city.streets.front().net->getEdge(city.streets.front().getName())->getFromNode()->getPosition().distanceTo(city.streets.front().net->getEdge(city.streets.front().getName())->getToNode()->getPosition()) << endl;
		cout << " ---> " << city.streets.back().net->getEdge(city.streets.back().getName())->getFromNode()->getPosition().distanceTo(city.streets.back().net->getEdge(city.streets.back().getName())->getToNode()->getPosition()) << endl;

		cout << " distance croisées des deux extremes: " << endl;
		cout << " ---> " << city.streets.back().net->getEdge(city.streets.back().getName())->getFromNode()->getPosition().distanceTo(city.streets.front().net->getEdge(city.streets.front().getName())->getToNode()->getPosition()) << endl;
		cout << " ---> " << city.streets.front().net->getEdge(city.streets.front().getName())->getFromNode()->getPosition().distanceTo(city.streets.back().net->getEdge(city.streets.back().getName())->getToNode()->getPosition()) << endl;

		Position *pos = new Position(&city.streets.front());
		pos->print();
		cout << " Position: " << pos->pos2d.x() << " , " << pos->pos2d.y() << endl;
		cout << " node From: " << pos->street->net->getEdge(pos->street->getName())->getFromNode()->getPosition().x() << " , " << pos->street->net->getEdge(pos->street->getName())->getFromNode()->getPosition().y() << endl;
		cout << " node To: " << pos->street->net->getEdge(pos->street->getName())->getToNode()->getPosition().x() << " , " << pos->street->net->getEdge(pos->street->getName())->getToNode()->getPosition().y() << endl;
	}*/

}

bool
AGActivityGen::timeTripValidation(AGTrip trip)
{
	if(trip.getDay() > this->durationInDays+1)
		return false;
	if(trip.getDay() == 1) //first day
	{
		if(trip.getTime() < this->beginTime)
			return false;
		if(this->durationInDays == 0 && trip.getTime() > this->endTime)
			return false;
	}
	if(trip.getDay() == this->durationInDays+1) //last day
	{
		if(trip.getTime() > this->endTime)
			return false;
		if(this->durationInDays == 0 && trip.getTime() < this->beginTime)
			return false;
	}
	return true;
}

void
AGActivityGen::generateOutputFile(std::list<AGTrip>& trips)
{
	AGActivityTripWriter atw(this->outputFile);
	list<AGTrip>::iterator it;
	//variables for TESTS:
	int firstTrip = trips.front().getTime() + trips.front().getDay()*86400;
	int lastTrip = trips.front().getTime() + trips.front().getDay()*86400;
	//END var TESTS
	for(it=trips.begin() ; it!=trips.end() ; ++it)
	{
		atw.addTrip(*it);
		//TEST
		if(it->getTime() + 86400*it->getDay() > lastTrip)
			lastTrip = it->getTime() + 86400*it->getDay();
		if(it->getTime() + 86400*it->getDay() < firstTrip)
			firstTrip = it->getTime() + 86400*it->getDay();
		//END TEST
	}
	//PRINT TEST
	AGTime first(firstTrip);
	AGTime last(lastTrip);
	cout << "first real trip: " << first.getDay() << ", " << first.getHour() << ":" << first.getMinute() << ":" << first.getSecond() << endl;
	cout << "last real trip: " << last.getDay() << ", " << last.getHour() << ":" << last.getMinute() << ":" << last.getSecond() << endl;
	//END TEST
	atw.writeOutputFile();
}

void
AGActivityGen::makeActivityTrips(int days, int beginSec, int endSec)
{
	durationInDays = days;
	beginTime = beginSec;
	endTime = endSec;
	/**
	 * making the activity trips
	 */
	AGActivities acts(&city, durationInDays+1);
	acts.generateActivityTrips();

	/**
	 * validating the trips with the simulation's time limits
	 */
	//list<Trip>* trips = &(acts.trips);
	list<AGTrip> expTrips;
	map<string, int> carUsed;
	list<AGTrip>::iterator it;
	//multiplication of days
	for(it=acts.trips.begin() ; it!=acts.trips.end() ; ++it)
	{
		if(it->isDaily())
		{
			for(int currday=1 ; currday<this->durationInDays+2 ; ++currday)
			{
				AGTrip tr(it->getDep(), it->getArr(), it->getVehicleName(), it->getTime(), currday);
				tr.setType(it->getType());
				if(carUsed.find(tr.getVehicleName()) != carUsed.end())
				{
					++carUsed.find(tr.getVehicleName())->second;
				}
				else
				{
					carUsed[tr.getVehicleName()] = 1;
				}
				std::ostringstream os;
				os << tr.getVehicleName() << ":" << carUsed.find(tr.getVehicleName())->second;
				tr.setVehicleName(os.str());
				if(timeTripValidation(tr))
					expTrips.push_back(tr);
				//else
				//	cout << "trop tard 1 pour " << tr.getVehicleName() << " " << tr.getTime() << " day: " << tr.getDay() << endl;
			}
		}
		else
		{
			AGTrip tr(it->getDep(), it->getArr(), it->getVehicleName(), it->getTime(), it->getDay());
			tr.setType(it->getType());
			if(carUsed.find(tr.getVehicleName()) != carUsed.end())
			{
				++carUsed.find(tr.getVehicleName())->second;
			}
			else
			{
				carUsed[tr.getVehicleName()] = 1;
			}
			std::ostringstream os;
			os << tr.getVehicleName() << ":" << carUsed.find(tr.getVehicleName())->second;
			tr.setVehicleName(os.str());
			if(timeTripValidation(tr))
				expTrips.push_back(tr);
			//else
			//	cout << "trop tard 2 pour " << tr.getVehicleName() << " " << tr.getTime() << " day: " << tr.getDay() << endl;
		}
	}

	cout << "total trips generated: " << acts.trips.size() << endl;
	cout << "total trips finally taken: " << expTrips.size() << endl;

	//TODO add slight variation in trips

	/**
	 * trip file generation
	 */
	generateOutputFile(expTrips);
}

/****************************************************************************/
