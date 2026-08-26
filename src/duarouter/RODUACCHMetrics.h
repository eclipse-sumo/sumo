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
/// @file    RODUACCHMetrics.h
/// @author  Pranav Sateesh
/// @date    2026
///
// duarouter's lazy CCH metric store over the shared graph mapping
// (utils/router/CCHGraphBase.h); RODUACCHGraph is its ROEdge instantiation.
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/CCHGraphBase.h>

class ROEdge;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the ROEdge instantiation of the CCH graph mapping
using RODUACCHGraph = CCHGraphBase<ROEdge, ROVehicle>;


/**
 * @class RODUACCHMetrics
 * @brief Lazy per-vehicle-class CCH metric store for duarouter.
 *
 * The metric provider handed to CCHRouter (a plain function pointer). On the
 * first query for a class, the metric is customized under a mutex from the
 * graph's fillInputWeights and cached for the rest of the run (weights are
 * static). RoutingKit metrics BORROW their input-weight buffer, so buffer
 * and metric live together in the map (std::map nodes are address-stable).
 */
class RODUACCHMetrics {
public:
    /// @brief install the shared graph + effort function (call once, before routing)
    static void init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort);

    /// @brief the metric for a class, built on first use; matches
    /// CCHRouter::MetricProvider so it can be passed as the provider.
    static const RoutingKit::CustomizableContractionHierarchyMetric* get(SUMOVehicleClass vClass);

private:
    struct ClassMetric {
        std::vector<unsigned> weights;
        std::unique_ptr<RoutingKit::CustomizableContractionHierarchyMetric> metric;
    };
    static const RODUACCHGraph* myGraph;
    static RODUACCHGraph::EffortOperation myEffort;
    static std::map<SUMOVehicleClass, ClassMetric> myMetrics;
    static std::mutex myLock;
};
