/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSLCHelper.h
/// @author  Jakob Erdmann
/// @date    Fri, 19.06.2020
///
// Common functions for lane change models
/****************************************************************************/
#pragma once
#include <config.h>
#include <vector>

// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCHelper
 * @brief A lane change model developed by J. Erdmann
 */
class MSLCHelper {
public:

    /* @brief Computes the artificial bonus distance for roundabout lanes
     *        this additional distance reduces the sense of urgency within
     *        roundabouts and thereby promotes the use of the inner roundabout
     *        lane in multi-lane roundabouts.
     * @param[in] veh The ego vehicle
     * @param[in] curr continuation info along veh's current lane
     * @param[in] neigh continuation info along a neighboring lane (in MSLCM_2013::_wantsChange() the considered lane for a lanechange)
     * @param[in] best continuation info along the strategicaly preferred lane
     * @return the bonus distance for accepting a strategically-unfavourable roundabout-inside-lane
     */
    static double getRoundaboutDistBonus(const MSVehicle& veh,
                                         double bonusParam,
                                         const MSVehicle::LaneQ& curr,
                                         const MSVehicle::LaneQ& neigh,
                                         const MSVehicle::LaneQ& best);


    /* @brief save space for vehicles which need to counter-lane-change in
     * order to avoid avoid counter-lane-change-deadlock
     *
     * @param[in] veh The ego vehicle
     * @param[in] blocker The vehicle which is blocked from lane changing
     * @param[in] lcaCounter The lane change direction opposite to ego's current wish
     * @param[in] leftSpace The remaining distance that can be driving without lane change
     * @param[in,out] leadingBlockerLength: the length to reserve at the end of the lane
     * @return Whether sufficient space has been reserved (by vehicle or blocker)
     */
    static bool saveBlockerLength(const MSVehicle& veh, MSVehicle* blocker, int lcaCounter, double leftSpace, bool reliefConnection, double& leadingBlockerLength);

    /* @brief return saveable space
     * @param[in] requested The space that should be saved for another vehicle
     * @param[in] leftSpace The remaining distance that can be driving without lane change
     * @return Whether the requested space can be reserved
     */
    static bool canSaveBlockerLength(const MSVehicle& veh, double requested, double leftSpace);

    /// @brief return whether the vehicles are on the same junction but on divergent paths
    static bool divergentRoute(const MSVehicle& v1, const MSVehicle& v2);

    static double getSpeedPreservingSecureGap(const MSVehicle& leader, const MSVehicle& follower, double currentGap, double leaderPlannedSpeed);
};
