/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSLink.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A connnection between lanes
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <algorithm>
#include <limits>
#include <utils/iodevices/OutputDevice.h>
#include "MSNet.h"
#include "MSJunction.h"
#include "MSLink.h"
#include "MSLane.h"
#include <microsim/pedestrians/MSPerson.h>
#include "MSEdge.h"
#include "MSGlobals.h"
#include "MSVehicle.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/pedestrians/MSPModel.h>

//#define MSLink_DEBUG_CROSSING_POINTS
//#define MSLink_DEBUG_OPENED
//#define DEBUG_APPROACHING
//#define DEBUG_COND (myLane->getID()=="43[0]_0" && myLaneBefore->getID()==":33_0_0")
//#define DEBUG_COND (myLane->getID()=="end_0")
//#define DEBUG_COND (true)

// ===========================================================================
// static member variables
// ===========================================================================
const SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);
// additional caution is needed when approaching a zipper link
const SUMOTime MSLink::myLookaheadTimeZipper = TIME2STEPS(4);

const double MSLink::ZIPPER_ADAPT_DIST(100);

// time to link in seconds below which adaptation should take place
#define ZIPPER_ADAPT_TIME 10
// the default safety gap when passing before oncoming pedestrians
#define JM_CROSSING_GAP_DEFAULT 10

// ===========================================================================
// member method definitions
// ===========================================================================
MSLink::MSLink(MSLane* predLane, MSLane* succLane, MSLane* via, LinkDirection dir, LinkState state, double length, double foeVisibilityDistance, bool keepClear, MSTrafficLightLogic* logic, int tlIndex) :
    myLane(succLane),
    myLaneBefore(predLane),
    myIndex(-1),
    myTLIndex(tlIndex),
    myLogic(logic),
    myState(state),
    myLastStateChange(SUMOTime_MIN),
    myDirection(dir),
    myLength(length),
    myFoeVisibilityDistance(foeVisibilityDistance),
    myHasFoes(false),
    myAmCont(false),
    myKeepClear(keepClear),
    myInternalLane(via),
    myInternalLaneBefore(0),
    myMesoTLSPenalty(0),
    myGreenFraction(1),
    myWalkingAreaFoe(0),
    myParallelRight(0),
    myParallelLeft(0),
    myJunction(0) {
}


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(int index, bool hasFoes, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes,
                              MSLane* internalLaneBefore) {
//#ifdef MSLink_DEBUG_CROSSING_POINTS
//    std::cout << " setRequestInformation() for junction " << getViaLaneOrLane()->getEdge().getFromJunction()->getID()
//            << "\nInternalLanes = " << toString(getViaLaneOrLane()->getEdge().getFromJunction()->getInternalLanes())
//            << std::endl;
//#endif
    myIndex = index;
    myHasFoes = hasFoes;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    for (std::vector<MSLane*>::const_iterator it_lane = foeLanes.begin(); it_lane != foeLanes.end(); ++it_lane) {
        // cannot assign vector due to const-ness
        myFoeLanes.push_back(*it_lane);
    }
    myJunction = const_cast<MSJunction*>(myLane->getEdge().getFromJunction()); // junctionGraph is initialized after the whole network is loaded
    myInternalLaneBefore = internalLaneBefore;
    MSLane* lane = 0;
    if (internalLaneBefore != 0) {
        // this is an exit link. compute crossing points with all foeLanes
        lane = internalLaneBefore;
        //} else if (myLane->getEdge().isCrossing()) {
        //    // this is the link to a pedestrian crossing. compute crossing points with all foeLanes
        //    // @note not currently used by pedestrians
        //    lane = myLane;
    }
#ifdef MSLink_DEBUG_CROSSING_POINTS
    std::cout << " link " << myIndex << " to " << getViaLaneOrLane()->getID() << " internalLaneBefore=" << (lane == 0 ? "NULL" : lane->getID()) << " has foes: " << toString(foeLanes) << "\n";
#endif
    if (lane != 0) {
        const bool beforeInternalJunction = lane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().isInternal();
        if (lane->getIncomingLanes().size() != 1) {
            throw ProcessError("Internal lane '" + lane->getID() + "' has " + toString(lane->getIncomingLanes().size()) + " predecessors");
        }
        // compute crossing points
        for (std::vector<const MSLane*>::const_iterator it_lane = myFoeLanes.begin(); it_lane != myFoeLanes.end(); ++it_lane) {
            const bool sameTarget = myLane == (*it_lane)->getLinkCont()[0]->getLane();
            if (sameTarget && !beforeInternalJunction) {
                //if (myLane == (*it_lane)->getLinkCont()[0]->getLane()) {
                // this foeLane has the same target and merges at the end (lane exits the junction)
                myLengthsBehindCrossing.push_back(std::make_pair(0, 0)); // dummy value, never used
#ifdef MSLink_DEBUG_CROSSING_POINTS
                std::cout
                        << " " << lane->getID()
                        << " merges with " << (*it_lane)->getID()
                        << " nextLane " << lane->getLinkCont()[0]->getViaLaneOrLane()->getID()
                        << " dist1=" << myLengthsBehindCrossing.back().first
                        << " dist2=" << myLengthsBehindCrossing.back().second
                        << "\n";
#endif
            } else {
                std::vector<double> intersections1 = lane->getShape().intersectsAtLengths2D((*it_lane)->getShape());
#ifdef MSLink_DEBUG_CROSSING_POINTS
//                std::cout << " intersections1=" << toString(intersections1) << "\n";
#endif
                bool haveIntersection = true;
                if (intersections1.size() == 0) {
                    intersections1.push_back(-10000.0); // disregard this foe (using maxdouble leads to nasty problems down the line)
                    haveIntersection = false;
                } else if (intersections1.size() > 1) {
                    std::sort(intersections1.begin(), intersections1.end());
                }
                std::vector<double> intersections2 = (*it_lane)->getShape().intersectsAtLengths2D(lane->getShape());
#ifdef MSLink_DEBUG_CROSSING_POINTS
                //std::cout << " intersections2=" << toString(intersections2) << "\n";
#endif
                if (intersections2.size() == 0) {
                    intersections2.push_back(0);
                } else if (intersections2.size() > 1) {
                    std::sort(intersections2.begin(), intersections2.end());
                }
                if (haveIntersection) {
                    // lane width affects the crossing point
                    intersections1.back() -= (*it_lane)->getWidth() / 2;
                    intersections2.back() -= lane->getWidth() / 2;
                    // also length/geometry factor. (XXX: Why subtract width/2 *before* converting geometric position to lane pos? refs #3031)
                    intersections1.back() = lane->interpolateGeometryPosToLanePos(intersections1.back());
                    intersections2.back() = (*it_lane)->interpolateGeometryPosToLanePos(intersections2.back());

                    if (internalLaneBefore->getLogicalPredecessorLane()->getEdge().isInternal() && !(*it_lane)->getEdge().isCrossing())  {
                        // wait at the internal junction
                        // (except for foes that are crossings since there is no internal junction)
                        intersections1.back() = 0;
                    }
                }

                myLengthsBehindCrossing.push_back(std::make_pair(
                                                      lane->getLength() - intersections1.back(),
                                                      (*it_lane)->getLength() - intersections2.back()));

#ifdef MSLink_DEBUG_CROSSING_POINTS
                std::cout
                        << " intersection of " << lane->getID()
                        << " totalLength=" << lane->getLength()
                        << " with " << (*it_lane)->getID()
                        << " totalLength=" << (*it_lane)->getLength()
                        << " dist1=" << myLengthsBehindCrossing.back().first
                        << " dist2=" << myLengthsBehindCrossing.back().second
                        << "\n";
#endif
            }
        }
        // check for overlap with internal lanes from the same source lane
        const MSLane* pred = lane->getLogicalPredecessorLane();
        // to avoid overlap with vehicles that came from pred (especially when pred has endOffset > 0)
        // we add all other internal lanes from pred as foeLanes
        const MSLinkCont& predLinks = pred->getLinkCont();
        for (MSLinkCont::const_iterator it = predLinks.begin(); it != predLinks.end(); ++it) {
            const MSLane* sibling = (*it)->getViaLane();
            if (sibling != lane && sibling != 0) {
                // assume that siblings have a similar shape for their whole length
                const double minLength = MIN2(lane->getLength(), sibling->getLength());
                myLengthsBehindCrossing.push_back(std::make_pair(
                                                      lane->getLength() - minLength,
                                                      sibling->getLength() - minLength));
                myFoeLanes.push_back(sibling);
#ifdef MSLink_DEBUG_CROSSING_POINTS
                std::cout << " adding same-origin foe" << sibling->getID()
                          << " dist1=" << myLengthsBehindCrossing.back().first
                          << " dist2=" << myLengthsBehindCrossing.back().second
                          << "\n";
#endif
            }
        }
    }
    if (MSGlobals::gLateralResolution > 0) {
        // check for links with the same origin lane and the same destination edge
        const MSEdge* myTarget = &myLane->getEdge();
        // save foes for entry links
        const MSLinkCont& predLinks = myLaneBefore->getLinkCont();
        for (MSLinkCont::const_iterator it = predLinks.begin(); it != predLinks.end(); ++it) {
            const MSEdge* target = &((*it)->getLane()->getEdge());
            if (*it != this && target == myTarget) {
                mySublaneFoeLinks.push_back(*it);
            }
        }
        // save foes for exit links
        if (fromInternalLane()) {
            //std::cout << " setRequestInformation link=" << getViaLaneOrLane()->getID() << " before=" << myLaneBefore->getID() << " before2=" << myLaneBefore->getIncomingLanes().front().lane->getID() << "\n";
            const MSLinkCont& predLinks2 = myLaneBefore->getIncomingLanes().front().lane->getLinkCont();
            for (MSLinkCont::const_iterator it = predLinks2.begin(); it != predLinks2.end(); ++it) {
                const MSEdge* target = &((*it)->getLane()->getEdge());
                if ((*it)->getViaLane() != myInternalLaneBefore && target == myTarget) {
                    //std::cout << " add sublaneFoe=" << (*it)->getViaLane()->getID() << "\n";
                    mySublaneFoeLanes.push_back((*it)->getViaLane());
                }
            }
        }
    }
}


