/****************************************************************************/
/// @file    NBTypeCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for the available types of an edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBTypeCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NBTypeCont::setDefaults(int defaultNoLanes,
                        SUMOReal defaultSpeed,
                        int defaultPriority) {
    myDefaultType.noLanes = defaultNoLanes;
    myDefaultType.speed = defaultSpeed;
    myDefaultType.priority = defaultPriority;
}


bool
NBTypeCont::insert(const std::string& id, int noLanes, SUMOReal maxSpeed, int prio,
                   SUMOReal width, SUMOVehicleClass vClass, bool oneWayIsDefault, SUMOReal sidewalkWidth) {
    SVCPermissions permissions = (vClass == SVC_IGNORING ? SVCAll : vClass);
    return insert(id, noLanes, maxSpeed, prio, permissions, width, oneWayIsDefault, sidewalkWidth);
}


bool
NBTypeCont::insert(const std::string& id, int noLanes, SUMOReal maxSpeed, int prio,
                   SVCPermissions permissions, SUMOReal width, bool oneWayIsDefault, SUMOReal sidewalkWidth) {
    TypesCont::iterator i = myTypes.find(id);
    if (i != myTypes.end()) {
        return false;
    }
    myTypes[id] = TypeDefinition(noLanes, maxSpeed, prio, width, permissions, oneWayIsDefault, sidewalkWidth);
    return true;
}


bool
NBTypeCont::knows(const std::string& type) const {
    return myTypes.find(type) != myTypes.end();
}


bool
NBTypeCont::markAsToDiscard(const std::string& id) {
    TypesCont::iterator i = myTypes.find(id);
    if (i == myTypes.end()) {
        return false;
    }
    (*i).second.discard = true;
    return true;
}


// ------------ Type-dependant Retrieval methods
int
NBTypeCont::getNumLanes(const std::string& type) const {
    return getType(type).noLanes;
}


SUMOReal
NBTypeCont::getSpeed(const std::string& type) const {
    return getType(type).speed;
}


int
NBTypeCont::getPriority(const std::string& type) const {
    return getType(type).priority;
}


bool
NBTypeCont::getIsOneWay(const std::string& type) const {
    return getType(type).oneWay;
}


bool
NBTypeCont::getShallBeDiscarded(const std::string& type) const {
    return getType(type).discard;
}


SVCPermissions
NBTypeCont::getPermissions(const std::string& type) const {
    return getType(type).permissions;
}


SUMOReal
NBTypeCont::getWidth(const std::string& type) const {
    return getType(type).width;
}


SUMOReal
NBTypeCont::getSidewalkWidth(const std::string& type) const {
    return getType(type).sidewalkWidth;
}


const NBTypeCont::TypeDefinition&
NBTypeCont::getType(const std::string& name) const {
    TypesCont::const_iterator i = myTypes.find(name);
    if (i == myTypes.end()) {
        return myDefaultType;
    }
    return (*i).second;
}

/****************************************************************************/

