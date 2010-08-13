/****************************************************************************/
/// @file    AGCity.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Main object containing all others of the city: in particular
// streets, households, bus lines, work positions and school
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
		pop += (int)(it->getPopDensity());
		work += (int)(it->getWorkDensity());
		++NrStreets;
	}
	statData.workPositions = work;
	statData.factorInhabitants = (float)statData.inhabitants / (float)pop;
	//can be improved with other input data
	float neededWorkPositionsInCity = (1.0f - statData.unemployement)
					* ((float)statData.getPeopleYoungerThan(statData.limitAgeRetirement)
					- (float)statData.getPeopleYoungerThan(statData.limitAgeChildren))
					+ (float)statData.incomingTraffic;
	if(work < neededWorkPositionsInCity)
	{
		statData.factorWorkPositions = neededWorkPositionsInCity / (float) work;
	}
	else
	{
		statData.factorWorkPositions = 1.0f;
	}
	//statData.factorWorkPositions = 1.0f + ((float) statData.incomingTraffic / (float) work);

	for(it = streets.begin() ; it!=streets.end() ; ++it)
	{
		it->setPopulation((int)(it->getPopDensity() * statData.factorInhabitants));
		it->setWorkPositions((int)(it->getWorkDensity() * statData.factorWorkPositions));
		it->print();
	}

	//completing streets from edges of the network not handled/present in STAT file (no population no work position)
	map<string, ROEdge*>::const_iterator itE;
	vector<AGStreet>::iterator itS;

	cout << "taille Edges: " << net->getEdgeMap().size() << endl;
	cout << "taille Streets: " << streets.size() << endl;

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
			//cout << "ajout: " << itE->second->getID() << endl;
			AGStreet str(itE->second->getID(), net);
			streets.push_back(str);
		}
	}
}

void
AGCity::generateWorkPositions()
{
	vector<AGStreet>::iterator it;
	statData.workPositions = 0;
	AGWorkPosition *wp;

	for(it = streets.begin() ; it != streets.end() ; ++it)
	{
		for(int i=0 ; i<it->getWorkDensity() ; ++i)
		{
			//cout << "wp1 " << i << " etat: " << workPositions.size() << " - " << statData.workPositions << endl;
			wp = new AGWorkPosition(&*it, &statData);
			//wp->print();
			workPositions.push_back(*wp);
			//cout << "wp2" << endl;
		}
	}
	//workpositionsOutside the city
	generateOutgoingWP();
}

