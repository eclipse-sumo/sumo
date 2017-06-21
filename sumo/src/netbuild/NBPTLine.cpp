/****************************************************************************/
/// @file    NBPTLine.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPTLine.cpp 24570 2017-06-07 06:54:55Z namdre $
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
#include "NBPTLine.h"
#include "NBPTStop.h"

NBPTLine::NBPTLine(std::string name) : myName(name), myPTLineId(-1) {

}
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
    if (myName != "") {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }

    for (std::vector<NBPTStop*>::iterator it = myPTStops.begin(); it != myPTStops.end(); it++) {
        device.openTag(SUMO_TAG_BUS_STOP);
        device.writeAttr(SUMO_ATTR_ID, (*it)->getID());
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
