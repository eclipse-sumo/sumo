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
/// @file    ROCCHMetrics.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// The CCH metric store of the router applications (see ROCCHMetrics.h).
/****************************************************************************/
#include <config.h>

#include "ROCCHMetrics.h"

#include "ROEdge.h"
#include "ROVehicle.h"

// ===========================================================================
// static member definitions
// ===========================================================================
ROCCHMetricFamily* ROCCHMetrics::myFamily = nullptr;


// ===========================================================================
// ROCCHMetrics method definitions
// ===========================================================================
void
ROCCHMetrics::init(const ROCCHGraph* graph, ROCCHGraph::EffortOperation effort,
                   SUMOTime begin, SUMOTime weightPeriod) {
    delete myFamily;
    // no reference-vehicle factory: each (type, period) pair is customized
    // exactly once per fill, so the first querying vehicle of a type is that
    // type's effort reference
    myFamily = new ROCCHMetricFamily(graph, effort, begin, weightPeriod,
                                     nullptr, &ROCCHMetrics::patchRestrictions);
}


const RoutingKit::CustomizableContractionHierarchyMetric*
ROCCHMetrics::get(SUMOVehicleClass vClass, SUMOTime time, const ROVehicle* veh) {
    if (myFamily == nullptr) {
        return nullptr;  // not initialised -> caller falls back to A*
    }
    // keyed by the TYPE (nullptr covers vehicle-less queries): everything the
    // effort function reads from the type is exact per metric -- see
    // CCHMetricFamily.h
    return myFamily->get(veh == nullptr ? nullptr : veh->getType(), vClass, time, veh);
}


SUMOTime
ROCCHMetrics::periodEnd(SUMOTime time) {
    return myFamily == nullptr ? SUMOTime_MAX : myFamily->periodEnd(time);
}


void
ROCCHMetrics::reset(const ROVehicle* /* veh */) {
    if (myFamily != nullptr) {
        myFamily->flagStale();
    }
}


void
ROCCHMetrics::patchRestrictions(const ROCCHGraph* graph, const ROVehicle* veh,
                                std::vector<unsigned>& weights) {
    if (veh == nullptr || veh->getType()->paramRestrictions.empty()) {
        return;
    }
    // mask the edges that restrict this type (restricts() depends only on
    // the type's paramRestrictions vector). arcsOfEdge of a real edge is
    // exactly the arcs headed by it -- via chains hold internal edges only --
    // matching what the exact routers check per relaxation.
    for (const ROEdge* const e : ROEdge::getAllEdges()) {
        if (!e->isInternal() && !e->isTazConnector() && e->restricts(veh)) {
            for (const unsigned a : graph->arcsOfEdge(e)) {
                weights[a] = RoutingKit::inf_weight;
            }
        }
    }
}