std::pair<double, double>
getLastIntersections(const MSLane* lane, const MSLane* foe);

void
MSLink::setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime, const double arrivalSpeed, const double leaveSpeed,
                       const bool setRequest, const SUMOTime arrivalTimeBraking, const double arrivalSpeedBraking, const SUMOTime waitingTime, double dist) {
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, approaching->getVehicleType().getLength());
#ifdef DEBUG_APPROACHING
    if (DEBUG_COND) {
        std::cout << SIMTIME << " Link ''" << (myLaneBefore == 0 ? "NULL" : myLaneBefore->getID()) << "'->'" << (myLane == 0 ? "NULL" : myLane->getID()) << "' Adding approaching vehicle '" << approaching->getID() << "'\nCurrently registered vehicles:" << std::endl;
        for (auto i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
            std::cout << "'" << i->first->getID() << "'" << std::endl;
        }
    }
#endif
    myApproachingVehicles.insert(std::make_pair(approaching,
                                 ApproachingVehicleInformation(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, setRequest,
                                         arrivalTimeBraking, arrivalSpeedBraking, waitingTime, dist)));
}


void
MSLink::setApproaching(const SUMOVehicle* approaching, ApproachingVehicleInformation ai) {

#ifdef DEBUG_APPROACHING
    if (DEBUG_COND) {
        std::cout << SIMTIME << " Link ''" << (myLaneBefore == 0 ? "NULL" : myLaneBefore->getID()) << "'->'" << (myLane == 0 ? "NULL" : myLane->getID()) << "' Adding approaching vehicle '" << approaching->getID() << "'\nCurrently registered vehicles:" << std::endl;
        for (auto i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
            std::cout << "'" << i->first->getID() << "'" << std::endl;
        }
    }
#endif
    myApproachingVehicles.insert(std::make_pair(approaching, ai));
}


