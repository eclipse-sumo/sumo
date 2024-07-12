/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSDriveWay.h
/// @author  Jakob Erdmann
/// @date    December 2021
///
// A sequende of rail tracks (lanes) that may be used as a "set route" (Fahrstraße)
/****************************************************************************/
#include <config.h>
#include <cassert>
#include <utility>

#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSJunctionLogic.h>
#include "MSRailSignal.h"
#include "MSDriveWay.h"
#include "MSRailSignalControl.h"

// typical block length in germany on main lines is 3-5km on branch lines up to 7km
// special branches that are used by one train exclusively could also be up to 20km in length
// minimum block size in germany is 37.5m (LZB)
// larger countries (USA, Russia) might see blocks beyond 20km)
#define MAX_BLOCK_LENGTH 20000
#define MAX_SIGNAL_WARNINGS 10

#define DRIVEWAY_SANITY_CHECK

//#define DEBUG_BUILD_DRIVEWAY
//#define DEBUG_BUILD_SUBDRIVEWAY
//#define DEBUG_DRIVEWAY_BUILDROUTE
//#define DEBUG_CHECK_FLANKS
//#define DEBUG_SIGNALSTATE_PRIORITY
//#define DEBUG_FIND_PROTECTION
//#define DEBUG_ADD_FOES
//#define DEBUG_SIGNALSTATE
//#define DEBUG_MOVEREMINDER

#define DEBUG_COND DEBUG_HELPER(this)
//#define DEBUG_HELPER(obj) ((obj)->isSelected())
//#define DEBUG_HELPER(obj) ((obj)->getID() == "")
#define DEBUG_HELPER(obj) (true)

//#define DEBUG_COND_DW (dw->myNumericalID == 5)
#define DEBUG_COND_DW (true)

// ===========================================================================
// static value definitions
// ===========================================================================
int MSDriveWay::myGlobalDriveWayIndex(0);
int MSDriveWay::myDepartDriveWayIndex(0);
int MSDriveWay::myNumWarnings(0);
bool MSDriveWay::myWriteVehicles(false);
std::map<const MSLink*, std::vector<MSDriveWay*> > MSDriveWay::mySwitchDriveWays;
std::map<const MSEdge*, std::vector<MSDriveWay*> > MSDriveWay::myDepartureDriveways;
std::map<const MSEdge*, std::vector<MSDriveWay*> > MSDriveWay::myDepartureDrivewaysEnds;
std::map<const MSEdge*, std::vector<MSDriveWay*> > MSDriveWay::myEndingDriveways;

// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDriveWay::init() {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("railsignal-vehicle-output")) {
        OutputDevice::createDeviceByOption("railsignal-vehicle-output");
        myWriteVehicles = true;
    } else {
        myWriteVehicles = false;
    }
}

// ===========================================================================
// MSDriveWay method definitions
// ===========================================================================


MSDriveWay::MSDriveWay(const std::string& id, bool temporary) :
        MSMoveReminder("DriveWay_" + (temporary ? "tmp" : id)),
        Named(id),
        myNumericalID(temporary ? -1 : myGlobalDriveWayIndex++),
        myMaxFlankLength(0),
        myActive(nullptr),
        myProtectedBidi(nullptr),
        myCoreSize(0),
        myFoundSignal(false),
        myFoundReversal(false),
        myIsSubDriveway(false),
        myIsDepartDriveway(false)
{}


MSDriveWay::~MSDriveWay() {
    for (SUMOVehicle* veh : myTrains) {
        MSBaseVehicle* bVeh = dynamic_cast<MSBaseVehicle*>(veh);
        bVeh->removeReminder(this);
    }
    for (const MSLane* lane : myForward) {
        const_cast<MSLane*>(lane)->removeMoveReminder(this);
    }
    for (MSDriveWay* foe : myFoes) {
        if (foe == this) {
            continue;
        }
        auto it = std::find(foe->myFoes.begin(), foe->myFoes.end(), this);
        if (it != foe->myFoes.end()) {
            foe->myFoes.erase(it);
        }
    }
    for (const MSLink* link : myForwardSwitches) {
        std::vector<MSDriveWay*>& dws = mySwitchDriveWays[link];
        dws.erase(std::find(dws.begin(), dws.end(), this));
    }
    if (myLane != nullptr) {
        const MSEdge* first = &myLane->getEdge();
        if (myIsDepartDriveway) {
            std::vector<MSDriveWay*>& dws = myDepartureDriveways[first];
            dws.erase(std::find(dws.begin(), dws.end(), this));
            std::vector<MSDriveWay*>& dws2 = myDepartureDrivewaysEnds[&myForward.back()->getEdge()];
            dws2.erase(std::find(dws2.begin(), dws2.end(), this));
        }
    }
    if (myNumericalID != -1 && myForward.size() > 0 && !myIsSubDriveway) {
        std::vector<MSDriveWay*>& dws = myEndingDriveways[&myForward.back()->getEdge()];
        dws.erase(std::find(dws.begin(), dws.end(), this));
    }
    for (const MSDriveWay* sub : mySubDriveWays) {
        delete sub;
    }
}


bool
MSDriveWay::notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane) {
    UNUSED_PARAMETER(reason);
    UNUSED_PARAMETER(enteredLane);
#ifdef DEBUG_MOVEREMINDER
    std::cout << SIMTIME << " notifyEnter " << getDescription() << " veh=" << veh.getID() << " lane=" << enteredLane->getID() << " reason=" << reason << "\n";
#endif
    if (veh.isVehicle() && enteredLane == myLane && (reason == NOTIFICATION_DEPARTED || reason == NOTIFICATION_JUNCTION)) {
        SUMOVehicle& sveh = dynamic_cast<SUMOVehicle&>(veh);
        MSRouteIterator firstIt = std::find(sveh.getCurrentRouteEdge(), sveh.getRoute().end(), myLane->getNextNormal());
        if (myTrains.count(&sveh) == 0 && match(sveh.getRoute(), firstIt)) {
            myTrains.insert(&sveh);
            if (myWriteVehicles) {
                myVehicleEvents.push_back(VehicleEvent(SIMSTEP, true, veh.getID(), reason));
            }
            return true;
        }
    }
    return false;
}


