/****************************************************************************/
/// @file    NBTypeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for the available types of an edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
NBTypeCont::setDefaults(int defaultNoLanes,
                        SUMOReal defaultSpeed,
                        int defaultPriority)
{
    myDefaultType.noLanes = defaultNoLanes;
    myDefaultType.speed = defaultSpeed;
    myDefaultType.priority = defaultPriority;
}


bool
NBTypeCont::insert(const std::string &id, int noLanes, SUMOReal maxSpeed, int prio,
                   SUMOVehicleClass vClasses, bool oneWayIsDefault)
{
    TypesCont::iterator i = myTypes.find(id);
    if (i!=myTypes.end()) {
        return false;
    }
    NBTypeCont::TypeDefinition td(noLanes, maxSpeed, prio);
    if (vClasses!=SVC_UNKNOWN) {
        td.allowed.push_back(vClasses);
    }
    td.oneWay = oneWayIsDefault;
    myTypes[id] = td;
    return true;
}


int
NBTypeCont::getNoLanes(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.noLanes;
    }
    return (*i).second.noLanes;
}


SUMOReal
NBTypeCont::getSpeed(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.speed;
    }
    return (*i).second.speed;
}


int
NBTypeCont::getPriority(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.priority;
    }
    return (*i).second.priority;
}


bool
NBTypeCont::getIsOneWay(const std::string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.oneWay;
    }
    return (*i).second.oneWay;
}


const std::vector<SUMOVehicleClass> &
NBTypeCont::getAllowedClasses(const std::string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.allowed;
    }
    return (*i).second.allowed;
}


const std::vector<SUMOVehicleClass> &
NBTypeCont::getDisallowedClasses(const std::string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return myDefaultType.notAllowed;
    }
    return (*i).second.notAllowed;
}


bool
NBTypeCont::knows(const std::string &type) const
{
    return myTypes.find(type)!=myTypes.end();
}


int
NBTypeCont::getDefaultNoLanes()
{
    return myDefaultType.noLanes;
}


int
NBTypeCont::getDefaultPriority()
{
    return myDefaultType.priority;
}


SUMOReal
NBTypeCont::getDefaultSpeed()
{
    return myDefaultType.speed;
}


size_t
NBTypeCont::size()
{
    return myTypes.size();
}


NBNode::BasicNodeType
NBTypeCont::getJunctionType(SUMOReal speed1, SUMOReal speed2) const
{
    return myJunctionTypes.getType(speed1, speed2);
}


/****************************************************************************/

