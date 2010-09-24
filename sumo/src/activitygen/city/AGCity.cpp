/****************************************************************************/
/// @file    AGCity.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// City class that contains all other objects of the city: in particular
// streets, households, bus lines, work positions and school
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

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <utils/common/RandHelper.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include "AGStreet.h"
#include "AGWorkPosition.h"
#include "AGCity.h"
//#define DRIVING_LICENSE_AGE 18


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGCity::completeStreets()
{
	if(streetsCompleted)
		return;
	else
		streetsCompleted = true;

	NrStreets = 0;
	int pop=0, work=0;
	std::vector<AGStreet>::iterator it;

	for(it = streets.begin() ; it!=streets.end() ; ++it)
	{
		pop += (int)(it->getPopulation());
		work += (int)(it->getWorkplaceNumber());
		++NrStreets;
	}
	statData.factorInhabitants = (float)statData.inhabitants / (float)pop;
	//can be improved with other input data
	float neededWorkPositionsInCity = (1.0f - statData.unemployement)
					* ((float)statData.getPeopleYoungerThan(statData.limitAgeRetirement)
					- (float)statData.getPeopleYoungerThan(statData.limitAgeChildren))
					+ (float)statData.incomingTraffic;
	// we generate 5% more work positions that really needed: to avoid any expensive research of random work positions
	neededWorkPositionsInCity *= 1.05f;
	statData.workPositions = (int)neededWorkPositionsInCity;
	statData.factorWorkPositions = neededWorkPositionsInCity / (float) work;

	for(it = streets.begin() ; it!=streets.end() ; ++it)
	{
		it->setPopulation((int)(it->getPopulation() * statData.factorInhabitants));
		it->setWorkplaceNumber((int)(it->getWorkplaceNumber() * statData.factorWorkPositions));
		//it->print();
	}

	//completing streets from edges of the network not handled/present in STAT file (no population no work position)
	map<string, ROEdge*>::const_iterator itE;
	vector<AGStreet>::iterator itS;

	for(itE = net->getEdgeMap().begin() ; itE != net->getEdgeMap().end() ; ++itE)
	{
		for(itS = streets.begin() ; itS != streets.end() ; ++itS)
		{
			if(itS->getName() == itE->second->getID())
			{
				break;
			}
		}
		//if this edge isn't represented by a street
		if(itS == streets.end())
		{
			streets.push_back(AGStreet(itE->second));
		}
	}
}

void
AGCity::generateWorkPositions()
{
	vector<AGStreet>::iterator it;
	int workPositionCounter = 0;

	try
	{
		for(it = streets.begin() ; it != streets.end() ; ++it)
		{
			//cout << "number of work positions in street: " << it->getWorkplaceNumber() << endl;
			for(int i=0 ; i<it->getWorkplaceNumber() ; ++i)
			{
				workPositions.push_back(AGWorkPosition(*it, &statData));
				++workPositionCounter;
			}
		}
	} catch (const bad_alloc& e) {
		cout << "Number of work positions at bad_alloc exception: " << workPositionCounter << endl;
		throw(e);
	}
	//cout << "Inner work positions done. " << workPositionCounter << " generated." << endl;

	// Work positions outside the city
	generateOutgoingWP();
	cout << "--> work position: " << endl;
	cout << "  |-> in city: " << workPositionCounter << endl;
	cout << "  |-> out city: " << statData.workPositions - workPositionCounter << endl;
	cout << "  |-> in+out city: " << statData.workPositions << endl;
}

void
AGCity::generateOutgoingWP()
{
	// work positions outside the city
	float nbrWorkers = static_cast<float>(statData.getPeopleYoungerThan(statData.limitAgeRetirement) - statData.getPeopleYoungerThan(statData.limitAgeChildren));
	if(nbrWorkers <= 0)
		return;
	nbrWorkers *= (1.0f - statData.unemployement);
	/**
	 * N_out = N_in * (ProportionOut / (1 - ProportionOut)) = N_out = N_in * (Noutworkers / (Nworkers - Noutworkers))
	 */
	int nbrOutWorkPositions = static_cast<int>(workPositions.size() * (static_cast<float>(statData.outgoingTraffic))/(nbrWorkers - static_cast<float>(statData.outgoingTraffic)));

	if(cityGates.empty())
		return;

	for(int i=0 ; i<nbrOutWorkPositions ; ++i)
	{
		int posi = statData.getRandomCityGateByOutgoing();
		workPositions.push_back(AGWorkPosition(cityGates[posi].getStreet(), cityGates[posi].getPosition(), &statData));
	}
	//cout << "outgoing traffic: " << statData.outgoingTraffic << endl;
	//cout << "total number of workers in the city: " << nbrWorkers << endl;
	//cout << "work positions out side the city: " << nbrOutWorkPositions << endl;
	//cout << "work positions in and out of the city: " << workPositions.size() << endl;
	statData.workPositions = workPositions.size();
}

