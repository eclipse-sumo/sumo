/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    MSRailSignal.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Jan 2015
///
// A rail signal logic
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif
#include <utils/iodevices/OutputDevice_COUT.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include <microsim/MSLane.h>

#include "MSTLLogicControl.h"
#include "MSTrafficLightLogic.h"
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"
#include "MSRailSignal.h"

// typical block length in germany on main lines is 3-5km on branch lines up to 7km
// special branches that are used by one train exclusively could also be up to 20km in length
// minimum block size in germany is 37.5m (LZB)
// larger countries (USA, Russia) might see blocks beyond 20km)
#define MAX_BLOCK_LENGTH 20000
#define MAX_SIGNAL_WARNINGS 10

//#define DEBUG_BUILD_DRIVEWAY
//#define DEBUG_CHECK_FLANKS

#define DEBUG_SIGNALSTATE
#define DEBUG_SIGNALSTATE_PRIORITY
#define DEBUG_FIND_PROTECTION
//#define DEBUG_REROUTE

#define DEBUG_COND DEBUG_HELPER(this)
#define DEBUG_COND_LINKINFO DEBUG_HELPER(myLink->getTLLogic())
#define DEBUG_HELPER(obj) ((obj)->isSelected())
//#define DEBUG_HELPER(obj) ((obj)->getID() == "w2")
//#define DEBUG_HELPER(obj) (true)

// ===========================================================================
// static value definitions
// ===========================================================================
int MSRailSignal::myNumWarnings(0);

// ===========================================================================
// method definitions
// ===========================================================================
MSRailSignal::MSRailSignal(MSTLLogicControl& tlcontrol,
                           const std::string& id, const std::string& programID, SUMOTime delay,
                           const std::map<std::string, std::string>& parameters) :
    MSTrafficLightLogic(tlcontrol, id, programID, TLTYPE_RAIL_SIGNAL, delay, parameters),
    myCurrentPhase(DELTA_T, std::string(SUMO_MAX_CONNECTIONS, 'X'), -1), // dummy phase
    myPhaseIndex(0) {
    myDefaultCycleTime = DELTA_T;
}

void
MSRailSignal::init(NLDetectorBuilder&) {
    assert(myLanes.size() > 0);
    for (LinkVector& links : myLinks) { //for every link index
        if (links.size() != 1) {
            throw ProcessError("At railSignal '" + getID() + "' found " + toString(links.size())
                               + " links controlled by index " + toString(links[0]->getTLIndex()));
        }
        myLinkInfos.push_back(LinkInfo(links[0]));
    }
    updateCurrentPhase();
    setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
}


MSRailSignal::~MSRailSignal() {
}


// ----------- Handling of controlled links
void
MSRailSignal::adaptLinkInformationFrom(const MSTrafficLightLogic& logic) {
    MSTrafficLightLogic::adaptLinkInformationFrom(logic);
    updateCurrentPhase();
}


// ------------ Switching and setting current rows
SUMOTime
MSRailSignal::trySwitch() {
    updateCurrentPhase();
    return DELTA_T;
}



void
MSRailSignal::updateCurrentPhase() {
#ifdef DEBUG_SIGNALSTATE
    gDebugFlag4 = DEBUG_COND;
#endif
    // green by default so vehicles can be inserted at the borders of the network
    std::string state(myLinks.size(), 'G');
    for (LinkInfo& li : myLinkInfos) {
        if (li.myLink->getApproaching().size() > 0) {
            Approaching closest = getClosest(li.myLink);
            DriveWay& driveway = li.getDriveWay(closest.first);
            //std::cout << SIMTIME << " signal=" << getTLLinkID(li.myLink) << " veh=" << closest.first->getID() << " dw:\n";
            //driveway.writeBlocks(*OutputDevice_COUT::getDevice());
            MSEdgeVector occupied;
            if (!driveway.reserve(closest, occupied)) {
                state[li.myLink->getTLIndex()] = 'r';
                if (occupied.size() > 0) {
                    li.reroute(const_cast<SUMOVehicle*>(closest.first), occupied);
                }
#ifdef DEBUG_SIGNALSTATE
                if (gDebugFlag4) {
                    std::cout << SIMTIME << " rsl=" << li.getID() << " veh=" << closest.first->getID() << " notReserved\n";
                }
#endif
            } else {
                state[li.myLink->getTLIndex()] = 'G';
#ifdef DEBUG_SIGNALSTATE
                if (gDebugFlag4) {
                    std::cout << SIMTIME << " rsl=" << li.getID() << " veh=" << closest.first->getID() << " reserved\n";
                }
#endif
            }
        } else {
            DriveWay& driveway = li.myDriveways.front();
            if (driveway.conflictLaneOccupied() || driveway.conflictLinkApproached()) {
#ifdef DEBUG_SIGNALSTATE
                if (gDebugFlag4) {
                    std::cout << SIMTIME << " rsl=" << li.getID() << " red for default driveway\n";
                }
#endif
                state[li.myLink->getTLIndex()] = 'r';
            }
        }
    }
    if (myCurrentPhase.getState() != state) {
        myCurrentPhase.setState(state);
        myPhaseIndex = 1 - myPhaseIndex;
    }
#ifdef DEBUG_SIGNALSTATE
    gDebugFlag4 = false;
#endif
}


