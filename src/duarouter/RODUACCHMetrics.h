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
// (utils/router/CCHGraph.h); RODUACCHGraph is its ROEdge instantiation.
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/CCHGraph.h>

class ROEdge;
class ROVehicle;
class SUMOVTypeParameter;


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the ROEdge instantiation of the CCH graph mapping
using RODUACCHGraph = CCHGraph<ROEdge, ROVehicle>;


/**
 * @class RODUACCHMetrics
 * @brief Lazy per-(vehicle type, weight period) CCH metric store for duarouter.
 *
 * The metric provider handed to CCHRouter (a plain function pointer). On the
 * first query for a (type, period) pair, the metric is customized under a
 * mutex from the graph's fillInputWeights evaluated at the period's begin
 * with the querying vehicle as the effort reference (which also primes the
 * class's connection mask under that lock) and cached for the rest of the
 * run. Keying by the TYPE (rather than the class) makes everything the
 * effort function reads from the vehicle type exact per metric: the type's
 * maximum speed, per-class edge speed restrictions and restriction-params
 * values; only individual speed-factor draws within one type share a metric
 * (the same approximation the CH family makes with its per-class prototype
 * vehicles, but at finer granularity). Without weight files there is a
 * single period, so exactly one metric per type; with time-dependent weight
 * files this mirrors CHRouterWrapper's one-hierarchy-per-weight-period
 * semantics -- except that only the cheap re-customization is repeated,
 * never the topology build. RoutingKit metrics BORROW their input-weight
 * buffer, so buffer and metric live together in the map (std::map nodes are
 * address-stable).
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
    /// @brief (vehicle type, weight period) -> metric; the type pointer is
    /// stable (types are owned by RONet), nullptr covers vehicle-less queries
    typedef std::pair<const SUMOVTypeParameter*, int> MetricKey;
    static const RODUACCHGraph* myGraph;
    static RODUACCHGraph::EffortOperation myEffort;
    static SUMOTime myBegin;
    static SUMOTime myWeightPeriod;
    static std::map<MetricKey, ClassMetric> myMetrics;
    static std::mutex myLock;
};
