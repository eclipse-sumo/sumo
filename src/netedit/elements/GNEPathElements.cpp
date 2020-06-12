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

#include <netedit/GNENet.h>

#include "GNEPathElements.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPathElements::PathElement - methods
// ---------------------------------------------------------------------------

GNEPathElements::PathElement::PathElement(GNEJunction* junction) :
    myJunction(junction),
    myEdge(nullptr) {
}


GNEPathElements::PathElement::PathElement(GNEEdge* edge) :
    myJunction(nullptr),
    myEdge(edge) {
}


GNEJunction* 
GNEPathElements::PathElement::getJunction() const {
    return myJunction;
}


GNEEdge* 
GNEPathElements::PathElement::getEdge() const {
    return myEdge;
}


GNEPathElements::PathElement::PathElement():
    myJunction(nullptr),
    myEdge(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEPathElements - methods
// ---------------------------------------------------------------------------

GNEPathElements::GNEPathElements() {}


GNEPathElements::~GNEPathElements() {}


const std::vector<GNEPathElements::PathElement>&
GNEPathElements::getPath() const {
    return myPathElements;
}

// ---------------------------------------------------------------------------
// GNEPathElements - protected methods
// ---------------------------------------------------------------------------

void
GNEPathElements::replacePathEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& pathEdges, SUMOVehicleClass vClass) {
    // remove demandElement of parent edges
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getEdge()) {
            pathElement.getEdge()->removePathElement(elementChild);
        }
    }
    // set new route edges
    myPathElements.clear();
    for (const auto &edge : pathEdges) {
        myPathElements.push_back(GNEPathElements::PathElement(edge));
    }
    // add demandElement into parent edges
    for (const auto& pathElement : myPathElements) {
        if (pathElement.getEdge()) {
            pathElement.getEdge()->addPathElement(elementChild);
        }
    }
}

/****************************************************************************/
