/****************************************************************************/
/// @file    NBTypeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for the available types of an edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBTypeCont.h"
#include "NBJunctionTypesMatrix.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NBTypeCont::setDefaults(int defaultNoLanes,
                        SUMOReal defaultSpeed,
                        int defaultPriority) throw() {
    myDefaultType.noLanes = defaultNoLanes;
    myDefaultType.speed = defaultSpeed;
    myDefaultType.priority = defaultPriority;
}


bool
NBTypeCont::insert(const std::string& id, int noLanes, SUMOReal maxSpeed, int prio,
                   SUMOReal width, SUMOVehicleClass vClass, bool oneWayIsDefault) throw() {
    SUMOVehicleClasses allow;
    if (vClass != SVC_UNKNOWN) {
        allow.insert(vClass);
    }
    return insert(id, noLanes, maxSpeed, prio, allow, SUMOVehicleClasses(), width, oneWayIsDefault);
}


bool
NBTypeCont::insert(const std::string& id, int noLanes, SUMOReal maxSpeed, int prio,
                   const SUMOVehicleClasses& allow, const SUMOVehicleClasses& disallow,
                   SUMOReal width, bool oneWayIsDefault) throw() {
    TypesCont::iterator i = myTypes.find(id);
    if (i != myTypes.end()) {
        return false;
    }
    NBTypeCont::TypeDefinition td(noLanes, maxSpeed, prio, width);
    td.allowed.insert(allow.begin(), allow.end());
    td.notAllowed.insert(disallow.begin(), disallow.end());
    td.oneWay = oneWayIsDefault;
    myTypes[id] = td;
    return true;
}


bool
NBTypeCont::knows(const std::string& type) const throw() {
    return myTypes.find(type) != myTypes.end();
}


SumoXMLNodeType
NBTypeCont::getJunctionType(SUMOReal speed1, SUMOReal speed2) const throw() {
    return myJunctionTypes.getType(speed1, speed2);
}


bool
NBTypeCont::markAsToDiscard(const std::string& id) throw() {
    TypesCont::iterator i = myTypes.find(id);
    if (i == myTypes.end()) {
        return false;
    }
    (*i).second.discard = true;
    return true;
}


// ------------ Type-dependant Retrieval methods
int
NBTypeCont::getNumLanes(const std::string& type) const throw() {
    return getType(type).noLanes;
}


SUMOReal
NBTypeCont::getSpeed(const std::string& type) const throw() {
    return getType(type).speed;
}


int
NBTypeCont::getPriority(const std::string& type) const throw() {
    return getType(type).priority;
}


bool
NBTypeCont::getIsOneWay(const std::string& type) const throw() {
    return getType(type).oneWay;
}


bool
NBTypeCont::getShallBeDiscarded(const std::string& type) const throw() {
    return getType(type).discard;
}


const SUMOVehicleClasses&
NBTypeCont::getAllowedClasses(const std::string& type) const throw() {
    return getType(type).allowed;
}


const SUMOVehicleClasses&
NBTypeCont::getDisallowedClasses(const std::string& type) const throw() {
    return getType(type).notAllowed;
}


SUMOReal
NBTypeCont::getWidth(const std::string& type) const throw() {
    return getType(type).width;
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

