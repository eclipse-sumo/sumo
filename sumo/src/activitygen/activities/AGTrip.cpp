/****************************************************************************/
/// @file    AGTrip.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Class containing all information of a given trip (car, bus)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
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

#include "AGTrip.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
AGTrip::operator <(const AGTrip& trip) const {
    if (getDay() < trip.getDay()) {
        return true;
    }
    if (getDay() == trip.getDay())
        if (getTime() < trip.getTime()) {
            return true;
        }
    return false;
}

void
AGTrip::print() const {
    std::cout << "Trip: " << std::endl;
    std::cout << "\t-From= ";
    myFrom.print();
    std::cout << "\t-To= ";
    myTo.print();
    std::cout << "\t-At= " << myDepTime << " -Day= " << myDay << std::endl;
    std::cout << "\t-Vehicle= " << myVehicle << std::endl;
    std::cout << "\t-type= " << myType << std::endl;
}

void
AGTrip::addLayOver(AGPosition by) {
    myPassBy.push_back(by);
}

void
AGTrip::addLayOver(AGTrip& trip) {
    std::list<AGPosition>::iterator it;
    for (it = trip.myPassBy.begin(); it != trip.myPassBy.end(); ++it) {
        myPassBy.push_back(*it);
    }
    myPassBy.push_back(trip.myTo);
}

void
AGTrip::addLayOverWithoutDestination(AGTrip& trip) {
    std::list<AGPosition>::iterator it;
    for (it = trip.myPassBy.begin(); it != trip.myPassBy.end(); ++it) {
        myPassBy.push_back(*it);
    }
}

const std::list<AGPosition>*
AGTrip::getPassed() const {
    return &myPassBy;
}

const std::string&
AGTrip::getType() const {
    return myType;
}

void
AGTrip::setType(std::string type) {
    myType = type;
}

AGPosition
AGTrip::getDep() const {
    return myFrom;
}

AGPosition
AGTrip::getArr() const {
    return myTo;
}

int
AGTrip::getTime() const {
    return myDepTime;
}

int
AGTrip::getTimeTrip(SUMOReal secPerKm) const {
    SUMOReal dist = 0;
    std::list<AGPosition> positions;
    positions.push_back(myFrom);
    std::list<AGPosition>::const_iterator it;
    for (it = myPassBy.begin(); it != myPassBy.end(); ++it) {
        positions.push_back(*it);
    }
    positions.push_back(myTo);

    const AGPosition* temp = &positions.front();
    for (it = positions.begin(), ++it; it != positions.end(); ++it) {
        dist += temp->distanceTo(*it);
        temp = &*it;
    }
    return (int)(secPerKm * (dist / 1000.0));
}

int
AGTrip::getArrTime(SUMOReal secPerKm) const {
    return myDepTime + getTimeTrip(secPerKm);
}

int
AGTrip::getRideBackArrTime(SUMOReal secPerKm) const {
    return getArrTime(secPerKm) + (int)(secPerKm * myTo.distanceTo(myFrom) / 1000.0);
}

void
AGTrip::setDepTime(int time) {
    myDepTime = time;
}

int
AGTrip::estimateDepTime(int arrTime, SUMOReal secPerKm) const {
    return arrTime - getTimeTrip(secPerKm);
}

const std::string&
AGTrip::getVehicleName() const {
    return myVehicle;
}

void
AGTrip::setVehicleName(std::string name) {
    myVehicle = name;
}

void
AGTrip::setArr(AGPosition arrival) {
    myTo = AGPosition(arrival.getStreet(), arrival.getPosition());
}

void
AGTrip::setDep(AGPosition departure) {
    myFrom = AGPosition(departure.getStreet(), departure.getPosition());
}

bool
AGTrip::isDaily() const {
    return (myDay == 0);
}

int
AGTrip::getDay() const {
    return myDay;
}

void
AGTrip::setDay(int d) {
    myDay = d;
}

/****************************************************************************/
