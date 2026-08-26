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

// ===========================================================================
// static member definitions
// ===========================================================================
const RODUACCHGraph* RODUACCHMetrics::myGraph = nullptr;
RODUACCHGraph::EffortOperation RODUACCHMetrics::myEffort = nullptr;
std::map<SUMOVehicleClass, RODUACCHMetrics::ClassMetric> RODUACCHMetrics::myMetrics;
std::mutex RODUACCHMetrics::myLock;


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
