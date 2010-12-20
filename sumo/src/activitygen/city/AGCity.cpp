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
// method definitions
// ===========================================================================
void
AGCity::completeStreets() {
    if (streetsCompleted)
        return;
    else
        streetsCompleted = true;

    NrStreets = 0;
    int pop=0, work=0;
    std::vector<AGStreet>::iterator it;

    for (it = streets.begin() ; it!=streets.end() ; ++it) {
        pop += (int)(it->getPopulation());
        work += (int)(it->getWorkplaceNumber());
        ++NrStreets;
    }
    statData.factorInhabitants = (float)statData.inhabitants / (float)pop;
    //can be improved with other input data
    SUMOReal neededWorkPositionsInCity = (1.0 - statData.unemployement)
                                         * ((float)statData.getPeopleYoungerThan(statData.limitAgeRetirement)
                                            - (float)statData.getPeopleYoungerThan(statData.limitAgeChildren))
                                         + (float)statData.incomingTraffic;
    // we generate 5% more work positions that really needed: to avoid any expensive research of random work positions
    neededWorkPositionsInCity *= 1.05f;
    statData.workPositions = (int)neededWorkPositionsInCity;
    statData.factorWorkPositions = neededWorkPositionsInCity / (float) work;

    for (it = streets.begin() ; it!=streets.end() ; ++it) {
        it->setPopulation((int)(it->getPopulation() * statData.factorInhabitants));
        it->setWorkplaceNumber((int)(it->getWorkplaceNumber() * statData.factorWorkPositions));
        //it->print();
    }

    //completing streets from edges of the network not handled/present in STAT file (no population no work position)
    std::map<std::string, ROEdge*>::const_iterator itE;
    std::vector<AGStreet>::iterator itS;

    for (itE = net->getEdgeMap().begin() ; itE != net->getEdgeMap().end() ; ++itE) {
        for (itS = streets.begin() ; itS != streets.end() ; ++itS) {
            if (itS->getName() == itE->second->getID()) {
                break;
            }
        }
        //if this edge isn't represented by a street
        if (itS == streets.end()) {
            streets.push_back(AGStreet(itE->second));
        }
    }
}

