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
#include <microsim/MSLane.h>
#include "MSRailSignal.h"
#include "MSDriveWay.h"
#include "MSRailSignalControl.h"


//#define DEBUG_REGISTER_DRIVEWAY
//#define DEBUG_SIGNALSTATE
//#define DEBUG_RECHECKGREEN
//#define DEBUG_BUILD_DEADLOCK_CHECK

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
        myInstance->myDriveWaySucc.clear();
        myInstance->myDriveWayPred.clear();
        myInstance->myWrittenDeadlocks.clear();
        myInstance->myDWDeadlocks.clear();
        myInstance->myDeadlockChecks.clear();
    }
}


MSRailSignalControl::~MSRailSignalControl() {
}

void
MSRailSignalControl::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /*info*/) {
    if (isRailway(vehicle->getVClass())) {
        if (to == MSNet::VehicleState::NEWROUTE || to == MSNet::VehicleState::DEPARTED || to == MSNet::VehicleState::BUILT) {
            for (const MSEdge* edge : vehicle->getRoute().getEdges()) {
                myUsedEdges.insert(edge);
                if (myProtectedDriveways.count(edge) != 0) {
#ifdef DEBUG_REGISTER_DRIVEWAY
                    std::cout << "MSRailSignalControl edge=" << edge->getID() << " used by vehicle " << vehicle->getID() << ". Updating " << myProtectedDriveways[edge].size() << " driveways, time=" << time2string(SIMSTEP) << "\n";
#endif
                    updateDriveways(edge);
                }
            }
        }
        std::string dummyMsg;
        if ((to == MSNet::VehicleState::BUILT && (!vehicle->getParameter().wasSet(VEHPARS_FORCE_REROUTE) || vehicle->hasValidRoute(dummyMsg)))
                || (!vehicle->hasDeparted() && to == MSNet::VehicleState::NEWROUTE)) {
            // @note we could delay initialization until the departure time
            if (vehicle->getEdge()->getFunction() != SumoXMLEdgeFunc::CONNECTOR) {
                MSRailSignal::initDriveWays(vehicle, to == MSNet::VehicleState::NEWROUTE);
            }
        }
    }
}


void
MSRailSignalControl::registerProtectedDriveway(MSRailSignal* rs, const MSEdge* first, int driveWayID, const MSEdge* protectedBidi) {
    myProtectedDriveways[protectedBidi].push_back(ProtectedDriveway(rs, first, driveWayID));
#ifdef DEBUG_REGISTER_DRIVEWAY
    std::cout << "MSRailSignalControl edge=" << protectedBidi->getID() << " assumed secure by driveway " << driveWayID << " at signal " << Named::getIDSecure(rs) << " first=" << first->getID() << "\n";
#endif
}


