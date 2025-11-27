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
#include "MSBaseVehicle.h"

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

    friend class StopPathNode;

public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane speed trigger
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    MSStopOptimizer(MSBaseVehicle* veh, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, SUMOTime t, SUMOTime maxDelay) :
        myVehicle(veh),
        myRouter(router),
        myT(t),
        myMaxDelay(maxDelay)
    {}

    /** @brief Destructor */
    virtual ~MSStopOptimizer() {}


    ConstMSEdgeVector optimizeSkipped(const MSEdge* source, double sourcePos, std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges) const;


private:
    MSBaseVehicle* myVehicle;
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& myRouter;
    SUMOTime myT;
    SUMOTime myMaxDelay;

private:

    /// @brief information used during skip/alternative optimization
    struct StopPathNode : public StopEdgeInfo, std::enable_shared_from_this<StopPathNode> {
        StopPathNode(const MSStopOptimizer& _so, const StopEdgeInfo& o):
            StopEdgeInfo(o.edge, o.priority, o.arrival, o.pos),
            so(_so) {
                nameTag = o.nameTag;
                delay = o.delay;
            }

        // ordering criterion (minimize in order)
        const MSStopOptimizer& so;
        // @brief sum of skipped stop priority between source and this node
        double skippedPrio = 0;
        // @brief sum of reached stop priority between source and this node
        double reachedPrio = 0;

        int trackChanges = 0;
        double cost = 0;

        int stopIndex;
        int numSkipped = 0;
        int altIndex = 0;
        bool checked = false;
        ConstMSEdgeVector edges;
        std::shared_ptr<StopPathNode> prev = nullptr;

        std::shared_ptr<StopPathNode> getSuccessor(const std::vector<StopEdgeInfo>& stops, double minSkipped);
    };

    struct spnCompare {
        bool operator()(const std::shared_ptr<StopPathNode>& a,
                        const std::shared_ptr<StopPathNode>& b) const {
            if (a->skippedPrio == b->skippedPrio) {
                if (a->trackChanges == b->trackChanges) {
                    return a->cost > b->cost;
                }
                return a->trackChanges > b->trackChanges;
            }
            return a->skippedPrio > b->skippedPrio;
        }
    };

    bool reachableInTime(const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos,
        SUMOTime arrival, ConstMSEdgeVector& into) const;

};