void
AGCity::generateWorkPositions() {
    std::vector<AGStreet>::iterator it;
    int workPositionCounter = 0;

    try {
        for (it = streets.begin() ; it != streets.end() ; ++it) {
            //std::cout << "number of work positions in street: " << it->getWorkplaceNumber() << std::endl;
            for (int i=0 ; i<it->getWorkplaceNumber() ; ++i) {
                workPositions.push_back(AGWorkPosition(*it, &statData));
                ++workPositionCounter;
            }
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "Number of work positions at bad_alloc exception: " << workPositionCounter << std::endl;
        throw(e);
    }
    //std::cout << "Inner work positions done. " << workPositionCounter << " generated." << std::endl;

    // Work positions outside the city
    generateOutgoingWP();
    std::cout << "--> work position: " << std::endl;
    std::cout << "  |-> in city: " << workPositionCounter << std::endl;
    std::cout << "  |-> out city: " << statData.workPositions - workPositionCounter << std::endl;
    std::cout << "  |-> in+out city: " << statData.workPositions << std::endl;
}

void
AGCity::generateOutgoingWP() {
    // work positions outside the city
    SUMOReal nbrWorkers = static_cast<SUMOReal>(statData.getPeopleYoungerThan(statData.limitAgeRetirement) - statData.getPeopleYoungerThan(statData.limitAgeChildren));
    if (nbrWorkers <= 0)
        return;
    nbrWorkers *= (1.0 - statData.unemployement);
    /**
     * N_out = N_in * (ProportionOut / (1 - ProportionOut)) = N_out = N_in * (Noutworkers / (Nworkers - Noutworkers))
     */
    int nbrOutWorkPositions = static_cast<int>(workPositions.size() * (static_cast<float>(statData.outgoingTraffic))/(nbrWorkers - static_cast<float>(statData.outgoingTraffic)));

    if (cityGates.empty())
        return;

    for (int i=0 ; i<nbrOutWorkPositions ; ++i) {
        int posi = statData.getRandomCityGateByOutgoing();
        workPositions.push_back(AGWorkPosition(cityGates[posi].getStreet(), cityGates[posi].getPosition(), &statData));
    }
    //cout << "outgoing traffic: " << statData.outgoingTraffic << std::endl;
    //cout << "total number of workers in the city: " << nbrWorkers << std::endl;
    //cout << "work positions out side the city: " << nbrOutWorkPositions << std::endl;
    //cout << "work positions in and out of the city: " << workPositions.size() << std::endl;
    statData.workPositions = workPositions.size();
}

void
AGCity::completeBusLines() {
    std::list<AGBusLine>::iterator it;
    for (it=busLines.begin() ; it!=busLines.end() ; ++it) {
        //it->generateOpositDirection();
        it->setBusNames();
    }
}

void
AGCity::generatePopulation() {
    std::vector<AGStreet>::iterator it;
    int people;
    nbrCars = 0;
    int idHouseholds = 0;

    for (it=streets.begin() ; it!=streets.end() ; ++it) {
        people = it->getPopulation();
        while (people > 0) {
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
    std::list<AGHousehold>::iterator itt;
    for (itt=households.begin() ; itt != households.end() ; ++itt) {
        if (itt->getAdultNbr() == 1) {
            nbrSingle++;
            if (itt->adults.front().isWorking())
                workingP++;
        }
        if (itt->getAdultNbr() == 2) {
            nbrCouple += 2;
            if (itt->adults.front().isWorking())
                workingP++;
            if (itt->adults.back().isWorking())
                workingP++;
        }
        nbrChild += itt->getPeopleNbr() - itt->getAdultNbr();
        nbrHH++;
    }
    //cout << "number hh: " << nbrHH << std::endl;
    //cout << "number single: " << nbrSingle << std::endl;
    //cout << "number couple: " << nbrCouple << std::endl;
    //cout << "number 3 or more: " << nbr3More << std::endl;
    //cout << "number adults: " << nbrSingle + nbrCouple + nbr3More << std::endl;
    //cout << "number children: " << nbrChild << std::endl;
    //cout << "number people: " << nbrSingle + nbrCouple + nbr3More + nbrChild << std::endl;
    //END TEST

    std::cout << "--> population: " << std::endl;
    std::cout << "  |-> city households: " << nbrHH << std::endl;
    std::cout << "  |-> city people: " << nbrSingle + nbrCouple + nbrChild << std::endl;
    std::cout << "    |-> city single: " << nbrSingle << " / (in) couple: " << nbrCouple << std::endl;
    std::cout << "    |-> city adults: " << nbrSingle + nbrCouple << std::endl;
    std::cout << "      |-> estimation: " << statData.getPeopleOlderThan(statData.limitAgeChildren) << std::endl;
    std::cout << "      |-> retired: " << statData.getPeopleOlderThan(statData.limitAgeRetirement) << std::endl;
    std::cout << "    |-> city children: " << nbrChild << std::endl;
    std::cout << "      |-> estimation: " << statData.getPeopleYoungerThan(statData.limitAgeChildren) << std::endl;

}

void
AGCity::generateIncomingPopulation() {
    for (int i=0 ; i<statData.incomingTraffic ; ++i) {
        AGAdult ad(statData.getRandomPopDistributed(statData.limitAgeChildren, statData.limitAgeRetirement));
        peopleIncoming.push_back(ad);
    }
}

void
AGCity::schoolAllocation() {
    std::list<AGHousehold>::iterator it;
    bool shortage;
    for (it = households.begin() ; it != households.end() ; ++it) {
        shortage = !it->allocateChildrenSchool();
        if (shortage) {
            /*ofstream fichier("test.txt", ios::app);  // ouverture en écriture avec effacement du fichier ouvert
            if(fichier)
            {
            	fichier << "===> WARNING: Not enough school places in the city for all children..." << std::endl;
            	fichier.close();
            }
            else
            	cerr << "Impossible d'ouvrir le fichier !" << std::endl;*/

            //std::cout << "===> WARNING: Not enough school places in the city for all children..." << std::endl;
        }
    }
}

void
AGCity::workAllocation() {
    statData.AdultNbr = 0;
    //end tests
    /**
     * people from the city
     */
    std::list<AGHousehold>::iterator it;
    bool shortage;

    for (it = households.begin() ; it != households.end() ; ++it) {
        if (it->retiredHouseholders())
            continue;
        shortage = !it->allocateAdultsWork();
        if (shortage) {
            std::cout << "===> ERROR: Not enough work positions in the city for all working people..." << std::endl;
        }
        statData.AdultNbr += it->getAdultNbr(); //TESTING
    }

    /**
     * people from outside
     */
    std::list<AGAdult>::iterator itA;
    for (itA=peopleIncoming.begin() ; itA!=peopleIncoming.end() ; ++itA) {
        if (statData.workPositions > 0) {
            itA->tryToWork(1, &workPositions);
        } else {
            //shouldn't happen
            std::cout << "not enough work for incoming people..." << std::endl;
        }
    }

    //BEGIN TESTS
    int workingP = 0;
    std::list<AGHousehold>::iterator itt;
    for (itt=households.begin() ; itt != households.end() ; ++itt) {
        if (itt->getAdultNbr() == 1) {
            if (itt->adults.front().isWorking())
                workingP++;
        }
        if (itt->getAdultNbr() == 2) {
            if (itt->adults.front().isWorking())
                workingP++;
            if (itt->adults.back().isWorking())
                workingP++;
        }
    }
    std::cout << "  |-> working people: " << peopleIncoming.size() + workingP << std::endl;
    std::cout << "    |-> working people in city: " << workingP << std::endl;
    std::cout << "    |-> working people from outside: " << peopleIncoming.size() << std::endl;
    //END TESTS
}

void
AGCity::carAllocation() {
    statData.hhFarFromPT = 0;
    nbrCars = 0;
    std::list<AGHousehold>::iterator it;
    for (it=households.begin() ; it!=households.end() ; ++it) {
        if (!it->isCloseFromPubTransport(&(statData.busStations))) {
            statData.hhFarFromPT++;
            nbrCars++;
            it->addACar();
        }
        statData.householdsNbr++;
    }
    // new rate: the rate on the people that have'nt any car yet:
    // nR = (R * Drivers - AlreadyCars) / (Drivers - AlreadyCars)
    SUMOReal newRate = (statData.carRate * statData.getPeopleOlderThan(statData.limitAgeChildren) - statData.hhFarFromPT) / (statData.getPeopleOlderThan(statData.limitAgeChildren) - statData.hhFarFromPT);
    //std::cout << " - " << newRate << std::endl;
    if (newRate < 0 || newRate >= 1)
        newRate = 0;

    nbrCars = 0;
    int nbrAdults = 0;
    for (it=households.begin() ; it!=households.end() ; ++it) {
        it->generateCars(newRate);
        nbrCars += it->getCarNbr();
        nbrAdults += it->getAdultNbr();
    }
    //TEST RESULTS
    //std::cout << "number of cars: " << nbrCars << std::endl;
    //std::cout << "number of adults: " << statData.getPeopleOlderThan(statData.limitAgeChildren) << std::endl;
    //std::cout << "real number of adults: " << nbrAdults << std::endl;
    //std::cout << "number of people far from public transport: " << statData.hhFarFromPT << std::endl;
    //std::cout << "original rate: " << setprecision(4) << statData.carRate << std::endl;
    //std::cout << "new rate: " << setprecision(4) << newRate << std::endl;
    //std::cout << "real rate: " << setprecision(4) << (float)nbrCars / (float)statData.getPeopleOlderThan(statData.limitAgeChildren) << std::endl;
    //END TEST RESULTS
}

const AGStreet&
AGCity::getStreet(const std::string& edge) {
    /**
     * verify if it is the first time this function is called
     * in this case, we have to complete the streets with the
     * network edges this means that streets are completely
     * loaded (no any more to be read from stat-file)
     */
    if (!streetsCompleted) {
        statData.consolidateStat();
        completeStreets();
        std::cout << "first completed in getStreet() of City: Consolidation of data not needed in ActivityGen any more" << std::endl;
    }
    //rest of the function
    std::vector<AGStreet>::iterator it = streets.begin();
    while (it != streets.end()) {
        if (it->getName() == edge)
            return *it;
        ++it;
    }
    std::cout << "===> ERROR: WRONG STREET EDGE (" << edge << ") given and not found in street set." << std::endl;
    throw(std::runtime_error("Street not found with edge id " + edge));
}

const AGStreet&
AGCity::getRandomStreet() {
    if (streets.empty())
        throw(std::runtime_error("No street found in this city"));
    int alea = RandHelper::rand(streets.size());
    return streets.at(alea);
}

/****************************************************************************/
