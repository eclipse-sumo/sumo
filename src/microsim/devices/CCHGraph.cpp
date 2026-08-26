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
/// @file    CCHGraph.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// MSEdge instantiation of the shared CCH graph mapping (see CCHGraph.h).
/****************************************************************************/
#include <config.h>

#include "CCHGraph.h"

#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>


// ===========================================================================
// method definitions
// ===========================================================================
CCHGraph::CCHGraph(const std::set<SUMOVehicleClass>& classes) :
    CCHGraphBase<MSEdge, SUMOVehicle>(MSEdge::getAllEdges()) {
    // per-arc CONNECTION-level permissions: for each present class, mark the
    // arcs whose connection that class may traverse -- exactly the arcs
    // getViaSuccessors(vClass) would return. Built once at startup
    // permissions; runtime closures are AND'd in live at fill time.
    myArcPerm.assign(arcCount(), 0);
    std::vector<bool> allowed;
    for (const SUMOVehicleClass vc : classes) {
        markClassAllowedArcs(vc, allowed);
        for (unsigned a = 0; a < arcCount(); a++) {
            if (allowed[a]) {
                myArcPerm[a] |= (SVCPermissions)vc;
            }
        }
    }
}


unsigned
CCHGraph::computeArcWeight(unsigned a, EffortOperation effort, SUMOVehicleClass maskClass,
                           const SUMOVehicle* veh, double time) const {
    // Per-class / closure gate. Forbidden (inf_weight) if EITHER:
    //  - the connection does not permit the class (static, connection-level,
    //    matches A*'s getViaSuccessors(vClass)); OR
    //  - the destination edge does not currently permit the class (dynamic,
    //    edge-level -- catches runtime closures via MSLane::setPermissions).
    if (maskClass != SVC_IGNORING
            && (((myArcPerm[a] & maskClass) == 0)
                || ((edgeOf(myArcHead[a])->getPermissions() & maskClass) == 0))) {
        return RoutingKit::inf_weight;
    }
    return computeArcWeightRaw(a, effort, veh, time);
}


void
CCHGraph::fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                           const SUMOVehicle* veh, double time,
                           std::vector<unsigned>& weight) const {
    weight.resize(arcCount());
    for (unsigned a = 0; a < arcCount(); a++) {
        weight[a] = computeArcWeight(a, effort, maskClass, veh, time);
    }
}
