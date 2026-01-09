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
/// @file    VehicleGroupStop.cpp
/// @author  Generated
/// @date    2025-01-09
///
// Functions to manage sequential vehicle stops at the same position
/****************************************************************************/
#include <config.h>

#include <map>
#include <queue>
#include <algorithm>
#include "VehicleGroupStop.h"
#include "Vehicle.h"
#include "Helper.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSBaseVehicle.h>
#include <utils/vehicle/SUMOVehicleParameter.h>


// ===========================================================================
// static members
// ===========================================================================
static std::map<std::string, std::vector<std::string>> gStopSequences;
static std::map<std::string, std::queue<std::string>> gStopQueues;

/**
 * Generate a unique key for a stop location
 */
static std::string generateStopKey(const std::string& edgeID, double pos) {
    return edgeID + "_" + std::to_string((int)pos);
}


// ===========================================================================
// method definitions
// ===========================================================================
void
VehicleGroupStop::setSequentialStop(
    const std::vector<std::string>& vehIDs,
    const std::string& edgeID,
    double pos,
    int laneIndex,
    double duration,
    int flags,
    double startPos,
    double until,
    double spacing,
    bool allowUpstreamOverflow) {

    if (vehIDs.empty()) {
        throw std::invalid_argument("Vehicle ID list cannot be empty");
    }

    MSNet* net = MSNet::getInstance();
    if (net == nullptr) {
        throw std::runtime_error("Network not initialized");
    }

    const MSEdge* edge = net->getEdge(edgeID);
    if (edge == nullptr) {
        throw std::invalid_argument("Edge '" + edgeID + "' not found");
    }

    std::string stopKey = generateStopKey(edgeID, pos);
    gStopSequences[stopKey] = vehIDs;

    // Validate that all vehicle IDs exist
    for (const auto& vehID : vehIDs) {
        try {
            Helper::getVehicle(vehID);
        } catch (const std::exception& e) {
            throw std::invalid_argument("Vehicle '" + vehID + "' not found: " + e.what());
        }
    }

    // Get edge properties
    double edgeLength = edge->getLength();
    const std::vector<MSLane*>& lanes = edge->getLanes();

    if (laneIndex < 0 || laneIndex >= (int)lanes.size()) {
        throw std::invalid_argument("Invalid lane index " + std::to_string(laneIndex) + " for edge '" + edgeID + "'");
    }

    const MSLane* lane = lanes[laneIndex];
    double maxLanePos = lane->getLength();

    // Try to fit all vehicles on the current edge
    int vehicleIndex = 0;
    std::vector<std::string> overflowVehicles;

    for (const auto& vehID : vehIDs) {
        try {
            MSBaseVehicle* vehicle = Helper::getVehicle(vehID);
            double vehicleLength = vehicle->getVehicleType().getLength();
            double minGap = vehicle->getVehicleType().getMinGap();

            // Calculate position for this vehicle
            // Vehicles are placed in reverse order (first vehicle at pos, next one behind)
            double stopPos = pos - (vehicleIndex * (vehicleLength + spacing + minGap));

            // Check if position is within lane bounds
            if (stopPos < 0) {
                // Position is before the start of current edge
                if (allowUpstreamOverflow) {
                    overflowVehicles.push_back(vehID);
                    continue;
                } else {
                    throw std::runtime_error("Not enough space on edge '" + edgeID + "' for vehicle '" + vehID + "'");
                }
            }

            // Set the stop for this vehicle
            SUMOVehicleParameter::Stop stopParams = Helper::buildStopParameters(
                edgeID, stopPos, laneIndex, startPos, flags, duration, until);

            std::string error;
            if (!vehicle->addTraciStop(stopParams, error)) {
                throw std::runtime_error("Failed to add stop for vehicle '" + vehID + "': " + error);
            }

            vehicleIndex++;

        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error setting stop for vehicle: ") + e.what());
        }
    }

    // Handle vehicles that don't fit on current edge
    if (!overflowVehicles.empty() && allowUpstreamOverflow) {
        // Calculate required space
        double requiredSpace = 0;
        for (const auto& vehID : overflowVehicles) {
            MSBaseVehicle* vehicle = Helper::getVehicle(vehID);
            double vehicleLength = vehicle->getVehicleType().getLength();
            double minGap = vehicle->getVehicleType().getMinGap();
            requiredSpace += vehicleLength + spacing + minGap;
        }

        // Find upstream edges with available space
        auto upstreamPositions = findUpstreamStoppingPositions(edgeID, requiredSpace);

        if (upstreamPositions.empty()) {
            throw std::runtime_error("No suitable upstream edge found for overflow vehicles");
        }

        // Place overflow vehicles on upstream edges
        int overflowIndex = 0;
        for (const auto& upstreamEdge : upstreamPositions) {
            if (overflowIndex >= (int)overflowVehicles.size()) {
                break;
            }

            const std::string& upstreamEdgeID = upstreamEdge.first;
            double upstreamPos = upstreamEdge.second;

            // Place multiple vehicles on this upstream edge if space allows
            for (int i = 0; i < 1 && overflowIndex < (int)overflowVehicles.size(); i++, overflowIndex++) {
                try {
                    const std::string& vehID = overflowVehicles[overflowIndex];
                    MSBaseVehicle* vehicle = Helper::getVehicle(vehID);

                    SUMOVehicleParameter::Stop stopParams = Helper::buildStopParameters(
                        upstreamEdgeID, upstreamPos, laneIndex, startPos, flags, duration, until);

                    std::string error;
                    if (!vehicle->addTraciStop(stopParams, error)) {
                        throw std::runtime_error("Failed to add upstream stop for vehicle '" + vehID + "': " + error);
                    }
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string("Error setting upstream stop: ") + e.what());
                }
            }
        }
    }
}


