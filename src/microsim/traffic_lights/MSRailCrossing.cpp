/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSRailCrossing.cpp
/// @author  Jakob Erdmann
/// @author  Erik Tunsch
/// @date    Dez 2015
///
// A rail signal logic
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <utils/common/StringUtils.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSStop.h>
#include <microsim/MSVehicleType.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#include <mesosim/MEVehicle.h>
#include "MSTrafficLightLogic.h"
#include "MSRailCrossing.h"
#include <microsim/MSLane.h>
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSRailCrossing::MSRailCrossing(MSTLLogicControl& tlcontrol,
                               const std::string& id, const std::string& programID, SUMOTime delay,
                               const Parameterised::Map& parameters) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, 0, TrafficLightType::RAIL_CROSSING, Phases(), 0, delay, parameters) {
    // dummy phase, used to avoid crashing in MSTrafficLightLogic::setTrafficLightSignals()
    myPhases.push_back(new MSPhaseDefinition(1, std::string(SUMO_MAX_CONNECTIONS, 'X')));
    myDefaultCycleTime = 1;
}


MSRailCrossing::~MSRailCrossing() {}


void
MSRailCrossing::init(NLDetectorBuilder&) {
    const Parameterised::Map test = getParametersMap();
    myTimeGap = string2time(getParameter("time-gap", "15"));
    //use time-gap by default
    mySpaceGap = StringUtils::toDouble(getParameter("space-gap", "-1"));
    myMinGreenTime = string2time(getParameter("min-green", "5"));
    myOpeningDelay = string2time(getParameter("opening-delay", "3"));
    myOpeningTime = string2time(getParameter("opening-time", "3")); // red-yellow while opening
    /// XXX compute reasonable time depending on link length
    myYellowTime = string2time(getParameter("yellow-time", "5"));
    delete myPhases.front();
    myPhases.clear();
    myPhases.push_back(new MSPhaseDefinition(1, std::string(myLinks.size(), LINKSTATE_TL_GREEN_MAJOR)));
    myPhases.push_back(new MSPhaseDefinition(myYellowTime, std::string(myLinks.size(), LINKSTATE_TL_YELLOW_MINOR)));
    myPhases.push_back(new MSPhaseDefinition(1, std::string(myLinks.size(), LINKSTATE_TL_RED)));
    myPhases.push_back(new MSPhaseDefinition(myOpeningTime, std::string(myLinks.size(), LINKSTATE_TL_REDYELLOW)));
    // init phases
    updateCurrentPhase();
    setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
    myNumLinks = (int)myLinks.size();
}


void
MSRailCrossing::setParameter(const std::string& key, const std::string& value) {
    // some pre-defined parameters can be updated at runtime
    if (key == "time-gap") {
        myTimeGap = string2time(value);
    } else if (key == "space-gap") {
        mySpaceGap = StringUtils::toDouble(value);
    } else if (key == "min-green") {
        myMinGreenTime = string2time(value);
    } else if (key == "opening-delay") {
        myOpeningDelay = string2time(value);
    } else if (key == "opening-time") {
        myOpeningTime = string2time(value); // TODO update phases
    } else if (key == "yellow-time") {
        myYellowTime = string2time(value); // TODO update phases
    }
    Parameterised::setParameter(key, value);
}


// ----------- Handling of controlled links
void
MSRailCrossing::adaptLinkInformationFrom(const MSTrafficLightLogic& logic) {
    MSTrafficLightLogic::adaptLinkInformationFrom(logic);
    updateCurrentPhase();
}


// ------------ Switching and setting current rows
SUMOTime
MSRailCrossing::trySwitch() {
    const int oldStep = myStep;
    SUMOTime nextTry = updateCurrentPhase();
    //if (getID() == "cluster_1088529493_1260626727") std::cout << " myStep=" << myStep << " nextTry=" << nextTry << "\n";
    if (myStep != oldStep) {
        myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    }
    return nextTry;
}