void
MSLink::addBlockedLink(MSLink* link) {
    myBlockedFoeLinks.insert(link);
}



bool
MSLink::willHaveBlockedFoe() const {
    for (std::set<MSLink*>::const_iterator i = myBlockedFoeLinks.begin(); i != myBlockedFoeLinks.end(); ++i) {
        if ((*i)->isBlockingAnyone()) {
            return true;
        }
    }
    return false;
}


void
MSLink::removeApproaching(const SUMOVehicle* veh) {

#ifdef DEBUG_APPROACHING
    if (DEBUG_COND) {
        std::cout << SIMTIME << " Link ''" << (myLaneBefore == 0 ? "NULL" : myLaneBefore->getID()) << "'->'" << (myLane == 0 ? "NULL" : myLane->getID()) << std::endl;
        std::cout << "' Removing approaching vehicle '" << veh->getID() << "'\nCurrently registered vehicles:" << std::endl;
        for (auto i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
            std::cout << "'" << i->first->getID() << "'" << std::endl;
        }
    }
#endif
    myApproachingVehicles.erase(veh);
}


MSLink::ApproachingVehicleInformation
MSLink::getApproaching(const SUMOVehicle* veh) const {
    std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.find(veh);
    if (i != myApproachingVehicles.end()) {
        return i->second;
    } else {
        return ApproachingVehicleInformation(-1000, -1000, 0, 0, false, -1000, 0, 0, 0);
    }
}


SUMOTime
MSLink::getLeaveTime(const SUMOTime arrivalTime, const double arrivalSpeed,
                     const double leaveSpeed, const double vehicleLength) const {
    return arrivalTime + TIME2STEPS((getLength() + vehicleLength) / MAX2(0.5 * (arrivalSpeed + leaveSpeed), NUMERICAL_EPS));
}


bool
MSLink::opened(SUMOTime arrivalTime, double arrivalSpeed, double leaveSpeed, double vehicleLength,
               double impatience, double decel, SUMOTime waitingTime, double posLat,
               std::vector<const SUMOVehicle*>* collectFoes, bool ignoreRed, const SUMOVehicle* ego) const {
    if (haveRed() && !ignoreRed) {
        return false;
    }
    if (myAmCont && MSGlobals::gUsingInternalLanes) {
        return true;
    }
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, vehicleLength);
    if (MSGlobals::gLateralResolution > 0) {
        // check for foes on the same lane
        for (std::vector<MSLink*>::const_iterator it = mySublaneFoeLinks.begin(); it != mySublaneFoeLinks.end(); ++it) {
            const MSLink* foeLink = *it;
            assert(myLane != foeLink->getLane());
            for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = foeLink->myApproachingVehicles.begin(); i != foeLink->myApproachingVehicles.end(); ++i) {
                const SUMOVehicle* foe = i->first;
                if (
                    // there only is a conflict if the paths cross
                    ((posLat < foe->getLateralPositionOnLane() && myLane->getIndex() > foeLink->myLane->getIndex())
                     || (posLat > foe->getLateralPositionOnLane() && myLane->getIndex() < foeLink->myLane->getIndex()))
                    // the vehicle that arrives later must yield
                    && (arrivalTime > i->second.arrivalTime
                        // if both vehicles arrive at the same time, the one
                        // to the left must yield
                        || (arrivalTime == i->second.arrivalTime && posLat > foe->getLateralPositionOnLane()))) {
                    if (blockedByFoe(i->first, i->second, arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, false,
                                     impatience, decel, waitingTime, ego)) {
#ifdef MSLink_DEBUG_OPENED
                        if (gDebugFlag1) {
                            std::cout << SIMTIME << " blocked by " << foe->getID() << " arrival=" << arrivalTime << " foeArrival=" << i->second.arrivalTime << "\n";
                        }
#endif
                        if (collectFoes == 0) {
#ifdef MSLink_DEBUG_OPENED
                            if (gDebugFlag1) {
                                std::cout << " link=" << getViaLaneOrLane()->getID() << " blocked by sublaneFoe=" << foe->getID() << " foeLink=" << foeLink->getViaLaneOrLane()->getID() << " posLat=" << posLat << "\n";
                            }
#endif
                            return false;
                        } else {
                            collectFoes->push_back(i->first);
                        }
                    }
                }
            }
        }
    }
    if (havePriority() && myState != LINKSTATE_ZIPPER) {
        // priority usually means the link is open but there are exceptions:
        // zipper still needs to collect foes
        // sublane model could have detected a conflict
        return collectFoes == 0 || collectFoes->size() == 0;
    }
    if ((myState == LINKSTATE_STOP || myState == LINKSTATE_ALLWAY_STOP) && waitingTime == 0) {
        return false;
    }

#ifdef MSLink_DEBUG_OPENED
    if (gDebugFlag1) {
        std::cout << SIMTIME << " opened link=" << getViaLaneOrLane()->getID() << " foeLinks=" << myFoeLinks.size() << "\n";
    }
#endif

    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->haveRed()) {
                continue;
            }
        }
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, myLane == (*i)->getLane(),
                                impatience, decel, waitingTime, collectFoes, ego)) {
            return false;
        }
    }
    if (collectFoes != 0 && collectFoes->size() > 0) {
        return false;
    }
    return true;
}


