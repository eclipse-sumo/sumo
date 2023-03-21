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
/// @file    MSRailSignal.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Jakob Erdmann
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
#include <utils/foxtools/MFXWorkerThread.h>
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
#include "MSRailSignalConstraint.h"
#include "MSRailSignalControl.h"
#include "MSRailSignal.h"

// typical block length in germany on main lines is 3-5km on branch lines up to 7km
// special branches that are used by one train exclusively could also be up to 20km in length
// minimum block size in germany is 37.5m (LZB)
// larger countries (USA, Russia) might see blocks beyond 20km)
#define MAX_BLOCK_LENGTH 20000
#define MAX_SIGNAL_WARNINGS 10

//#define DEBUG_SELECT_DRIVEWAY
//#define DEBUG_BUILD_DRIVEWAY
//#define DEBUG_DRIVEWAY_UPDATE
//#define DEBUG_DRIVEWAY_BUILDROUTE
//#define DEBUG_CHECK_FLANKS

#define DEBUG_SIGNALSTATE
#define DEBUG_SIGNALSTATE_PRIORITY
#define DEBUG_FIND_PROTECTION
//#define DEBUG_RECHECKGREEN
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

std::vector<std::pair<MSLink*, int> > MSRailSignal::mySwitchedGreenFlanks;
std::map<std::pair<int, int>, bool> MSRailSignal::myDriveWayCompatibility;
int MSRailSignal::myDriveWayIndex(0);

bool MSRailSignal::myStoreVehicles(false);
MSRailSignal::VehicleVector MSRailSignal::myBlockingVehicles;
MSRailSignal::VehicleVector MSRailSignal::myRivalVehicles;
MSRailSignal::VehicleVector MSRailSignal::myPriorityVehicles;
std::string MSRailSignal::myConstraintInfo;

// ===========================================================================
// method definitions
// ===========================================================================
MSRailSignal::MSRailSignal(MSTLLogicControl& tlcontrol,
                           const std::string& id, const std::string& programID, SUMOTime delay,
                           const Parameterised::Map& parameters) :
    MSTrafficLightLogic(tlcontrol, id, programID, 0, TrafficLightType::RAIL_SIGNAL, delay, parameters),
    myCurrentPhase(DELTA_T, std::string(SUMO_MAX_CONNECTIONS, 'X')), // dummy phase
    myPhaseIndex(0) {
    myDefaultCycleTime = DELTA_T;
    myMovingBlock = OptionsCont::getOptions().getBool("railsignal-moving-block");
    MSRailSignalControl::getInstance().addSignal(this);
}

void
MSRailSignal::init(NLDetectorBuilder&) {
    if (myLanes.size() == 0) {
        WRITE_WARNINGF(TL("Rail signal at junction '%' does not control any links"), getID());
    }
    for (LinkVector& links : myLinks) { //for every link index
        if (links.size() != 1) {
            throw ProcessError("At railSignal '" + getID() + "' found " + toString(links.size())
                               + " links controlled by index " + toString(links[0]->getTLIndex()));
        }
        myLinkInfos.push_back(LinkInfo(links[0]));
    }
    updateCurrentPhase();
    setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
    myNumLinks = (int)myLinks.size();
}


MSRailSignal::~MSRailSignal() {
    removeConstraints();
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
            const bool mustWait = !constraintsAllow(closest.first);
            MSEdgeVector occupied;
            if (mustWait || !driveway.reserve(closest, occupied)) {
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
                if (driveway.myFlank.size() > 0 && myCurrentPhase.getState()[li.myLink->getTLIndex()] != 'G') {
                    // schedule recheck
                    mySwitchedGreenFlanks.push_back(std::make_pair(li.myLink, driveway.myNumericalID));
                }
#ifdef DEBUG_SIGNALSTATE
                if (gDebugFlag4) {
                    std::cout << SIMTIME << " rsl=" << li.getID() << " veh=" << closest.first->getID() << " reserved\n";
                }
#endif
            }
        } else {
            if (li.myDriveways.empty()) {
#ifdef DEBUG_SIGNALSTATE
                if (gDebugFlag4) {
                    std::cout << SIMTIME << " rsl=" << li.getID() << " red for unitialized signal (no driveways yet)\n";
                }
#endif
                state[li.myLink->getTLIndex()] = 'r';
            } else {
                DriveWay& driveway = li.myDriveways.front();
                if (driveway.conflictLaneOccupied() || driveway.conflictLinkApproached()) {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4) {
                        std::cout << SIMTIME << " rsl=" << li.getID() << " red for default driveway (" << toString(driveway.myRoute) << ")\n";
                    }
#endif
                    state[li.myLink->getTLIndex()] = 'r';
                } else {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4) {
                        std::cout << SIMTIME << " rsl=" << li.getID() << " green for default driveway (" << toString(driveway.myRoute) << ")\n";
                    }
#endif
                }
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


