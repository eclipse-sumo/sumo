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
        myInstance->myDriveWayCompatibility.clear();
        myInstance->myDriveWaySucc.clear();
        myInstance->myDriveWayPred.clear();
        myInstance->myWrittenDeadlocks.clear();
        myInstance->myDeadlockChecks.clear();
    }
}


MSRailSignalControl::~MSRailSignalControl() {
}

void
MSRailSignalControl::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /*info*/) {
    if (isRailway(vehicle->getVClass())) {
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
MSRailSignalControl::addSignal(MSRailSignal* signal) {
    mySignals.push_back(signal);
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


void
MSRailSignalControl::notifyApproach(const MSLink* link) {
    const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(link->getTLLogic());
    assert(rs != nullptr);
    myActiveSignals.insert(const_cast<MSRailSignal*>(rs));
}


void
MSRailSignalControl::updateSignals(SUMOTime t) {
    UNUSED_PARAMETER(t);
    // there are 4 states for a signal
    // 1. approached and green
    // 2. approached and red
    // 3. not approached and trains could pass
    // 4. not approached and trains coult not pass
    //
    // for understanding conflicts better in sumo-gui, we want to show (3) as green. This
    // means we have to keep updating signals in state (4) until they change to (3)

    //std::cout << SIMTIME << " activeSignals=" << myActiveSignals.size() << "\n";
    for (auto it = myActiveSignals.begin(); it != myActiveSignals.end();) {
        MSRailSignal* rs = *it;
        //std::cout << SIMTIME << " update " << rs->getID() << "\n";
        const bool keepActive = rs->updateCurrentPhase();
        if (rs->isActive()) {
            rs->setTrafficLightSignals(t);
        }
        if (!keepActive) {
            it = myActiveSignals.erase(it);
        } else {
            it++;
        }
    }
}


/****************************************************************************/