bool
MSDriveWay::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/, Notification reason, const MSLane* enteredLane) {
    UNUSED_PARAMETER(reason);
    UNUSED_PARAMETER(enteredLane);
#ifdef DEBUG_MOVEREMINDER
    std::cout << SIMTIME << " notifyLeave " << getDescription() << " veh=" << veh.getID() << " lane=" << Named::getIDSecure(enteredLane) << " reason=" << toString(reason) << "\n";
#endif
    if (veh.isVehicle()) {
        // leaving network with departure, teleport etc
        if (reason != MSMoveReminder::NOTIFICATION_JUNCTION && reason != MSMoveReminder::NOTIFICATION_SEGMENT) {
            myTrains.erase(&dynamic_cast<SUMOVehicle&>(veh));
            if (myWriteVehicles) {
                myVehicleEvents.push_back(VehicleEvent(SIMSTEP, false, veh.getID(), reason));
            }
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}


bool
MSDriveWay::notifyLeaveBack(SUMOTrafficObject& veh, Notification reason, const MSLane* leftLane) {
    UNUSED_PARAMETER(reason);
    UNUSED_PARAMETER(leftLane);
#ifdef DEBUG_MOVEREMINDER
    std::cout << SIMTIME << " notifyLeaveBack " << getDescription() << " veh=" << veh.getID() << " lane=" << Named::getIDSecure(leftLane) << " reason=" << toString(reason) << "\n";
#endif
    if (veh.isVehicle()) {
        if (leftLane == myForward.back()) {
            myTrains.erase(&dynamic_cast<SUMOVehicle&>(veh));
            if (myWriteVehicles) {
                myVehicleEvents.push_back(VehicleEvent(SIMSTEP, false, veh.getID(), reason));
            }
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

bool
MSDriveWay::reserve(const Approaching& closest, MSEdgeVector& occupied) {
    /*
    if (foeDriveWayOccupied(true, closest.first, occupied)) {
        return false;
    }
    */
    if (conflictLaneOccupied(true, closest.first)) {
        for (const MSLane* bidi : myBidi) {
            if (!bidi->empty() && bidi->getBidiLane() != nullptr) {
                occupied.push_back(&bidi->getBidiLane()->getEdge());
            }
        }
#ifdef DEBUG_SIGNALSTATE
        if (gDebugFlag4) {
            std::cout << "  conflictLaneOccupied by=" << toString(MSRailSignal::blockingVehicles()) << " ego=" << Named::getIDSecure(closest.first) << "\n";
        }
#endif
        return false;
    }
    for (MSLink* link : myProtectingSwitches) {
        if (!findProtection(closest, link)) {
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << "  no protection at switch " << link->getDescription() << "\n";
            }
#endif
            return false;
        }
    }
    for (MSLink* foeLink : myConflictLinks) {
        if (hasLinkConflict(closest, foeLink)) {
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << "  linkConflict with " << getTLLinkID(foeLink) << "\n";
            }
#endif
            return false;
        }
    }
    if (deadlockLaneOccupied(closest.first)) {
        return false;
    }
    myActive = closest.first;
    return true;
}


bool
MSDriveWay::conflictLinkApproached() const {
    for (MSLink* foeLink : myConflictLinks) {
        if (foeLink->getApproaching().size() > 0) {
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << SIMTIME << " foeLink=" << foeLink->getDescription() << " approachedBy=" << foeLink->getApproaching().begin()->first->getID() << "\n";
            }
#endif
            return true;
        }
    }
    return false;
}


bool
MSDriveWay::hasLinkConflict(const Approaching& veh, MSLink* foeLink) const {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
    if (gDebugFlag4) {
        std::cout << "   checkLinkConflict foeLink=" << getTLLinkID(foeLink) << "\n";
    }
#endif
    if (foeLink->getApproaching().size() > 0) {
        Approaching foe = foeLink->getClosest();
#ifdef DEBUG_SIGNALSTATE_PRIORITY
        if (gDebugFlag4) {
            std::cout << "     approaching foe=" << foe.first->getID() << "\n";
        }
#endif
        const MSTrafficLightLogic* foeTLL = foeLink->getTLLogic();
        assert(foeTLL != nullptr);
        const MSRailSignal* constFoeRS = dynamic_cast<const MSRailSignal*>(foeTLL);
        MSRailSignal* foeRS = const_cast<MSRailSignal*>(constFoeRS);
        if (foeRS != nullptr) {
            const MSDriveWay& foeDriveWay = foeRS->retrieveDriveWayForVeh(foeLink->getTLIndex(), foe.first);
            if (foeDriveWay.conflictLaneOccupied(false, foe.first) ||
                    foeDriveWay.deadlockLaneOccupied(nullptr, false) ||
                    !foeRS->constraintsAllow(foe.first) ||
                    !overlap(foeDriveWay)) {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
                if (gDebugFlag4) {
                    if (foeDriveWay.conflictLaneOccupied(false, foe.first)) {
                        std::cout << "     foe blocked\n";
                    } else if (!foeRS->constraintsAllow(foe.first)) {
                        std::cout << "     foe constrained\n";
                    } else {
                        std::cout << "     no overlap\n";
                    }
                }
#endif
                return false;
            }
#ifdef DEBUG_SIGNALSTATE_PRIORITY
            if (gDebugFlag4) {
                std::cout
                        << "  aSB=" << veh.second.arrivalSpeedBraking << " foeASB=" << foe.second.arrivalSpeedBraking
                        << "  aT=" << veh.second.arrivalTime << " foeAT=" << foe.second.arrivalTime
                        << "  aS=" << veh.first->getSpeed() << " foeS=" << foe.first->getSpeed()
                        << "  aD=" << veh.second.dist << " foeD=" << foe.second.dist
                        << "  aW=" << veh.first->getWaitingTime() << " foeW=" << foe.first->getWaitingTime()
                        << "  aN=" << veh.first->getNumericalID() << " foeN=" << foe.first->getNumericalID()
                        << "\n";
            }
#endif
            const bool yield = mustYield(veh, foe);
            if (MSRailSignal::storeVehicles()) {
                MSRailSignal::rivalVehicles().push_back(foe.first);
                if (yield) {
                    MSRailSignal::priorityVehicles().push_back(foe.first);
                }
            }
            return yield;
        }
    }
    return false;
}


bool
MSDriveWay::mustYield(const Approaching& veh, const Approaching& foe) {
    if (foe.second.arrivalSpeedBraking == veh.second.arrivalSpeedBraking) {
        if (foe.second.arrivalTime == veh.second.arrivalTime) {
            if (foe.first->getSpeed() == veh.first->getSpeed()) {
                if (foe.second.dist == veh.second.dist) {
                    if (foe.first->getWaitingTime() == veh.first->getWaitingTime()) {
                        return foe.first->getNumericalID() < veh.first->getNumericalID();
                    } else {
                        return foe.first->getWaitingTime() > veh.first->getWaitingTime();
                    }
                } else {
                    return foe.second.dist < veh.second.dist;
                }
            } else {
                return foe.first->getSpeed() > veh.first->getSpeed();
            }
        } else {
            return foe.second.arrivalTime < veh.second.arrivalTime;
        }
    } else {
        return foe.second.arrivalSpeedBraking > veh.second.arrivalSpeedBraking;
    }
}


bool
MSDriveWay::conflictLaneOccupied(bool store, const SUMOVehicle* ego) const {
    for (const MSLane* lane : myConflictLanes) {
        if (!lane->isEmpty()) {
            std::string joinVehicle = "";
            if (ego != nullptr && !MSGlobals::gUseMesoSim) {
                const SUMOVehicleParameter::Stop* stop = ego->getNextStopParameter();
                if (stop != nullptr) {
                    joinVehicle = stop->join;
                }
            }
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << SIMTIME << " conflictLane " << lane->getID() << " occupied ego=" << Named::getIDSecure(ego) << " vehNumber=" << lane->getVehicleNumber() << "\n";
                if (joinVehicle != "") {
                    std::cout << "  joinVehicle=" << joinVehicle << " occupant=" << toString(lane->getVehiclesSecure()) << "\n";
                    lane->releaseVehicles();
                }
            }
#endif
            if (lane->getVehicleNumberWithPartials() == 1) {
                MSVehicle* foe = lane->getLastAnyVehicle();
                if (joinVehicle != "") {
                    if (foe->getID() == joinVehicle && foe->isStopped()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4) {
                            std::cout << "    ignore join-target '" << joinVehicle << "\n";
                        }
#endif
                        continue;
                    }
                }
                if (ego != nullptr) {
                    if (foe == ego && std::find(myForward.begin(), myForward.end(), lane) == myForward.end()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4) {
                            std::cout << "    ignore ego as oncoming '" << ego->getID() << "\n";
                        }
#endif
                        continue;
                    }
                    if (foe->isStopped() && foe->getNextStopParameter()->join == ego->getID()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4) {
                            std::cout << "    ignore " << foe->getID() << " for which ego is join-target\n";
                        }
#endif
                        continue;
                    }
                }
            }
            if (MSRailSignal::storeVehicles() && store) {
                MSRailSignal::blockingVehicles().push_back(lane->getLastAnyVehicle());
            }
            return true;
        }
    }
    return false;
}


