/****************************************************************************/
/// @file    MSJunction.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// The base class for an intersection
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;

// ===========================================================================
// static member definitions
// ===========================================================================

// ===========================================================================
// member method definition
// ===========================================================================
MSJunction::MSJunction(const std::string& id, SumoXMLNodeType type, const Position& position,
                       const PositionVector& shape) :
    Named(id),
    myType(type),
    myPosition(position),
    myShape(shape) {
}


MSJunction::~MSJunction() {}


const Position&
MSJunction::getPosition() const {
    return myPosition;
}


void
MSJunction::postloadInit() {}


void
MSJunction::passedJunction(const MSVehicle* vehicle) {
    myLinkLeaders.erase(vehicle);
}


bool
MSJunction::isLeader(const MSVehicle* ego, const MSVehicle* foe) {
    if (foe->getLane()->getEdge().getToJunction() != this) {
        // foe is already past the junction so is definitely a leader
        return true;
    }
    if (myLinkLeaders.find(ego) == myLinkLeaders.end() || myLinkLeaders[ego].count(foe) == 0) {
        // we are not yet the leader for foe, thus foe will be our leader
        myLinkLeaders[foe].insert(ego);
        return true;
    } else {
        return false;
    }
}
/****************************************************************************/