bool
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, double arrivalSpeed, double leaveSpeed,
                      bool sameTargetLane, double impatience, double decel, SUMOTime waitingTime,
                      std::vector<const SUMOVehicle*>* collectFoes, const SUMOVehicle* ego) const {
    for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
#ifdef MSLink_DEBUG_OPENED
        if (gDebugFlag1) {
            if (ego != 0
                    && ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_SPEED, 0) >= i->first->getSpeed()
                    && ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_PROB, 0) > 0) {
                std::cout << "    foe link=" << getViaLaneOrLane()->getID()
                          << " foeVeh=" << i->first->getID() << " (below ignore speed)"
                          << " ignoreFoeProb=" << ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_PROB, 0)
                          << "\n";
            }
        }
#endif
        if (i->first != ego
                && (ego == 0
                    || ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_PROB, 0) == 0
                    || ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_SPEED, 0) < i->first->getSpeed()
                    || ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_FOE_PROB, 0) < RandHelper::rand())
                && blockedByFoe(i->first, i->second, arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, sameTargetLane,
                                impatience, decel, waitingTime, ego)) {
            if (collectFoes == 0) {
                return true;
            } else {
                collectFoes->push_back(i->first);
            }
        }
    }
    return false;
}


bool
MSLink::blockedByFoe(const SUMOVehicle* veh, const ApproachingVehicleInformation& avi,
                     SUMOTime arrivalTime, SUMOTime leaveTime, double arrivalSpeed, double leaveSpeed,
                     bool sameTargetLane, double impatience, double decel, SUMOTime waitingTime,
                     const SUMOVehicle* ego) const {
#ifdef MSLink_DEBUG_OPENED
    if (gDebugFlag1) {
        std::cout << "    foe link=" << getViaLaneOrLane()->getID()
                  << " foeVeh=" << veh->getID()
                  << " req=" << avi.willPass
                  << " aT=" << avi.arrivalTime
                  << " lT=" << avi.leavingTime
                  << "\n";
    }
#endif
    if (!avi.willPass) {
        return false;
    }
    if (myState == LINKSTATE_ALLWAY_STOP) {
        assert(waitingTime > 0);
        if (waitingTime > avi.waitingTime) {
            return false;
        }
        if (waitingTime == avi.waitingTime && arrivalTime < avi.arrivalTime) {
            return false;
        }
    }
    const SUMOTime foeArrivalTime = (SUMOTime)((1.0 - impatience) * avi.arrivalTime + impatience * avi.arrivalTimeBraking);
    const SUMOTime lookAhead = (myState == LINKSTATE_ZIPPER
                                ? myLookaheadTimeZipper
                                : (ego == 0
                                   ? myLookaheadTime
                                   : TIME2STEPS(ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_TIMEGAP_MINOR, STEPS2TIME(myLookaheadTime)))));
    //if (ego != 0) std::cout << SIMTIME << " ego=" << ego->getID() << " jmTimegapMinor=" << ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_TIMEGAP_MINOR, -1) << " lookAhead=" << lookAhead << "\n";
#ifdef MSLink_DEBUG_OPENED
    if (gDebugFlag1) {
        std::cout << "       imp=" << impatience << " atb=" << avi.arrivalTimeBraking << " at2=" << foeArrivalTime << " lA=" << lookAhead << " egoLT=" << leaveTime << "\n";
    }
#endif
    if (avi.leavingTime < arrivalTime) {
        // ego wants to be follower
        if (sameTargetLane && (arrivalTime - avi.leavingTime < lookAhead
                               || unsafeMergeSpeeds(avi.leaveSpeed, arrivalSpeed,
                                       veh->getVehicleType().getCarFollowModel().getMaxDecel(), decel))) {
#ifdef MSLink_DEBUG_OPENED
            if (gDebugFlag1) {
                std::cout << "      blocked (cannot follow)\n";
            }
#endif
            return true;
        }
    } else if (foeArrivalTime > leaveTime + lookAhead) {
        // ego wants to be leader.
        if (sameTargetLane && unsafeMergeSpeeds(leaveSpeed, avi.arrivalSpeedBraking,
                                                decel, veh->getVehicleType().getCarFollowModel().getMaxDecel())) {
#ifdef MSLink_DEBUG_OPENED
            if (gDebugFlag1) {
                std::cout << "      blocked (cannot lead)\n";
            }
#endif
            return true;
        }
    } else {
        // even without considering safeHeadwayTime there is already a conflict
#ifdef MSLink_DEBUG_OPENED
        if (gDebugFlag1) {
            std::cout << "      blocked (hard conflict)\n";
        }
#endif
        return true;
    }
    return false;
}


bool
MSLink::maybeOccupied(MSLane* lane) {
    MSVehicle* veh = lane->getLastAnyVehicle();
    double distLeft = 0;
    if (veh == 0) {
        return false;
    } else {
        distLeft = lane->getLength() - veh->getBackPositionOnLane(lane);
        assert(distLeft > 0);
        // can we be sure that the vehicle leaves this lane in the next step?
        bool result = distLeft > (veh->getSpeed() - veh->getCarFollowModel().getMaxDecel());
        return result;
    }
}


bool
MSLink::hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, double speed, double decel) const {
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, speed, speed, myLane == (*i)->getLane(), 0, decel, 0)) {
            return true;
        }
    }
    for (std::vector<const MSLane*>::const_iterator i = myFoeLanes.begin(); i != myFoeLanes.end(); ++i) {
        if ((*i)->getVehicleNumberWithPartials() > 0) {
            return true;
        }
    }
    return false;
}


LinkDirection
MSLink::getDirection() const {
    return myDirection;
}


void
MSLink::setTLState(LinkState state, SUMOTime t) {
    if (myState != state) {
        myLastStateChange = t;
    }
    myState = state;
}


MSLane*
MSLink::getLane() const {
    return myLane;
}

