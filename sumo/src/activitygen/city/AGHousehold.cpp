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
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// method definitions
// ===========================================================================
void
AGHousehold::generatePeople() {
    AGDataAndStatistics *ds = &(myCity->statData);
    //the first adult
    AGAdult pers(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitEndAge));
    adults.push_back(pers);

    //the second adult
    if (decisionProba(ds->secondPersProb)) {
        if (pers.getAge() < ds->limitAgeRetirement) {
            AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeChildren, ds->limitAgeRetirement));
            adults.push_back(pers2);
        } else {
            AGAdult pers2(ds->getRandomPopDistributed(ds->limitAgeRetirement, ds->limitEndAge));
            adults.push_back(pers2);
        }
    }

    //Children
    if (pers.getAge() < ds->limitAgeRetirement) {
        int numChild = ds->getPoissonsNumberOfChildren(ds->meanNbrChildren);
        while (numChild > 0) {
            AGChild chl(ds->getRandomPopDistributed(0, ds->limitAgeChildren));
            children.push_back(chl);
            --numChild;
        }
    }
}

void
AGHousehold::generateCars(SUMOReal rate) {
    int peopleInNeed = static_cast<int>(adults.size()) - static_cast<int>(cars.size());
    while (peopleInNeed > 0) {
        if (decisionProba(rate)) {
            addACar();
        }
        --peopleInNeed;
    }
}

void
AGHousehold::addACar() {
    int numCar = static_cast<int>(cars.size() + 1);
    cars.push_back(AGCar(idHH, numCar));
}

int
AGHousehold::getCarNbr() {
    return static_cast<int>(cars.size());
}

int
AGHousehold::getPeopleNbr() {
    return static_cast<int>(adults.size() + children.size());
}

int
AGHousehold::getAdultNbr() {
    return static_cast<int>(adults.size());
}

bool
AGHousehold::isCloseFromPubTransport(std::list<AGPosition> *pubTransport) {
    SUMOReal distToPT = location.minDistanceTo(*pubTransport);
    if (distToPT > myCity->statData.maxFootDistance)
        return false;
    return true;
}

bool
AGHousehold::isCloseFromPubTransport(std::map<int, AGPosition> *pubTransport) {
    SUMOReal distToPT = location.minDistanceTo(*pubTransport);
    if (distToPT > myCity->statData.maxFootDistance)
        return false;
    return true;
}

void
AGHousehold::regenerate() {
    //only allocation of work or school to people will change
    std::list<AGChild>::iterator itC;
    std::list<AGAdult>::iterator itA;
    for (itC=children.begin() ; itC != children.end() ; ++itC) {
        if (itC->haveASchool()) {
            if (itC->leaveSchool())
                itC->alocateASchool(&(myCity->schools), getPosition());
        } else
            itC->alocateASchool(&(myCity->schools), getPosition());
    }
    for (itA=adults.begin() ; itA!=adults.end() ; ++itA) {
        if (itA->isWorking())
            itA->resignFromWorkPosition();

        if (myCity->statData.workPositions > 0) {
            itA->tryToWork(1-myCity->statData.unemployement, &(myCity->workPositions));

        } else {
            std::cout << "Not enough work positions in AGHousehold::regenerate. Should not happen!" << std::endl;
        }
    }
}

bool
AGHousehold::allocateChildrenSchool() {
    std::list<AGChild>::iterator it;
    bool oneRemainsAtHome = false;

    for (it = children.begin() ; it != children.end() ; ++it) {
        if (!it->alocateASchool(&(myCity->schools), location))
            oneRemainsAtHome = true;
    }
    return !oneRemainsAtHome;
}

bool
AGHousehold::allocateAdultsWork() {
    std::list<AGAdult>::iterator it;
    for (it=adults.begin() ; it!=adults.end() ; ++it) {
        if (myCity->statData.workPositions <= 0) {
            std::cout << "Not enough free work positions in AGHousehold::allocateAdultsWork. Should not happen." << std::endl;
            return false;

        } else {
            it->tryToWork(1-myCity->statData.unemployement, &(myCity->workPositions));
        }
    }
    return true;
}

bool
AGHousehold::decisionProba(SUMOReal p) {
    return (RandHelper::rand()<p);
}

AGPosition
AGHousehold::getPosition() {
    return location;
}

AGCity*
AGHousehold::getTheCity() {
    return myCity;
}

bool
AGHousehold::retiredHouseholders() {
    return (adults.front().getAge() >= myCity->statData.limitAgeRetirement);
}

/****************************************************************************/