void
VehicleGroupStop::advanceQueue(const std::string& stopID) {
    auto it = gStopQueues.find(stopID);
    if (it == gStopQueues.end()) {
        throw std::invalid_argument("Stop queue '" + stopID + "' not found");
    }

    if (!it->second.empty()) {
        it->second.pop();
    }

    if (it->second.empty()) {
        gStopQueues.erase(it);
    }
}


std::vector<std::string>
VehicleGroupStop::getStopSequence(const std::string& edgeID, double pos) {
    std::string stopKey = generateStopKey(edgeID, pos);
    auto it = gStopSequences.find(stopKey);

    if (it != gStopSequences.end()) {
        return it->second;
    }

    return std::vector<std::string>();
}


double
VehicleGroupStop::getSequentialStopPosition(
    const std::string& vehID,
    const std::string& edgeID,
    double basePos,
    double spacing) {

    MSNet* net = MSNet::getInstance();
    MSBaseVehicle* vehicle = Helper::getVehicle(vehID);

    std::string stopKey = generateStopKey(edgeID, basePos);
    auto it = gStopSequences.find(stopKey);

    if (it == gStopSequences.end()) {
        return basePos;
    }

    // Find position of vehicle in sequence
    const auto& sequence = it->second;
    int vehicleIndex = -1;

    for (int i = 0; i < (int)sequence.size(); i++) {
        if (sequence[i] == vehID) {
            vehicleIndex = i;
            break;
        }
    }

    if (vehicleIndex == -1) {
        return basePos;
    }

    // Calculate position based on vehicle index
    double vehicleLength = vehicle->getVehicleType().getLength();
    double minGap = vehicle->getVehicleType().getMinGap();
    double stopPos = basePos - (vehicleIndex * (vehicleLength + spacing + minGap));

    return stopPos;
}


void
VehicleGroupStop::clearStopSequence(const std::string& edgeID, double pos, double tolerance) {
    std::string stopKey = generateStopKey(edgeID, pos);

    auto it = gStopSequences.find(stopKey);
    if (it != gStopSequences.end()) {
        gStopSequences.erase(it);
    }

    auto qIt = gStopQueues.find(stopKey);
    if (qIt != gStopQueues.end()) {
        gStopQueues.erase(qIt);
    }
}


std::vector<std::pair<std::string, double>>
VehicleGroupStop::findUpstreamStoppingPositions(
    const std::string& edgeID,
    double requiredSpace,
    int maxUpstreamEdges) {

    std::vector<std::pair<std::string, double>> positions;
    MSNet* net = MSNet::getInstance();

    if (net == nullptr) {
        return positions;
    }

    const MSEdge* currentEdge = net->getEdge(edgeID);
    if (currentEdge == nullptr) {
        return positions;
    }

    // Traverse upstream edges
    std::vector<const MSEdge*> upstreamEdges;
    const MSEdgeVector& incoming = currentEdge->getPredecessors();

    for (const auto& predEdge : incoming) {
        if (upstreamEdges.size() >= (size_t)maxUpstreamEdges) {
            break;
        }
        upstreamEdges.push_back(predEdge);
    }

    // For each upstream edge, find a suitable position
    for (const auto& upEdge : upstreamEdges) {
        double edgeLength = upEdge->getLength();
        // Try to place vehicles near the end of the upstream edge
        // (closest to the current edge)
        double stopPos = edgeLength - 1.0;

        if (hasSpaceAtPosition(upEdge->getID(), stopPos, requiredSpace, 0.5)) {
            positions.push_back({upEdge->getID(), stopPos});
        }
    }

    return positions;
}


bool
VehicleGroupStop::hasSpaceAtPosition(
    const std::string& edgeID,
    double pos,
    double vehicleLength,
    double vehicleMinGap) {

    MSNet* net = MSNet::getInstance();
    if (net == nullptr) {
        return false;
    }

    const MSEdge* edge = net->getEdge(edgeID);
    if (edge == nullptr) {
        return false;
    }

    if (edge->getLanes().empty()) {
        return false;
    }

    const MSLane* lane = edge->getLanes()[0];
    double laneLength = lane->getLength();

    // Check if position + vehicle length + min gap is within lane bounds
    return (pos + vehicleLength + vehicleMinGap) <= laneLength &&
           (pos - vehicleMinGap) >= 0;
}
