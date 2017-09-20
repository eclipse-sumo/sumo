/****************************************************************************/
/// @file    NBPTLine.cpp
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of one direction of a single pt line
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
#include <utils/iodevices/OutputDevice.h>

#include <utility>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include "NBPTLine.h"
#include "NBPTStop.h"

NBPTLine::NBPTLine(const std::string& name, const std::string& type) : 
    myName(name), 
    myType(type),
    myPTLineId(-1), 
    myRef(name) 
{ }

void NBPTLine::addPTStop(NBPTStop* pStop) {
    myPTStops.push_back(pStop);

}
std::string NBPTLine::getName() {
    return myName;
}
std::vector<NBPTStop*> NBPTLine::getStops() {
    return myPTStops;
}
void NBPTLine::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_PT_LINE);
    device.writeAttr(SUMO_ATTR_ID, myPTLineId);
    if (!myName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myName));
    }

    device.writeAttr(SUMO_ATTR_LINE, myRef);
    device.writeAttr(SUMO_ATTR_TYPE, myType);
    device.writeAttr("completeness", toString((double)myPTStops.size()/(double)myNumOfStops));

    if (!myRoute.empty()) {
        device.openTag(SUMO_TAG_ROUTE);
        device.writeAttr(SUMO_ATTR_EDGES, myRoute);
        device.closeTag();
    }

    for (auto& myPTStop : myPTStops) {
        device.openTag(SUMO_TAG_BUS_STOP);
        device.writeAttr(SUMO_ATTR_ID, myPTStop->getID());
        device.writeAttr(SUMO_ATTR_NAME, myPTStop->getName());
        device.closeTag();
    }
//    device.writeAttr(SUMO_ATTR_LANE, myLaneId);
//    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
//    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
//    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, "true");
    device.closeTag();

}
void NBPTLine::setId(long long int id) {
    myPTLineId = id;
}
void NBPTLine::addWayNode(long long int way, long long int node) {
    std::string wayStr = toString(way);
    if (wayStr != myCurrentWay) {
        myCurrentWay = wayStr;
        myWays.push_back(wayStr);
    }
    myWaysNodes[wayStr].push_back(node);

}
const std::vector<std::string>& NBPTLine::getMyWays() const {
    return myWays;
}
std::vector<long long int>* NBPTLine::getWaysNodes(std::string wayId) {
    if (myWaysNodes.find(wayId) != myWaysNodes.end()) {
        return &myWaysNodes[wayId];
    }
    return nullptr;
}
void NBPTLine::setRef(std::string ref) {
    myRef = std::move(ref);
}

void NBPTLine::addEdgeVector(std::vector<NBEdge*>::iterator fr, std::vector<NBEdge*>::iterator to) {
    myRoute.insert(myRoute.end(), fr, to);

}
void NBPTLine::setMyNumOfStops(unsigned long numStops) {
    myNumOfStops = numStops;
}