bool
MSDriveWay::foeDriveWayOccupied(bool store, const SUMOVehicle* ego, MSEdgeVector& occupied) const {
    for (const MSDriveWay* foeDW : myFoes) {
        if (!foeDW->myTrains.empty()) {
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4 || DEBUG_COND_DW) {
                std::cout << SIMTIME << " foeDriveWay " << foeDW->getID() << " occupied ego=" << Named::getIDSecure(ego) << " foeVeh=" << toString(foeDW->myTrains) << "\n";
            }
#endif
            if (foeDW->myTrains.size() == 1) {
                SUMOVehicle* foe = *foeDW->myTrains.begin();
                std::string joinVehicle = "";
                if (ego != nullptr && !MSGlobals::gUseMesoSim) {
                    const SUMOVehicleParameter::Stop* stop = ego->getNextStopParameter();
                    if (stop != nullptr) {
                        joinVehicle = stop->join;
                    }
                }
                if (joinVehicle != "") {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4 || DEBUG_COND_DW) {
                        std::cout << "  joinVehicle=" << joinVehicle << "\n";
                    }
#endif
                    if (foe->getID() == joinVehicle && foe->isStopped()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4 || DEBUG_COND_DW) {
                            std::cout << "    ignore join-target '" << joinVehicle << "\n";
                        }
#endif
                        continue;
                    }
                }
                if (ego != nullptr) {
                    /*
                    if (foe == ego && std::find(myBidi.begin(), myBidi.end(), lane) != myBidi.end()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4) {
                            std::cout << "    ignore ego as oncoming '" << ego->getID() << "\n";
                        }
#endif
                        continue;
                    }
                    */
                    if (foe->isStopped() && foe->getNextStopParameter()->join == ego->getID()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4 || DEBUG_COND_DW) {
                            std::cout << "    ignore " << foe->getID() << " for which ego is join-target\n";
                        }
#endif
                        continue;
                    }
                }
            }
            if (MSRailSignal::storeVehicles() && store) {
                for (SUMOVehicle* foe : foeDW->myTrains) {
                    MSRailSignal::blockingVehicles().push_back(foe);
                }
            }
            for (const SUMOVehicle* foe : foeDW->myTrains) {
                occupied.push_back(const_cast<MSEdge*>(foe->getEdge()));
                MSEdge* bidi = const_cast<MSEdge*>(foe->getEdge()->getBidiEdge());
                if (bidi != nullptr) {
                    occupied.push_back(bidi);
                }
                /// @todo: if foe occupies more than one edge we should add all of them to the occupied vector
            }
            return true;
        }
    }
    return false;
}


bool
MSDriveWay::deadlockLaneOccupied(const SUMOVehicle* ego, bool store) const {
    for (const MSLane* lane : myBidiExtended) {
        if (!lane->empty()) {
            assert(myBidi.size() != 0);
            const MSEdge* lastBidi = myBidi.back()->getNextNormal();
            MSVehicle* foe = lane->getVehiclesSecure().front();
            if (foe == ego) {
                continue;
            }
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << "  check for deadlock with " << foe->getID() << "\n";
            }
#endif
            // check of foe will enter myBidi (need to check at most
            // myBidiExtended.size edges)
            const int minEdges = (int)myBidiExtended.size();
            auto foeIt = foe->getCurrentRouteEdge() + 1;
            auto foeEnd = foe->getRoute().end();
            bool conflict = false;
            for (int i = 0; i < minEdges && foeIt != foeEnd; i++) {
                if ((*foeIt) == lastBidi) {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4) {
                        std::cout << "    vehicle will enter " << lastBidi->getID() << "\n";
                    }
#endif
                    conflict = true;
                    break;
                }
                foeIt++;
            }
            lane->releaseVehicles();
            if (conflict) {
                if (MSRailSignal::storeVehicles() && store) {
                    MSRailSignal::blockingVehicles().push_back(foe);
                }
                return true;
            }
        }
    }
    return false;
}


bool
MSDriveWay::findProtection(const Approaching& veh, MSLink* link) const {
    double flankApproachingDist = std::numeric_limits<double>::max();
    if (link->getApproaching().size() > 0) {
        Approaching closest = link->getClosest();
        flankApproachingDist = closest.second.dist;
    }
#ifdef DEBUG_FIND_PROTECTION
    if (gDebugFlag4) {
        std::cout << SIMTIME << " findProtection for link=" << link->getDescription() << " flankApproachingDist=" << flankApproachingDist << "\n";
    }
#endif
    for (MSLink* l2 : link->getLaneBefore()->getLinkCont()) {
        if (l2->getLane() != link->getLane()) {
#ifdef DEBUG_FIND_PROTECTION
            if (gDebugFlag4) {
                std::cout << " protectionCandidate=" << l2->getDescription() << " l2Via=" << Named::getIDSecure(l2->getViaLane())
                          << " occupied=" << (l2->getViaLane() != nullptr && !l2->getViaLane()->isEmpty()) << "\n";
            }
#endif
            if (l2->getViaLane() != nullptr && !l2->getViaLane()->isEmpty()) {
#ifdef DEBUG_FIND_PROTECTION
                if (gDebugFlag4) {
                    std::cout << "   protection from internal=" << l2->getViaLane()->getID() << "\n";
                }
#endif
                return true;
            }
            if (l2->getApproaching().size() > 0) {
                Approaching closest2 = l2->getClosest();
                if (closest2.second.dist < flankApproachingDist) {
#ifdef DEBUG_FIND_PROTECTION
                    if (gDebugFlag4) {
                        std::cout << "   protection from veh=" << closest2.first->getID() << "\n";
                    }
#endif
                    return true;
                }
            }
        }
    }
    if (link->getApproaching().size() == 0) {
        return true;
    } else {
        // find protection further upstream
        MSDriveWay tmp("tmp", true);
        const MSLane* before = link->getLaneBefore();
        tmp.myFlank.push_back(before);
        LaneVisitedMap visited;
        for (auto ili : before->getIncomingLanes()) {
            tmp.findFlankProtection(ili.viaLink, myMaxFlankLength, visited, ili.viaLink, tmp.myFlank);
        }
        tmp.myConflictLanes = tmp.myFlank;
        tmp.myRoute = myRoute;
        tmp.myCoreSize = myCoreSize;
        MSEdgeVector occupied;
        if (gDebugFlag4) std::cout << SIMTIME << " tmpDW flank=" << toString(tmp.myFlank)
            << " protSwitch=" << MSRailSignal::describeLinks(tmp.myProtectingSwitches)
                << " cLinks=" << MSRailSignal::describeLinks(tmp.myConflictLinks) << "\n";
        return tmp.reserve(veh, occupied);
    }
}


bool
MSDriveWay::overlap(const MSDriveWay& other) const {
    for (int i = 0; i < myCoreSize; i++) {
        for (int j = 0; j < other.myCoreSize; j++) {
            const MSEdge* edge = myRoute[i];
            const MSEdge* edge2 = other.myRoute[j];
            if (edge->getToJunction() == edge2->getToJunction()
                    || edge->getToJunction() == edge2->getFromJunction()) {
                // XXX might be rail_crossing with parallel tracks
                return true;
            }
        }
    }
    return false;
}

