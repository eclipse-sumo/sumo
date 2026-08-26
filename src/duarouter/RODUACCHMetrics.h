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
#include <tuple>
#include <utility>
#include <vector>
#include <utils/common/SUMOTime.h>
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
 * @brief Lazy per-(vehicle class, weight period) CCH metric store for duarouter.
 *
 * The metric provider handed to CCHRouter (a plain function pointer). On the
 * first query for a (class, period) pair, the metric is customized under a
 * mutex from the graph's fillInputWeights evaluated at the period's begin
 * (which also primes the class's connection mask under that lock) and cached
 * for the rest of the run. Without weight files there is a single period, so
 * exactly one metric per class; with time-dependent weight files this mirrors
 * CHRouterWrapper's one-hierarchy-per-weight-period semantics -- except that
 * only the cheap re-customization is repeated, never the topology build.
 * RoutingKit metrics BORROW their input-weight buffer, so buffer and metric
 * live together in the map (std::map nodes are address-stable).
 */
class RODUACCHMetrics {
public:
    /// @brief install the shared graph, effort function and weight-period
    /// grid (call once, before routing); weightPeriod SUMOTime_MAX = static
    static void init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort,
                     SUMOTime begin, SUMOTime weightPeriod);

    /// @brief the metric for the vehicle's class and restriction profile at
    /// a query time, built on first use; matches CCHRouter::MetricProvider.
    /// Vehicles whose type carries restriction-params values get their own
    /// metric with the edges that restrict them masked to inf_weight -- the
    /// restricts() relation only depends on the type's paramRestrictions
    /// vector, so vehicles sharing that vector share the metric.
    static const RoutingKit::CustomizableContractionHierarchyMetric* get(
        SUMOVehicleClass vClass, SUMOTime time, const ROVehicle* veh);

    /// @brief the end of the weight period containing the given time
    /// (SUMOTime_MAX when the weights are static); CCHRouter's PeriodEnd hook
    static SUMOTime periodEnd(SUMOTime time);

private:
    struct ClassMetric {
        std::vector<unsigned> weights;
        std::unique_ptr<RoutingKit::CustomizableContractionHierarchyMetric> metric;
    };
    /// @brief (vClass, weight period, restriction profile) -> metric
    typedef std::tuple<SUMOVehicleClass, int, int> MetricKey;
    static const RODUACCHGraph* myGraph;
    static RODUACCHGraph::EffortOperation myEffort;
    static SUMOTime myBegin;
    static SUMOTime myWeightPeriod;
    static std::map<MetricKey, ClassMetric> myMetrics;
    /// @brief dedup of the restriction-profile vectors (empty vector = 0)
    static std::map<std::vector<double>, int> myProfiles;
    static std::mutex myLock;
};
