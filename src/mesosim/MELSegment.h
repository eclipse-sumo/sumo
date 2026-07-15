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
/// @file    MELSegment.h
/// @author  Jakob Erdmann
/// @date    Tue, July 2026
///
// A LIFT-model segment 
/****************************************************************************/
#pragma once
#include <config.h>

#include <deque>
#include "MESegment.h"


// ===========================================================================
// class declarations
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MELSegment
 * @brief A single mesoscopic segment (cell)
 */
class MELSegment : public MESegment {
public:


public:
    /** @brief constructor
     * @param[in] id The id of this segment (currently: "<EDGEID>:<SEGMENTNO>")
     * @param[in] parent The edge this segment is located within
     * @param[in] next The following segment (belonging to the same edge)
     * @param[in] length The segment's length
     * @param[in] speed The speed allowed on this segment
     * @param[in] idx The running index of this segment within the segment's edge
     * @param[in] multiQueue whether to install multiple queues on this segment
     * @param[in] edgeType edge type specific meso parameters such as the different taus
     */
    MELSegment(const std::string& id,
              const MSEdge& parent, MESegment* next,
              const double length, const double speed,
              const int idx,
              const bool multiQueue,
              const MesoEdgeType& edgeType);

    /** @brief Removes the vehicle from the segment, adapting its parameters
     *
     * @param[in] veh The vehicle in question
     * @param[in] next The subsequent segment for delay calculation
     * @param[in] time the leave time
     * @todo Isn't always time == veh->getEventTime?
     */
    void send(MEVehicle* veh, MESegment* const next, const int nextQIdx, SUMOTime time, const MSMoveReminder::Notification reason) override;

    /// @brief update entry blockTime for all queues
    void updateEntryBlockTime(SUMOTime time) override; 

protected:
    /// @brief update blockTime of Queue q
    void updateBlockTime(Queue& q, const Queue& qNext, const MESegment* const next, const MEVehicle* veh) override;

    /// @brief check jam-avoidance constraints during insertion
    bool hasSpaceForInsertion(const Queue& q, int qIdx, double newOccupancy, SUMOTime entryTime) const override;

private:
    /// @brief upstream arrival times of traveling gaps between cars (for each queue)
    // @note: could be slightly faster as a double-index ring-buffer
    typedef std::vector<SUMOTime> GapTimes;
    std::vector<GapTimes> myGapTimes;

private:
    /// @brief Invalidated copy constructor.
    MELSegment(const MELSegment&);

    /// @brief Invalidated assignment operator.
    MELSegment& operator=(const MELSegment&);

    /// @brief constructor for dummy segment
    MELSegment(const std::string& id);
};
