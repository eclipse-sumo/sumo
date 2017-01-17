/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Represents a SUMO edge and contains people and work densities
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

#include "AGStreet.h"
#include "router/ROEdge.h"
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGStreet::AGStreet(const std::string& id, RONode* from, RONode* to, int index, const int priority) :
    ROEdge(id, from, to, index, priority), myPopulation(0.), myNumWorkplaces(0.) {
}


void
AGStreet::print() const {
    std::cout << "- AGStreet: Name=" << getID() << " Length=" << getLength() << " pop=" << myPopulation << " work=" << myNumWorkplaces << std::endl;
}


SUMOReal
AGStreet::getPopulation() const {
    return myPopulation;
}


void
AGStreet::setPopulation(const SUMOReal population) {
    myPopulation = population;
}


SUMOReal
AGStreet::getWorkplaceNumber() const {
    return myNumWorkplaces;
}


void
AGStreet::setWorkplaceNumber(const SUMOReal workPositions) {
    myNumWorkplaces = workPositions;
}


bool
AGStreet::allows(const SUMOVehicleClass vclass) const {
    return (getPermissions() & vclass) == vclass;
}


/****************************************************************************/
