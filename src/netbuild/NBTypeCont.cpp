/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBTypeCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
///
// A storage for the available types of an edge
/****************************************************************************/
#include <config.h>

#include <string>
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>

#include "NBTypeCont.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// NBTypeCont::EdgeTypeDefinition - methods
// ---------------------------------------------------------------------------

NBTypeCont::LaneTypeDefinition::LaneTypeDefinition() :
    speed(NBEdge::UNSPECIFIED_SPEED),
    permissions(SVC_UNSPECIFIED),
    width(NBEdge::UNSPECIFIED_WIDTH) {
}


NBTypeCont::LaneTypeDefinition::LaneTypeDefinition(const EdgeTypeDefinition* edgeTypeDefinition) :
    speed(edgeTypeDefinition->speed),
    permissions(edgeTypeDefinition->permissions),
    width(edgeTypeDefinition->width) {
}


NBTypeCont::LaneTypeDefinition::LaneTypeDefinition(const double _speed, const double _width, SVCPermissions _permissions, const std::set<SumoXMLAttr> &_attrs) :
    speed(_speed),
    permissions(_permissions),
    width(_width),
    attrs(_attrs) {
}


NBTypeCont::LaneTypeDefinition::LaneTypeDefinition(const LaneTypeDefinition* laneTypeDefinition) :
    speed(laneTypeDefinition->speed),
    permissions(laneTypeDefinition->permissions),
    width(laneTypeDefinition->width),
    restrictions(laneTypeDefinition->restrictions),
    attrs(laneTypeDefinition->attrs) {
}

// ---------------------------------------------------------------------------
// NBTypeCont::EdgeTypeDefinition - methods
// ---------------------------------------------------------------------------

NBTypeCont::EdgeTypeDefinition::EdgeTypeDefinition() :
    speed((double) 13.89), priority(-1),
    permissions(SVC_UNSPECIFIED),
    spreadType(LaneSpreadFunction::RIGHT),
    oneWay(true), discard(false),
    width(NBEdge::UNSPECIFIED_WIDTH),
    widthResolution(0),
    maxWidth(0),
    minWidth(0),
    sidewalkWidth(NBEdge::UNSPECIFIED_WIDTH),
    bikeLaneWidth(NBEdge::UNSPECIFIED_WIDTH) {
    // set laneTypes
    laneTypeDefinitions.resize(1);
}


NBTypeCont::EdgeTypeDefinition::EdgeTypeDefinition(const EdgeTypeDefinition* edgeType) :
    speed(edgeType->speed), 
    priority(edgeType->priority),
    permissions(edgeType->permissions),
    spreadType(edgeType->spreadType),
    oneWay(edgeType->oneWay), 
    discard(edgeType->discard),
    width(edgeType->width),
    widthResolution(edgeType->widthResolution),
    maxWidth(edgeType->maxWidth),
    minWidth(edgeType->minWidth),
    sidewalkWidth(edgeType->sidewalkWidth),
    bikeLaneWidth(edgeType->bikeLaneWidth),
    restrictions(edgeType->restrictions),
    attrs(edgeType->attrs),
    laneTypeDefinitions(edgeType->laneTypeDefinitions) {
}


NBTypeCont::EdgeTypeDefinition::EdgeTypeDefinition(int numLanes, double _speed, int _priority,
        double _width, SVCPermissions _permissions, LaneSpreadFunction _spreadType, bool _oneWay, double _sideWalkWidth,
        double _bikeLaneWidth, double _widthResolution, double _maxWidth, double _minWidth) :
    speed(_speed), priority(_priority),
    permissions(_permissions),
    spreadType(_spreadType),
    oneWay(_oneWay),
    discard(false),
    width(_width),
    widthResolution(_widthResolution),
    maxWidth(_maxWidth),
    minWidth(_minWidth),
    sidewalkWidth(_sideWalkWidth),
    bikeLaneWidth(_bikeLaneWidth) {
    // set laneTypes
    laneTypeDefinitions.resize(numLanes);
}


