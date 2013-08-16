/****************************************************************************/
/// @file    AGHousehold.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// A household contains the people and cars of the city: roughly represents
// families with their address, cars, adults and possibly children
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
// method definitions
// ===========================================================================
void
AGHousehold::generatePeople(int numAdults, int numChilds, bool firstRetired) {
    AGDataAndStatistics* ds = &(myCity->statData);
    //the first adult
    AGAdult pers(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitEndAge));
    if (firstRetired) {
        pers = AGAdult(ds->getRandomPopDistributed(ds->limitAgeRetirement, ds->limitEndAge));
    }
    myAdults.push_back(pers);
    //further adults
    while (static_cast<int>(myAdults.size()) < numAdults) {
        if (firstRetired) {
            AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeRetirement, ds->limitEndAge));
            myAdults.push_back(pers2);
        } else {
            AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitAgeRetirement));
            myAdults.push_back(pers2);
        }
    }
    //Children
    while (static_cast<int>(myChildren.size()) < numChilds) {
        AGChild chl(ds->getRandomPopDistributed(0, ds->limitAgeChildren));
        myChildren.push_back(chl);
    }
}

void
AGHousehold::generateCars(SUMOReal rate) {
    int peopleInNeed = static_cast<int>(myAdults.size()) - static_cast<int>(myCars.size());
    while (peopleInNeed > 0) {
        if (RandHelper::rand() < rate) {
            addACar();
        }
        --peopleInNeed;
    }
}

void
AGHousehold::addACar() {
    int numCar = static_cast<int>(myCars.size() + 1);
    myCars.push_back(AGCar(myId, numCar));
}

int
AGHousehold::getCarNbr() {
    return static_cast<int>(myCars.size());
}

unsigned int
AGHousehold::getPeopleNbr() {
    return static_cast<unsigned int>(myAdults.size() + myChildren.size());
}

unsigned int
AGHousehold::getAdultNbr() {
    return static_cast<unsigned int>(myAdults.size());
}

const std::list<AGAdult>&
AGHousehold::getAdults() const {
    return myAdults;
}

const std::list<AGChild>&
AGHousehold::getChildren() const {
    return myChildren;
}

const std::list<AGCar>&
AGHousehold::getCars() const {
    return myCars;
}

bool
AGHousehold::isCloseFromPubTransport(std::list<AGPosition>* pubTransport) {
    SUMOReal distToPT = myLocation.minDistanceTo(*pubTransport);
    if (distToPT > myCity->statData.maxFootDistance) {
        return false;
    }
    return true;
}

bool
AGHousehold::isCloseFromPubTransport(std::map<int, AGPosition>* pubTransport) {
    SUMOReal distToPT = myLocation.minDistanceTo(*pubTransport);
    if (distToPT > myCity->statData.maxFootDistance) {
        return false;
    }
    return true;
}

void
AGHousehold::regenerate() {
    //only allocation of work or school to people will change
    std::list<AGChild>::iterator itC;
    std::list<AGAdult>::iterator itA;
    for (itC = myChildren.begin(); itC != myChildren.end(); ++itC) {
        if (itC->haveASchool()) {
            if (itC->leaveSchool()) {
                itC->allocateASchool(&(myCity->schools), getPosition());
            }
        } else {
            itC->allocateASchool(&(myCity->schools), getPosition());
        }
    }
    for (itA = myAdults.begin(); itA != myAdults.end(); ++itA) {
        if (itA->isWorking()) {
            itA->resignFromWorkPosition();
        }

        if (myCity->statData.workPositions > 0) {
            itA->tryToWork(1 - myCity->statData.unemployement, &(myCity->workPositions));

        } else {
            std::cout << "Not enough work positions in AGHousehold::regenerate. Should not happen!" << std::endl;
        }
    }
}

bool
AGHousehold::allocateChildrenSchool() {
    std::list<AGChild>::iterator it;
    bool oneRemainsAtHome = false;

    for (it = myChildren.begin(); it != myChildren.end(); ++it) {
        if (!it->allocateASchool(&(myCity->schools), myLocation)) {
            oneRemainsAtHome = true;
        }
    }
    return !oneRemainsAtHome;
}

bool
AGHousehold::allocateAdultsWork() {
    std::list<AGAdult>::iterator it;
    for (it = myAdults.begin(); it != myAdults.end(); ++it) {
        if (myCity->statData.workPositions <= 0) {
            std::cout << "Not enough free work positions in AGHousehold::allocateAdultsWork. Should not happen." << std::endl;
            return false;

        } else {
            it->tryToWork(1 - myCity->statData.unemployement, &(myCity->workPositions));
        }
    }
    return true;
}

AGPosition
AGHousehold::getPosition() {
    return myLocation;
}

AGCity*
AGHousehold::getTheCity() {
    return myCity;
}

bool
AGHousehold::retiredHouseholders() {
    return (myAdults.front().getAge() >= myCity->statData.limitAgeRetirement);
}

/****************************************************************************/
