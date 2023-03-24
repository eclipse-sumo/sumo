/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSRailSignalControl.h
/// @author  Jakob Erdmann
/// @date    Sept 2020
///
// Centralized services for rail signal control (Singleton)
// - monitors track usage for long-range deadlock prevention
/****************************************************************************/
#pragma once
#include <config.h>
#include <microsim/MSNet.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSRailSignal;
class MSEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRailSignalControl
 * @brief A signal for rails
 */
class MSRailSignalControl : public MSNet::VehicleStateListener {
public:
    ~MSRailSignalControl();

    static MSRailSignalControl& getInstance();

    static bool hasInstance() {
        return myInstance != nullptr;
    }

    static void cleanup();

    /** @brief Perform resets events when quick-loading state */
    static void clearState();

    /** @brief Called if a vehicle changes its state
     * @param[in] vehicle The vehicle which changed its state
     * @param[in] to The state the vehicle has changed to
     * @param[in] info Additional information on the state change
     */
    void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info = "");

    /// @brief mark driveway that must receive additional checks if protectedBidi is ever used by a train route
    void registerProtectedDriveway(MSRailSignal* rs, int driveWayID, const MSEdge* protectedBidi);

    const std::set<const MSEdge*>& getUsedEdges() const {
        return myUsedEdges;
    }

    void addSignal(MSRailSignal* signal);

    const std::vector<MSRailSignal*>& getSignals() const {
        return mySignals;
    }


private:
    /** @brief Constructor */
    MSRailSignalControl();

    /// @brief compute additioanl deadlock-check requirements for registered driveways
    void updateDriveways(const MSEdge* used);

    /// @brief all rail edges that are part of a known route
    std::set<const MSEdge*> myUsedEdges;

    /// @brief map of driveways that must perform additional checks if the key edge is used by a train route
    std::map<const MSEdge*, std::vector<std::pair<MSRailSignal*, int> > > myProtectedDriveways;

    /// @brief list of all rail signals
    std::vector<MSRailSignal*> mySignals;

    static MSRailSignalControl* myInstance;


};