bool
MSRailSignal::constraintsAllow(const SUMOVehicle* veh) const {
    if (myConstraints.size() == 0) {
        return true;
    } else {
        const std::string tripID = veh->getParameter().getParameter("tripId", veh->getID());
        auto it = myConstraints.find(tripID);
        if (it != myConstraints.end()) {
            for (MSRailSignalConstraint* c : it->second) {
                // ignore insertion constraints here
                if (!c->isInsertionConstraint() && !c->cleared()) {
#ifdef DEBUG_SIGNALSTATE
                    if (gDebugFlag4) {
                        std::cout << "  constraint '" << c->getDescription() << "' not cleared\n";
                    }
#endif
                    if (myStoreVehicles) {
                        myConstraintInfo = c->getDescription();
                    }
                    return false;
                }
            }
        }
        return true;
    }
}


void
MSRailSignal::addConstraint(const std::string& tripId, MSRailSignalConstraint* constraint) {
    myConstraints[tripId].push_back(constraint);
}


bool
MSRailSignal::removeConstraint(const std::string& tripId, MSRailSignalConstraint* constraint) {
    if (myConstraints.count(tripId) != 0) {
        auto& constraints = myConstraints[tripId];
        auto it = std::find(constraints.begin(), constraints.end(), constraint);
        if (it != constraints.end()) {
            delete *it;
            constraints.erase(it);
            return true;
        }
    }
    return false;
}

