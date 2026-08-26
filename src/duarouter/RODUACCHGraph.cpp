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
/// @file    RODUACCHGraph.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// ROEdge instantiation of the shared CCH graph mapping (see RODUACCHGraph.h).
/****************************************************************************/
#include <config.h>

#include "RODUACCHGraph.h"

#include <router/ROEdge.h>
#include <router/ROLane.h>
#include <router/RONode.h>

// ===========================================================================
// static member definitions
// ===========================================================================
const RODUACCHGraph* RODUACCHMetrics::myGraph = nullptr;
RODUACCHGraph::EffortOperation RODUACCHMetrics::myEffort = nullptr;
std::map<SUMOVehicleClass, RODUACCHMetrics::ClassMetric> RODUACCHMetrics::myMetrics;
std::mutex RODUACCHMetrics::myLock;


// ===========================================================================
// RODUACCHGraph method definitions
// ===========================================================================
RODUACCHGraph::RODUACCHGraph(const std::vector<ROEdge*>& allEdges) :
    CCHGraphBase<ROEdge, ROVehicle>(allEdges) {
}


void
RODUACCHGraph::fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                                const ROVehicle* veh, double time,
                                std::vector<unsigned>& weight) const {
    // Which arcs may this class traverse? Walk getViaSuccessors(maskClass)
    // live -- exactly the connections the exact routers would relax. Weights
    // are static in duarouter, so this runs once per class (see
    // RODUACCHMetrics::get) and needs no precomputed permission mask.
    std::vector<bool> allowed;
    markClassAllowedArcs(maskClass, allowed);
    weight.resize(arcCount());
    for (unsigned a = 0; a < arcCount(); a++) {
        const ROEdge* to = edgeOf(myArcHead[a]);
        if (!allowed[a] || (maskClass != SVC_IGNORING && (to->getPermissions() & maskClass) == 0)) {
            weight[a] = RoutingKit::inf_weight;
        } else {
            weight[a] = computeArcWeightRaw(a, effort, veh, time);
        }
    }
}


// ===========================================================================
// RODUACCHMetrics method definitions
// ===========================================================================
void
RODUACCHMetrics::init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort) {
    myGraph = graph;
    myEffort = effort;
    myMetrics.clear();
}


const RoutingKit::CustomizableContractionHierarchyMetric*
RODUACCHMetrics::get(SUMOVehicleClass vClass) {
    if (myGraph == nullptr) {
        return nullptr;  // not initialised -> caller falls back to A*
    }
    // Lazy build on first query for a class (duarouter streams vehicles, so
    // classes are not known up front). Weights are static for the whole run,
    // so a metric is customized exactly once. The mutex also serialises
    // concurrent first queries from parallel routing threads; afterwards the
    // map is read-only for that key (references into std::map are stable).
    std::lock_guard<std::mutex> lock(myLock);
    auto it = myMetrics.find(vClass);
    if (it == myMetrics.end()) {
        ClassMetric& cm = myMetrics[vClass];
        myGraph->fillInputWeights(myEffort, vClass, nullptr, 0., cm.weights);
        cm.metric.reset(new RoutingKit::CustomizableContractionHierarchyMetric(
                            myGraph->cch(), cm.weights));
        cm.metric->customize();
        return cm.metric.get();
    }
    return it->second.metric.get();
}