SUMOTime
MSRailCrossing::updateCurrentPhase() {
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    SUMOTime stayRedUntil = now;
    // check rail links for approaching foes to determine whether and how long
    // the crossing must remain closed
    for (const MSLink* const link : myIncomingRailLinks) {
        for (const auto& it_avi : link->getApproaching()) {
            const MSLink::ApproachingVehicleInformation& avi = it_avi.second;
            if (avi.arrivalTime - myYellowTime - now < myTimeGap) {
                stayRedUntil = MAX2(stayRedUntil, avi.leavingTime + myOpeningDelay);
            }
            if (mySpaceGap >= 0 && avi.dist < mySpaceGap) {
                // TODO maybe check the incoming lanes because stopped vehicles do not register at the oncoming junction
                stayRedUntil = MAX2(stayRedUntil, avi.leavingTime + myOpeningDelay);
            }
        }
        if (link->getViaLane() != nullptr && link->getViaLane()->getVehicleNumberWithPartials() > 0) {
            // do not open if there is still a train on the crossing
            stayRedUntil = MAX2(stayRedUntil, now + DELTA_T + myOpeningDelay);
        }
        if (MSGlobals::gUseMesoSim && MSGlobals::gMesoNet != nullptr && link->getLaneBefore() != nullptr && link->getLane() != nullptr) {
            const MSEdge& railEdge = link->getLaneBefore()->getEdge();
            const MSEdge& targetEdge = link->getLane()->getEdge();
            const double maxLookbackDist = MAX2(mySpaceGap, STEPS2TIME(myTimeGap + myYellowTime) * MAX2(1.0, railEdge.getSpeedLimit()) * 2.0);
            std::vector<std::pair<const MSEdge*, double>> stack;
            std::set<const MSEdge*> visited;
            stack.push_back(std::make_pair(&railEdge, 0.));
            visited.insert(&railEdge);
            while (!stack.empty()) {
                const MSEdge* currEdge = stack.back().first;
                const double downstreamEdgesDist = stack.back().second;
                stack.pop_back();
                std::vector<MESegment*> segs;
                for (MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(*currEdge); s != nullptr; s = s->getNextSegment()) {
                    segs.push_back(s);
                }
                double remOnEdge = 0.;
                for (int sIdx = (int)segs.size() - 1; sIdx >= 0; --sIdx) {
                    MESegment* s = segs[sIdx];
                    for (int qIdx = 0; qIdx < s->numQueues(); ++qIdx) {
                        for (const MEVehicle* veh : s->getQueue(qIdx)) {
                            if (link->getApproaching().count(veh) > 0) {
                                continue;
                            }
                            const MSRouteIterator curRouteIt = veh->getCurrentRouteEdge();
                            const MSRouteIterator routeEnd = veh->getRoute().end();
                            MSRouteIterator railEdgeIt = routeEnd;
                            for (MSRouteIterator it = curRouteIt; it != routeEnd; ++it) {
                                if (*it == &railEdge && (it + 1) != routeEnd && *(it + 1) == &targetEdge) {
                                    railEdgeIt = it;
                                    break;
                                }
                            }
                            if (railEdgeIt == routeEnd) {
                                continue;
                            }
                            SUMOTime estArrival = MAX2(now, veh->getEventTime());
                            const double vCurr = MAX2(0.1, currEdge->getVehicleMaxSpeed(veh));
                            estArrival += TIME2STEPS(remOnEdge / vCurr);
                            double routeDownstreamDist = remOnEdge;
                            for (MSRouteIterator it = curRouteIt + 1; it <= railEdgeIt; ++it) {
                                const double vEdge = MAX2(0.1, (*it)->getVehicleMaxSpeed(veh));
                                estArrival += TIME2STEPS((*it)->getLength() / vEdge);
                                routeDownstreamDist += (*it)->getLength();
                            }
                            for (const MSStop& stop : veh->getStops()) {
                                if (!stop.reached && stop.edge <= railEdgeIt
                                        && (stop.edge > curRouteIt || (stop.edge == curRouteIt && stop.segment != nullptr && stop.segment->getIndex() > s->getIndex()))) {
                                    if (stop.duration > 0) {
                                        estArrival += stop.duration;
                                    }
                                    if (stop.pars.until >= 0) {
                                        estArrival = MAX2(estArrival, stop.pars.until);
                                    }
                                }
                            }
                            const double distToCrossing = s->getLength() + routeDownstreamDist;
                            const double arrSpeed = railEdge.getVehicleMaxSpeed(veh);
                            const double leaveSpeed = link->getViaLaneOrLane()->getVehicleMaxSpeed(veh);
                            const SUMOTime estLeaving = link->getLeaveTime(estArrival, arrSpeed, leaveSpeed, veh->getVehicleType().getLength());
                            if (estArrival - myYellowTime - now < myTimeGap) {
                                stayRedUntil = MAX2(stayRedUntil, estLeaving + myOpeningDelay);
                            }
                            if (mySpaceGap >= 0 && distToCrossing < mySpaceGap) {
                                stayRedUntil = MAX2(stayRedUntil, estLeaving + myOpeningDelay);
                            }
                        }
                    }
                    remOnEdge += s->getLength();
                }
                const double nextDownstreamDist = downstreamEdgesDist + currEdge->getLength();
                if (nextDownstreamDist <= maxLookbackDist) {
                    for (const MSEdge* pred : currEdge->getPredecessors()) {
                        if ((pred->getPermissions() & SVC_RAIL_CLASSES) != 0 && visited.insert(pred).second) {
                            stack.push_back(std::make_pair(pred, nextDownstreamDist));
                        }
                    }
                }
            }
        }
    }
    //if (getID() == "cluster_1088529493_1260626727") std::cout << SIMTIME << " stayRedUntil=" << stayRedUntil;
    const SUMOTime wait = stayRedUntil - now;

    if (myStep == 0) {
        // 'G': check whether the crossing can stay open
        if (wait == 0) {
            return DELTA_T;
        } else {
            myStep++;
            return myYellowTime;
        }
    } else if (myStep == 1) {
        // 'y': yellow time is over. switch to red
        myStep++;
        return MAX2(DELTA_T, wait);
    } else if (myStep == 2) {
        // 'r': check whether we may open again
        if (wait == 0) {
            myStep++;
            return myOpeningTime;
        } else {
            return wait;
        }
    } else { // (myStep == 3)
        // 'u': opening time is over, switch to green
        if (wait == 0) {
            myStep = 0;
            return myMinGreenTime;
        } else {
            // train approached during opening sequence, close again
            myStep = 2;
            return wait;
        }
    }
}


// ------------ Conversion between time and phase
SUMOTime
MSRailCrossing::getPhaseIndexAtTime(SUMOTime) const {
    return 0;
}

SUMOTime
MSRailCrossing::getOffsetFromIndex(int) const {
    return 0;
}

int
MSRailCrossing::getIndexFromOffset(SUMOTime) const {
    return 0;
}


void
MSRailCrossing::addLink(MSLink* link, MSLane* lane, int pos) {
    if (pos >= 0) {
        MSTrafficLightLogic::addLink(link, lane, pos);
    } else {
        myIncomingRailLinks.push_back(link);
    }
}


/****************************************************************************/