bool
MSLink::lastWasContMajor() const {
    if (myInternalLane == 0 || myAmCont) {
        return false;
    } else {
        MSLane* pred = myInternalLane->getLogicalPredecessorLane();
        if (!pred->getEdge().isInternal()) {
            return false;
        } else {
            MSLane* pred2 = pred->getLogicalPredecessorLane();
            assert(pred2 != 0);
            MSLink* predLink = MSLinkContHelper::getConnectingLink(*pred2, *pred);
            assert(predLink != 0);
            return predLink->havePriority();
        }
    }
}


void
MSLink::writeApproaching(OutputDevice& od, const std::string fromLaneID) const {
    if (myApproachingVehicles.size() > 0) {
        od.openTag("link");
        od.writeAttr(SUMO_ATTR_FROM, fromLaneID);
        const std::string via = getViaLane() == 0 ? "" : getViaLane()->getID();
        od.writeAttr(SUMO_ATTR_VIA, via);
        od.writeAttr(SUMO_ATTR_TO, getLane() == 0 ? "" : getLane()->getID());
        std::vector<std::pair<SUMOTime, const SUMOVehicle*> > toSort; // stabilize output
        for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator it = myApproachingVehicles.begin(); it != myApproachingVehicles.end(); ++it) {
            toSort.push_back(std::make_pair(it->second.arrivalTime, it->first));
        }
        std::sort(toSort.begin(), toSort.end());
        for (std::vector<std::pair<SUMOTime, const SUMOVehicle*> >::const_iterator it = toSort.begin(); it != toSort.end(); ++it) {
            od.openTag("approaching");
            const ApproachingVehicleInformation& avi = myApproachingVehicles.find(it->second)->second;
            od.writeAttr(SUMO_ATTR_ID, it->second->getID());
            od.writeAttr(SUMO_ATTR_IMPATIENCE, it->second->getImpatience());
            od.writeAttr("arrivalTime", time2string(avi.arrivalTime));
            od.writeAttr("arrivalTimeBraking", time2string(avi.arrivalTimeBraking));
            od.writeAttr("leaveTime", time2string(avi.leavingTime));
            od.writeAttr("arrivalSpeed", toString(avi.arrivalSpeed));
            od.writeAttr("arrivalSpeedBraking", toString(avi.arrivalSpeedBraking));
            od.writeAttr("leaveSpeed", toString(avi.leaveSpeed));
            od.writeAttr("willPass", toString(avi.willPass));
            od.closeTag();
        }
        od.closeTag();
    }
}


double
MSLink::getInternalLengthsAfter() const {
    double len = 0.;
    MSLane* lane = myInternalLane;

    while (lane != 0 && lane->isInternal()) {
        len += lane->getLength();
        lane = lane->getLinkCont()[0]->getViaLane();
    }
    return len;
}

double
MSLink::getInternalLengthsBefore() const {
    double len = 0.;
    const MSLane* lane = myInternalLane;

    while (lane != 0 && lane->isInternal()) {
        len += lane->getLength();
        if (lane->getIncomingLanes().size() == 1) {
            lane = lane->getIncomingLanes()[0].lane;
        } else {
            break;
        }
    }
    return len;
}


double
MSLink::getLengthsBeforeCrossing(const MSLane* foeLane) const {
    MSLane* via = myInternalLane;
    double totalDist = 0.;
    bool foundCrossing = false;
    while (via != 0) {
        MSLink* link = via->getLinkCont()[0];
        double dist = link->getLengthBeforeCrossing(foeLane);
        if (dist != INVALID_DOUBLE) {
            // found conflicting lane
            totalDist += dist;
            foundCrossing = true;
            break;
        } else {
            totalDist += via->getLength();
            via = link->getViaLane();
        }
    }
    if (foundCrossing) {
        return totalDist;
    } else {
        return INVALID_DOUBLE;
    }
}


double
MSLink::getLengthBeforeCrossing(const MSLane* foeLane) const {
    int foe_ix;
    for (foe_ix = 0; foe_ix != (int)myFoeLanes.size(); ++foe_ix) {
        if (myFoeLanes[foe_ix] == foeLane) {
            break;
        }
    }
    if (foe_ix == (int)myFoeLanes.size()) {
        // no conflict with the given lane, indicate by returning -1
#ifdef MSLink_DEBUG_CROSSING_POINTS
        std::cout << "No crossing of lanes '" << foeLane->getID() << "' and '" << myInternalLaneBefore->getID() << "'" << std::endl;
#endif
        return INVALID_DOUBLE;
    } else {
        // found conflicting lane index
        double dist = myInternalLaneBefore->getLength() - myLengthsBehindCrossing[foe_ix].first;
        if (dist == -10000.) {
            // this is the value in myLengthsBehindCrossing, if the relation allows intersection but none is present for the actual geometry.
            return INVALID_DOUBLE;
        }
#ifdef MSLink_DEBUG_CROSSING_POINTS
        std::cout << "Crossing of lanes '" << myInternalLaneBefore->getID() << "' and '" << foeLane->getID()
                  << "' at distance " << dist << " (approach along '"
                  <<  myInternalLaneBefore->getEntryLink()->getLaneBefore()->getID() << "')" << std::endl;
#endif
        return dist;
    }
}


MSLane*
MSLink::getViaLane() const {
    return myInternalLane;
}


bool
MSLink::isExitLink() const {
    if (MSGlobals::gUsingInternalLanes) {
        return getInternalLaneBefore() != 0 && myLane->getEdge().getFunction() == EDGEFUNC_NORMAL;
    } else {
        return false;
    }
}


MSLink*
MSLink::getCorrespondingExitLink() const {
    MSLane* lane = myInternalLane;
    MSLink* link = 0;
    while (lane != 0) {
        link = lane->getLinkCont()[0];
        lane = link->getViaLane();
    }
    return link;
}


bool
MSLink::isInternalJunctionLink() const {
    return getInternalLaneBefore() != 0 && myInternalLane != 0;
}

