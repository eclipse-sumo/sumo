/****************************************************************************/
/// @file    AGTrip.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Class containing all information of a given trip (car, bus)
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

#include "AGTrip.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
AGTrip::operator <(AGTrip& trip) {
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
AGTrip::print() {
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

std::list<AGPosition>*
AGTrip::getPassed() {
    return &myPassBy;
}

std::string
AGTrip::getType() {
    return myType;
}

void
AGTrip::setType(std::string type) {
    myType = type;
}

AGPosition
AGTrip::getDep() {
    return myFrom;
}

AGPosition
AGTrip::getArr() {
    return myTo;
}

int
AGTrip::getTime() {
    return myDepTime;
}

int
AGTrip::getTimeTrip(SUMOReal secPerKm) {
    SUMOReal dist = 0;
    std::list<AGPosition> positions;
    positions.push_back(myFrom);
    std::list<AGPosition>::iterator it;
    for (it = myPassBy.begin(); it != myPassBy.end(); ++it) {
        positions.push_back(*it);
    }
    positions.push_back(myTo);

    bool firstPass = true;
    AGPosition* temp;
    for (it = positions.begin(); it != positions.end(); ++it) {
        if (firstPass) {
            temp = &*it;
            continue;
        }
        dist += temp->distanceTo(*it);
        temp = &*it;
    }
    return (int)(secPerKm * (dist / 1000.0));
}

int
AGTrip::getArrTime(SUMOReal secPerKm) {
    return myDepTime + getTimeTrip(secPerKm);
}

int
AGTrip::getRideBackArrTime(SUMOReal secPerKm) {
    return getArrTime(secPerKm) + (int)(secPerKm * myTo.distanceTo(myFrom) / 1000.0);
}

void
AGTrip::setDepTime(int time) {
    myDepTime = time;
}

int
AGTrip::estimateDepTime(int arrTime, SUMOReal secPerKm) {
    return arrTime - getTimeTrip(secPerKm);
}

std::string
AGTrip::getVehicleName() {
    return myVehicle;
}

void
AGTrip::setVehicleName(std::string name) {
    myVehicle = name;
}

void
AGTrip::setArr(AGPosition arrival) {
    myTo = *new AGPosition(arrival.getStreet(), arrival.getPosition());
}

void
AGTrip::setDep(AGPosition departure) {
    myFrom = *new AGPosition(departure.getStreet(), departure.getPosition());
}

bool
AGTrip::isDaily() {
    return (myDay == 0);
}

int
AGTrip::getDay() {
    return myDay;
}

void
AGTrip::setDay(int d) {
    myDay = d;
}

/****************************************************************************/
