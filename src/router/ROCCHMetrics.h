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
/// @file    ROCCHMetrics.h
/// @author  Pranav Sateesh
/// @date    2026
///
// The CCH metric store of the router applications (duarouter, marouter):
// thin static trampolines (CCHRouter's MetricProvider, PeriodEnd and reset
// hooks are plain function pointers) over the shared CCHMetricFamily in
// STATIC mode -- lazy per-(vehicle type, weight period) customization,
// filled with the querying vehicle, with the restriction-params edges of a
// type masked to inf_weight by a post-fill patch. marouter has no weight
// periods but changes the travel times after every assignment iteration and
// resets its router there: the reset flags every metric for
// re-customization on the next query. See utils/router/CCHMetricFamily.h
// for the semantics.
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/CCHGraph.h>
#include <utils/router/CCHMetricFamily.h>

class ROEdge;
class ROVehicle;
class SUMOVTypeParameter;


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the ROEdge instantiation of the CCH graph mapping
using ROCCHGraph = CCHGraph<ROEdge, ROVehicle>;
/// @brief the ROEdge instantiation of the CCH metric store, keyed by the
/// vehicle-type parameters (stable pointers, owned by RONet)
using ROCCHMetricFamily = CCHMetricFamily<ROEdge, ROVehicle, SUMOVTypeParameter>;


/**
 * @class ROCCHMetrics
 * @brief function-pointer front end of the router applications' STATIC metric family
 */
class ROCCHMetrics {
public:
    /// @brief install the shared graph, effort function and weight-period
    /// grid (call once, before routing); weightPeriod SUMOTime_MAX = static
    static void init(const ROCCHGraph* graph, ROCCHGraph::EffortOperation effort,
                     SUMOTime begin, SUMOTime weightPeriod);

    /// @brief the metric for the vehicle's type at a query time, built on
    /// first use; matches CCHRouter::MetricProvider
    static const RoutingKit::CustomizableContractionHierarchyMetric* get(
        SUMOVehicleClass vClass, SUMOTime time, const ROVehicle* veh);

    /// @brief the end of the weight period containing the given time
    /// (SUMOTime_MAX when the weights are static); CCHRouter's PeriodEnd hook
    static SUMOTime periodEnd(SUMOTime time);

    /// @brief the edge weights changed behind the metrics (marouter after an
    /// assignment iteration, through CCHRouter::reset): every metric is
    /// refilled and re-customized on the next query; CCHRouter's ResetHook
    static void reset(const ROVehicle* veh);

private:
    /// @brief post-fill hook: mask the edges that restrict the querying
    /// vehicle's type to inf_weight (restriction-params)
    static void patchRestrictions(const ROCCHGraph* graph, const ROVehicle* veh,
                                  std::vector<unsigned>& weights);

    static ROCCHMetricFamily* myFamily;
};