bool
MSLink::fromInternalLane() const {
    return isExitLink() || isInternalJunctionLink();
}

MSLink::LinkLeaders
MSLink::getLeaderInfo(const MSVehicle* ego, double dist, std::vector<const MSPerson*>* collectBlockers, bool isShadowLink) const {
    LinkLeaders result;
    if (ego != 0 && ego->getLaneChangeModel().isOpposite()) {
        // ignore link leaders
        return result;
    }
    //gDebugFlag1 = true;
    // this link needs to start at an internal lane (either an exit link or between two internal lanes)
    // or it must be queried by the pedestrian model (ego == 0)
    if (fromInternalLane() || ego == 0) {
        if (gDebugFlag1) {
            std::cout << SIMTIME << " getLeaderInfo link=" << getViaLaneOrLane()->getID() << "\n";
        }
        // this is an exit link
        for (int i = 0; i < (int)myFoeLanes.size(); ++i) {
            const MSLane* foeLane = myFoeLanes[i];
            // distance from the querying vehicle to the crossing point with foeLane
            double distToCrossing = dist - myLengthsBehindCrossing[i].first;
            const bool sameTarget = (myLane == foeLane->getLinkCont()[0]->getLane());
            const bool sameSource = (myInternalLaneBefore != 0 && myInternalLaneBefore->getLogicalPredecessorLane() == foeLane->getLogicalPredecessorLane());
            const double crossingWidth = (sameTarget || sameSource) ? 0 : foeLane->getWidth();
            const double foeCrossingWidth = (sameTarget || sameSource) ? 0 : myInternalLaneBefore->getWidth();
            if (gDebugFlag1) {
                std::cout << " distToCrossing=" << distToCrossing << " foeLane=" << foeLane->getID() << "\n";
            }
            if (distToCrossing + crossingWidth < 0) {
                continue; // vehicle is behind the crossing point, continue with next foe lane
            }
            const double foeDistToCrossing = foeLane->getLength() - myLengthsBehindCrossing[i].second;
            // it is not sufficient to return the last vehicle on the foeLane because ego might be its leader
            // therefore we return all vehicles on the lane
            //
            // special care must be taken for continuation lanes. (next lane is also internal)
            // vehicles on these lanes should always block (gap = -1)
            const bool contLane = (foeLane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().isInternal());
            // vehicles on cont. lanes or on internal lanes with the same target as this link can never be ignored
            const bool cannotIgnore = (contLane || sameTarget || sameSource) && ego != 0;
            MSLane::AnyVehicleIterator end = foeLane->anyVehiclesEnd();
            for (MSLane::AnyVehicleIterator it_veh = foeLane->anyVehiclesBegin(); it_veh != end; ++it_veh) {
                MSVehicle* leader = (MSVehicle*)*it_veh;
                const bool isOpposite = leader->getLaneChangeModel().isOpposite();
                if (gDebugFlag1) {
                    std::cout << " candiate leader=" << leader->getID()
                              << " cannotIgnore=" << cannotIgnore
                              << " willPass=" << foeLane->getLinkCont()[0]->getApproaching(leader).willPass
                              << " isFrontOnLane=" << leader->isFrontOnLane(foeLane)
                              << " isOpposite=" << isOpposite << "\n";
                }
                if (leader == ego) {
                    continue;
                }
                if (!cannotIgnore && !foeLane->getLinkCont()[0]->getApproaching(leader).willPass && leader->isFrontOnLane(foeLane) && !isOpposite && !leader->isStopped()) {
                    continue;
                }
                if (cannotIgnore || leader->getWaitingTime() < MSGlobals::gIgnoreJunctionBlocker) {
                    // compute distance between vehicles on the the superimposition of both lanes
                    // where the crossing point is the common point
                    double gap;
                    bool fromLeft = true;
                    if (ego == 0) {
                        // request from pedestrian model. return distance between leaderBack and crossing point
                        const double leaderBack = leader->getBackPositionOnLane(foeLane);
                        //std::cout << "   foeLane=" << foeLane->getID() << " leaderBack=" << leaderBack << " foeDistToCrossing=" << foeDistToCrossing << " foeLength=" << foeLane->getLength() << " foebehind=" << myLengthsBehindCrossing[i].second << " dist=" << dist << " behind=" << myLengthsBehindCrossing[i].first << "\n";
                        gap = foeDistToCrossing - leaderBack;
                        // distToCrossing should not take into account the with of the foe lane
                        // (which was subtracted in setRequestInformation)
                        // Instead, the width of the foe vehicle is used directly by the caller.
                        distToCrossing += foeLane->getWidth() / 2;
                        if (gap + foeCrossingWidth < 0) {
                            // leader is completely past the crossing point
                            // or there is no crossing point
                            continue; // next vehicle
                        }
                        // we need to determine whether the vehicle passes the
                        // crossing from the left or the right (heuristic)
                        fromLeft = foeDistToCrossing > 0.5 * foeLane->getLength();
                    } else if ((contLane && !sameSource) || isOpposite) {
                        gap = -1; // always break for vehicles which are on a continuation lane or for opposite-direction vehicles
                    } else {
                        const double leaderBack = leader->getBackPositionOnLane(foeLane);
                        const double leaderBackDist = foeDistToCrossing - leaderBack;
                        if (gDebugFlag1) {
                            std::cout << " distToCrossing=" << distToCrossing << " leader back=" << leaderBack << " backDist=" << leaderBackDist << "\n";
                        }
                        if (leaderBackDist + foeCrossingWidth < 0) {
                            // leader is completely past the crossing point
                            // or there is no crossing point
                            continue; // next vehicle
                        }
                        gap = distToCrossing - leaderBackDist - ((sameTarget || sameSource) ? ego->getVehicleType().getMinGap() : 0);
                    }
                    if (gDebugFlag1) {
                        std::cout << " leader=" << leader->getID() << " contLane=" << contLane << " cannotIgnore=" << cannotIgnore << "\n";
                    }
                    // if the foe is already moving off the intersection, we may
                    // advance up to the crossing point unless we have the same target
                    const bool stopAsap = leader->isFrontOnLane(foeLane) ? cannotIgnore : sameTarget;
                    result.push_back(LinkLeader(leader, gap, stopAsap ? -1 : distToCrossing, fromLeft));
                }

            }
            if (ego != 0) {
                // check for crossing pedestrians (keep driving if already on top of the crossing
                const double distToPeds = distToCrossing - MSPModel::SAFETY_GAP;
                const double vehWidth = ego->getVehicleType().getWidth() + MSPModel::SAFETY_GAP; // + configurable safety gap
                /// @todo consider lateral position (depending on whether the crossing is encountered on the way in or out)
                // @check lefthand?!
                const bool wayIn = myLengthsBehindCrossing[i].first < myLaneBefore->getLength() * 0.5;
                const double vehSideOffset = (foeDistToCrossing + myLaneBefore->getWidth() * 0.5 - vehWidth * 0.5
                                              + ego->getLateralPositionOnLane() * (wayIn ? -1 : 1));
                if (distToPeds >= -MSPModel::SAFETY_GAP && MSPModel::getModel()->blockedAtDist(foeLane, vehSideOffset, vehWidth,
                        ego->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_CROSSING_GAP, JM_CROSSING_GAP_DEFAULT),
                        collectBlockers)) {
                    result.push_back(LinkLeader((MSVehicle*)0, -1, distToPeds));
                }
            }
        }

        //std::cout << SIMTIME << " ego=" << Named::getIDSecure(ego) << " link=" << getViaLaneOrLane()->getID() << " myWalkingAreaFoe=" << Named::getIDSecure(myWalkingAreaFoe) << "\n";
        if (ego != 0 && myWalkingAreaFoe != 0 && myWalkingAreaFoe->getEdge().getPersons().size() > 0) {
            // pedestrians may be on an arbitrary path across this
            // walkingarea. make sure to keep enough distance.
            // This is a simple but conservative solution that could be improved
            // by ignoring pedestrians that are "obviously" not on a collision course
            double distToPeds = std::numeric_limits<double>::max();
            const std::set<MSTransportable*>& persons = myWalkingAreaFoe->getEdge().getPersons();
            for (std::set<MSTransportable*>::const_iterator it = persons.begin(); it != persons.end(); ++it) {
                MSPerson* p = dynamic_cast<MSPerson*>(*it);
                distToPeds = MIN2(distToPeds, ego->getPosition().distanceTo2D(p->getPosition()) - p->getVehicleType().getLength() - MSPModel::SAFETY_GAP);
                if (collectBlockers != 0) {
                    collectBlockers->push_back(p);
                }
            }
            result.push_back(LinkLeader((MSVehicle*)0, -1, distToPeds));
        }

        if (MSGlobals::gLateralResolution > 0 && ego != 0 && !isShadowLink) {
            // check for foes on the same lane
            for (std::vector<MSLane*>::const_iterator it = mySublaneFoeLanes.begin(); it != mySublaneFoeLanes.end(); ++it) {
                const MSLane* foeLane = *it;
                MSLane::AnyVehicleIterator end = foeLane->anyVehiclesEnd();
                for (MSLane::AnyVehicleIterator it_veh = foeLane->anyVehiclesBegin(); it_veh != end; ++it_veh) {
                    MSVehicle* leader = (MSVehicle*)*it_veh;
                    if (leader == ego) {
                        continue;
                    }
                    const double maxLength = MAX2(myInternalLaneBefore->getLength(), foeLane->getLength());
                    const double gap = dist - maxLength - ego->getVehicleType().getMinGap() + leader->getBackPositionOnLane(foeLane);
                    if (gap < -(ego->getVehicleType().getMinGap() + leader->getLength())) {
                        // ego is ahead of leader
                        continue;
                    }

                    const double posLat = ego->getLateralPositionOnLane();
                    const double posLatLeader = leader->getLateralPositionOnLane() + leader->getLatOffset(foeLane);
                    if (gDebugFlag1) {
                        std::cout << " sublaneFoe lane=" << myInternalLaneBefore->getID()
                                  << " foeLane=" << foeLane->getID()
                                  << " leader=" << leader->getID()
                                  << " egoLane=" << ego->getLane()->getID()
                                  << " leaderLane=" << leader->getLane()->getID()
                                  << " egoLat=" << posLat
                                  << " leaderLat=" << posLatLeader
                                  << " leaderLatOffset=" << leader->getLatOffset(foeLane)
                                  << " egoIndex=" << myInternalLaneBefore->getIndex()
                                  << " foeIndex=" << foeLane->getIndex()
                                  << " dist=" << dist
                                  << " leaderBack=" << leader->getBackPositionOnLane(foeLane)
                                  << "\n";
                    }
                    // there only is a conflict if the paths cross
                    if ((posLat < posLatLeader && myInternalLaneBefore->getIndex() > foeLane->getIndex())
                            || (posLat > posLatLeader && myInternalLaneBefore->getIndex() < foeLane->getIndex())) {
                        if (gDebugFlag1) {
                            std::cout << SIMTIME << " blocked by " << leader->getID() << " (sublane split) foeLane=" << foeLane->getID() << "\n";
                        }
                        result.push_back(LinkLeader(leader, gap, -1));
                    }
                }
            }
        }
    }
    return result;
}


