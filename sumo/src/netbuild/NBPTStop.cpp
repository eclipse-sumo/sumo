/****************************************************************************/
/// @file    NBPTStop.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of a single pt stop
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

#include <utils/iodevices/OutputDevice.h>
#include "NBPTStop.h"


// ===========================================================================
// method definitions
// ===========================================================================
NBPTStop::NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length, std::string name, SVCPermissions svcPermissions) :
    myPTStopId(ptStopId),
    myPosition(position),
    myEdgeId(edgeId),
    myOrigEdgeId(origEdgeId),
    myPTStopLength(length),
    myName(name),
    myPermissions(svcPermissions),
    myIsMultipleStopPositions(false) {

}

std::string
NBPTStop::getID() const {
    return myPTStopId;
}

const std::string
NBPTStop::getOrigEdgeId() {
    return myOrigEdgeId;
}

const std::string
NBPTStop::getEdgeId() {
    return myEdgeId;
}

const std::string
NBPTStop::getName() {
    return myName;
}
const Position& NBPTStop::getPosition() {
    return myPosition;
}

void NBPTStop::computExtent(double center, double edgeLength) {
    myStartPos = MAX2(0.0, center - myPTStopLength / 2.);
    myEndPos = MIN2(center + myPTStopLength / 2., edgeLength);
}

void NBPTStop::setLaneID(const std::string& laneId) {
    myLaneId = laneId;
}
void NBPTStop::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_BUS_STOP);
    device.writeAttr(SUMO_ATTR_ID, myPTStopId);
    if (!myName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    device.writeAttr(SUMO_ATTR_LANE, myLaneId);
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, "true");
    device.closeTag();

}
void NBPTStop::reshiftPostion(const double offsetX, const double offsetY) {
    myPosition.add(offsetX, offsetY, 0);
    for (auto& myPlatformCand : myPlatformCands) {
        Position* pos = (&myPlatformCand)->getMyPos();
        pos->add(offsetX, offsetY, 0);
    }


}
SVCPermissions NBPTStop::getPermissions() {
    return myPermissions;
}
void NBPTStop::addPlatformCand(NBPTPlatform platform) {
    myPlatformCands.push_back(platform);
}
std::vector<NBPTPlatform>& NBPTStop::getPlatformCands() {
    return myPlatformCands;
}
bool NBPTStop::getIsMultipleStopPositions() {
    return myIsMultipleStopPositions;
}

void NBPTStop::setIsMultipleStopPositions(bool multipleStopPositions) {
    myIsMultipleStopPositions = multipleStopPositions;
}
double NBPTStop::getLength() {
    return myPTStopLength;
}
void NBPTStop::setEdgeId(std::string edgeId) {
    myEdgeId = edgeId;
}
void NBPTStop::registerAdditionalEdge(std::string wayId, std::string edgeId) {
    myAdditionalEdgeCandidates[wayId] = edgeId;
}
const std::map<std::string, std::string>& NBPTStop::getMyAdditionalEdgeCandidates() const {
    return myAdditionalEdgeCandidates;
}
void NBPTStop::setMyOrigEdgeId(const std::string& myOrigEdgeId) {
    NBPTStop::myOrigEdgeId = myOrigEdgeId;
}
void NBPTStop::setMyPTStopLength(double myPTStopLength) {
    NBPTStop::myPTStopLength = myPTStopLength;
}