void
MSRailSignalControl::updateDriveways(const MSEdge* used) {
    for (const ProtectedDriveway& pdw : myProtectedDriveways[used]) {
        if (pdw.rs != nullptr) {
            pdw.rs->updateDriveway(pdw.dwID);
        } else {
            MSDriveWay::updateDepartDriveway(pdw.first, pdw.dwID);
        }
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


bool
MSRailSignalControl::haveDeadlock(const SUMOVehicle* veh) const {
    std::set<const SUMOVehicle*> seen;
    std::vector<std::pair<const MSRailSignal*, const SUMOVehicle*> > list;
    const SUMOVehicle* cur = veh;
    while (seen.count(cur) == 0) {
        auto it = myWaitRelations.find(cur);
        if (it != myWaitRelations.end()) {
            seen.insert(cur);
            list.push_back(it->second);
            cur = it->second.second;
        } else {
            return false;
        }
    }
    if (cur == veh) {
        if (OptionsCont::getOptions().isSet("deadlock-output")) {
            if (myWrittenDeadlocks.count(seen) == 0) {
                myWrittenDeadlocks.insert(seen);
                std::vector<std::string> signals;
                std::vector<std::string> vehicles;
                for (auto item : list) {
                    signals.push_back(item.first->getID());
                    vehicles.push_back(item.second->getID());
                }
                OutputDevice& od = OutputDevice::getDeviceByOption("deadlock-output");
                od.openTag(SUMO_TAG_DEADLOCK);
                od.writeAttr(SUMO_ATTR_TIME, time2string(SIMSTEP));
                od.writeAttr(SUMO_ATTR_SIGNALS, signals);
                od.writeAttr("vehicles", vehicles);
                od.closeTag();
            }
        }
        return true;
    } else {
        // it's a deadlock but does not involve veh
        return false;
    }
}


void
MSRailSignalControl::addDeadlockCheck(std::vector<const MSRailSignal*> signals) {
    if (MSDriveWay::haveDriveWays()) {
        WRITE_WARNING("Deadlocks should be loaded before any vehicles");
    }
    const int n = (int)signals.size();
    for (int i = 0; i < n; i++) {
        std::vector<const MSRailSignal*> others;
        for (int j = 0; j < n; j++) {
            others.push_back(signals[(i + j + 1) % n]);
        }
        myDeadlockChecks[signals[i]] = others;
    }
}

void
MSRailSignalControl::addDrivewayFollower(const MSDriveWay* dw, const MSDriveWay* dw2) {
    //std::cout << " addDrivewayFollower " << dw->getID() << " " << dw2->getID() << "\n";
    myDriveWaySucc[dw].insert(dw2);
    myDriveWayPred[dw2].insert(dw);
}


void
MSRailSignalControl::addDWDeadlockChecks(const MSRailSignal* rs, MSDriveWay* dw) {
    auto itDL = MSRailSignalControl::getInstance().getDeadlockChecks().find(rs);
    if (itDL == MSRailSignalControl::getInstance().getDeadlockChecks().end()) {
        return;
    }
    const std::vector<const MSRailSignal*>& others = itDL->second;
    // the driveway could be part of a deadlock. check whether it would be blocked by the next signal in the circle
    std::vector<const MSDriveWay*> deadlockFoes;
    findDeadlockFoes(dw, others, deadlockFoes);
}


void
MSRailSignalControl::findDeadlockFoes(const MSDriveWay* dw, const std::vector<const MSRailSignal*>& others, std::vector<const MSDriveWay*> deadlockFoes) {
#ifdef DEBUG_BUILD_DEADLOCK_CHECK
    //std::cout << " findDLfoes dw=" << dw->getID() << " dlFoes=" << toString(deadlockFoes) << "\n";
#endif
    int circleIndex = (int)deadlockFoes.size();
    if (circleIndex < (int)others.size()) {
        const MSRailSignal* other = others[circleIndex];
        deadlockFoes.push_back(dw);
        for (const MSDriveWay* follower : myDriveWaySucc[dw]) {
            for (MSDriveWay* foe : follower->getFoes()) {
                if (foe->getForward().back()->getEdge().getToJunction()->getID() == other->getID()) {
                    findDeadlockFoes(foe, others, deadlockFoes);
                }
            }
        }
    } else {
#ifdef DEBUG_BUILD_DEADLOCK_CHECK
        std::cout << " add deadlock check foes=" << toString(deadlockFoes) << "\n";;
#endif
        for (const MSDriveWay* dldw : deadlockFoes) {
            if (dldw->isDepartDriveway()) {
                const_cast<MSDriveWay*>(dldw)->addDWDeadlock(deadlockFoes);
            } else {
                for (const MSDriveWay* pred : myDriveWayPred[dldw]) {
                    if (!pred->isDepartDriveway()) {
                        const MSRailSignal* predRS = dynamic_cast<const MSRailSignal*>(pred->getOrigin()->getTLLogic());
                        if (std::find(others.begin(), others.end(), predRS) != others.end()) {
                            // driveways that participate in the deadlock don't need to
                            // do checking since they cannot prevent the deadlock
                            // (unless they are departDriveways)
                            continue;
                        }
                        const_cast<MSDriveWay*>(pred)->addDWDeadlock(deadlockFoes);
                    }
                }
            }
        }
    }
}


/****************************************************************************/