MSLane*
MSLink::getViaLaneOrLane() const {
    if (myInternalLane != 0) {
        return myInternalLane;
    }
    return myLane;
}


const MSLane*
MSLink::getLaneBefore() const {
    if (myInternalLaneBefore != 0) {
        if (myLaneBefore != myInternalLaneBefore) {
            throw ProcessError("lane before mismatch!");
        }
    }
    return myLaneBefore;
}


MSLink*
MSLink::getParallelLink(int direction) const {
    if (direction == -1) {
        return myParallelRight;
    } else if (direction == 1) {
        return myParallelLeft;
    } else {
        assert(false);
        return 0;
    }
}


MSLink*
MSLink::computeParallelLink(int direction) {
    MSLane* before = getLaneBefore()->getParallelLane(direction);
    MSLane* after = getLane()->getParallelLane(direction);
    if (before != 0 && after != 0) {
        return MSLinkContHelper::getConnectingLink(*before, *after);
    } else {
        return 0;
    }
}

void
MSLink::passedJunction(const MSVehicle* vehicle) const {
    if (myJunction != 0) {
        myJunction->passedJunction(vehicle);
    }
}


bool
MSLink::isLeader(const MSVehicle* ego, const MSVehicle* foe) const {
    if (myJunction != 0) {
        return myJunction->isLeader(ego, foe);
    } else {
        // unregulated junction
        return false;
    }
}

