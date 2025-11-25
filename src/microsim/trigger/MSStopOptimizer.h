/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSStopOptimizer.h
/// @author  Jakob Erdmann
/// @date    Nov 2025
///
// Optimizes prioritized stops
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <microsim/MSBaseVehicle.h>

// ===========================================================================
// class declarations
// ===========================================================================

typedef MSBaseVehicle::StopEdgeInfo StopEdgeInfo;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSStopOptimizer
 * @brief Finds a sequence of skips and stop alternatives to maximize priority
 * of reached stops
 */
class MSStopOptimizer {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane speed trigger
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    MSStopOptimizer(MSBaseVehicle* veh) :
        myVehicle(veh) {}

    /** @brief Destructor */
    virtual ~MSStopOptimizer() {}

    /* @brief increase the total priority of reachable stops
     * - backtrack along the planned route to avoid dead-ends
     * - use alternative stops (with the same name) to replace unreachable stops
     * */
    ConstMSEdgeVector optimizeSkipped(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                                      const MSEdge* source, double sourcePos, std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges, SUMOTime maxDelay) const;

    /// @brief find a route starting from originStop that reaches or skips the remaining stop and return the total stop priority achieved
    ConstMSEdgeVector routeAlongStops(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                                      std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges,
                                      int originStop, SUMOTime maxDelay, double& skippedPrio2) const;

    bool reachableInTime(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
            const MSEdge* from, double fromPos,
            const MSEdge* to, double toPos,
            SUMOTime maxCost,
            ConstMSEdgeVector& into) const;


private:

    MSBaseVehicle* myVehicle;

};
