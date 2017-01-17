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

#include <string>
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBTypeCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NBTypeCont::setDefaults(int defaultNumLanes,
                        SUMOReal defaultLaneWidth,
                        SUMOReal defaultSpeed,
                        int defaultPriority,
                        SVCPermissions defaultPermissions) {
    myDefaultType.numLanes = defaultNumLanes;
    myDefaultType.width = defaultLaneWidth;
    myDefaultType.speed = defaultSpeed;
    myDefaultType.priority = defaultPriority;
    myDefaultType.permissions = defaultPermissions;
}


void
NBTypeCont::insert(const std::string& id, int numLanes, SUMOReal maxSpeed, int prio,
                   SVCPermissions permissions, SUMOReal width, bool oneWayIsDefault, SUMOReal sidewalkWidth, SUMOReal bikeLaneWidth) {

    TypeDefinition newType(numLanes, maxSpeed, prio, width, permissions, oneWayIsDefault, sidewalkWidth, bikeLaneWidth);
    TypesCont::iterator old = myTypes.find(id);
    if (old != myTypes.end()) {
        newType.restrictions.insert(old->second.restrictions.begin(), old->second.restrictions.end());
        newType.attrs.insert(old->second.attrs.begin(), old->second.attrs.end());
    }
    myTypes[id] = newType;
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


bool
NBTypeCont::markAsSet(const std::string& id, const SumoXMLAttr attr) {
    TypesCont::iterator i = myTypes.find(id);
    if (i == myTypes.end()) {
        return false;
    }
    (*i).second.attrs.insert(attr);
    return true;
}


bool
NBTypeCont::addRestriction(const std::string& id, const SUMOVehicleClass svc, const SUMOReal speed) {
    TypesCont::iterator i = myTypes.find(id);
    if (i == myTypes.end()) {
        return false;
    }
    (*i).second.restrictions[svc] = speed;
    return true;
}


bool
NBTypeCont::copyRestrictionsAndAttrs(const std::string& fromId, const std::string& toId) {
    TypesCont::iterator from = myTypes.find(fromId);
    TypesCont::iterator to = myTypes.find(toId);
    if (from == myTypes.end() || to == myTypes.end()) {
        return false;
    }
    to->second.restrictions.insert(from->second.restrictions.begin(), from->second.restrictions.end());
    to->second.attrs.insert(from->second.attrs.begin(), from->second.attrs.end());
    return true;
}


void
NBTypeCont::writeTypes(OutputDevice& into) const {
    for (TypesCont::const_iterator i = myTypes.begin(); i != myTypes.end(); ++i) {
        into.openTag(SUMO_TAG_TYPE);
        into.writeAttr(SUMO_ATTR_ID, i->first);
        const NBTypeCont::TypeDefinition& type = i->second;
        if (type.attrs.count(SUMO_ATTR_PRIORITY) > 0) {
            into.writeAttr(SUMO_ATTR_PRIORITY, type.priority);
        }
        if (type.attrs.count(SUMO_ATTR_NUMLANES) > 0) {
            into.writeAttr(SUMO_ATTR_NUMLANES, type.numLanes);
        }
        if (type.attrs.count(SUMO_ATTR_SPEED) > 0) {
            into.writeAttr(SUMO_ATTR_SPEED, type.speed);
        }
        if (type.attrs.count(SUMO_ATTR_DISALLOW) > 0 || type.attrs.count(SUMO_ATTR_ALLOW) > 0) {
            writePermissions(into, type.permissions);
        }
        if (type.attrs.count(SUMO_ATTR_ONEWAY) > 0) {
            into.writeAttr(SUMO_ATTR_ONEWAY, type.oneWay);
        }
        if (type.attrs.count(SUMO_ATTR_DISCARD) > 0) {
            into.writeAttr(SUMO_ATTR_DISCARD, type.discard);
        }
        if (type.attrs.count(SUMO_ATTR_WIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_WIDTH, type.width);
        }
        if (type.attrs.count(SUMO_ATTR_SIDEWALKWIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_SIDEWALKWIDTH, type.sidewalkWidth);
        }
        if (type.attrs.count(SUMO_ATTR_BIKELANEWIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_BIKELANEWIDTH, type.bikeLaneWidth);
        }
        for (std::map<SUMOVehicleClass, SUMOReal>::const_iterator j = type.restrictions.begin(); j != type.restrictions.end(); ++j) {
            into.openTag(SUMO_TAG_RESTRICTION);
            into.writeAttr(SUMO_ATTR_VCLASS, getVehicleClassNames(j->first));
            into.writeAttr(SUMO_ATTR_SPEED, j->second);
            into.closeTag();
        }
        into.closeTag();
    }
    if (!myTypes.empty()) {
        into.lf();
    }
}


// ------------ Type-dependant Retrieval methods
int
NBTypeCont::getNumLanes(const std::string& type) const {
    return getType(type).numLanes;
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


bool
NBTypeCont::wasSet(const std::string& type, const SumoXMLAttr attr) const {
    return getType(type).attrs.count(attr) > 0;
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


SUMOReal
NBTypeCont::getBikeLaneWidth(const std::string& type) const {
    return getType(type).bikeLaneWidth;
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

