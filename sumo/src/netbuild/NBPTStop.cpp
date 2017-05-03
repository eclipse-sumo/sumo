/****************************************************************************/
/// @file    NBPtStop.cpp
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
NBPTStop::NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length, std::string name) :
    myPTStopId(ptStopId),
    myPosition(position),
    myEdgeId(edgeId),
    myOrigEdgeId(origEdgeId),
    myPTStopLength(length),
    myName(name),
    myFriendlyPos(false) {

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
    myFrom = center - myPTStopLength / 2.;
    myTo = center + myPTStopLength / 2.;
    if (myFrom < 0 || myTo > edgeLength) {
        myFriendlyPos = true;
    }
}
void NBPTStop::setLaneID(const std::string& laneId) {
    myLaneId = laneId;
}
void NBPTStop::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_BUS_STOP);
    device.writeAttr(SUMO_ATTR_ID, myPTStopId);
    if (myName != "") {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    device.writeAttr(SUMO_ATTR_LANE, myLaneId);
    device.writeAttr(SUMO_ATTR_STARTPOS, myFrom);
    device.writeAttr(SUMO_ATTR_ENDPOS, myTo);
    if (myFriendlyPos) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, "true");
    }
    device.closeTag();

}
void NBPTStop::reshiftPostion(const double offsetX, const double offsetY) {
    myPosition.add(offsetX, offsetY, 0);

}