bool
NBTypeCont::EdgeTypeDefinition::needsLaneType() const {
    for (const LaneTypeDefinition& laneType : laneTypeDefinitions) {
        if (laneType.attrs.count(SUMO_ATTR_SPEED) > 0 && laneType.speed != NBEdge::UNSPECIFIED_SPEED && laneType.speed != speed) {
            return true;
        }
        if ((laneType.attrs.count(SUMO_ATTR_DISALLOW) > 0 || laneType.attrs.count(SUMO_ATTR_ALLOW) > 0)
                && laneType.permissions != permissions) {
            return true;
        }
        if (laneType.attrs.count(SUMO_ATTR_WIDTH) > 0 && laneType.width != width && laneType.width != NBEdge::UNSPECIFIED_WIDTH) {
            return true;
        }
        if (laneType.restrictions.size() > 0) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// NBTypeCont - methods
// ---------------------------------------------------------------------------

NBTypeCont::NBTypeCont() :
    myDefaultType(new EdgeTypeDefinition()) {}


NBTypeCont::~NBTypeCont() {
    clearTypes();
    delete myDefaultType;
}


void
NBTypeCont::clearTypes() {
    // remove edge types
    for (const auto& edgeType : myEdgeTypes) {
        delete edgeType.second;
    }
    // clear edge types
    myEdgeTypes.clear();
}


void
NBTypeCont::setEdgeTypeDefaults(int defaultNumLanes,
                                double defaultLaneWidth,
                                double defaultSpeed,
                                int defaultPriority,
                                SVCPermissions defaultPermissions,
                                LaneSpreadFunction defaultSpreadType) {
    myDefaultType->laneTypeDefinitions.clear();
    myDefaultType->laneTypeDefinitions.resize(defaultNumLanes);
    myDefaultType->width = defaultLaneWidth;
    myDefaultType->speed = defaultSpeed;
    myDefaultType->priority = defaultPriority;
    myDefaultType->permissions = defaultPermissions;
    myDefaultType->spreadType = defaultSpreadType;
}


void
NBTypeCont::insertEdgeType(const std::string& id, int numLanes, double maxSpeed, int prio,
                           SVCPermissions permissions, LaneSpreadFunction spreadType, double width,
                           bool oneWayIsDefault, double sidewalkWidth, double bikeLaneWidth,
                           double widthResolution, double maxWidth, double minWidth) {
    // Create edge type definition
    EdgeTypeDefinition* newType = new EdgeTypeDefinition(numLanes, maxSpeed, prio, width, permissions, spreadType, oneWayIsDefault, sidewalkWidth, bikeLaneWidth, widthResolution, maxWidth, minWidth);
    // check if edgeType already exist in types
    TypesCont::iterator old = myEdgeTypes.find(id);
    // if exists, then update restrictions and attributes
    if (old != myEdgeTypes.end()) {
        newType->restrictions.insert(old->second->restrictions.begin(), old->second->restrictions.end());
        newType->attrs.insert(old->second->attrs.begin(), old->second->attrs.end());
        delete old->second;
    }
    // insert it in types
    myEdgeTypes[id] = newType;
}


void
NBTypeCont::insertEdgeType(const std::string& id, const EdgeTypeDefinition* edgeType) {
    // Create edge type definition
    EdgeTypeDefinition* newType = new EdgeTypeDefinition(edgeType);
    // check if edgeType already exist in types
    TypesCont::iterator old = myEdgeTypes.find(id);
    // if exists, then update restrictions and attributes
    if (old != myEdgeTypes.end()) {
        newType->restrictions.insert(old->second->restrictions.begin(), old->second->restrictions.end());
        newType->attrs.insert(old->second->attrs.begin(), old->second->attrs.end());
        delete old->second;
    }
    // insert it in types
    myEdgeTypes[id] = newType;
}


void
NBTypeCont::insertLaneType(const std::string& edgeTypeID, int index, double maxSpeed, SVCPermissions permissions,
                           double width, const std::set<SumoXMLAttr>& attrs) {
    EdgeTypeDefinition* et = myEdgeTypes.at(edgeTypeID);
    while ((int)et->laneTypeDefinitions.size() <= index) {
        et->laneTypeDefinitions.push_back(et);
    }
    // add LaneTypeDefinition with the given attributes
    et->laneTypeDefinitions[index] = LaneTypeDefinition(maxSpeed, width, permissions, attrs);
}


int
NBTypeCont::size() const {
    return (int)myEdgeTypes.size();
}


void
NBTypeCont::removeEdgeType(const std::string& id) {
    // check if edgeType already exist in types
    const auto it = myEdgeTypes.find(id);
    // if exists, then remove it
    if (it != myEdgeTypes.end()) {
        // remove it from map
        delete it->second;
        myEdgeTypes.erase(it);
    }
}


void
NBTypeCont::updateEdgeTypeID(const std::string& oldId, const std::string& newId) {
    // check if edgeType already exist in types
    const auto oldIt = myEdgeTypes.find(oldId);
    const auto newIt = myEdgeTypes.find(newId);
    // if exists, then remove it
    if ((oldIt != myEdgeTypes.end()) && (newIt == myEdgeTypes.end())) {
        // obtain pointer
        auto edgeType = oldIt->second;
        // remove it from map
        myEdgeTypes.erase(oldIt);
        // add it again
        myEdgeTypes[newId] = edgeType;
    }
}


NBTypeCont::TypesCont::const_iterator
NBTypeCont::begin() const {
    return myEdgeTypes.cbegin();
}


NBTypeCont::TypesCont::const_iterator
NBTypeCont::end() const {
    return myEdgeTypes.cend();
}


bool
NBTypeCont::knows(const std::string& type) const {
    return myEdgeTypes.find(type) != myEdgeTypes.end();
}


bool
NBTypeCont::markEdgeTypeAsToDiscard(const std::string& id) {
    TypesCont::iterator i = myEdgeTypes.find(id);
    if (i == myEdgeTypes.end()) {
        return false;
    }
    i->second->discard = true;
    return true;
}


bool
NBTypeCont::markEdgeTypeAsSet(const std::string& id, const SumoXMLAttr attr) {
    TypesCont::iterator i = myEdgeTypes.find(id);
    if (i == myEdgeTypes.end()) {
        return false;
    }
    i->second->attrs.insert(attr);
    return true;
}


bool
NBTypeCont::addEdgeTypeRestriction(const std::string& id, const SUMOVehicleClass svc, const double speed) {
    TypesCont::iterator i = myEdgeTypes.find(id);
    if (i == myEdgeTypes.end()) {
        return false;
    }
    i->second->restrictions[svc] = speed;
    return true;
}


bool
NBTypeCont::copyEdgeTypeRestrictionsAndAttrs(const std::string& fromId, const std::string& toId) {
    TypesCont::iterator from = myEdgeTypes.find(fromId);
    TypesCont::iterator to = myEdgeTypes.find(toId);
    if (from == myEdgeTypes.end() || to == myEdgeTypes.end()) {
        return false;
    }
    to->second->restrictions.insert(from->second->restrictions.begin(), from->second->restrictions.end());
    to->second->attrs.insert(from->second->attrs.begin(), from->second->attrs.end());
    return true;
}


bool
NBTypeCont::markLaneTypeAsSet(const std::string& id, int index, const SumoXMLAttr attr) {
    TypesCont::iterator i = myEdgeTypes.find(id);
    if (i == myEdgeTypes.end()) {
        return false;
    }
    i->second->laneTypeDefinitions[index].attrs.insert(attr);
    return true;
}


bool
NBTypeCont::addLaneTypeRestriction(const std::string& id, const SUMOVehicleClass svc, const double speed) {
    TypesCont::iterator i = myEdgeTypes.find(id);
    if (i == myEdgeTypes.end()) {
        return false;
    }
    i->second->laneTypeDefinitions.back().restrictions[svc] = speed;
    return true;
}


void
NBTypeCont::writeEdgeTypes(OutputDevice& into) const {
    // iterate over edge types
    for (const auto& edgeType : myEdgeTypes) {
        // open edge type tag
        into.openTag(SUMO_TAG_TYPE);
        // write ID
        into.writeAttr(SUMO_ATTR_ID, edgeType.first);
        // write priority
        if (edgeType.second->attrs.count(SUMO_ATTR_PRIORITY) > 0) {
            into.writeAttr(SUMO_ATTR_PRIORITY, edgeType.second->priority);
        }
        // write numLanes
        if (edgeType.second->attrs.count(SUMO_ATTR_NUMLANES) > 0 || edgeType.second->laneTypeDefinitions.size() > 1) {
            into.writeAttr(SUMO_ATTR_NUMLANES, edgeType.second->laneTypeDefinitions.size());
        }
        // write speed
        if (edgeType.second->attrs.count(SUMO_ATTR_SPEED) > 0) {
            into.writeAttr(SUMO_ATTR_SPEED, edgeType.second->speed);
        }
        // write permissions
        if ((edgeType.second->attrs.count(SUMO_ATTR_DISALLOW) > 0) || (edgeType.second->attrs.count(SUMO_ATTR_ALLOW) > 0)) {
            writePermissions(into, edgeType.second->permissions);
        }
        // write spreadType (unless default)
        if ((edgeType.second->attrs.count(SUMO_ATTR_SPREADTYPE) > 0) && edgeType.second->spreadType != LaneSpreadFunction::RIGHT) {
            into.writeAttr(SUMO_ATTR_SPREADTYPE, SUMOXMLDefinitions::LaneSpreadFunctions.getString(edgeType.second->spreadType));
        }
        // write oneWay
        if (edgeType.second->attrs.count(SUMO_ATTR_ONEWAY) > 0) {
            into.writeAttr(SUMO_ATTR_ONEWAY, edgeType.second->oneWay);
        }
        // write discard
        if (edgeType.second->attrs.count(SUMO_ATTR_DISCARD) > 0) {
            into.writeAttr(SUMO_ATTR_DISCARD, edgeType.second->discard);
        }
        // write width
        if (edgeType.second->attrs.count(SUMO_ATTR_WIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_WIDTH, edgeType.second->width);
        }
        // write sidewalkwidth
        if (edgeType.second->attrs.count(SUMO_ATTR_SIDEWALKWIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_SIDEWALKWIDTH, edgeType.second->sidewalkWidth);
        }
        // write bikelanewidth
        if (edgeType.second->attrs.count(SUMO_ATTR_BIKELANEWIDTH) > 0) {
            into.writeAttr(SUMO_ATTR_BIKELANEWIDTH, edgeType.second->bikeLaneWidth);
        }
        // write restrictions
        for (const auto& restriction : edgeType.second->restrictions) {
            // open restriction tag
            into.openTag(SUMO_TAG_RESTRICTION);
            // write vclass
            into.writeAttr(SUMO_ATTR_VCLASS, getVehicleClassNames(restriction.first));
            // write speed
            into.writeAttr(SUMO_ATTR_SPEED, restriction.second);
            // close restriction tag
            into.closeTag();
        }
        // iterate over lanes
        if (edgeType.second->needsLaneType()) {
            int index = 0;
            for (const auto& laneType : edgeType.second->laneTypeDefinitions) {
                // open lane type taG
                into.openTag(SUMO_TAG_LANETYPE);
                into.writeAttr(SUMO_ATTR_INDEX, index++);
                // write speed
                if (laneType.attrs.count(SUMO_ATTR_SPEED) > 0 && laneType.speed != NBEdge::UNSPECIFIED_SPEED
                        && laneType.speed != edgeType.second->speed) {
                    into.writeAttr(SUMO_ATTR_SPEED, laneType.speed);
                }
                // write permissions
                if (laneType.attrs.count(SUMO_ATTR_DISALLOW) > 0 || laneType.attrs.count(SUMO_ATTR_ALLOW) > 0) {
                    writePermissions(into, laneType.permissions);
                }
                // write width
                if (laneType.attrs.count(SUMO_ATTR_WIDTH) > 0 && laneType.width != edgeType.second->width
                        && laneType.width != NBEdge::UNSPECIFIED_WIDTH) {
                    into.writeAttr(SUMO_ATTR_WIDTH, laneType.width);
                }
                // write restrictions
                for (const auto& restriction : laneType.restrictions) {
                    // open restriction tag
                    into.openTag(SUMO_TAG_RESTRICTION);
                    // write vclass
                    into.writeAttr(SUMO_ATTR_VCLASS, getVehicleClassNames(restriction.first));
                    // write speed
                    into.writeAttr(SUMO_ATTR_SPEED, restriction.second);
                    // close restriction tag
                    into.closeTag();
                }
                // close lane type tag
                into.closeTag();
            }
        }
        // close edge type tag
        into.closeTag();
    }
    //write endlype
    if (!myEdgeTypes.empty()) {
        into.lf();
    }
}


int
NBTypeCont::getEdgeTypeNumLanes(const std::string& type) const {
    return (int)getEdgeType(type)->laneTypeDefinitions.size();
}


double
NBTypeCont::getEdgeTypeSpeed(const std::string& type) const {
    return getEdgeType(type)->speed;
}


int
NBTypeCont::getEdgeTypePriority(const std::string& type) const {
    return getEdgeType(type)->priority;
}


bool
NBTypeCont::getEdgeTypeIsOneWay(const std::string& type) const {
    return getEdgeType(type)->oneWay;
}


bool
NBTypeCont::getEdgeTypeShallBeDiscarded(const std::string& type) const {
    return getEdgeType(type)->discard;
}

double
NBTypeCont::getEdgeTypeWidthResolution(const std::string& type) const {
    return getEdgeType(type)->widthResolution;
}

double
NBTypeCont::getEdgeTypeMaxWidth(const std::string& type) const {
    return getEdgeType(type)->maxWidth;
}

double
NBTypeCont::getEdgeTypeMinWidth(const std::string& type) const {
    return getEdgeType(type)->minWidth;
}

bool
NBTypeCont::wasSetEdgeTypeAttribute(const std::string& type, const SumoXMLAttr attr) const {
    return getEdgeType(type)->attrs.count(attr) > 0;
}


SVCPermissions
NBTypeCont::getEdgeTypePermissions(const std::string& type) const {
    return getEdgeType(type)->permissions;
}


LaneSpreadFunction
NBTypeCont::getEdgeTypeSpreadType(const std::string& type) const {
    return getEdgeType(type)->spreadType;
}


double
NBTypeCont::getEdgeTypeWidth(const std::string& type) const {
    return getEdgeType(type)->width;
}


double
NBTypeCont::getEdgeTypeSidewalkWidth(const std::string& type) const {
    return getEdgeType(type)->sidewalkWidth;
}


double
NBTypeCont::getEdgeTypeBikeLaneWidth(const std::string& type) const {
    return getEdgeType(type)->bikeLaneWidth;
}


const NBTypeCont::EdgeTypeDefinition*
NBTypeCont::getEdgeType(const std::string& name) const {
    // try to find name in edge types
    TypesCont::const_iterator i = myEdgeTypes.find(name);
    // check if return edge types, or default edge types
    if (i == myEdgeTypes.end()) {
        return myDefaultType;
    } else {
        return i->second;
    }
}

/****************************************************************************/