void
AGCity::completeBusLines()
{
	list<AGBusLine>::iterator it;
	for(it=busLines.begin() ; it!=busLines.end() ; ++it)
	{
		//it->generateOpositDirection();
		it->setBusNames();
	}
}

void
AGCity::generatePopulation()
{
	vector<AGStreet>::iterator it;
	int people;
	nbrCars = 0;
	int idHouseholds = 0;

	for(it=streets.begin() ; it!=streets.end() ; ++it)
	{
		people = it->getPopulation();
		while(people > 0)
		{
			++idHouseholds;
			households.push_back(AGHousehold(&*it, this, idHouseholds));
			households.back().generatePeople(); //&statData
			//households.back().generateCars(statData.carRate);
			people -= households.back().getPeopleNbr();
		}
	}

	//people from outside of the city generation:
	generateIncomingPopulation();

	//TEST
	int nbrSingle = 0;
	int nbrCouple = 0;
	int nbrChild = 0;
	int nbrHH = 0;
	int workingP = 0;
	list<AGHousehold>::iterator itt;
	for(itt=households.begin() ; itt != households.end() ; ++itt)
	{
		if(itt->getAdultNbr() == 1)
		{
			nbrSingle++;
			if(itt->adults.front().isWorking())
				workingP++;
		}
		if(itt->getAdultNbr() == 2)
		{
			nbrCouple += 2;
			if(itt->adults.front().isWorking())
				workingP++;
			if(itt->adults.back().isWorking())
				workingP++;
		}
		nbrChild += itt->getPeopleNbr() - itt->getAdultNbr();
		nbrHH++;
	}
	//cout << "number hh: " << nbrHH << endl;
	//cout << "number single: " << nbrSingle << endl;
	//cout << "number couple: " << nbrCouple << endl;
	//cout << "number 3 or more: " << nbr3More << endl;
	//cout << "number adults: " << nbrSingle + nbrCouple + nbr3More << endl;
	//cout << "number children: " << nbrChild << endl;
	//cout << "number people: " << nbrSingle + nbrCouple + nbr3More + nbrChild << endl;
	//END TEST

	cout << "--> population: " << endl;
	cout << "  |-> city households: " << nbrHH << endl;
	cout << "  |-> city people: " << nbrSingle + nbrCouple + nbrChild << endl;
	cout << "    |-> city single: " << nbrSingle << " / (in) couple: " << nbrCouple << endl;
	cout << "    |-> city adults: " << nbrSingle + nbrCouple << endl;
	cout << "      |-> estimation: " << statData.getPeopleOlderThan(statData.limitAgeChildren) << endl;
	cout << "      |-> retired: " << statData.getPeopleOlderThan(statData.limitAgeRetirement) << endl;
	cout << "    |-> city children: " << nbrChild << endl;
	cout << "      |-> estimation: " << statData.getPeopleYoungerThan(statData.limitAgeChildren) << endl;

}

void
AGCity::generateIncomingPopulation()
{
	for(int i=0 ; i<statData.incomingTraffic ; ++i)
	{
		AGAdult ad(statData.getRandomPopDistributed(statData.limitAgeChildren, statData.limitAgeRetirement));
		peopleIncoming.push_back(ad);
	}
}

void
AGCity::schoolAllocation()
{
	list<AGHousehold>::iterator it;
	bool shortage;
	for(it = households.begin() ; it != households.end() ; ++it)
	{
		shortage = !it->allocateChildrenSchool();
		if(shortage)
		{
			/*ofstream fichier("test.txt", ios::app);  // ouverture en écriture avec effacement du fichier ouvert
			if(fichier)
			{
				fichier << "===> WARNING: Not enough school places in the city for all children..." << endl;
				fichier.close();
			}
			else
				cerr << "Impossible d'ouvrir le fichier !" << endl;*/

			//cout << "===> WARNING: Not enough school places in the city for all children..." << endl;
		}
	}
}

