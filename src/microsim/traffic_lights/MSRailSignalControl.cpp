/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSRailSignalControl.cpp
/// @author  Jakob Erdmann
/// @date    Sept 2020
///
// Centralized services for rail signal control (Singleton)
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include "MSRailSignal.h"
#include "MSRailSignalControl.h"


//#define DEBUG_REGISTER_DRIVEWAY

// ===========================================================================
// static value definitions
// ===========================================================================
MSRailSignalControl* MSRailSignalControl::myInstance(nullptr);

// ===========================================================================
// method definitions
// ===========================================================================
MSRailSignalControl::MSRailSignalControl()
{}

MSRailSignalControl&
MSRailSignalControl::getInstance() {
    if (myInstance == nullptr) {
        myInstance = new MSRailSignalControl();
        MSNet::getInstance()->addVehicleStateListener(myInstance);
    }
    return *myInstance;
}

void
MSRailSignalControl::cleanup() {
    delete myInstance;
    myInstance = nullptr;
}

void
MSRailSignalControl::clearState() {
    if (myInstance != nullptr) {
        myInstance->myUsedEdges.clear();
        myInstance->myProtectedDriveways.clear();
    }
}


MSRailSignalControl::~MSRailSignalControl() {
}

void
MSRailSignalControl::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /*info*/) {
    if (isRailway(vehicle->getVClass())) {
        if (to == MSNet::VehicleState::NEWROUTE || to == MSNet::VehicleState::DEPARTED) {
            for (const MSEdge* edge : vehicle->getRoute().getEdges()) {
                myUsedEdges.insert(edge);
                if (myProtectedDriveways.count(edge) != 0) {
                    updateDriveways(edge);
                }
            }
        }
        if (to == MSNet::VehicleState::BUILT || (!vehicle->hasDeparted() && to == MSNet::VehicleState::NEWROUTE)) {
            // @note we could delay initialization until the departure time
            MSRailSignal::initDriveWays(vehicle, to == MSNet::VehicleState::NEWROUTE);
        }
    }
}


void
MSRailSignalControl::registerProtectedDriveway(MSRailSignal* rs, int driveWayID, const MSEdge* protectedBidi) {
    myProtectedDriveways[protectedBidi].push_back(std::make_pair(rs, driveWayID));
#ifdef DEBUG_REGISTER_DRIVEWAY
    std::cout << "MSRailSignalControl edge=" << protectedBidi->getID() << " assumed secure by driveway " << driveWayID << " at signal " << rs->getID() << "\n";
#endif
}

void
MSRailSignalControl::updateDriveways(const MSEdge* used) {
    for (auto item : myProtectedDriveways[used]) {
        item.first->updateDriveway(item.second);
    }
    myProtectedDriveways.erase(used);
}

void
MSRailSignalControl::addSignal(MSRailSignal* signal) {
    mySignals.push_back(signal);
}

/****************************************************************************/
