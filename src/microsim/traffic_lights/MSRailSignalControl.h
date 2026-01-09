/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2026 German Aerospace Center (DLR) and others.
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
class MSRailSignalConstraint;
class MSEdge;
class MSDriveWay;

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

    /// @brief reset all waiting-for relationships at the start of the simulation step
    void resetWaitRelations() {
        myWaitRelations.clear();
        myWrittenDeadlocks.clear();
    }

    void addWaitRelation(const SUMOVehicle* waits, const MSRailSignal* rs, const SUMOVehicle* reason, MSRailSignalConstraint* constraint = nullptr);

    void addDrivewayFollower(const MSDriveWay* dw, const MSDriveWay* dw2);

    /// @brief check whether the given signal and driveway are part of a deadlock circle
    void addDWDeadlockChecks(const MSRailSignal* rs, MSDriveWay* dw);

    /// @brief whether there is a circle in the waiting-for relationships that contains the given vehicle
    bool haveDeadlock(const SUMOVehicle* veh) const;

    void addDeadlockCheck(std::vector<const MSRailSignal*> signals);

    /** @brief Called if a vehicle changes its state
     * @param[in] vehicle The vehicle which changed its state
     * @param[in] to The state the vehicle has changed to
     * @param[in] info Additional information on the state change
     */
    void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info = "");

    void addSignal(MSRailSignal* signal);

    const std::vector<MSRailSignal*>& getSignals() const {
        return mySignals;
    }

    const std::map<const MSRailSignal*, std::vector<const MSRailSignal*> >& getDeadlockChecks() const {
        return myDeadlockChecks;
    }

    /// switch rail signal to active
    void notifyApproach(const MSLink* link);

    /// @brief update active rail signals
    void updateSignals(SUMOTime t);


    static bool isSignalized(SUMOVehicleClass svc) {
        return (mySignalizedClasses & svc) == svc;
    }

    static void initSignalized(SVCPermissions svc) {
        mySignalizedClasses = svc;
    }

protected:

    void findDeadlockFoes(const MSDriveWay* dw, const std::vector<const MSRailSignal*>& others, std::vector<const MSDriveWay*> deadlockFoes);


private:
    /** @brief Constructor */
    MSRailSignalControl();

    /// @brief compute additioanl deadlock-check requirements for registered driveways
    void updateDriveways(const MSEdge* used);

    /// @brief all rail edges that are part of a known route
    std::set<const MSEdge*> myUsedEdges;

    struct WaitRelation {
        WaitRelation(const MSRailSignal* _railSignal = nullptr, const SUMOVehicle* _foe = nullptr, MSRailSignalConstraint* _constraint = nullptr) :
            railSignal(_railSignal), foe(_foe), constraint(_constraint) {}
        // indices along route
        const MSRailSignal* railSignal;
        const SUMOVehicle* foe;
        MSRailSignalConstraint* constraint;
    };
    std::map<const SUMOVehicle*, WaitRelation> myWaitRelations;

    mutable std::set<std::set<const SUMOVehicle*> > myWrittenDeadlocks;

    std::map<const MSRailSignal*, std::vector<const MSRailSignal*> > myDeadlockChecks;
    std::map<const MSDriveWay*, std::set<const MSDriveWay*>> myDriveWaySucc;
    std::map<const MSDriveWay*, std::set<const MSDriveWay*>> myDriveWayPred;

    /// @brief list of all rail signals
    std::vector<MSRailSignal*> mySignals;

    /// @brief list of signals that switched green along with driveway index
    std::vector<std::pair<MSLink*, int> > mySwitchedGreenFlanks;
    std::map<std::pair<int, int>, bool> myDriveWayCompatibility;
    std::set<MSRailSignal*, ComparatorNumericalIdLess> myActiveSignals;

    /// @brief signalized classes
    static SVCPermissions mySignalizedClasses;

    static MSRailSignalControl* myInstance;


};
