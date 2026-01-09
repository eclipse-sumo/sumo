/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2025 German Aerospace Center (DLR) and others.
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
/// @file    VehicleGroupStop.h
/// @author  Generated
/// @date    2025-01-09
///
// Functions to manage sequential vehicle stops at the same position
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <stdexcept>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class VehicleGroupStop
 * @brief Manages sequential stops of multiple vehicles at the same position
 *
 * This class provides functionality to:
 * - Stop multiple vehicles at the same position sequentially
 * - Automatically place vehicles in a queue-like formation
 * - Overflow to upstream edges when current edge space is insufficient
 */
class VehicleGroupStop {
public:
    /**
     * @brief Set multiple vehicles to stop at the same position with sequential placement
     *
     * @param vehIDs List of vehicle IDs to stop
     * @param edgeID The edge where vehicles should stop
     * @param pos The initial stop position on the edge
     * @param laneIndex The lane index for stopping
     * @param duration Stop duration in seconds
     * @param flags Stop flags (default: STOP_DEFAULT)
     * @param startPos Start position for stopping
     * @param until Stop until this time (seconds)
     * @param spacing The spacing between consecutive vehicles in meters
     * @param allowUpstreamOverflow If true, vehicles overflow to upstream edges
     *
     * @throws std::invalid_argument if vehIDs is empty or invalid
     * @throws std::runtime_error if stopping fails for any vehicle
     */
    static void setSequentialStop(
        const std::vector<std::string>& vehIDs,
        const std::string& edgeID,
        double pos,
        int laneIndex = 0,
        double duration = -1.0,
        int flags = 0,
        double startPos = -1.0,
        double until = -1.0,
        double spacing = 2.5,
        bool allowUpstreamOverflow = true);

    /**
     * @brief Advance the queue by one vehicle (next vehicle in queue starts moving)
     *
     * This function should be called after a stopped vehicle departs.
     * It removes the vehicle from the stop and adjusts other vehicles if needed.
     *
     * @param stopID The identifier for the stop group
     *
     * @throws std::invalid_argument if stopID is not found
     */
    static void advanceQueue(const std::string& stopID);

    /**
     * @brief Get the sequential stop order for a given edge and position
     *
     * @param edgeID The edge ID
     * @param pos The position on the edge
     * @return Vector of vehicle IDs in stop order
     */
    static std::vector<std::string> getStopSequence(const std::string& edgeID, double pos);

    /**
     * @brief Get the current position where a vehicle should stop in the sequence
     *
     * @param vehID The vehicle ID
     * @param edgeID The edge ID
     * @param basePos The base position on the edge
     * @param spacing The spacing between vehicles
     * @return The adjusted position for this vehicle in the sequence
     */
    static double getSequentialStopPosition(
        const std::string& vehID,
        const std::string& edgeID,
        double basePos,
        double spacing = 2.5);

    /**
     * @brief Clear all sequential stop information for a given stop location
     *
     * @param edgeID The edge ID
     * @param pos The position on the edge (tolerance-based matching)
     */
    static void clearStopSequence(const std::string& edgeID, double pos, double tolerance = 1.0);

private:
    /**
     * @brief Find upstream edges and available positions
     *
     * @param edgeID The starting edge ID
     * @param requiredSpace The required space for vehicles
     * @param maxUpstreamEdges Maximum number of upstream edges to check
     * @return Vector of pairs (edge_id, available_position)
     */
    static std::vector<std::pair<std::string, double>> findUpstreamStoppingPositions(
        const std::string& edgeID,
        double requiredSpace,
        int maxUpstreamEdges = 5);

    /**
     * @brief Check if a position on an edge has enough space for a vehicle
     *
     * @param edgeID The edge ID
     * @param pos The position on the edge
     * @param vehicleLength The length of the vehicle
     * @param vehicleMinGap The minimum gap required for the vehicle
     * @return True if space is available
     */
    static bool hasSpaceAtPosition(
        const std::string& edgeID,
        double pos,
        double vehicleLength,
        double vehicleMinGap);
};

#endif