// ------------ Static Information Retrieval
int
MSRailSignal::getPhaseNumber() const {
    return 0;
}

const MSTrafficLightLogic::Phases&
MSRailSignal::getPhases() const {
    return myPhases;
}

const MSPhaseDefinition&
MSRailSignal::getPhase(int) const {
    return myCurrentPhase;
}

// ------------ Dynamic Information Retrieval
int
MSRailSignal::getCurrentPhaseIndex() const {
    return myPhaseIndex;
}

const MSPhaseDefinition&
MSRailSignal::getCurrentPhaseDef() const {
    return myCurrentPhase;
}

// ------------ Conversion between time and phase
SUMOTime
MSRailSignal::getPhaseIndexAtTime(SUMOTime) const {
    return 0;
}

SUMOTime
MSRailSignal::getOffsetFromIndex(int) const {
    return 0;
}

int
MSRailSignal::getIndexFromOffset(SUMOTime) const {
    return 0;
}


void
MSRailSignal::addLink(MSLink* link, MSLane* lane, int pos) {
    if (pos >= 0) {
        MSTrafficLightLogic::addLink(link, lane, pos);
    } // ignore uncontrolled link
}


std::string
MSRailSignal::getTLLinkID(MSLink* link) {
    return link->getTLLogic()->getID() + "_" + toString(link->getTLIndex());
}

std::string
MSRailSignal::getClickableTLLinkID(MSLink* link) {
    return "junction '" +  link->getTLLogic()->getID() + "', link " + toString(link->getTLIndex());
}

std::string
MSRailSignal::describeLinks(std::vector<MSLink*> links) {
    std::string result;
    for (MSLink* link : links) {
        result += link->getDescription() + " ";
    }
    return result;
}

MSRailSignal::Approaching
MSRailSignal::getClosest(MSLink* link) {
    assert(link->getApproaching().size() > 0);
    double minDist = std::numeric_limits<double>::max();
    auto closestIt = link->getApproaching().begin();
    for (auto apprIt = link->getApproaching().begin(); apprIt != link->getApproaching().end(); apprIt++) {
        if (apprIt->second.dist < minDist) {
            minDist = apprIt->second.dist;
            closestIt = apprIt;
        }
    }
    // maybe a parallel link has a closer vehicle
    /*
    for (MSLink* link2 : link->getLaneBefore()->getLinkCont()) {
        if (link2 != link) {
            for (auto apprIt2 = link2->getApproaching().begin(); apprIt2 != link2->getApproaching().end(); apprIt2++) {
                if (apprIt2->second.dist < minDist) {
                    minDist = apprIt2->second.dist;
                    closestIt = apprIt2;
                }
            }
        }
    }
    */
    return *closestIt;
}

void
MSRailSignal::writeBlocks(OutputDevice& od) const {
    od.openTag("railSignal");
    od.writeAttr(SUMO_ATTR_ID, getID());
    for (const LinkInfo& li : myLinkInfos) {
        MSLink* link = li.myLink;
        od.openTag("link");
        od.writeAttr(SUMO_ATTR_TLLINKINDEX, link->getTLIndex());
        od.writeAttr(SUMO_ATTR_FROM, link->getLaneBefore()->getID());
        od.writeAttr(SUMO_ATTR_TO, link->getViaLaneOrLane()->getID());
        for (const DriveWay& dw : li.myDriveways) {
            dw.writeBlocks(od);
        }
        od.closeTag(); // link
    }
    od.closeTag(); // railSignal
}


