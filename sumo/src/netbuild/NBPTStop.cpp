/****************************************************************************/
/// @file    NBPtStop.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPtStop.cpp 23150 2017-02-27 12:08:30Z behrisch $
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
#include <utils/iodevices/OutputDevice.h>
#include "NBPTStop.h"
NBPTStop::NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length, std::string name):
    myPTStopId(ptStopId),
    myPosition(position),
    myEdgeId(edgeId),
    myOrigEdgeId(origEdgeId),
    friendlyPos(false),
    myPTStopLength(length),
    myName(name)
{

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
    myFrom = center - myPTStopLength/2.;
    myTo = center + myPTStopLength/2.;
    if (myFrom < 0 || myTo > edgeLength) {
        friendlyPos = true;
    }
}
void NBPTStop::setLaneID(const std::string& laneId) {
    myLaneId = laneId;
}
void NBPTStop::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_BUS_STOP);
    device.writeAttr(SUMO_ATTR_ID,myPTStopId);
    device.writeAttr(SUMO_ATTR_NAME,myName);
    device.writeAttr(SUMO_ATTR_LANE,myLaneId);
    device.writeAttr(SUMO_ATTR_STARTPOS,myFrom);
    device.writeAttr(SUMO_ATTR_ENDPOS,myTo);
    if (friendlyPos) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS,"true");
    }
    device.closeTag();

}
void NBPTStop::reshiftPostion(const double offsetX, const double offsetY) {
    myPosition.add(offsetX, offsetY, 0);

}