bool
MSDriveWay::flankConflict(const MSDriveWay& other) const {
    for (const MSLane* lane : myForward) {
        for (const MSLane* lane2 : other.myForward) {
            if (lane == lane2) {
                return true;
            }
        }
        for (const MSLane* lane2 : other.myBidi) {
            if (lane == lane2) {
                if (bidiBlockedBy(other)) {
                    // it's only a deadlock if both trains block symmetrically
                    return true;
                }
            }
        }
    }
    return false;
}


bool
MSDriveWay::crossingConflict(const MSDriveWay& other) const {
    for (const MSLane* lane : myForward) {
        for (const MSLane* lane2 : other.myForward) {
            if (lane->isNormal() && lane2->isNormal() && lane->getEdge().getToJunction() == lane2->getEdge().getToJunction()) {
                return true;
            }
        }
    }
    return false;
}


bool
MSDriveWay::bidiBlockedBy(const MSDriveWay& other) const {
    for (const MSLane* lane : myBidi) {
        for (const MSLane* lane2 : other.myForward) {
            if (lane == lane2) {
                return true;
            }
        }
    }
    for (const MSLane* lane : myBidiExtended) {
        for (const MSLane* lane2 : other.myForward) {
            if (lane == lane2) {
                if (overlap(other)) {
                    return true;
                }
            }
        }
    }
    return false;
}


void
MSDriveWay::writeBlocks(OutputDevice& od) const {
    od.openTag(myIsSubDriveway ? "subDriveWay" : "driveWay");
    od.writeAttr(SUMO_ATTR_ID, myID);
    od.writeAttr(SUMO_ATTR_EDGES, toString(myRoute));
    if (myCoreSize != (int)myRoute.size()) {
        od.writeAttr("core", myCoreSize);
    }
    od.openTag("forward");
    od.writeAttr(SUMO_ATTR_LANES, toString(myForward));
    od.closeTag();
    if (!myIsSubDriveway) {
        od.openTag("bidi");
        od.writeAttr(SUMO_ATTR_LANES, toString(myBidi));
        if (myBidiExtended.size() > 0) {
            od.lf();
            od << "                   ";
            od.writeAttr("deadlockCheck", toString(myBidiExtended));
        }
        od.closeTag();
        od.openTag("flank");
        od.writeAttr(SUMO_ATTR_LANES, toString(myFlank));
        od.closeTag();

        od.openTag("protectingSwitches");
        std::vector<std::string> links;
        for (MSLink* link : myProtectingSwitches) {
            links.push_back(getJunctionLinkID(link));
        }
        od.writeAttr("links", joinToString(links, " "));
        od.closeTag();

        od.openTag("conflictLinks");

        std::vector<std::string> signals;
        for (MSLink* link : myConflictLinks) {
            signals.push_back(getTLLinkID(link));
        }
        od.writeAttr("signals", joinToString(signals, " "));
        od.closeTag();

        std::vector<std::string> foes;
        for (MSDriveWay* dw : myFoes) {
            foes.push_back(dw->myID);
        }
        if (foes.size() > 0) {
            od.openTag("foes");
            od.writeAttr("driveWays", joinToString(foes, " "));
            od.closeTag();
        }
    }
    od.closeTag(); // driveWay

    for (const MSDriveWay* sub : mySubDriveWays) {
        sub->writeBlocks(od);
    }
}


void
MSDriveWay::writeBlockVehicles(OutputDevice& od) const {
    od.openTag(myIsSubDriveway ? "subDriveWay" : "driveWay");
    od.writeAttr(SUMO_ATTR_ID, myID);
    for (const VehicleEvent& ve : myVehicleEvents) {
        od.openTag(ve.isEntry ? "entry" : "exit");
        od.writeAttr(SUMO_ATTR_ID, ve.id);
        od.writeAttr(SUMO_ATTR_TIME, time2string(ve.time));
        od.writeAttr("reason", ve.reason);
        od.closeTag(); // event
    }
    od.closeTag(); // driveWay

    for (const MSDriveWay* sub : mySubDriveWays) {
        sub->writeBlockVehicles(od);
    }
}


