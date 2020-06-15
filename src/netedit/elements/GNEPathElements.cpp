/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEPathElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2020
///
// A abstract class for representation of element paths
/****************************************************************************/
#include <config.h>

#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/GNENet.h>

#include "GNEPathElements.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPathElements::PathElement - methods
// ---------------------------------------------------------------------------

GNEPathElements::PathElement::PathElement(GNELane* lane) :
    myLane(lane) {
}


GNEJunction* 
GNEPathElements::PathElement::getJunction() const {
    return myLane->getParentEdge()->getSecondParentJunction();
}


GNELane* 
GNEPathElements::PathElement::getLane() const {
    return myLane;
}


GNEPathElements::PathElement::PathElement():
    myLane(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEPathElements - methods
// ---------------------------------------------------------------------------

GNEPathElements::GNEPathElements(GNEDemandElement* demandElement):
    myDemandElement(demandElement) {
}


GNEPathElements::~GNEPathElements() {}


const std::vector<GNEPathElements::PathElement>&
GNEPathElements::getPath() const {
    return myPathElements;
}


void 
GNEPathElements::drawPathChildren(const GUIVisualizationSettings& s, const GNELane* lane) const {
    for (const auto &pathElement : myPathElements) {
        if (pathElement.getLane() == lane) {
            myDemandElement->drawPartialGL(s, lane);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEPathElements - protected methods
// ---------------------------------------------------------------------------

void
GNEPathElements::updatePathLanes(SUMOVehicleClass vClass, GNELane* fromLane, GNELane* toLane, const std::vector<GNEEdge*> &viaEdges) {
    // declare a edge vector
    std::vector<GNEEdge*> edges;
    // add from-via-edge lanes
    edges.push_back(fromLane->getParentEdge());
    for (const auto &edge : viaEdges) {
        edges.push_back(edge);
    }
    edges.push_back(toLane->getParentEdge());
    // remove consecutive (adjacent) duplicates
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // calculate path
    const std::vector<GNEEdge*> path = myDemandElement->getNet()->getPathCalculator()->calculatePath(vClass, edges);
    // remove demandElement of parent lanes
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getLane()) {
            pathElement.getLane()->removePathElement(myDemandElement);
        }
    }
    // set new route lanes
    myPathElements.clear();
    // check if path was sucesfully calculated
    if (path.size() > 0) {
        for (int i = 0; i < (int)path.size(); i++) {
            if (i == 0) {
                myPathElements.push_back(fromLane);
            } else if (i == (int)path.size()) {
                myPathElements.push_back(toLane);
            } else {
                myPathElements.push_back(path.at(i)->getLaneByAllowedVClass(vClass));
            }
        }
    } else {
        myPathElements = {fromLane, toLane};
    }
    // add demandElement into parent lanes
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getLane()) {
            pathElement.getLane()->addPathElement(myDemandElement);
        }
    }
}


void
GNEPathElements::invalidatePathLanes(SUMOVehicleClass vClass, GNELane* fromLane, GNELane* toLane, const std::vector<GNEEdge*> &viaEdges) {
    // declare a edge vector
    std::vector<GNEEdge*> edges;
    // add from-via-edge lanes
    edges.push_back(fromLane->getParentEdge());
    for (const auto &edge : viaEdges) {
        edges.push_back(edge);
    }
    edges.push_back(toLane->getParentEdge());
    // remove consecutive (adjacent) duplicates
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // calculate path
    const std::vector<GNEEdge*> path/* = element->getNet()->getPathCalculator()->calculatePath(vClass, edges)*/;
    // remove demandElement of parent lanes
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getLane()) {
            pathElement.getLane()->removePathElement(myDemandElement);
        }
    }
    // set new route lanes
    myPathElements.clear();
    // check if path was sucesfully calculated
    if (path.size() > 0) {
        for (int i = 0; i < (int)path.size(); i++) {
            if (i == 0) {
                myPathElements.push_back(fromLane);
            } else if (i == (int)path.size()) {
                myPathElements.push_back(toLane);
            } else {
                myPathElements.push_back(path.at(i)->getLaneByAllowedVClass(vClass));
            }
        }
    } else {
        myPathElements = {fromLane, toLane};
    }
    // add demandElement into parent lanes
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getLane()) {
            pathElement.getLane()->addPathElement(myDemandElement);
        }
    }
}

/****************************************************************************/
