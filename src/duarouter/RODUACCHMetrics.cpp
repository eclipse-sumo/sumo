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
/// @file    RODUACCHMetrics.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// duarouter's lazy CCH metric store (see RODUACCHMetrics.h).
/****************************************************************************/
#include <config.h>

#include "RODUACCHMetrics.h"

#include <router/ROEdge.h>
#include <router/ROVehicle.h>

// ===========================================================================
// static member definitions
// ===========================================================================
const RODUACCHGraph* RODUACCHMetrics::myGraph = nullptr;
RODUACCHGraph::EffortOperation RODUACCHMetrics::myEffort = nullptr;
SUMOTime RODUACCHMetrics::myBegin = 0;
SUMOTime RODUACCHMetrics::myWeightPeriod = SUMOTime_MAX;
std::map<RODUACCHMetrics::MetricKey, RODUACCHMetrics::ClassMetric> RODUACCHMetrics::myMetrics;
std::mutex RODUACCHMetrics::myLock;


// ===========================================================================
// RODUACCHMetrics method definitions
// ===========================================================================
void
RODUACCHMetrics::init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort,
                      SUMOTime begin, SUMOTime weightPeriod) {
    myGraph = graph;
    myEffort = effort;
    myBegin = begin;
    myWeightPeriod = weightPeriod;
    myMetrics.clear();
}


const RoutingKit::CustomizableContractionHierarchyMetric*
RODUACCHMetrics::get(SUMOVehicleClass vClass, SUMOTime time, const ROVehicle* veh) {
    if (myGraph == nullptr) {
        return nullptr;  // not initialised -> caller falls back to A*
    }
    // Weight period of the query time (period 0 covers everything before
    // begin and the whole run when no weight files are loaded).
    int period = 0;
    if (myWeightPeriod > 0 && myWeightPeriod != SUMOTime_MAX && time > myBegin) {
        period = (int)((time - myBegin) / myWeightPeriod);
    }
    // Lazy build on the first query for a (type, period) pair (duarouter
    // streams vehicles, so the types are not known up front); the weights are
    // evaluated at the period's begin with the querying vehicle as the effort
    // reference, so each pair is customized exactly once and reflects the
    // type's maximum speed, per-class edge speed restrictions and
    // restriction-params. The mutex also serialises concurrent first queries
    // from parallel routing threads (satisfying primeClassMask's
    // synchronization contract); afterwards the map is read-only for that
    // key (references into std::map are stable).
    std::lock_guard<std::mutex> lock(myLock);
    const MetricKey key = std::make_pair(veh == nullptr ? nullptr : veh->getType(), period);
    auto it = myMetrics.find(key);
    if (it == myMetrics.end()) {
        ClassMetric& cm = myMetrics[key];
        const double periodBegin = STEPS2TIME(myBegin + period * myWeightPeriod);
        myGraph->fillInputWeights(myEffort, vClass, veh, periodBegin, cm.weights);
        if (veh != nullptr && !veh->getType()->paramRestrictions.empty()) {
            // mask the edges that restrict this type (restricts() depends
            // only on the type's paramRestrictions vector). arcsOfEdge of a
            // real edge is exactly the arcs headed by it -- via chains hold
            // internal edges only -- matching what the exact routers check
            // per relaxation.
            for (const ROEdge* const e : ROEdge::getAllEdges()) {
                if (!e->isInternal() && !e->isTazConnector() && e->restricts(veh)) {
                    for (const unsigned a : myGraph->arcsOfEdge(e)) {
                        cm.weights[a] = RoutingKit::inf_weight;
                    }
                }
            }
        }
        cm.metric.reset(new RoutingKit::CustomizableContractionHierarchyMetric(
                            myGraph->cch(), cm.weights));
        cm.metric->customize();
        return cm.metric.get();
    }
    return it->second.metric.get();
}


SUMOTime
RODUACCHMetrics::periodEnd(SUMOTime time) {
    if (myWeightPeriod <= 0 || myWeightPeriod == SUMOTime_MAX) {
        return SUMOTime_MAX;
    }
    const int period = time > myBegin ? (int)((time - myBegin) / myWeightPeriod) : 0;
    return myBegin + (period + 1) * myWeightPeriod;
}