void
MSDriveWay::buildRoute(const MSLink* origin, double length,
                                   MSRouteIterator next, MSRouteIterator end,
                                   LaneVisitedMap& visited,
                                   std::set<MSLink*>& flankSwitches) {
    bool seekForwardSignal = true;
    bool seekBidiSwitch = true;
    bool foundUnsafeSwitch = false;
    MSLane* toLane = origin ? origin->getViaLaneOrLane() : (*next)->getLanes()[0];
    const std::string warnID = origin ? getClickableTLLinkID(origin) : "lane '" + toLane->getID() + "'";
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
    gDebugFlag4 = DEBUG_HELPER(orignRS);
    if (gDebugFlag4) std::cout << "buildRoute origin=" << warnID << " vehRoute=" << toString(ConstMSEdgeVector(next, end))
                                   << " visited=" << formatVisitedMap(visited) << "\n";
#endif
    while ((seekForwardSignal || seekBidiSwitch)) {
        if (length > MAX_BLOCK_LENGTH) {
            if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                WRITE_WARNING("Block after rail signal " + warnID +
                              " exceeds maximum length (stopped searching after edge '" + toLane->getEdge().getID() + "' (length=" + toString(length) + "m).");
            }
            myNumWarnings++;
            // length exceeded
            return;
        }
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
        if (gDebugFlag4) {
            std::cout << "   toLane=" << toLane->getID() << " visited=" << formatVisitedMap(visited) << "\n";
        }
#endif
        if (toLane->getEdge().isNormal()) {
            myRoute.push_back(&toLane->getEdge());
            if (next != end) {
                next++;
            }
        }
        appendMapIndex(visited, toLane);
        length += toLane->getLength();
        MSLane* bidi = toLane->getBidiLane();
        if (seekForwardSignal) {
            if (!foundUnsafeSwitch) {
                myForward.push_back(toLane);
                if (myForward.size() == 1) {
                    myLane = toLane;
                    toLane->addMoveReminder(this);
                }
            }
        } else if (bidi == nullptr) {
            seekBidiSwitch = false;
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
            if (gDebugFlag4) {
                std::cout << "      noBidi, abort search for bidiSwitch\n";
            }
#endif
        }
        if (bidi != nullptr) {
            if (foundUnsafeSwitch) {
                myBidiExtended.push_back(bidi);
            } else {
                myBidi.push_back(bidi);
            }
            if (!seekForwardSignal) {
                // look for switch that could protect from oncoming vehicles
                for (const auto& ili : bidi->getIncomingLanes()) {
                    if (ili.viaLink->getDirection() == LinkDirection::TURN) {
                        continue;
                    }
                    for (const MSLink* const link : ili.lane->getLinkCont()) {
                        if (link->getDirection() == LinkDirection::TURN) {
                            continue;
                        }
                        if (link->getViaLaneOrLane() != bidi) {
                            // this switch is special beause it still lies on the current route
                            //myProtectingSwitches.push_back(ili.viaLink);
                            const MSEdge* const bidiNext = bidi->getNextNormal();
                            myCoreSize = (int)myRoute.size();
                            if (MSRailSignalControl::getInstance().getUsedEdges().count(bidiNext) == 0) {
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                                if (gDebugFlag4) {
                                    std::cout << "      abort: found protecting switch " << ili.viaLink->getDescription() << "\n";
                                }
#endif
                                // if bidi is actually used by a train (rather than
                                // the other route) we must later adapt this driveway for additional checks (myBidiExtended)
                                myProtectedBidi = bidiNext;
                                std::set<const MSEdge*> visitedEdges;
                                for (auto item : visited) {
                                    visitedEdges.insert(&item.first->getEdge());
                                }
                                while (next != end && visitedEdges.count(*next) == 0) {
                                    // the driveway is route specific but only but stop recording if it loops back on itself
                                    visitedEdges.insert(*next);
                                    const MSEdge* nextBidi = (*next)->getBidiEdge();
                                    if (nextBidi != nullptr) {
                                        visitedEdges.insert(nextBidi);
                                    }
                                    myRoute.push_back(*next);
                                    next++;
                                }
                                return;
                            } else {
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                                if (gDebugFlag4) {
                                    std::cout << "      found unsafe switch " << ili.viaLink->getDescription() << " (used=" << bidiNext->getID() << ")\n";
                                }
#endif
                                // trains along our route beyond this switch
                                // might create deadlock
                                foundUnsafeSwitch = true;
                                // the switch itself must still be guarded to ensure safety
                                for (const auto& ili2 : bidi->getIncomingLanes()) {
                                    if (ili2.viaLink->getDirection() != LinkDirection::TURN) {
                                        flankSwitches.insert(ili.viaLink);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        const std::vector<MSLink*>& links = toLane->getLinkCont();
        toLane = nullptr;
        for (const MSLink* const link : links) {
            if ((next != end && &link->getLane()->getEdge() == *next)
                    && isRailway(link->getViaLaneOrLane()->getPermissions())) {
                toLane = link->getViaLaneOrLane();
                if (link->getTLLogic() != nullptr) {
                    if (link == origin) {
                        WRITE_WARNINGF(TL("Found circular block at railSignal % (% edges, length %)"), warnID, toString(myRoute.size()), toString(length));
                        //std::cout << getClickableTLLinkID(origin) << " circularBlock2=" << toString(myRoute) << "\n";
                        return;
                    }
                    seekForwardSignal = false;
                    myFoundSignal = true;
                    seekBidiSwitch = bidi != nullptr;
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                    if (gDebugFlag4) {
                        std::cout << "      found forwardSignal " << link->getTLLogic()->getID() << " seekBidiSwitch=" << seekBidiSwitch << "\n";
                    }
#endif
                }
                if (links.size() > 1) {
                    // switch on driveway
                    mySwitchDriveWays[link].push_back(this);
                    myForwardSwitches.push_back(link);
                }
                break;
            }
        }
        if (toLane == nullptr) {
            if (next != end) {
                // no connection found, jump to next route edge
                toLane = (*next)->getLanes()[0];
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                if (gDebugFlag4) {
                    std::cout << "      abort: turn-around or jump\n";
                }
#endif
                myFoundReversal = true;
                return;
            } else {
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                if (gDebugFlag4) {
                    std::cout << "      abort: no next lane available\n";
                }
#endif
                return;
            }
        }
    }
}


void
MSDriveWay::checkFlanks(const MSLink* originLink, const std::vector<const MSLane*>& lanes, const LaneVisitedMap& visited, bool allFoes, std::set<MSLink*>& flankSwitches) const {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << " checkFlanks lanes=" << toString(lanes) << "\n  visited=" << formatVisitedMap(visited) << " allFoes=" << allFoes << "\n";
#endif
    const MSLink* reverseOriginLink = originLink != nullptr && originLink->getLane()->getBidiLane() != nullptr && originLink->getLaneBefore()->getBidiLane() != nullptr
                                      ? originLink->getLane()->getBidiLane()->getLinkTo(originLink->getLaneBefore()->getBidiLane())
                                      : nullptr;
    //std::cout << "   originLink=" << originLink->getDescription() << "\n";
    if (reverseOriginLink != nullptr) {
        reverseOriginLink = reverseOriginLink->getCorrespondingExitLink();
        //std::cout << "   reverseOriginLink=" << reverseOriginLink->getDescription() << "\n";
    }
    for (int i = 0; i < (int)lanes.size(); i++) {
        const MSLane* lane = lanes[i];
        const MSLane* prev = i > 0 ? lanes[i - 1] : nullptr;
        const MSLane* next = i + 1 < (int)lanes.size() ? lanes[i + 1] : nullptr;
        if (lane->isInternal()) {
            continue;
        }
        for (auto ili : lane->getIncomingLanes()) {
            if (ili.viaLink == originLink
                    || ili.viaLink == reverseOriginLink
                    || ili.viaLink->getDirection() == LinkDirection::TURN
                    || ili.viaLink->getDirection() == LinkDirection::TURN_LEFTHAND) {
                continue;
            }
            if (ili.lane != prev && ili.lane != next) {
#ifdef DEBUG_CHECK_FLANKS
                std::cout << " add flankSwitch junction=" << ili.viaLink->getJunction()->getID() << " index=" << ili.viaLink->getIndex() << " iLane=" << ili.lane->getID() << " prev=" << Named::getIDSecure(prev) <<  " targetLane=" << lane->getID() << " next=" << Named::getIDSecure(next) << "\n";
#endif
                flankSwitches.insert(ili.viaLink);
            } else if (allFoes) {
                // link is part of the driveway, find foes that cross the driveway without entering
                checkCrossingFlanks(ili.viaLink, visited, flankSwitches);
            }
        }
    }
}


void
MSDriveWay::checkCrossingFlanks(MSLink* dwLink, const LaneVisitedMap& visited, std::set<MSLink*>& flankSwitches) const {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << "  checkCrossingFlanks  dwLink=" << dwLink->getDescription() << " visited=" << formatVisitedMap(visited) << "\n";
#endif
    const MSJunction* junction = dwLink->getJunction();
    if (junction == nullptr) {
        return; // unregulated junction;
    }
    const MSJunctionLogic* logic = junction->getLogic();
    if (logic == nullptr) {
        return; // unregulated junction;
    }
    for (const MSEdge* in : junction->getIncoming()) {
        if (in->isInternal()) {
            continue;
        }
        for (MSLane* inLane : in->getLanes()) {
            const MSLane* inBidi = inLane->getBidiLane();
            if (isRailway(inLane->getPermissions()) && visited.count(inLane) == 0 && (inBidi == nullptr || visited.count(inBidi) == 0)) {
                for (MSLink* link : inLane->getLinkCont()) {
                    if (link->getIndex() >= 0 && logic->getFoesFor(dwLink->getIndex()).test(link->getIndex())
                            && visited.count(link->getLane()) == 0) {
#ifdef DEBUG_CHECK_FLANKS
                        std::cout << " add crossing flankSwitch junction=" << junction->getID() << " index=" << link->getIndex() << "\n";
#endif
                        if (link->getViaLane() == nullptr) {
                            flankSwitches.insert(link);
                        } else {
                            flankSwitches.insert(link->getViaLane()->getLinkCont().front());
                        }
                    }
                }
            }
        }
    }
}

void
MSDriveWay::findFlankProtection(MSLink* link, double length, LaneVisitedMap& visited, MSLink* origLink, std::vector<const MSLane*>& flank) {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << "  findFlankProtection link=" << link->getDescription() << " length=" << length << " origLink=" << origLink->getDescription() << "\n";
#endif
    if (link->getTLLogic() != nullptr) {
        // guarded by signal
#ifdef DEBUG_CHECK_FLANKS
        std::cout << "   flank guarded by " << link->getTLLogic()->getID() << "\n";
#endif
        // @note, technically it's enough to collect links from foe driveways
        // but this also adds "unused" conflict links which may aid comprehension
        myConflictLinks.push_back(link);
        addFoes(link);
    } else if (length > MAX_BLOCK_LENGTH) {
        // length exceeded
        if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
            WRITE_WARNING("Incoming block at junction '" + origLink->getJunction()->getID() + "', link " + toString(origLink->getIndex()) + " exceeds maximum length (stopped searching after lane '" + link->getLane()->getID() + "' (length=" + toString(length) + "m).");
        }
        myNumWarnings++;
    } else {
        // find normal lane before this link
        const MSLane* lane = link->getLaneBefore();
        const bool isNew = visited.count(lane) == 0;
        if (isNew || (visited[lane] > visited[origLink->getLane()] && std::find(myForward.begin(), myForward.end(), lane) == myForward.end())) {
            if (isNew) {
                appendMapIndex(visited, lane);
            }
            length += lane->getLength();
            if (lane->isInternal()) {
                flank.push_back(lane);
                findFlankProtection(lane->getIncomingLanes().front().viaLink, length, visited, origLink, flank);
            } else {
                bool foundPSwitch = false;
                for (MSLink* l2 : lane->getLinkCont()) {
#ifdef DEBUG_CHECK_FLANKS
                    std::cout << "   lane=" << lane->getID() << " visitedIndex=" << visited[lane] << " origIndex=" << visited[origLink->getLane()] << " cand=" << l2->getDescription() << "\n";
#endif
                    if (l2->getDirection() != LinkDirection::TURN && l2->getLane() != link->getLane()) {
                        foundPSwitch = true;
                        // found potential protection
#ifdef DEBUG_CHECK_FLANKS
                        std::cout << "   protectingSwitch=" << l2->getDescription() << " for flank=" << link->getDescription() << "\n";
#endif
                        myProtectingSwitches.push_back(link);
                        addSwitchFoes(link);
                        if (std::find(myBidi.begin(), myBidi.end(), origLink->getLane()) != myBidi.end()) {
#ifdef DEBUG_CHECK_FLANKS
                            std::cout << "     (is bidi-switch)\n";
#endif
                            myProtectingSwitchesBidi.push_back(link);
                        }
                    }
                }
                if (!foundPSwitch) {
                    flank.push_back(lane);
                    // continue search for protection upstream recursively
                    for (auto ili : lane->getIncomingLanes()) {
                        if (ili.viaLink->getDirection() != LinkDirection::TURN) {
                            findFlankProtection(ili.viaLink, length, visited, origLink, flank);
                        }
                    }
                }
            }
        } else {
#ifdef DEBUG_CHECK_FLANKS
            std::cout << "    laneBefore=" << lane->getID() << " already visited. index=" << visited[lane] << " origAfter=" << origLink->getLane()->getID() << " origIndex=" << visited[origLink->getLane()] << "\n";
#endif
        }
    }
    myMaxFlankLength = MAX2(myMaxFlankLength, length);
}


MSDriveWay*
MSDriveWay::buildDriveWay(const std::string& id, const MSLink* link, MSRouteIterator first, MSRouteIterator end) {
    // collect lanes and links that are relevant for setting this signal for the current driveWay
    // For each driveway we collect
    //   - conflictLanes (signal must be red if any conflict lane is occupied)
    //   - conflictLinks (signal must be red if any conflict link is approached by a vehicle
    //      - that cannot break in time (arrivalSpeedBraking > 0)
    //      - approached by a vehicle with higher switching priority (see #3941)
    // These objects are construct in steps:
    //
    // forwardBlock
    // - search forward recursive from outgoing lane until controlled railSignal link found
    //   -> add all found lanes to conflictLanes
    //
    // bidiBlock (if any forwardBlock edge has bidi edge)
    // - search bidi backward recursive until first switch
    //   - from switch search backward recursive all other incoming until controlled rail signal link
    //     -> add final links to conflictLinks
    //
    // flanks
    // - search backward recursive from flanking switches
    //   until controlled railSignal link or protecting switch is found
    //   -> add all found lanes to conflictLanes
    //   -> add final links to conflictLinks

    MSDriveWay* dw = new MSDriveWay(id);
    dw->myIsDepartDriveway = link == nullptr;
    LaneVisitedMap visited;
    std::vector<const MSLane*> before;
    MSLane* fromBidi = nullptr;
    if (link != nullptr) {
        appendMapIndex(visited, link->getLaneBefore());
        fromBidi = link->getLaneBefore()->getBidiLane();
    }
    std::set<MSLink*> flankSwitches; // list of switches that threaten the driveway and for which protection must be found

    if (fromBidi != nullptr) {
        before.push_back(fromBidi);
    }
    dw->buildRoute(link, 0., first, end, visited, flankSwitches);
    if (dw->myProtectedBidi == nullptr) {
        dw->myCoreSize = (int)dw->myRoute.size();
    }
    dw->checkFlanks(link, dw->myForward, visited, true, flankSwitches);
    dw->checkFlanks(link, dw->myBidi, visited, false, flankSwitches);
    dw->checkFlanks(link, before, visited, true, flankSwitches);
    for (MSLink* fsLink : flankSwitches) {
        //std::cout << getID() << " flankSwitch=" << link->getDescription() << "\n";
        dw->findFlankProtection(fsLink, 0, visited, fsLink, dw->myFlank);
    }
    std::set<MSLink*> flankSwitchesBidiExtended;
    dw->checkFlanks(link, dw->myBidiExtended, visited, false, flankSwitchesBidiExtended);
    for (MSLink* link : flankSwitchesBidiExtended) {
        //std::cout << getID() << " flankSwitchBEx=" << link->getDescription() << "\n";
        dw->findFlankProtection(link, 0, visited, link, dw->myBidiExtended);
    }

#ifdef DEBUG_BUILD_DRIVEWAY
    if (DEBUG_COND_DW) {
        std::cout << SIMTIME << " buildDriveWay " << dw->myID << " link=" << (link == nullptr ? "NULL" : link->getDescription())
                  << "\n    route=" << toString(dw->myRoute)
                  << "\n    forward=" << toString(dw->myForward)
                  << "\n    bidi=" << toString(dw->myBidi)
                  << "\n    flank=" << toString(dw->myFlank)
                  << "\n    flankSwitch=" << MSRailSignal::describeLinks(std::vector<MSLink*>(flankSwitches.begin(), flankSwitches.end()))
                  << "\n    protSwitch=" << MSRailSignal::describeLinks(dw->myProtectingSwitches)
                  << "\n    coreSize=" << dw->myCoreSize
                  << "\n";
    }
#endif
    MSRailSignal* rs = link ? const_cast<MSRailSignal*>(static_cast<const MSRailSignal*>(link->getTLLogic())) : nullptr;
    if (!rs || !rs->isMovingBlock()) {
        dw->myConflictLanes.insert(dw->myConflictLanes.end(), dw->myForward.begin(), dw->myForward.end());
    }
    dw->myConflictLanes.insert(dw->myConflictLanes.end(), dw->myBidi.begin(), dw->myBidi.end());
    dw->myConflictLanes.insert(dw->myConflictLanes.end(), dw->myFlank.begin(), dw->myFlank.end());
    if (dw->myProtectedBidi != nullptr) {
        MSRailSignalControl::getInstance().registerProtectedDriveway(rs, dw->myRoute.front(), dw->myNumericalID, dw->myProtectedBidi);
    }

    dw->addBidiFoes(rs);
    // make foes unique and symmetrical
    std::set<MSDriveWay*, ComparatorNumericalIdLess> uniqueFoes(dw->myFoes.begin(), dw->myFoes.end());
    std::set<MSLink*> uniqueCLink(dw->myConflictLinks.begin(), dw->myConflictLinks.end());
    dw->myFoes.clear();
    const MSEdge* lastEdge = &dw->myForward.back()->getEdge();
    for (MSDriveWay* foe : uniqueFoes) {
        const MSEdge* foeLastEdge = &foe->myForward.back()->getEdge();
        const bool sameLast = foeLastEdge == lastEdge;
        const bool dwOnFoeBidi = std::find(foe->myBidi.begin(), foe->myBidi.end(), dw->myForward.back()) != foe->myBidi.end();
        const bool foeOnDwBidi = std::find(dw->myBidi.begin(), dw->myBidi.end(), foe->myForward.back()) != dw->myBidi.end();
        if (sameLast || dwOnFoeBidi || foeOnDwBidi) {
#ifdef DEBUG_BUILD_DRIVEWAY
            if (DEBUG_COND_DW) {
                std::cout << " symmetrical full-length foe " << foe->getID() << " dwOnFoeBidi=" << dwOnFoeBidi << " foeOnDwBidi=" << foeOnDwBidi << "\n";
            }
#endif
            if (sameLast || foe->bidiBlockedBy(*dw)) {
                foe->myFoes.push_back(dw);
            }
            if (sameLast || dw->bidiBlockedBy(*foe)) {
                dw->myFoes.push_back(foe);
            }
        } else {
#ifdef DEBUG_BUILD_DRIVEWAY
            if (DEBUG_COND_DW) {
                std::cout << " symmetrical buildSubFoe for foe " << foe->getID() << "\n";
            }
#endif
            foe->buildSubFoe(dw);
            dw->buildSubFoe(foe);
        }
        if (link) {
            foe->addConflictLink(link);
        }
        for (auto ili : foe->myForward.front()->getIncomingLanes()) {
            if (ili.viaLink->getTLLogic() != nullptr) {
                // ignore links that originate on myBidi
                const MSLane* origin = ili.viaLink->getLaneBefore();
                if (std::find(dw->myBidi.begin(), dw->myBidi.end(), origin) == dw->myBidi.end()) {
                    uniqueCLink.insert(ili.viaLink);
                }
            }
        }
    }
    dw->myConflictLinks.clear();
    dw->myConflictLinks.insert(dw->myConflictLinks.begin(), uniqueCLink.begin(), uniqueCLink.end());
    myEndingDriveways[lastEdge].push_back(dw);
    if ((!rs || !rs->isMovingBlock()) && (rs || !OptionsCont::getOptions().getBool("railsignal-moving-block"))) {
        // every driveway is it's own foe (also all driveways that depart in the same block)
        for (MSDriveWay* sameEnd : myEndingDriveways[lastEdge]) {
            if (uniqueFoes.count(sameEnd) == 0) {
                dw->myFoes.push_back(sameEnd);
                if (sameEnd != dw) {
                    sameEnd->myFoes.push_back(dw);
                }
            }
        }
    }
    return dw;
}

std::string
MSDriveWay::getTLLinkID(const MSLink* link) {
    return link->getTLLogic()->getID() + "_" + toString(link->getTLIndex());
}

std::string
MSDriveWay::getJunctionLinkID(const MSLink* link) {
    return link->getJunction()->getID() + "_" + toString(link->getIndex());
}

std::string
MSDriveWay::getClickableTLLinkID(const MSLink* link) {
    return "junction '" +  link->getTLLogic()->getID() + "', link " + toString(link->getTLIndex());
}

std::string
MSDriveWay::formatVisitedMap(const LaneVisitedMap& visited) {
    std::vector<const MSLane*> lanes(visited.size(), nullptr);
    for (auto item : visited) {
        lanes[item.second] = item.first;
    }
    return toString(lanes);
}


void
MSDriveWay::appendMapIndex(LaneVisitedMap& map, const MSLane* lane) {
    // avoid undefined behavior from evaluation order
    const int tmp = (int)map.size();
    map[lane] = tmp;
}

bool
MSDriveWay::match(const MSRoute& route, MSRouteIterator firstIt) const {
    // @todo optimize: it is sufficient to check for specific edges (after each switch)
    auto itRoute = firstIt;
    auto itDwRoute = myRoute.begin();
    bool match = true;
    while (itRoute != route.end() && itDwRoute != myRoute.end()) {
        if (*itRoute != *itDwRoute) {
            match = false;
            //std::cout << "  check dw=" << " match failed at vehEdge=" << (*itRoute)->getID() << " dwEdge=" << (*itDwRoute)->getID() << "\n";
            break;
        }
        itRoute++;
        itDwRoute++;
    }
    // if the vehicle arrives before the end of this driveway,
    // we'd rather build a new driveway to avoid superfluous restrictions
    if (match && itDwRoute == myRoute.end()
            && (itRoute == route.end() || myFoundSignal || myFoundReversal || myIsSubDriveway)) {
        //std::cout << "  using dw=" << "\n";
        if (myFoundReversal && itRoute != route.end()) {
            // check whether the current route requires an extended driveway
            const MSEdge* next = *itRoute;
            const MSEdge* prev = myRoute.back();
            if (prev->getBidiEdge() != next && prev->getBidiEdge() != nullptr
                    && prev->isConnectedTo(*next, (SUMOVehicleClass)(SVC_RAIL_CLASSES & prev->getPermissions()))) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void
MSDriveWay::addFoes(const MSLink* link) {
#ifdef DEBUG_ADD_FOES
    std::cout << "driveway " << myID << " (" << myNumericalID << ") addFoes for link " << link->getDescription() << "\n";
#endif
    const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(link->getTLLogic());
    if (rs != nullptr) {
        for (MSDriveWay* foe : rs->retrieveDriveWays(link->getTLIndex())) {
            if (foe != this && (flankConflict(*foe) || foe->flankConflict(*this) || crossingConflict(*foe) || foe->crossingConflict(*this))) {
#ifdef DEBUG_ADD_FOES
                std::cout << "   foe=" << foe->myID << " (" << foe->getNumericalID() << ")\n";
#endif
                myFoes.push_back(foe);
            }
        }
    }
}


void
MSDriveWay::addSwitchFoes(const MSLink* link) {
#ifdef DEBUG_ADD_FOES
    std::cout << "driveway " << myID << " (" << myNumericalID << ") addSwitchFoes for link " << link->getDescription() << "\n";
#endif
    auto it = mySwitchDriveWays.find(link);
    if (it != mySwitchDriveWays.end()) {
        for (MSDriveWay* foe : it->second) {
            if (foe != this && (flankConflict(*foe) || foe->flankConflict(*this) || crossingConflict(*foe) || foe->crossingConflict(*this))) {
#ifdef DEBUG_ADD_FOES
                std::cout << "   foe=" << foe->myID
                    << " fc1=" << flankConflict(*foe) << " fc2=" << foe->flankConflict(*this)
                    << " cc1=" << crossingConflict(*foe) << " cc2=" << foe->crossingConflict(*this) << "\n";
#endif
                myFoes.push_back(foe);
            }
        }
    }
}


void
MSDriveWay::addBidiFoes(const MSRailSignal* ownSignal) {
#ifdef DEBUG_ADD_FOES
    std::cout << "driveway " << myID << " (" << myNumericalID << ") addBidiFoes\n";
#endif
    for (const MSLane* bidi : myBidi) {
        for (auto ili : bidi->getIncomingLanes()) {
            const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(ili.viaLink->getTLLogic());
            if (rs != nullptr && rs != ownSignal &&
                    std::find(myBidi.begin(), myBidi.end(), ili.lane) != myBidi.end()) {
                addFoes(ili.viaLink);
            }
        }
        const MSEdge* bidiEdge = &bidi->getEdge();
        if (myDepartureDriveways.count(bidiEdge) != 0) {
            for (MSDriveWay* foe : myDepartureDriveways[bidiEdge]) {
#ifdef DEBUG_ADD_FOES
                std::cout << "  foe " << foe->getID() << " departs on bidi=" << bidiEdge->getID() << "\n";
#endif
                myFoes.push_back(foe);
            }
        }
        if (myDepartureDrivewaysEnds.count(bidiEdge) != 0) {
            for (MSDriveWay* foe : myDepartureDrivewaysEnds[bidiEdge]) {
#ifdef DEBUG_ADD_FOES
                std::cout << "  foe " << foe->getID() << " ends on bidi=" << bidiEdge->getID() << "\n";
#endif
                myFoes.push_back(foe);
            }
        }
    }
}


void
MSDriveWay::buildSubFoe(MSDriveWay* foe) {
    // we already know that the last edge of this driveway doesn't impact the foe
    int subLast = myForward.size() - 2;
#ifdef DEBUG_BUILD_SUBDRIVEWAY
    if (subLast < 0) {
        std::cout << "  " << getID() << " cannot build subDriveWay for foe " << foe->getID() << " because myForward has only a single lane\n";
    }
#endif
    while (subLast >= 0) {
        const MSLane* lane = myForward[subLast];
        MSDriveWay tmp("tmp", true);
        tmp.myForward.push_back(lane);
#ifdef DEBUG_BUILD_SUBDRIVEWAY
        std::cout << "  subLast=" << subLast << " lane=" << lane->getID() << " fc=" << tmp.flankConflict(*foe) << " cc=" << tmp.crossingConflict(*foe)
            << " bc=" << (std::find(foe->myBidi.begin(), foe->myBidi.end(), lane) != foe->myBidi.end()) << "\n";
#endif
        if (tmp.flankConflict(*foe) || tmp.crossingConflict(*foe) ||
                std::find(foe->myBidi.begin(), foe->myBidi.end(), lane) != foe->myBidi.end()) {
            break;
        }
        subLast--;
    }
    if (subLast < 0) {
        WRITE_WARNINGF("No point of conflict found between driveway '%' and driveway '%' when creating sub-driveway", getID(), foe->getID());
        return;
    }
    int subSize = subLast + 1;
    for (MSDriveWay* cand : mySubDriveWays) {
        if ((int)cand->myForward.size() == subSize) {
            // can re-use existing sub-driveway
            foe->myFoes.push_back(cand);
            cand->myFoes.push_back(foe);
            return;
        }
    }
    MSDriveWay* sub = new MSDriveWay(getID() + "." + toString(mySubDriveWays.size()));
    sub->myLane = myLane;
    sub->myIsSubDriveway = true;
    myLane->addMoveReminder(sub);
    sub->myForward.insert(sub->myForward.begin(), myForward.begin(), myForward.begin() + subSize);
    for (const MSLane* lane : sub->myForward) {
        if (lane->isNormal()) {
            sub->myRoute.push_back(&lane->getEdge());
        }
    }
    sub->myCoreSize = sub->myRoute.size();

    // copy trains that are currently on this driveway (and associated entry events)
    for (SUMOVehicle* veh : myTrains) {
        if (std::find(sub->myRoute.begin(), sub->myRoute.end(), veh->getEdge()) != sub->myRoute.end()) {
            sub->myTrains.insert(veh);
            dynamic_cast<MSBaseVehicle*>(veh)->addReminder(sub);
            for (const VehicleEvent& ve : myVehicleEvents) {
                if (ve.id == veh->getID()) {
                    sub->myVehicleEvents.push_back(ve);
                }
            }
        }
    }

    foe->myFoes.push_back(sub);
    sub->myFoes.push_back(foe);
    mySubDriveWays.push_back(sub);
#ifdef DEBUG_BUILD_SUBDRIVEWAY
    std::cout << SIMTIME << " buildSubFoe dw=" << getID() << " foe=" << foe->getID() << " sub=" << sub->getID() << " route=" << toString(sub->myRoute) << "\n";
#endif
}


void
MSDriveWay::addConflictLink(const MSLink* link) {
    if (link->getTLLogic() != nullptr) {
        // ignore links that originate on myBidi
        const MSLane* origin = link->getLaneBefore();
        if (std::find(myBidi.begin(), myBidi.end(), origin) == myBidi.end()) {
            if (std::find(myConflictLinks.begin(), myConflictLinks.end(), link) == myConflictLinks.end()) {
                myConflictLinks.push_back(const_cast<MSLink*>(link));
            }
        }
    }
}


const MSDriveWay*
MSDriveWay::getDepartureDriveway(const SUMOVehicle* veh) {
    const MSEdge* edge = veh->getEdge();
    if (edge->getFromJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL) {
        for (const MSLane* lane : edge->getLanes()) {
            for (auto ili : lane->getIncomingLanes()) {
                const MSLink* entry = ili.viaLink->getCorrespondingEntryLink();
                const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(entry->getTLLogic());
                if (rs != nullptr) {
                    const MSDriveWay* dw = &const_cast<MSRailSignal*>(rs)->retrieveDriveWayForVeh(entry->getTLIndex(), veh);
                    if (&dw->myForward.front()->getEdge() == edge) {
                        return dw;
                    }
                }
            }
        }
    }
    for (MSDriveWay* dw : myDepartureDriveways[edge]) {
        if (dw->match(veh->getRoute(), veh->getCurrentRouteEdge())) {
            return dw;
        }
    }
    const std::string id = edge->getFromJunction()->getID() + ".d" + toString(myDepartDriveWayIndex++);
    MSDriveWay* dw = buildDriveWay(id, nullptr, veh->getCurrentRouteEdge(), veh->getRoute().end());
    myDepartureDriveways[edge].push_back(dw);
    myDepartureDrivewaysEnds[&dw->myForward.back()->getEdge()].push_back(dw);
    return dw;
}


void
MSDriveWay::updateDepartDriveway(const MSEdge* first, int dwID) {
    for (MSDriveWay* oldDW : myDepartureDriveways[first]) {
        if (oldDW->getNumericalID() == dwID) {
#ifdef DEBUG_BUILD_DRIVEWAY
            if (DEBUG_COND_DW) {
                std::cout << "updateDepartDriveway " << oldDW->getID() << "\n";
            }
#endif
            MSDriveWay* dw = buildDriveWay(oldDW->getID(), nullptr, oldDW->myRoute.begin(), oldDW->myRoute.end());
            myDepartureDriveways[first].push_back(dw);
            myDepartureDrivewaysEnds[&dw->myForward.back()->getEdge()].push_back(dw);
            delete oldDW;
            return;
        }
    }
    WRITE_WARNINGF("Could not update depart-driveway % starting on edge %", dwID, first->getID());
}


void
MSDriveWay::writeDepatureBlocks(OutputDevice& od, bool writeVehicles) {
    for (auto item  : myDepartureDriveways) {
        const MSEdge* edge = item.first;
        if (item.second.size() > 0) {
            od.openTag("departJunction");
            od.writeAttr(SUMO_ATTR_ID, edge->getFromJunction()->getID());
            for (const MSDriveWay* dw : item.second) {
                if (writeVehicles) {
                    dw->writeBlockVehicles(od);
                } else {
                    dw->writeBlocks(od);
                }
            }
            od.closeTag(); // departJunction
        }
    }
}

/****************************************************************************/
