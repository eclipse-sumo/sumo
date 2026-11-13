/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNELane2laneConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
///
// File for lane2lane geometry classes and functions
/****************************************************************************/

#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEJunction.h>

#include "GNELane2laneConnection.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNELane2laneConnection::GNELane2laneConnection(const GNELane* fromLane) :
    myFromLane(fromLane) {
}


void
GNELane2laneConnection::updateLane2laneConnection() {
    // declare numPoints
    const int numPoints = 5;
    const int maximumLanes = 10;
    // clear connectionsMap
    myConnectionsMap.clear();
    // iterate over outgoingEdge's lanes
    for (const auto& outgoingEdge : myFromLane->getParentEdge()->getToJunction()->getGNEOutgoingEdges()) {
        for (const auto& outgoingLane : outgoingEdge->getChildLanes()) {
            // get NBEdges from and to
            const NBEdge* NBEdgeFrom = myFromLane->getParentEdge()->getNBEdge();
            const NBEdge* NBEdgeTo = outgoingLane->getParentEdge()->getNBEdge();
            // declare shape
            PositionVector shape;
            // only create smooth shapes if Edge From has as maximum 10 lanes
            if ((NBEdgeFrom->getNumLanes() <= maximumLanes) && (NBEdgeFrom->getToNode()->getShape().area() > 4)) {
                // calculate smooth shape
                shape = NBEdgeFrom->getToNode()->computeSmoothShape(
                            NBEdgeFrom->getLaneShape(myFromLane->getIndex()),
                            NBEdgeTo->getLaneShape(outgoingLane->getIndex()),
                            numPoints, NBEdgeFrom->getTurnDestination() == NBEdgeTo,
                            (double) numPoints * (double) NBEdgeFrom->getNumLanes(),
                            (double) numPoints * (double) NBEdgeTo->getNumLanes());
            } else {
                // create a shape using lane shape extremes
                shape = {myFromLane->getLaneShape().back(), outgoingLane->getLaneShape().front()};
                if (shape.length() < 3) {
                    shape.clear();
                    PositionVector laneShapeFrom = myFromLane->getLaneShape();
                    PositionVector laneShapeTo = outgoingLane->getLaneShape();
                    // apply offset to lane shape if we're in lane spread function center
                    if (NBEdgeFrom->getLaneSpreadFunction() == LaneSpreadFunction::CENTER) {
                        laneShapeFrom.move2side(0.3);
                    }
                    if (NBEdgeTo->getLaneSpreadFunction() == LaneSpreadFunction::CENTER) {
                        laneShapeTo.move2side(0.3);
                    }
                    // check if this connetion is a turn around
                    const bool turnAround = NBEdgeTo->getToNode() == NBEdgeFrom->getFromNode();
                    // add from lane shape one step before
                    if (laneShapeFrom.length() > 1) {
                        // set length depending of turn arounds
                        if (turnAround) {
                            shape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 0.5));
                        } else {
                            shape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 1));
                        }
                    }
                    // add from lane shape
                    shape.push_back(laneShapeFrom.back());
                    // add to lane shape
                    shape.push_back(laneShapeTo.front());
                    // add to lane shape one step after
                    if (laneShapeTo.length() > 1) {
                        // set length depending of turn arounds
                        if (turnAround) {
                            shape.push_back(laneShapeTo.positionAtOffset(0.5));
                        } else {
                            shape.push_back(laneShapeTo.positionAtOffset(1));
                        }
                    }
                }
            }
            // update connection map
            myConnectionsMap[outgoingLane].updateGeometry(shape);
        }
    }
}


bool
GNELane2laneConnection::exist(const GNELane* toLane) const {
    return (myConnectionsMap.count(toLane) > 0);
}


const GUIGeometry&
GNELane2laneConnection::getLane2laneGeometry(const GNELane* toLane) const {
    return myConnectionsMap.at(toLane);
}


GNELane2laneConnection::GNELane2laneConnection() :
    myFromLane(nullptr) {
}

/****************************************************************************/
