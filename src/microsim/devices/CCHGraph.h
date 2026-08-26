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
/// @file    CCHGraph.h
/// @author  Pranav Sateesh
/// @date    2026
///
// MSEdge instantiation of the shared CCH graph mapping (CCHGraphBase).
// The topology, TAZ handling and path expansion live in the base; the
// simulation-specific part is the weight fill: per-arc CONNECTION-level
// permission masks precomputed for the classes present in the demand,
// combined with the LIVE edge permissions at fill time so runtime closures
// (a permissions change) become inf_weight at the next customization.
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <vector>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/CCHGraphBase.h>

class MSEdge;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHGraph
 * @brief Metric-independent RoutingKit CCH topology over the MSEdge graph.
 */
class CCHGraph : public CCHGraphBase<MSEdge, SUMOVehicle> {
public:
    /** @brief Build the union line graph + CCH + per-arc class permissions.
     * @param[in] classes  the vehicle classes present in the demand; a
     *   per-arc CONNECTION-level permission bitmask is precomputed for these
     *   so the metric can mask exactly what A* (getViaSuccessors(vClass))
     *   would -- edge-level getPermissions() is too coarse on mixed-lane edges.
     */
    explicit CCHGraph(const std::set<SUMOVehicleClass>& classes);

    /** @brief Recompute the input weight of one arc -- the exact per-arc body
     * of fillInputWeights (same masking, folding and rounding), exposed so a
     * sparse update can refresh only the arcs of edges that actually moved. */
    unsigned computeArcWeight(unsigned a, EffortOperation effort, SUMOVehicleClass maskClass,
                              const SUMOVehicle* veh, double time) const;

    /** @brief Fill a centisecond input-weight buffer for one vehicle class.
     * @param[in] effort     the effort Operation (getEffort, or getEffortBike for bikes)
     * @param[in] maskClass  arcs whose connection or destination edge does NOT
     *   permit this class become inf_weight; SVC_IGNORING masks nothing. This
     *   is what makes a per-class metric class-correct AND closure-aware: a
     *   closure is a global permissions change (MSLane::setPermissions), so a
     *   closed edge simply stops permitting the class and its arcs go to inf.
     * @param[in] veh    reference vehicle for the effort floor (may be null)
     * @param[in] time   seconds, passed to the effort fn
     * @param[out] weight resized to arcCount(); w[a] in [0, inf_weight]
     */
    void fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                          const SUMOVehicle* veh, double time,
                          std::vector<unsigned>& weight) const;

private:
    /// @brief per-arc CONNECTION-level permission bitmask (which classes may
    /// traverse this connection, at build-time permissions). Matches A*'s
    /// getViaSuccessors(vClass); dynamic closures are AND'd in at fill time.
    std::vector<SVCPermissions> myArcPerm;
};
