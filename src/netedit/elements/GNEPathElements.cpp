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
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEPathElements.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPathElements - methods
// ---------------------------------------------------------------------------

GNEPathElements::GNEPathElements() {}


GNEPathElements::~GNEPathElements() {}


const std::vector<GNEEdge*>&
GNEPathElements::getPathEdges() const {
    return myRouteEdges;
}

// ---------------------------------------------------------------------------
// GNEPathElements - protected methods
// ---------------------------------------------------------------------------

void
GNEPathElements::replacePathEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& routeEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myRouteEdges) {
        edge->removePathElement(elementChild);
    }
    // set new route edges
    myRouteEdges = routeEdges;
    // add demandElement into parent edges
    for (const auto& edge : myRouteEdges) {
        edge->addPathElement(elementChild);
    }
}

/****************************************************************************/
