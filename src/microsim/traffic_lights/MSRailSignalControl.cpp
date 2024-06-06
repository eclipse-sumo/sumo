/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include "MSDriveWay.h"
#include "MSRailSignalControl.h"


//#define DEBUG_REGISTER_DRIVEWAY
//#define DEBUG_SIGNALSTATE
//#define DEBUG_RECHECKGREEN

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
        myInstance->myDriveWayCompatibility.clear();
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
#ifdef DEBUG_REGISTER_DRIVEWAY
                    std::cout << "MSRailSignalControl edge=" << edge->getID() << " used by vehicle " << vehicle->getID() << ". Updating " << myProtectedDriveways[edge].size() << " driveways\n";
#endif
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


void
MSRailSignalControl::recheckGreen() {
    if (mySwitchedGreenFlanks.size() > 0) {
        for (const auto& item : mySwitchedGreenFlanks) {
            for (const auto& item2 : mySwitchedGreenFlanks) {
                if (item.second < item2.second) {
                    bool conflict = false;
                    std::pair<int, int> code(item.second, item2.second);
                    auto it = myDriveWayCompatibility.find(code);
                    if (it != myDriveWayCompatibility.end()) {
                        conflict = it->second;
                    } else {
                        // new driveway pair
                        const MSRailSignal* rs = static_cast<const MSRailSignal*>(item.first->getTLLogic());
                        const MSRailSignal* rs2 = static_cast<const MSRailSignal*>(item2.first->getTLLogic());
                        const MSDriveWay& dw = rs->retrieveDriveWay(item.second);
                        const MSDriveWay& dw2 = rs2->retrieveDriveWay(item2.second);
                        // overlap may return true if the driveways are consecutive forward sections
                        conflict = dw.flankConflict(dw2) || dw2.flankConflict(dw);
                        myDriveWayCompatibility[code] = conflict;
#ifdef DEBUG_RECHECKGREEN
                        std::cout << SIMTIME << " new code " << code.first << "," << code.second << " conflict=" << conflict << " dw=" << toString(dw.myRoute) << " dw2=" << toString(dw2.myRoute) << "\n";
#endif
                    }
                    if (conflict) {
                        MSRailSignal* rs = const_cast<MSRailSignal*>(static_cast<const MSRailSignal*>(item.first->getTLLogic()));
                        MSRailSignal* rs2 = const_cast<MSRailSignal*>(static_cast<const MSRailSignal*>(item2.first->getTLLogic()));
                        const MSRailSignal::Approaching& veh = item.first->getClosest();
                        const MSRailSignal::Approaching& veh2 = item2.first->getClosest();
                        if (MSDriveWay::mustYield(veh, veh2)) {
                            std::string state = rs->getCurrentPhaseDef().getState();
                            state[item.first->getTLIndex()] = 'r';
                            const_cast<MSPhaseDefinition&>(rs->getCurrentPhaseDef()).setState(state);
                            rs->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
#ifdef DEBUG_RECHECKGREEN
                            std::cout << SIMTIME << " reset to red " << getClickableTLLinkID(item.first)
                                      << " (" << veh.first->getID() << " yields to " << veh2.first->getID() << "\n";
#endif
#ifdef DEBUG_SIGNALSTATE
                            if (DEBUG_HELPER(rs)) {
                                std::cout << SIMTIME << " reset to red " << getClickableTLLinkID(item.first)
                                          << " (" << veh.first->getID() << " yields to " << veh2.first->getID() << "\n";
                            }
#endif
                        } else {
                            std::string state = rs2->getCurrentPhaseDef().getState();
                            state[item2.first->getTLIndex()] = 'r';
                            const_cast<MSPhaseDefinition&>(rs2->getCurrentPhaseDef()).setState(state);
                            rs2->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
#ifdef DEBUG_RECHECKGREEN
                            std::cout << SIMTIME << " reset to red " << getClickableTLLinkID(item2.first)
                                      << " (" << veh2.first->getID() << " yields to " << veh.first->getID() << "\n";
#endif
#ifdef DEBUG_SIGNALSTATE
                            if (DEBUG_HELPER(rs2)) {
                                std::cout << SIMTIME << " reset to red " << getClickableTLLinkID(item2.first)
                                          << " (" << veh2.first->getID() << " yields to " << veh.first->getID() << "\n";
                            }
#endif
                        }
                    }
                }
            }
        }
        mySwitchedGreenFlanks.clear();
    }
}


/****************************************************************************/