const MSLane*
MSLink::getInternalLaneBefore() const {
    return myInternalLaneBefore;
}


double
MSLink::getZipperSpeed(const MSVehicle* ego, const double dist, double vSafe,
                       SUMOTime arrivalTime,
                       std::vector<const SUMOVehicle*>* collectFoes) const {
    //gDebugFlag1 = ego->getID() == "left.7";
    if (myFoeLinks.size() == 0) {
        // link should have LINKSTATE_MAJOR in this case
        assert(false);
        return vSafe;
    } else if (myFoeLinks.size() > 1) {
        throw ProcessError("Zipper junctions with more than two conflicting lanes are not supported (at junction '"
                           + myJunction->getID() + "')");
    }
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    const double secondsToArrival = STEPS2TIME(arrivalTime - now);
    if (secondsToArrival > ZIPPER_ADAPT_TIME && dist > ZIPPER_ADAPT_DIST) {
        //if (gDebugFlag1) std::cout << SIMTIME << " getZipperSpeed ego=" << ego->getID()
        //    << " dist=" << dist
        //    << " ignoring foes (arrival in " << STEPS2TIME(arrivalTime - now) << ")\n";
        return vSafe;
    }
    //if (gDebugFlag1) std::cout << SIMTIME << " getZipperSpeed ego=" << ego->getID()
    //    << " egoAT=" << arrivalTime
    //    << " dist=" << dist
    //    << " vSafe=" << vSafe
    //    << " numFoes=" << collectFoes->size()
    //    << "\n";
    MSLink* foeLink = myFoeLinks[0];
    const double vSafeOrig = vSafe;
    for (std::vector<const SUMOVehicle*>::const_iterator i = collectFoes->begin(); i != collectFoes->end(); ++i) {
        const MSVehicle* foe = dynamic_cast<const MSVehicle*>(*i);
        assert(foe != 0);
        const ApproachingVehicleInformation& avi = foeLink->getApproaching(foe);
        if (    // ignore vehicles that arrive after us (unless they are ahead and we could easily brake for them)
            ((avi.arrivalTime > arrivalTime) && !couldBrakeForLeader(dist, avi.dist, ego, foe)) ||
            // also ignore vehicles that are behind us and are able to brake for us
            couldBrakeForLeader(avi.dist, dist, foe, ego) ||
            // resolve ties by lane index
            (avi.arrivalTime == arrivalTime && avi.dist == dist && ego->getLane()->getIndex() < foe->getLane()->getIndex())) {
            //if (gDebugFlag1) std::cout
            //    << "    ignoring foe=" << foe->getID()
            //        << " foeAT=" << avi.arrivalTime
            //        << " foeDist=" << avi.dist
            //        << " foeSpeed=" << foe->getSpeed()
            //        << " egoSpeed=" << ego->getSpeed()
            //        << " deltaDist=" << avi.dist - dist
            //        << " delteSpeed=" << foe->getSpeed() - foe->getCarFollowModel().getMaxDecel() - ego->getSpeed()
            //        << "\n";
            continue;
        }
        const double gap = dist - foe->getVehicleType().getLength() - ego->getVehicleType().getMinGap() - avi.dist;
        const double follow = ego->getCarFollowModel().followSpeed(
                                  ego, ego->getSpeed(), gap, foe->getSpeed(), foe->getCarFollowModel().getMaxDecel());
        // speed adaption to follow the foe can be spread over secondsToArrival
        const double followInTime = vSafeOrig + (follow - vSafeOrig) / MAX2((double)1, secondsToArrival / TS);
        vSafe = MIN2(vSafe, followInTime);
        //if (gDebugFlag1) std::cout << "    adapting to foe=" << foe->getID()
        //    << " foeDist=" << avi.dist
        //    << " follow=" << follow
        //    << " followInTime=" << followInTime
        //    << " gap=" << gap
        //    << " foeSpeed=" << foe->getSpeed()
        //    << " follow=" << follow
        //    << " foeAT=" << avi.arrivalTime
        //    << " foeLT=" << avi.leavingTime
        //    << " foeAS=" << avi.arrivalSpeed
        //    << " vSafe=" << vSafe
        //    << "\n";
    }
    return vSafe;
}


bool
MSLink::couldBrakeForLeader(double followDist, double leaderDist, const MSVehicle* follow, const MSVehicle* leader) {
    return (// leader is ahead of follower
               followDist > leaderDist &&
               // and follower could brake for 1 s to stay behind leader
               followDist - leaderDist > follow->getSpeed() - follow->getCarFollowModel().getMaxDecel() - leader->getSpeed());
}


void
MSLink::initParallelLinks() {
    myParallelRight = computeParallelLink(-1);
    myParallelLeft = computeParallelLink(1);
}

/****************************************************************************/

