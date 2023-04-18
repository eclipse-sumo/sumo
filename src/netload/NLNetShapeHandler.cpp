/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    NLNetShapeHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Clemens Honomichl
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @date    Mon, 9 Jul 2001
///
// The XML-Handler for network loading
/****************************************************************************/
#include <config.h>

#include <string>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSJunction.h>
#include <microsim/MSJunctionControl.h>
#include "NLNetShapeHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
NLNetShapeHandler::NLNetShapeHandler(const std::string& file, MSNet& net) :
    SUMOSAXHandler(file, "net"),
    myNet(net) {
}


NLNetShapeHandler::~NLNetShapeHandler() {}


void
NLNetShapeHandler::myStartElement(int element,
                                  const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_LANE:
            addLane(attrs);
            break;
        case SUMO_TAG_JUNCTION:
            addJunction(attrs);
            break;
        case SUMO_TAG_CONNECTION:
            if (myNet.hasInternalLinks()) {
                // see sortInternalShapes
                addConnection(attrs);
            }
            break;
        default:
            break;
    }
}


void
NLNetShapeHandler::addLane(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!myNet.hasInternalLinks() && id[0] == ':') {
        return;
    }
    MSLane* lane = MSLane::dictionary(id);
    if (lane == nullptr) {
        WRITE_WARNINGF("The lane '%' does not exist in the primary network.", id);
        return;
    }
    const PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    lane->addSecondaryShape(shape);
}


void
NLNetShapeHandler::addJunction(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!myNet.hasInternalLinks() && id[0] == ':') {
        return;
    }
    MSJunction* junction = myNet.getJunctionControl().get(id);
    if (junction == nullptr) {
        WRITE_WARNINGF("The junction '%' does not exist in the primary network.", id);
        return;
    }
    double x = attrs.get<double>(SUMO_ATTR_X, id.c_str(), ok);
    double y = attrs.get<double>(SUMO_ATTR_Y, id.c_str(), ok);
    double z = attrs.getOpt<double>(SUMO_ATTR_Z, id.c_str(), ok, 0);
    junction->addSecondaryPosition(Position(x, y, z));
}


void
NLNetShapeHandler::addConnection(const SUMOSAXAttributes& attrs) {
    if (!attrs.hasAttribute(SUMO_ATTR_VIA)) {
        return;
    }
    bool ok = true;
    const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, nullptr, ok);
    const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
    const int fromLaneIdx = attrs.get<int>(SUMO_ATTR_FROM_LANE, nullptr, ok);
    const int toLaneIdx = attrs.get<int>(SUMO_ATTR_TO_LANE, nullptr, ok);
    std::string viaID = attrs.get<std::string>(SUMO_ATTR_VIA, nullptr, ok);
    MSLane* lane = MSLane::dictionary(viaID);
    if (lane == nullptr) {
        // warning already given in addLane
        return;
    }
    std::string fromLaneID = fromID + "_" + toString(fromLaneIdx);
    std::string toLaneID = toID + "_" + toString(toLaneIdx);

    if (lane->getLinkCont()[0]->getLane()->getID() != toLaneID
            || lane->getIncomingLanes()[0].lane->getID() != fromLaneID) {
        // mismatch: find the correct lane that connects fromLaneID and toLaneID
        const MSJunction* junction = lane->getEdge().getToJunction();
        for (MSLane* lane2 : junction->getInternalLanes()) {
            if (lane2->getLinkCont()[0]->getLane()->getID() == toLaneID
                    && lane2->getIncomingLanes()[0].lane->getID() == fromLaneID) {
                myShuffledJunctions[junction][lane2] = lane;
                break;
            }
        }
    }
}


void
NLNetShapeHandler::sortInternalShapes() {
    // even when the alternative network has the same topology as
    // the primary network, the ids of internal lanes may differ
    // since they are based on a clockwise sorting of the edges.
    // hence we must verify connections and suffle the shapes as needed
    for (auto item : myShuffledJunctions) {
        const MSJunction* junction = item.first;
        std::map<MSLane*, PositionVector> shapes2;
        for (MSLane* lane : junction->getInternalLanes()) {
            shapes2[lane] = lane->getShape(true);
        }

        for (auto laneMap : item.second) {
            laneMap.first->addSecondaryShape(shapes2[laneMap.second]);
        }
    }
}
/****************************************************************************/