bool
MSRailSignal::hasOncomingRailTraffic(MSLink* link) {
    if (link->getJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL && link->getState() == LINKSTATE_TL_RED) {
        const MSEdge* bidi = link->getLaneBefore()->getEdge().getBidiEdge();
        if (bidi == nullptr) {
            return false;
        }
        const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(link->getTLLogic());
        if (rs != nullptr) {
            const LinkInfo& li = rs->myLinkInfos[link->getTLIndex()];
            for (const DriveWay& dw : li.myDriveways) {
                //std::cout << SIMTIME <<< " hasOncomingRailTraffic link=" << getTLLinkID(link) << " dwRoute=" << toString(dw.myRoute) << " bidi=" << toString(dw.myBidi) << "\n";
                for (MSLane* lane : dw.myBidi) {
                    if (!lane->isEmpty()) {
                        return true;
                    }
                }
                for (const MSLane* lane : dw.myFlank) {
                    if (!lane->isEmpty()) {
                        MSVehicle* veh = lane->getFirstAnyVehicle();
                        if (std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
                            return true;
                        }
                    }
                }
                for (MSLink* foeLink : dw.myConflictLinks) {
                    if (foeLink->getApproaching().size() != 0) {
                        Approaching closest = getClosest(foeLink);
                        const SUMOVehicle* veh = closest.first;
                        if (std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// ===========================================================================
// LinkInfo method definitions
// ===========================================================================

MSRailSignal::LinkInfo::LinkInfo(MSLink* link):
    myLink(link), myUniqueDriveWay(false),
    myLastRerouteTime(-1),
    myLastRerouteVehicle(nullptr) {
    ConstMSEdgeVector dummyRoute;
    dummyRoute.push_back(&link->getLane()->getEdge());
    buildDriveWay(dummyRoute.begin(), dummyRoute.end());
}


std::string
MSRailSignal::LinkInfo::getID() const {
    return myLink->getTLLogic()->getID() + "_" + toString(myLink->getTLIndex());
}


MSRailSignal::DriveWay&
MSRailSignal::LinkInfo::getDriveWay(const SUMOVehicle* veh) {
    if (myUniqueDriveWay) {
        return myDriveways.front();
    }
    MSEdge* first = &myLink->getLane()->getEdge();
    MSRouteIterator firstIt = std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), first);
    if (firstIt == veh->getRoute().end()) {
        WRITE_WARNING("Invalid approach information after rerouting");
        return myDriveways.front();
    }
    //std::cout << SIMTIME << " veh=" << veh->getID() << " rsl=" << getID() << " dws=" << myDriveways.size() << "\n";
    for (DriveWay& dw : myDriveways) {
        // @todo optimize: it is sufficient to check for specific edges (after each switch)
        auto itRoute = firstIt;
        auto itDwRoute = dw.myRoute.begin();
        bool match = true;
        while (itRoute != veh->getRoute().end() && itDwRoute != dw.myRoute.end()) {
            if (*itRoute != *itDwRoute) {
                match = false;
                //std::cout << "  check dw=" << dw.myIndex << " match failed at vehEdge=" << (*itRoute)->getID() << " dwEdge=" << (*itDwRoute)->getID() << "\n";
                break;
            }
            itRoute++;
            itDwRoute++;
        }
        if (match) {
            //std::cout << "  using dw=" << dw.myIndex << "\n";
            return dw;
        }
    }
    return buildDriveWay(firstIt, veh->getRoute().end());
}


MSRailSignal::DriveWay&
MSRailSignal::LinkInfo::buildDriveWay(MSRouteIterator first, MSRouteIterator end) {
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
    // bidiBlock (if any forwardBlock edge edge has bidi edge)
    // - search bidi backward recursive until first switch
    //   - from switch search backward recursive all other incoming until controlled rail signal link
    //     -> add final links to conflictLinks
    //
    // flanks
    // - search backward recursive from flanking switches
    //   until controlled railSignal link or protecting switch is found
    //   -> add all found lanes to conflictLanes
    //   -> add final links to conflictLinks

    DriveWay dw((int)myDriveways.size());
    LaneSet visited;
    std::vector<MSLane*> before;
    visited.insert(myLink->getLaneBefore());
    MSLane* fromBidi = myLink->getLaneBefore()->getBidiLane();
    if (fromBidi != nullptr) {
        // do not extend to forward block beyond the entering track (in case of a loop)
        visited.insert(fromBidi);
        before.push_back(fromBidi);
    }
    dw.buildRoute(myLink, 0., first, end, visited);
    dw.checkFlanks(dw.myForward, visited, true);
    dw.checkFlanks(dw.myBidi, visited, false);
    dw.checkFlanks(before, visited, true);

    for (MSLink* link : dw.myFlankSwitches) {
        //std::cout << getID() << " flankSwitch=" << link->getDescription() << "\n";
        dw.findFlankProtection(link, 0, visited, link);
    }

#ifdef DEBUG_BUILD_DRIVEWAY
    if (DEBUG_COND_LINKINFO || true) {
        std::cout << "  buildDriveWay railSignal=" << getID() << " dw=" << dw.myIndex
                  << "\n    route=" << toString(dw.myRoute)
                  << "\n    forward=" << toString(dw.myForward)
                  << "\n    bidi=" << toString(dw.myBidi)
                  << "\n    protSwitch=" << describeLinks(dw.myProtectingSwitches)
                  << "\n";
    }
#endif

    dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myForward.begin(), dw.myForward.end());
    dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myBidi.begin(), dw.myBidi.end());
    dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myFlank.begin(), dw.myFlank.end());

    myDriveways.push_back(dw);
    return myDriveways.back();
}


void
MSRailSignal::LinkInfo::reroute(SUMOVehicle* veh, const MSEdgeVector& occupied) {
    MSDevice_Routing* rDev = static_cast<MSDevice_Routing*>(veh->getDevice(typeid(MSDevice_Routing)));
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    if (rDev != nullptr 
            && rDev->mayRerouteRailSignal()
            && (myLastRerouteVehicle != veh
             // reroute each vehicle only once if no periodic routing is allowed,
             // otherwise with the specified period
             || (rDev->getPeriod() > 0 && myLastRerouteTime + rDev->getPeriod() <= now))) {
        myLastRerouteVehicle = veh;
        myLastRerouteTime = now;

#ifdef DEBUG_REROUTE
        ConstMSEdgeVector oldRoute = veh->getRoute().getEdges();
        if (DEBUG_COND_LINKINFO) {
            std::cout << SIMTIME << " reroute veh=" << veh->getID() << " rs=" << getID() << " occupied=" << toString(occupied) << "\n";
        }
#endif
        MSRoutingEngine::reroute(*veh, now, "railSignal:" + getID(), false, true, occupied);
#ifdef DEBUG_REROUTE
        // attention this works only if we are not parallel!
        if (DEBUG_COND_LINKINFO) {
            if (veh->getRoute().getEdges() != oldRoute) {
                std::cout << "    rerouting successful\n";
            }
        }
#endif
    }
}


// ===========================================================================
// DriveWay method definitions
// ===========================================================================

bool
MSRailSignal::DriveWay::reserve(const Approaching& closest, MSEdgeVector& occupied) {
    const SUMOVehicleParameter::Stop* stop = closest.first->getNextStopParameter();
    const std::string joinVehicle = stop != nullptr ? stop->join : "";
    if (conflictLaneOccupied(joinVehicle)) {
        for (MSLane* bidi : myBidi) {
            if (!bidi->empty() && bidi->getBidiLane() != nullptr) {
                occupied.push_back(&bidi->getBidiLane()->getEdge());
            }
        }
#ifdef DEBUG_SIGNALSTATE
        if (gDebugFlag4) {
            std::cout << "  conflictLaneOccupied\n";
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
    myActive = closest.first;
    return true;
}


bool
MSRailSignal::DriveWay::conflictLinkApproached() const {
    for (MSLink* foeLink : myConflictLinks) {
        if (foeLink->getApproaching().size() > 0) {
            return true;
        }
    }
    return false;
}


bool
MSRailSignal::DriveWay::hasLinkConflict(const Approaching& veh, MSLink* foeLink) const {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
    if (gDebugFlag4) {
        std::cout << "   checkLinkConflict foeLink=" << getTLLinkID(foeLink) << "\n";
    }
#endif
    if (foeLink->getApproaching().size() > 0) {
        Approaching foe = getClosest(foeLink);
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
            const DriveWay& foeDriveWay = foeRS->myLinkInfos[foeLink->getTLIndex()].getDriveWay(foe.first);
            if (foeDriveWay.conflictLaneOccupied() ||
                    !overlap(foeDriveWay)) {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
                if (gDebugFlag4) {
                    if (foeDriveWay.conflictLaneOccupied()) {
                        std::cout << "     foe blocked\n";
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
                        << "\n";
            }
#endif
            if (foe.second.arrivalSpeedBraking == veh.second.arrivalSpeedBraking) {
                if (foe.second.arrivalTime == veh.second.arrivalTime) {
                    if (foe.first->getSpeed() == veh.first->getSpeed()) {
                        if (foe.second.dist  == veh.second.dist) {
                            return foe.first->getNumericalID() < veh.first->getNumericalID();
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
    }
    return false;
}


bool
MSRailSignal::DriveWay::conflictLaneOccupied(const std::string& joinVehicle) const {
    for (const MSLane* lane : myConflictLanes) {
        if (!lane->isEmpty()) {
#ifdef DEBUG_SIGNALSTATE
            if (gDebugFlag4) {
                std::cout << SIMTIME << " conflictLane " << lane->getID() << " occupied\n";
                if (joinVehicle != "") {
                    std::cout << "  joinVehicle=" << joinVehicle << " occupant=" << toString(lane->getVehiclesSecure()) << "\n";
                    lane->releaseVehicles();
                }
            }
#endif
            if (lane->getVehicleNumber() == 1 && joinVehicle != "") {
                std::vector<MSVehicle*> vehs = lane->getVehiclesSecure();
                const bool ignoreJoinTarget = vehs.front()->getID() == joinVehicle && vehs.front()->isStopped();
                lane->releaseVehicles();
                if (ignoreJoinTarget) {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4) {
                        std::cout << "    ignore join-target '" << joinVehicle << ";\n";
                    }
#endif
                    continue;
                }
            }
            return true;
        }
    }
    return false;
}


bool
MSRailSignal::DriveWay::findProtection(const Approaching& veh, MSLink* link) const {
    double flankApproachingDist = std::numeric_limits<double>::max();
    if (link->getApproaching().size() > 0) {
        Approaching closest = getClosest(link);
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
                std::cout << " protectionCandidate=" << l2->getDescription() << " l2Via=" << Named::getIDSecure(l2->getViaLane()) << " occupied=" << !l2->getViaLane()->isEmpty() << "\n";
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
                Approaching closest2 = getClosest(l2);
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
        DriveWay tmp(-myIndex);
        const MSLane* before = link->getLaneBefore();
        tmp.myFlank.push_back(before);
        LaneSet visited;
        for (auto ili : before->getIncomingLanes()) {
            tmp.findFlankProtection(ili.viaLink, myMaxFlankLength, visited, ili.viaLink);
        }
        tmp.myConflictLanes = tmp.myFlank;
        tmp.myRoute = myRoute;
        MSEdgeVector occupied;
        if (gDebugFlag4) std::cout << SIMTIME << " tmpDW flank=" << toString(tmp.myFlank)
                                       << " protSwitch=" << describeLinks(tmp.myProtectingSwitches) << " cLinks=" << describeLinks(tmp.myConflictLinks) << "\n";
        return tmp.reserve(veh, occupied);
    }
}


bool
MSRailSignal::DriveWay::overlap(const DriveWay& other) const {
    for (const MSEdge* edge : myRoute) {
        for (const MSEdge* edge2 : other.myRoute) {
            if (edge->getToJunction() == edge2->getToJunction()
                    || edge->getToJunction() == edge2->getFromJunction()) {
                // XXX might be rail_crossing with parallel tracks
                return true;
            }
        }
    }
    return false;
}

void
MSRailSignal::DriveWay::writeBlocks(OutputDevice& od) const {
    od.openTag("driveWay");
    od.writeAttr(SUMO_ATTR_EDGES, toString(myRoute));
    od.openTag("forward");
    od.writeAttr(SUMO_ATTR_LANES, toString(myForward));
    od.closeTag();
    od.openTag("bidi");
    od.writeAttr(SUMO_ATTR_LANES, toString(myBidi));
    od.closeTag();
    od.openTag("flank");
    od.writeAttr(SUMO_ATTR_LANES, toString(myFlank));
    od.closeTag();
    od.openTag("conflictLinks");
    std::vector<std::string> signals;
    for (MSLink* link : myConflictLinks) {
        signals.push_back(getTLLinkID(link));
    }
    od.writeAttr("signals", joinToString(signals, " "));
    od.closeTag();
    od.closeTag(); // driveWay
}


void
MSRailSignal::DriveWay::buildRoute(MSLink* origin, double length,
                                   MSRouteIterator next, MSRouteIterator end,
                                   LaneSet& visited) {
    bool seekForwardSignal = true;
    bool seekBidiSwitch = true;
    MSLane* toLane = origin->getViaLaneOrLane();
    //std::cout << "buildRoute origin=" << getTLLinkID(origin) << " vehRoute=" << toString(ConstMSEdgeVector(next, end)) << " visited=" << joinNamedToString(visited, " ") << "\n";
    while ((seekForwardSignal || seekBidiSwitch)) {
        if (length > MAX_BLOCK_LENGTH) {
            if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                WRITE_WARNING("Block after rail signal " + getClickableTLLinkID(origin) +
                              " exceeds maximum length (stopped searching after edge '" + toLane->getEdge().getID() + "' (length=" + toString(length) + "m).");
            }
            myNumWarnings++;
            // length exceeded
            return;
        }
        //std::cout << "   toLane=" << toLane->getID() << " visited=" << joinNamedToString(visited, " ") << "\n";
        if (visited.count(toLane) != 0) {
            WRITE_WARNING("Found circular block after railSignal " + getClickableTLLinkID(origin) + " (" + toString(myRoute.size()) + " edges, length " + toString(length) + ")");
            return;
        }
        if (toLane->getEdge().isNormal()) {
            myRoute.push_back(&toLane->getEdge());
            if (next != end) {
                next++;
            }
        }
        visited.insert(toLane);
        length += toLane->getLength();
        MSLane* bidi = toLane->getBidiLane();
        if (seekForwardSignal) {
            myForward.push_back(toLane);
        } else if (bidi == nullptr) {
            seekBidiSwitch = false;
        }
        if (bidi != nullptr) {
            myBidi.push_back(bidi);
            visited.insert(bidi);
            if (!seekForwardSignal) {
                // look for switch that could protect from oncoming vehicles
                for (const auto& ili : bidi->getIncomingLanes()) {
                    if (ili.viaLink->getDirection() == LINKDIR_TURN) {
                        continue;
                    }
                    for (MSLink* link : ili.lane->getLinkCont()) {
                        if (link->getDirection() == LINKDIR_TURN) {
                            continue;
                        }
                        if (link->getViaLaneOrLane() != bidi) {
                            // this switch is special beause it still lies on the current route
                            myProtectingSwitches.push_back(ili.viaLink);
                            return;
                        }
                    }
                }
            }
        }
        const MSLinkCont& links = toLane->getLinkCont();
        toLane = nullptr;
        for (MSLink* link : links) {
            if (((next != end && &link->getLane()->getEdge() == *next) ||
                    (next == end && link->getDirection() != LINKDIR_TURN))
                    && isRailway(link->getViaLaneOrLane()->getPermissions())) {
                toLane = link->getViaLaneOrLane();
                if (link->getTLLogic() != nullptr) {
                    if (link->getTLLogic() == origin->getTLLogic()) {
                        WRITE_WARNING("Found circular block at railSignal " + getClickableTLLinkID(origin) + " (" + toString(myRoute.size()) + " edges, length " + toString(length) + ")");
                        return;
                    }
                    seekForwardSignal = false;
                    seekBidiSwitch = bidi != nullptr;
                }
                break;
            }
        }
        if (toLane == nullptr) {
            if (next != end) {
                // no connection found, jump to next route edge
                toLane = (*next)->getLanes()[0];
            } else {
                return;
            }
        }
    }
}


void
MSRailSignal::DriveWay::checkFlanks(const std::vector<MSLane*>& lanes, const LaneSet& visited, bool allFoes) {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << " checkFlanks lanes=" << toString(lanes) << "\n  visited=" << joinNamedToString(visited, " ") << " allFoes=" << allFoes << "\n";
#endif
    for (MSLane* lane : lanes) {
        if (lane->isInternal()) {
            continue;
        }
        for (auto ili : lane->getIncomingLanes()) {
            if (visited.count(ili.lane->getNormalPredecessorLane()) == 0) {
#ifdef DEBUG_CHECK_FLANKS
                std::cout << " add flankSwitch junction=" << ili.viaLink->getJunction()->getID() << " index=" << ili.viaLink->getIndex() << "\n";
#endif
                myFlankSwitches.push_back(ili.viaLink);
            } else if (allFoes) {
                // link is part of the driveway, find foes that cross the driveway without entering
                checkCrossingFlanks(ili.viaLink, visited);
            }
        }
    }
}


void
MSRailSignal::DriveWay::checkCrossingFlanks(MSLink* dwLink, const LaneSet& visited) {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << "  checkCrossingFlanks  dwLink=" << dwLink->getDescription() << " visited=" << joinNamedToString(visited, " ") << "\n";
#endif
    const MSJunction* junction = dwLink->getJunction();
    const MSJunctionLogic* logic = junction->getLogic();
    assert(logic != nullptr);
    for (const MSEdge* in : junction->getIncoming()) {
        if (in->isInternal()) {
            continue;
        }
        for (MSLane* inLane : in->getLanes()) {
            if (isRailway(inLane->getPermissions()) && visited.count(inLane) == 0) {
                for (MSLink* link : inLane->getLinkCont()) {
                    if (link->getIndex() >= 0 && logic->getFoesFor(dwLink->getIndex()).test(link->getIndex())
                            && visited.count(link->getLane()) == 0) {
#ifdef DEBUG_CHECK_FLANKS
                        std::cout << " add crossing flankSwitch junction=" << junction->getID() << " index=" << link->getIndex() << "\n";
#endif
                        if (link->getViaLane() == nullptr) {
                            myFlankSwitches.push_back(link);
                        } else {
                            myFlankSwitches.push_back(link->getViaLane()->getLinkCont().front());
                        }
                    }
                }
            }
        }
    }
}

void
MSRailSignal::DriveWay::findFlankProtection(MSLink* link, double length, LaneSet& visited, MSLink* origLink) {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << "  findFlankProtection link=" << link->getDescription() << " length=" << length << " origLink=" << origLink->getDescription() << "\n";
#endif
    if (link->getTLLogic() != nullptr) {
        // guarded by signal
        myConflictLinks.push_back(link);
    } else if (length > MAX_BLOCK_LENGTH) {
        // length exceeded
        if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
            WRITE_WARNING("Incoming block at junction '" + origLink->getJunction()->getID() + "', link " + toString(origLink->getIndex()) + " exceeds maximum length (stopped searching after lane '" + link->getLane()->getID() + "' (length=" + toString(length) + "m).");
        }
        myNumWarnings++;
    } else {
        // find normal lane before this link
        const MSLane* lane = link->getLaneBefore();
        if (visited.count(lane) == 0) {
            visited.insert(lane);
            length += lane->getLength();
            if (lane->isInternal()) {
                myFlank.push_back(lane);
                findFlankProtection(lane->getIncomingLanes().front().viaLink, length, visited, origLink);
            } else {
                bool foundPSwitch = false;
                for (MSLink* l2 : lane->getLinkCont()) {
#ifdef DEBUG_CHECK_FLANKS
                    std::cout << "   lane=" << lane->getID() << " cand=" << l2->getDescription() << "\n";
#endif
                    if (l2->getDirection() != LINKDIR_TURN && l2->getLane() != link->getLane()) {
                        foundPSwitch = true;
                        // found potential protection
#ifdef DEBUG_CHECK_FLANKS
                        std::cout << "   protectingSwitch=" << l2->getDescription() << " for flank=" << link->getDescription() << "\n";
#endif
                        myProtectingSwitches.push_back(link);
                    }
                }
                if (!foundPSwitch) {
                    myFlank.push_back(lane);
                    // continue search for protection upstream recursively
                    for (auto ili : lane->getIncomingLanes()) {
                        if (ili.viaLink->getDirection() != LINKDIR_TURN) {
                            findFlankProtection(ili.viaLink, length, visited, origLink);
                        }
                    }
                }
            }
        }
    }
    myMaxFlankLength = MAX2(myMaxFlankLength, length);
}


/****************************************************************************/