void
AGCity::workAllocation()
{
	//tests
	//cout << "beginning work" << endl;
	statData.workingPeople = 0;
	statData.AdultNbr = 0;
	//end tests
	/**
	 * people from the city
	 */
	list<AGHousehold>::iterator it;
	bool shortage;

	for(it = households.begin() ; it != households.end() ; ++it)
	{
		if(it->retiredHouseholders())
			continue;
		shortage = !it->allocateAdultsWork();
		if(shortage)
		{
			cout << "===> ERROR: Not enough work positions in the city for all working people..." << endl;
		}
		statData.AdultNbr += it->getAdultNbr(); //TESTING
	}

	/**
	 * people from outside
	 */
	list<AGAdult>::iterator itA;
	for(itA=peopleIncoming.begin() ; itA!=peopleIncoming.end() ; ++itA)
	{
		if(statData.workPositions > 0)
		{
			itA->tryToWork(1, &workPositions);
		} else {
			//shouldn't happen
			cout << "not enough work for incoming people..." << endl;
		}
	}

	//BEGIN TESTS
	int workingP = 0;
	list<AGHousehold>::iterator itt;
	for(itt=households.begin() ; itt != households.end() ; ++itt)
	{
		if(itt->getAdultNbr() == 1)
		{
			if(itt->adults.front().isWorking())
				workingP++;
		}
		if(itt->getAdultNbr() == 2)
		{
			if(itt->adults.front().isWorking())
				workingP++;
			if(itt->adults.back().isWorking())
				workingP++;
		}
	}
	cout << "  |-> working people: " << peopleIncoming.size() + workingP << endl;
	cout << "    |-> working people in city: " << workingP << endl;
	cout << "    |-> working people from outside: " << peopleIncoming.size() << endl;
	//END TESTS
}

void
AGCity::carAllocation()
{
	statData.hhFarFromPT = 0;
	nbrCars = 0;
	list<AGHousehold>::iterator it;
	for(it=households.begin() ; it!=households.end() ; ++it)
	{
		if(!it->isCloseFromPubTransport(&(statData.busStations)) )
		{
			statData.hhFarFromPT++;
			nbrCars++;
			it->addACar();
		}
		statData.householdsNbr++;
	}
	// new rate: the rate on the people that have'nt any car yet:
	// nR = (R * Drivers - AlreadyCars) / (Drivers - AlreadyCars)
	float newRate = (statData.carRate * statData.getPeopleOlderThan(statData.limitAgeChildren) - statData.hhFarFromPT) / (statData.getPeopleOlderThan(statData.limitAgeChildren) - statData.hhFarFromPT);
	//cout << " - " << newRate << endl;
	if(newRate < 0 || newRate >= 1)
		newRate = 0;

	nbrCars = 0;
	int nbrAdults = 0;
	for(it=households.begin() ; it!=households.end() ; ++it)
	{
		it->generateCars(newRate);
		nbrCars += it->getCarNbr();
		nbrAdults += it->getAdultNbr();
	}
	//TEST RESULTS
	//cout << "number of cars: " << nbrCars << endl;
	//cout << "number of adults: " << statData.getPeopleOlderThan(statData.limitAgeChildren) << endl;
	//cout << "real number of adults: " << nbrAdults << endl;
	//cout << "number of people far from public transport: " << statData.hhFarFromPT << endl;
	//cout << "original rate: " << setprecision(4) << statData.carRate << endl;
	//cout << "new rate: " << setprecision(4) << newRate << endl;
	//cout << "real rate: " << setprecision(4) << (float)nbrCars / (float)statData.getPeopleOlderThan(statData.limitAgeChildren) << endl;
	//END TEST RESULTS
}

const AGStreet&
AGCity::getStreet(const string& edge)
{
	/**
	 * verify if it is the first time this function is called
	 * in this case, we have to complete the streets with the
	 * network edges this means that streets are completely
	 * loaded (no any more to be read from stat-file)
	 */
	if(!streetsCompleted)
	{
		statData.consolidateStat();
		completeStreets();
		cout << "first completed in getStreet() of City: Consolidation of data not needed in ActivityGen any more" << endl;
	}
	//rest of the function
	vector<AGStreet>::iterator it = streets.begin();
	while(it != streets.end())
	{
		if(it->getName() == edge)
			return *it;
		++it;
	}
	cout << "===> ERROR: WRONG STREET EDGE (" << edge << ") given and not found in street set." << endl;
	throw(runtime_error("Street not found with edge id " + edge));
}

const AGStreet&
AGCity::getRandomStreet()
{
	if(streets.empty())
		throw(runtime_error("No street found in this city"));
	int alea = RandHelper::rand(streets.size());
	return streets.at(alea);
}

/****************************************************************************/