void
MSRailSignal::removeConstraints() {
    for (auto item : myConstraints) {
        for (MSRailSignalConstraint* c : item.second) {
            delete c;
        }
    }
    myConstraints.clear();
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
MSRailSignal::getJunctionLinkID(MSLink* link) {
    return link->getJunction()->getID() + "_" + toString(link->getIndex());
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

std::string
MSRailSignal::formatVisitedMap(const LaneVisitedMap& visited) {
    std::vector<const MSLane*> lanes(visited.size(), nullptr);
    for (auto item : visited) {
        lanes[item.second] = item.first;
    }
    return toString(lanes);
}


void
MSRailSignal::appendMapIndex(LaneVisitedMap& map, const MSLane* lane) {
    // avoid undefined behavior from evaluation order
    const int tmp = (int)map.size();
    map[lane] = tmp;
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


void
MSRailSignal::initDriveWays(const SUMOVehicle* ego, bool update) {
    const ConstMSEdgeVector& edges = ego->getRoute().getEdges();
    int endIndex = ego->getParameter().arrivalEdge;
    if (endIndex < 0) {
        endIndex = (int)edges.size() - 1;
    }
    for (int i = ego->getParameter().departEdge; i <= endIndex - 1; i++) {
        const MSEdge* e = edges[i];
        if (e->getToJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL) {
            const MSEdge* e2 = edges[i + 1];
            for (MSLane* lane : e->getLanes()) {
                for (MSLink* link : lane->getLinkCont()) {
                    if (&link->getLane()->getEdge() == e2) {
                        MSRailSignal* rs = const_cast<MSRailSignal*>(dynamic_cast<const MSRailSignal*>(link->getTLLogic()));
                        if (rs != nullptr) {
                            LinkInfo& li = rs->myLinkInfos[link->getTLIndex()];
                            if (li.myDriveways.empty()) {
                                // init driveway
                                li.getDriveWay(ego);
                                if (update && rs->isActive()) {
                                    // vehicle may have rerouted it's intial trip
                                    // after the states have been set
                                    // @note: This is a hack because it could lead to invalid tls-output
                                    // (it's still an improvement over switching based on default driveways)
                                    rs->trySwitch();
                                    rs->setTrafficLightSignals(SIMSTEP);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


bool
MSRailSignal::hasOncomingRailTraffic(MSLink* link, const MSVehicle* ego, bool& brakeBeforeSignal) {
    // @note: this check is intended to prevent deadlock / collision by an inserted vehicle that
    // waits at a red signal and thus checks different things than ::reverse()
    bool hadOncoming = false;
    if (link->getJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL) {
        const MSEdge* bidi = link->getLaneBefore()->getEdge().getBidiEdge();
        if (bidi == nullptr) {
            brakeBeforeSignal = false;
            return false;
        }
        const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(link->getTLLogic());
        if (rs != nullptr) {
            const LinkInfo& li = rs->myLinkInfos[link->getTLIndex()];
            for (const DriveWay& dw : li.myDriveways) {
                //std::cout << SIMTIME << " hasOncomingRailTraffic link=" << getTLLinkID(link) << " dwRoute=" << toString(dw.myRoute) << " bidi=" << toString(dw.myBidi) << "\n";
                for (const MSLane* lane : dw.myBidi) {
                    if (!lane->isEmpty()) {
                        MSVehicle* veh = lane->getFirstAnyVehicle();
                        if (std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
#ifdef DEBUG_SIGNALSTATE
                            if (DEBUG_HELPER(rs)) {
                                std::cout << " oncoming vehicle on bidi-lane " << lane->getID() << "\n";
                            }
#endif
                            return true;
                        }
                    }
                }
                for (const MSLane* lane : dw.myFlank) {
                    if (!lane->isEmpty()) {
                        MSVehicle* veh = lane->getFirstAnyVehicle();
                        if (std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
#ifdef DEBUG_SIGNALSTATE
                            if (DEBUG_HELPER(rs)) {
                                std::cout << " oncoming vehicle on flank-lane " << lane->getID() << "\n";
                            }
#endif
                            return true;
                        }
                    }
                }
                if (dw.myProtectingSwitchesBidi.size() > 0) {
#ifdef DEBUG_SIGNALSTATE
                    gDebugFlag4 = DEBUG_HELPER(rs);
#endif
                    // yield to all foeLinks beyond switch
                    Approaching approaching(ego,
                                            MSLink::ApproachingVehicleInformation(SIMSTEP, 0, 0, 0, false, 0, 0, std::numeric_limits<double>::max(), 0, 0));
                    for (MSLink* const switchLink : dw.myProtectingSwitchesBidi) {
                        myBlockingVehicles.clear();
                        myRivalVehicles.clear();
                        myPriorityVehicles.clear();
                        myConstraintInfo = "";
                        myStoreVehicles = true;
                        const bool hasProtection = dw.findProtection(approaching, switchLink);
                        myStoreVehicles = false;
                        if (!hasProtection) {
                            for (const SUMOVehicle* veh : myBlockingVehicles) {
                                hadOncoming = true;
                                if (!brakeBeforeSignal || std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
#ifdef DEBUG_SIGNALSTATE
                                    if (DEBUG_HELPER(rs)) {
                                        std::cout << "  no protection at bidi-switch " << switchLink->getDescription() << " from veh=" << veh->getID() << "\n";
                                        gDebugFlag4 = false;
                                    }
#endif
                                    return true;
                                }
                            }
                            for (const SUMOVehicle* veh : myRivalVehicles) {
                                hadOncoming = true;
                                if (!brakeBeforeSignal || std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
#ifdef DEBUG_SIGNALSTATE
                                    if (DEBUG_HELPER(rs)) {
                                        std::cout << "  no protection at bidi-switch " << switchLink->getDescription() << " from linkRival veh=" << veh->getID() << "\n";
                                        gDebugFlag4 = false;
                                    }
#endif
                                    return true;
                                }
                            }
                        }
                    }
#ifdef DEBUG_SIGNALSTATE
                    gDebugFlag4 = false;
#endif
                }
                for (MSLink* foeLink : dw.myConflictLinks) {
                    if (foeLink->getApproaching().size() != 0) {
                        Approaching closest = getClosest(foeLink);
                        const SUMOVehicle* veh = closest.first;
                        if (veh->getSpeed() > 0 && closest.second.arrivalSpeedBraking > 0
                                && std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), bidi) != veh->getRoute().end()) {
#ifdef DEBUG_SIGNALSTATE
                            if (DEBUG_HELPER(rs)) {
                                std::cout << " oncoming vehicle approaching foe link " << foeLink->getDescription() << "\n";
                            }
#endif
                            return true;
                        }
                    }
                }
            }
        }
    }
    brakeBeforeSignal = hadOncoming;
    return false;
}

bool
MSRailSignal::hasInsertionConstraint(MSLink* link, const MSVehicle* veh, std::string& info, bool& isInsertionOrder) {
    if (link->getJunction() != nullptr && link->getJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL) {
        const MSRailSignal* rs = dynamic_cast<const MSRailSignal*>(link->getTLLogic());
        if (rs != nullptr && rs->myConstraints.size() > 0) {
            const std::string tripID = veh->getParameter().getParameter("tripId", veh->getID());
            auto it = rs->myConstraints.find(tripID);
            if (it != rs->myConstraints.end()) {
                for (MSRailSignalConstraint* c : it->second) {
                    if (c->isInsertionConstraint() && !c->cleared()) {
#ifdef DEBUG_SIGNALSTATE
                        if (DEBUG_HELPER(rs)) {
                            std::cout << SIMTIME << " rsl=" << rs->getID() << " insertion constraint '" << c->getDescription() << "' for vehicle '" << veh->getID() << "' not cleared\n";
                        }
#endif
                        info = c->getDescription();
                        isInsertionOrder = c->getType() == MSRailSignalConstraint::ConstraintType::INSERTION_ORDER;
                        return true;
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
    myLink(link) {
    reset();
}


void
MSRailSignal::LinkInfo::reset() {
    myLastRerouteTime = -1;
    myLastRerouteVehicle = nullptr;
    myDriveways.clear();
}


std::string
MSRailSignal::LinkInfo::getID() const {
    return myLink->getTLLogic()->getID() + "_" + toString(myLink->getTLIndex());
}


MSRailSignal::DriveWay&
MSRailSignal::LinkInfo::getDriveWay(const SUMOVehicle* veh) {
    MSEdge* first = &myLink->getLane()->getEdge();
    MSRouteIterator firstIt = std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), first);
    if (firstIt == veh->getRoute().end()) {
        // possibly the vehicle has already gone past the first edge (i.e.
        // because first is short or the step-length is high)
        // lets look backward along the route
        // give some slack because the vehicle might have been braking from a higher speed and using ballistic integration
        double lookBack = SPEED2DIST(veh->getSpeed() + 10);
        int routeIndex = veh->getRoutePosition() - 1;
        while (lookBack > 0 && routeIndex > 0) {
            const MSEdge* prevEdge = veh->getRoute().getEdges()[routeIndex];
            if (prevEdge == first) {
                firstIt = veh->getRoute().begin() + routeIndex;
                break;
            }
            lookBack -= prevEdge->getLength();
            routeIndex--;
        }
    }
    if (firstIt == veh->getRoute().end()) {
        WRITE_WARNING("Invalid approach information to rail signal '" + getClickableTLLinkID(myLink) + "' after rerouting for vehicle '" + veh->getID()
                      + "' first driveway edge '" + first->getID() + "' time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        if (myDriveways.empty()) {
            ConstMSEdgeVector dummyRoute;
            dummyRoute.push_back(&myLink->getLane()->getEdge());
            DriveWay dw = buildDriveWay(dummyRoute.begin(), dummyRoute.end());
            myDriveways.push_back(dw);
        }
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
                //std::cout << "  check dw=" << " match failed at vehEdge=" << (*itRoute)->getID() << " dwEdge=" << (*itDwRoute)->getID() << "\n";
                break;
            }
            itRoute++;
            itDwRoute++;
        }
        // if the vehicle arrives before the end of this driveway,
        // we'd rather build a new driveway to avoid superfluous restrictions
        if (match && itDwRoute == dw.myRoute.end()
                && (itRoute == veh->getRoute().end() || dw.myFoundSignal || dw.myFoundReversal)) {
            //std::cout << "  using dw=" << "\n";
            return dw;
        }
#ifdef DEBUG_SELECT_DRIVEWAY
        std::cout << SIMTIME << " rs=" << getID() << " veh=" << veh->getID() << " other dwSignal=" << dw.myFoundSignal << " dwRoute=" << toString(dw.myRoute) << " route=" << toString(veh->getRoute().getEdges()) << "\n";
#endif
    }
    DriveWay dw = buildDriveWay(firstIt, veh->getRoute().end());
#ifdef DEBUG_SELECT_DRIVEWAY
    std::cout << SIMTIME << " rs=" << getID() << " veh=" << veh->getID() << " new dwSignal=" << dw.myFoundSignal << " dwRoute=" << toString(dw.myRoute) << " route=" << toString(veh->getRoute().getEdges()) << "\n";
#endif
    myDriveways.push_back(dw);
    return myDriveways.back();
}


MSRailSignal::DriveWay
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

    DriveWay dw;
    LaneVisitedMap visited;
    std::vector<const MSLane*> before;
    appendMapIndex(visited, myLink->getLaneBefore());
    MSLane* fromBidi = myLink->getLaneBefore()->getBidiLane();
    if (fromBidi != nullptr) {
        // do not extend to forward block beyond the entering track (in case of a loop)
        appendMapIndex(visited, fromBidi);
        before.push_back(fromBidi);
    }
    dw.buildRoute(myLink, 0., first, end, visited);
    if (dw.myProtectedBidi == nullptr) {
        dw.myCoreSize = (int)dw.myRoute.size();
    }
    dw.checkFlanks(myLink, dw.myForward, visited, true, dw.myFlankSwitches);
    dw.checkFlanks(myLink, dw.myBidi, visited, false, dw.myFlankSwitches);
    dw.checkFlanks(myLink, before, visited, true, dw.myFlankSwitches);
    for (MSLink* link : dw.myFlankSwitches) {
        //std::cout << getID() << " flankSwitch=" << link->getDescription() << "\n";
        dw.findFlankProtection(link, 0, visited, link, dw.myFlank);
    }
    std::vector<MSLink*> flankSwitchesBidiExtended;
    dw.checkFlanks(myLink, dw.myBidiExtended, visited, false, flankSwitchesBidiExtended);
    for (MSLink* link : flankSwitchesBidiExtended) {
        //std::cout << getID() << " flankSwitchBEx=" << link->getDescription() << "\n";
        dw.findFlankProtection(link, 0, visited, link, dw.myBidiExtended);
    }

#ifdef DEBUG_BUILD_DRIVEWAY
    if (DEBUG_COND_LINKINFO || true) {
        std::cout << "  buildDriveWay railSignal=" << getID()
                  << "\n    route=" << toString(dw.myRoute)
                  << "\n    forward=" << toString(dw.myForward)
                  << "\n    bidi=" << toString(dw.myBidi)
                  << "\n    flank=" << toString(dw.myFlank)
                  << "\n    flankSwitch=" << describeLinks(dw.myFlankSwitches)
                  << "\n    protSwitch=" << describeLinks(dw.myProtectingSwitches)
                  << "\n    coreSize=" << dw.myCoreSize
                  << "\n";
    }
#endif
    MSRailSignal* rs = const_cast<MSRailSignal*>(static_cast<const MSRailSignal*>(myLink->getTLLogic()));
    if (!rs->myMovingBlock) {
        dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myForward.begin(), dw.myForward.end());
    }
    dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myBidi.begin(), dw.myBidi.end());
    dw.myConflictLanes.insert(dw.myConflictLanes.end(), dw.myFlank.begin(), dw.myFlank.end());
    if (dw.myProtectedBidi != nullptr) {
        MSRailSignalControl::getInstance().registerProtectedDriveway(rs, dw.myNumericalID, dw.myProtectedBidi);
    }

    return dw;
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
    std::string joinVehicle = "";
    if (!MSGlobals::gUseMesoSim) {
        const SUMOVehicleParameter::Stop* stop = closest.first->getNextStopParameter();
        if (stop != nullptr) {
            joinVehicle = stop->join;
        }
    }
    if (conflictLaneOccupied(joinVehicle, true, closest.first)) {
        for (const MSLane* bidi : myBidi) {
            if (!bidi->empty() && bidi->getBidiLane() != nullptr) {
                occupied.push_back(&bidi->getBidiLane()->getEdge());
            }
        }
#ifdef DEBUG_SIGNALSTATE
        if (gDebugFlag4) {
            std::cout << "  conflictLaneOccupied by=" << toString(myBlockingVehicles) << " ego=" << Named::getIDSecure(closest.first) << "\n";
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
    if (deadlockLaneOccupied()) {
        return false;
    }
    myActive = closest.first;
    return true;
}


bool
MSRailSignal::DriveWay::conflictLinkApproached() const {
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
            if (foeDriveWay.conflictLaneOccupied("", false, foe.first) ||
                    foeDriveWay.deadlockLaneOccupied(false) ||
                    !foeRS->constraintsAllow(foe.first) ||
                    !overlap(foeDriveWay)) {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
                if (gDebugFlag4) {
                    if (foeDriveWay.conflictLaneOccupied("", false, foe.first)) {
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
            if (myStoreVehicles) {
                myRivalVehicles.push_back(foe.first);
                if (yield) {
                    myPriorityVehicles.push_back(foe.first);
                }
            }
            return yield;
        }
    }
    return false;
}


bool
MSRailSignal::DriveWay::mustYield(const Approaching& veh, const Approaching& foe) {
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
MSRailSignal::DriveWay::conflictLaneOccupied(const std::string& joinVehicle, bool store, const SUMOVehicle* ego) const {
    for (const MSLane* lane : myConflictLanes) {
        if (!lane->isEmpty()) {
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
                    if (foe == ego && std::find(myBidi.begin(), myBidi.end(), lane) != myBidi.end()) {
#ifdef DEBUG_SIGNALSTATE
                        if (gDebugFlag4) {
                            std::cout << "    ignore ego as oncoming '" << ego->getID() << "\n";
                        }
#endif
                        continue;
                    }
                }
            }
            if (myStoreVehicles && store) {
                myBlockingVehicles.push_back(lane->getLastAnyVehicle());
            }
            return true;
        }
    }
    return false;
}

bool
MSRailSignal::DriveWay::deadlockLaneOccupied(bool store) const {
    for (const MSLane* lane : myBidiExtended) {
        if (!lane->empty()) {
            assert(myBidi.size() != 0);
            const MSEdge* lastBidi = myBidi.back()->getNextNormal();
            MSVehicle* foe = lane->getVehiclesSecure().front();
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
                if (myStoreVehicles && store) {
                    myBlockingVehicles.push_back(foe);
                }
                return true;
            }
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
        DriveWay tmp(true);
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
                                       << " protSwitch=" << describeLinks(tmp.myProtectingSwitches) << " cLinks=" << describeLinks(tmp.myConflictLinks) << "\n";
        return tmp.reserve(veh, occupied);
    }
}


bool
MSRailSignal::DriveWay::overlap(const DriveWay& other) const {
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
MSRailSignal::DriveWay::flankConflict(const DriveWay& other) const {
    for (const MSLane* lane : myForward) {
        for (const MSLane* lane2 : other.myForward) {
            if (lane == lane2) {
                return true;
            }
        }
        for (const MSLane* lane2 : other.myBidi) {
            if (lane == lane2) {
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
    if (myCoreSize != (int)myRoute.size()) {
        od.writeAttr("core", myCoreSize);
    }
    od.openTag("forward");
    od.writeAttr(SUMO_ATTR_LANES, toString(myForward));
    od.closeTag();
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
    od.closeTag(); // driveWay
}


void
MSRailSignal::DriveWay::buildRoute(MSLink* origin, double length,
                                   MSRouteIterator next, MSRouteIterator end,
                                   LaneVisitedMap& visited) {
    bool seekForwardSignal = true;
    bool seekBidiSwitch = true;
    bool foundUnsafeSwitch = false;
    MSLane* toLane = origin->getViaLaneOrLane();
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
    gDebugFlag4 = true; //getClickableTLLinkID(origin) == "junction 's24', link 0";
    if (gDebugFlag4) std::cout << "buildRoute origin=" << getTLLinkID(origin) << " vehRoute=" << toString(ConstMSEdgeVector(next, end))
                                   << " visited=" << formatVisitedMap(visited) << "\n";
#endif
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
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
        if (gDebugFlag4) {
            std::cout << "   toLane=" << toLane->getID() << " visited=" << formatVisitedMap(visited) << "\n";
        }
#endif
        if (visited.count(toLane) != 0) {
            WRITE_WARNINGF(TL("Found circular block after railSignal % (% edges, length %)"), getClickableTLLinkID(origin), toString(myRoute.size()), toString(length));
            //std::cout << getClickableTLLinkID(origin) << " circularBlock1=" << toString(myRoute) << " visited=" << formatVisitedMap(visited) << "\n";
            return;
        }
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
            appendMapIndex(visited, bidi);
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
                                        myFlankSwitches.push_back(ili.viaLink);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        const std::vector<MSLink*>& links = toLane->getLinkCont();
        const MSEdge* current = &toLane->getEdge();
        toLane = nullptr;
        for (const MSLink* const link : links) {
            if ((next != end && &link->getLane()->getEdge() == *next)
                    && isRailway(link->getViaLaneOrLane()->getPermissions())) {
                toLane = link->getViaLaneOrLane();
                if (link->getLane()->getBidiLane() != nullptr && &link->getLane()->getEdge() == current->getBidiEdge()) {
                    // do not follow turn-arounds even if the route contains a reversal
#ifdef DEBUG_DRIVEWAY_BUILDROUTE
                    if (gDebugFlag4) {
                        std::cout << "      abort: turn-around\n";
                    }
#endif
                    myFoundReversal = true;
                    return;
                }
                if (link->getTLLogic() != nullptr) {
                    if (link->getTLLogic() == origin->getTLLogic()) {
                        WRITE_WARNINGF(TL("Found circular block at railSignal % (% edges, length %)"), getClickableTLLinkID(origin), toString(myRoute.size()), toString(length));
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
                break;
            }
        }
        if (toLane == nullptr) {
            if (next != end) {
                // no connection found, jump to next route edge
                toLane = (*next)->getLanes()[0];
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
MSRailSignal::DriveWay::checkFlanks(const MSLink* originLink, const std::vector<const MSLane*>& lanes, const LaneVisitedMap& visited, bool allFoes, std::vector<MSLink*>& flankSwitches) const {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << " checkFlanks lanes=" << toString(lanes) << "\n  visited=" << formatVisitedMap(visited) << " allFoes=" << allFoes << "\n";
#endif
    const MSLink* reverseOriginLink = originLink->getLane()->getBidiLane() != nullptr && originLink->getLaneBefore()->getBidiLane() != nullptr
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
                flankSwitches.push_back(ili.viaLink);
            } else if (allFoes) {
                // link is part of the driveway, find foes that cross the driveway without entering
                checkCrossingFlanks(ili.viaLink, visited, flankSwitches);
            }
        }
    }
}


void
MSRailSignal::DriveWay::checkCrossingFlanks(MSLink* dwLink, const LaneVisitedMap& visited, std::vector<MSLink*>& flankSwitches) const {
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
            if (isRailway(inLane->getPermissions()) && visited.count(inLane) == 0) {
                for (MSLink* link : inLane->getLinkCont()) {
                    if (link->getIndex() >= 0 && logic->getFoesFor(dwLink->getIndex()).test(link->getIndex())
                            && visited.count(link->getLane()) == 0) {
#ifdef DEBUG_CHECK_FLANKS
                        std::cout << " add crossing flankSwitch junction=" << junction->getID() << " index=" << link->getIndex() << "\n";
#endif
                        if (link->getViaLane() == nullptr) {
                            flankSwitches.push_back(link);
                        } else {
                            flankSwitches.push_back(link->getViaLane()->getLinkCont().front());
                        }
                    }
                }
            }
        }
    }
}

void
MSRailSignal::DriveWay::findFlankProtection(MSLink* link, double length, LaneVisitedMap& visited, MSLink* origLink, std::vector<const MSLane*>& flank) {
#ifdef DEBUG_CHECK_FLANKS
    std::cout << "  findFlankProtection link=" << link->getDescription() << " length=" << length << " origLink=" << origLink->getDescription() << "\n";
#endif
    if (link->getTLLogic() != nullptr) {
        // guarded by signal
#ifdef DEBUG_CHECK_FLANKS
        std::cout << "   flank guarded by " << link->getTLLogic()->getID() << "\n";
#endif
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

void
MSRailSignal::storeTraCIVehicles(int linkIndex) {
    myBlockingVehicles.clear();
    myRivalVehicles.clear();
    myPriorityVehicles.clear();
    myConstraintInfo = "";
    myStoreVehicles = true;
    LinkInfo& li = myLinkInfos[linkIndex];
    if (li.myLink->getApproaching().size() > 0) {
        Approaching closest = getClosest(li.myLink);
        DriveWay& driveway = li.getDriveWay(closest.first);
        MSEdgeVector occupied;
        // call for side effects
        driveway.reserve(closest, occupied);
        constraintsAllow(closest.first);
    } else if (li.myDriveways.size() > 0) {
        li.myDriveways.front().conflictLaneOccupied();
        li.myDriveways.front().conflictLinkApproached();
    }
    myStoreVehicles = false;
}

MSRailSignal::VehicleVector
MSRailSignal::getBlockingVehicles(int linkIndex) {
    storeTraCIVehicles(linkIndex);
    return myBlockingVehicles;
}

MSRailSignal::VehicleVector
MSRailSignal::getRivalVehicles(int linkIndex) {
    storeTraCIVehicles(linkIndex);
    return myRivalVehicles;
}

MSRailSignal::VehicleVector
MSRailSignal::getPriorityVehicles(int linkIndex) {
    storeTraCIVehicles(linkIndex);
    return myPriorityVehicles;
}

std::string
MSRailSignal::getConstraintInfo(int linkIndex) {
    storeTraCIVehicles(linkIndex);
    return myConstraintInfo;
}

const MSRailSignal::DriveWay&
MSRailSignal::retrieveDriveWay(int numericalID) const {
    for (const LinkInfo& li : myLinkInfos) {
        for (const DriveWay& dw : li.myDriveways) {
            if (dw.myNumericalID == numericalID) {
                return dw;
            }
        }
    }
    throw ProcessError("Invalid driveway id " + toString(numericalID) + " at railSignal '" + getID() + "'");
}


void
MSRailSignal::recheckGreen() {
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
                        const DriveWay& dw = rs->retrieveDriveWay(item.second);
                        const DriveWay& dw2 = rs2->retrieveDriveWay(item2.second);
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
                        const Approaching& veh = rs->getClosest(item.first);
                        const Approaching& veh2 = rs2->getClosest(item2.first);
                        if (DriveWay::mustYield(veh, veh2)) {
                            std::string state = rs->myCurrentPhase.getState();
                            state[item.first->getTLIndex()] = 'r';
                            rs->myCurrentPhase.setState(state);
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
                            std::string state = rs2->myCurrentPhase.getState();
                            state[item2.first->getTLIndex()] = 'r';
                            rs2->myCurrentPhase.setState(state);
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

void
MSRailSignal::updateDriveway(int numericalID) {
    for (LinkInfo& li : myLinkInfos) {
        for (auto it = li.myDriveways.begin(); it != li.myDriveways.end(); it++) {
            const DriveWay& dw = *it;
            if (dw.myNumericalID == numericalID) {
#ifdef DEBUG_DRIVEWAY_UPDATE
                std::cout << SIMTIME << " rail signal junction '" << getID() << "' requires update for driveway " << numericalID << "\n";
#endif
                std::vector<const MSEdge*> route = dw.myRoute;
                li.myDriveways.erase(it);
                if (li.myDriveways.size() == 0) {
                    // rebuild default driveway
                    li.myDriveways.push_back(li.buildDriveWay(route.begin(), route.end()));
                }
                return;
            }
        }
    }
}

std::string
MSRailSignal::getBlockingVehicleIDs() const {
    MSRailSignal* rs = const_cast<MSRailSignal*>(this);
    if (myLinkInfos.size() == 1) {
        return toString(rs->getBlockingVehicles(0));
    } else {
        std::string result;
        for (int i = 0; i < (int)myLinkInfos.size(); i++) {
            result += toString(i) + ": " + toString(rs->getBlockingVehicles(i)) + ";";
        }
        return result;
    }
}
std::string
MSRailSignal::getRivalVehicleIDs() const {
    MSRailSignal* rs = const_cast<MSRailSignal*>(this);
    if (myLinkInfos.size() == 1) {
        return toString(rs->getRivalVehicles(0));
    } else {
        std::string result;
        for (int i = 0; i < (int)myLinkInfos.size(); i++) {
            result += toString(i) + ": " + toString(rs->getRivalVehicles(i)) + ";";
        }
        return result;
    }
}
std::string
MSRailSignal::getPriorityVehicleIDs() const {
    MSRailSignal* rs = const_cast<MSRailSignal*>(this);
    if (myLinkInfos.size() == 1) {
        return toString(rs->getPriorityVehicles(0));
    } else {
        std::string result;
        for (int i = 0; i < (int)myLinkInfos.size(); i++) {
            result += toString(i) + ": " + toString(rs->getPriorityVehicles(i)) + ";";
        }
        return result;
    }
}
std::string
MSRailSignal::getConstraintInfo() const {
    MSRailSignal* rs = const_cast<MSRailSignal*>(this);
    if (myLinkInfos.size() == 1) {
        return rs->getConstraintInfo(0);
    } else {
        std::string result;
        for (int i = 0; i < (int)myLinkInfos.size(); i++) {
            result += toString(i) + ": " + rs->getConstraintInfo(i);
        }
        return result;
    }
}

void
MSRailSignal::setParameter(const std::string& key, const std::string& value) {
    // some pre-defined parameters can be updated at runtime
    if (key == "moving-block") {
        bool movingBlock = StringUtils::toBool(value);
        if (movingBlock != myMovingBlock) {
            // recompute driveways
            myMovingBlock = movingBlock;
            for (LinkInfo& li : myLinkInfos) {
                li.reset();
            }
            updateCurrentPhase();
            setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    Parameterised::setParameter(key, value);
}

/****************************************************************************/