void
AGCity::generateOutgoingWP()
{
	// work positions outside the city
	AGWorkPosition *wp;
	float nbrWorkers = (float)statData.getPeopleYoungerThan(statData.limitAgeRetirement) - (float)statData.getPeopleYoungerThan(statData.limitAgeChildren);
	if(nbrWorkers <= 0)
		return;
	nbrWorkers *= (1.0f - statData.unemployement);
	int nbrOutWorkPositions = (int)((float)workPositions.size() * ((float)statData.outgoingTraffic / nbrWorkers));

	list<AGPosition>::iterator itP = cityGates.begin();
	if(cityGates.empty())
		return;
	for(int i=0 ; i<nbrOutWorkPositions ; ++i)
	{
		if(itP == cityGates.end())
			itP = cityGates.begin();
		wp = new AGWorkPosition(itP->street, itP->position, &statData);
		workPositions.push_back(*wp);
		++itP;
	}
	cout << "outgoing traffic: " << statData.outgoingTraffic << endl;
	cout << "total number of workers in the city: " << nbrWorkers << endl;
	cout << "work positions out side the city: " << nbrOutWorkPositions << endl;
	cout << "work positions in and out of the city: " << workPositions.size() << endl;
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

	for(it=streets.begin() ; it!=streets.end() ; ++it)
	{
		people = it->getPopDensity();
		while(people > 0)
		{
			households.push_back(*(new AGHousehold(&*it, this)));
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
	int nbr3More = 0;
	int nbrChild = 0;
	int nbrHH = 0;
	list<AGHousehold>::iterator itt;
	for(itt=households.begin() ; itt != households.end() ; ++itt)
	{
		if(itt->getAdultNbr() == 1)
			nbrSingle++;
		if(itt->getAdultNbr() == 2)
			nbrCouple += 2;
		if(itt->getAdultNbr() >3)
			nbr3More += itt->getAdultNbr();
		nbrChild += itt->getPeopleNbr() - itt->getAdultNbr();
		nbrHH++;
	}
	cout << "number hh: " << nbrHH << endl;
	cout << "number single: " << nbrSingle << endl;
	cout << "number couple: " << nbrCouple << endl;
	cout << "number 3 or more: " << nbr3More << endl;
	cout << "number adults: " << nbrSingle + nbrCouple + nbr3More << endl;
	cout << "number children: " << nbrChild << endl;
	cout << "number people: " << nbrSingle + nbrCouple + nbr3More + nbrChild << endl;
	//END TEST
}

void
AGCity::generateIncomingPopulation()
{
	for(int i=0 ; i<statData.incomingTraffic ; ++i)
	{
		AGAdult ad(statData.getRandomPopDistributed(statData.limitAgeChildren, statData.limitAgeRetirement));
		peopleIncoming.push_back(ad);
	}
	cout << "incoming population: " << peopleIncoming.size() << endl;
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
	cout << "beginning work" << endl;
	statData.workingPeople = 0;
	statData.AdultNbr = 0;
	//end tests
	list<AGHousehold>::iterator it;
	bool shortage;
	cout << "-> Before --> Working people: " << statData.workingPeople << " - work positions: " << statData.workPositions << endl;

	for(it = households.begin() ; it != households.end() ; ++it)
	{
		shortage = !it->allocateAdultsWork();
		if(shortage)
		{
			ofstream fichier("test.txt", ios::app);  // ouverture en écriture avec effacement du fichier ouvert
			if(fichier)
			{
				fichier << "===> ERROR: Not enough work positions in the city for all working people..." << endl;
				fichier.close();
			}
			else
				cerr << "Impossible d'ouvrir le fichier !" << endl;
			//cout << "===> ERROR: Not enough work positions in the city for all working people..." << endl;
		}
		statData.AdultNbr += it->getAdultNbr(); //TESTING
	}
	cout << "-> After --> Working people: " << statData.workingPeople << " - work positions: " << statData.workPositions << endl;

	cout << " number of working people: " << statData.workingPeople << endl;
	cout << " number of Adults: " << statData.AdultNbr << endl;
	cout << " testing employment rate: " << (float)statData.workingPeople / (float)statData.AdultNbr << " ( stat: " << 1-statData.unemployement << " )" << endl;
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
	cout << " - " << newRate << endl;
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
	cout << "number of cars: " << nbrCars << endl;
	cout << "number of adults: " << statData.getPeopleOlderThan(statData.limitAgeChildren) << endl;
	cout << "real number of adults: " << nbrAdults << endl;
	cout << "number of people far from public transport: " << statData.hhFarFromPT << endl;
	cout << "original rate: " << setprecision(4) << statData.carRate << endl;
	cout << "new rate: " << setprecision(4) << newRate << endl;
	cout << "real rate: " << setprecision(4) << (float)nbrCars / (float)statData.getPeopleOlderThan(statData.limitAgeChildren) << endl;
	//END TEST RESULTS
}

AGStreet*
AGCity::getStreet(string edge)
{
	/**
	 * verify if it is the first time this function is called
	 * in this case, we have to complete the streets with the
	 * network edges this means that streets are completely
	 * loaded (no any more to be read from stat-file)
	 */
	if(!streetsCompleted)
	{
		completeStreets();
	}
	//rest of the function
	vector<AGStreet>::iterator it = streets.begin();
	while(it != streets.end())
	{
		if(it->getName() == edge)
			return &*it;
		++it;
	}
	cout << "===> ERROR: WRONG STREET EDGE (" << edge << ") given and not found in street set." << endl;
	return NULL;
}

AGStreet*
AGCity::getRandomStreet()
{
	if(streets.empty())
		return NULL;
	int alea = RandHelper::rand(streets.size());
	return &(streets.at(alea));
}

/****************************************************************************/
