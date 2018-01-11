/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTStop.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of a single pt stop
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
#include <utils/common/StringUtils.h>
#include "NBPTStop.h"
#include "NBEdge.h"
#include "NBEdgeCont.h"


// ===========================================================================
// method definitions
// ===========================================================================
NBPTStop::NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length,
                   std::string name, SVCPermissions svcPermissions)
    :
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

void NBPTStop::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_BUS_STOP);
    device.writeAttr(SUMO_ATTR_ID, myPTStopId);
    if (!myName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myName));
    }
    device.writeAttr(SUMO_ATTR_LANE, myLaneId);
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, "true");
    if (!myAccesses.empty()) {

        for (auto tuple : myAccesses) {
            device.openTag(SUMO_TAG_ACCESS);
            device.writeAttr(SUMO_ATTR_LANE, std::get<0>(tuple));
            device.writeAttr(SUMO_ATTR_POSITION, std::get<1>(tuple));
            device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
            device.closeTag();
        }
    }
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

bool
NBPTStop::setEdgeId(std::string edgeId, NBEdgeCont& ec) {
    myEdgeId = edgeId;
    return findLaneAndComputeBusStopExtend(ec);
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


bool
NBPTStop::findLaneAndComputeBusStopExtend(NBEdgeCont& ec) {
    NBEdge* edge = ec.getByID(myEdgeId);
    if (edge != nullptr) {
        int laneNr = -1;
        for (const auto& it : edge->getLanes()) {
            if ((it.permissions & getPermissions()) > 0) {
                ++laneNr;
                break;
            }
            laneNr++;
        }
        if (laneNr != -1) {
            myLaneId = edge->getLaneID(laneNr);
            const PositionVector& shape = edge->getLaneShape(laneNr);
            double offset = shape.nearest_offset_to_point2D(getPosition(), true);
            computExtent(offset, shape.length());
            return true;
        }
    }
    return false;
}
void NBPTStop::setMyPTStopId(std::string id) {
    myPTStopId = id;
}
void NBPTStop::addAccess(std::string laneID, double offset) {
    myAccesses.push_back(std::make_tuple(laneID, offset));
}


