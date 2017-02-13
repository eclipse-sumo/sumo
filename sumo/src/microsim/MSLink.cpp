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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define MSLink_DEBUG_CROSSING_POINTS

// ===========================================================================
// static member variables
// ===========================================================================
const SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);
// additional caution is needed when approaching a zipper link
const SUMOTime MSLink::myLookaheadTimeZipper = TIME2STEPS(4);

const SUMOReal MSLink::ZIPPER_ADAPT_DIST(100);

// time to link in seconds below which adaptation should take place
#define ZIPPER_ADAPT_TIME 10

// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* predLane, MSLane* succLane, LinkDirection dir, LinkState state, SUMOReal length, SUMOReal foeVisibilityDistance, bool keepClear, MSTrafficLightLogic* logic, int tlIndex) :
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
    myMesoTLSPenalty(0),
    myGreenFraction(1),
    myParallelRight(0),
    myParallelLeft(0),
    myJunction(0) {
}
#else
MSLink::MSLink(MSLane* predLane, MSLane* succLane, MSLane* via, LinkDirection dir, LinkState state, SUMOReal length, SUMOReal foeVisibilityDistance, bool keepClear, MSTrafficLightLogic* logic, int tlIndex) :
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
    myParallelRight(0),
    myParallelLeft(0),
    myJunction(0) {
}
#endif


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(int index, bool hasFoes, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes,
                              MSLane* internalLaneBefore) {
    myIndex = index;
    myHasFoes = hasFoes;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    for (std::vector<MSLane*>::const_iterator it_lane = foeLanes.begin(); it_lane != foeLanes.end(); ++it_lane) {
        // cannot assign vector due to const-ness
        myFoeLanes.push_back(*it_lane);
    }
    myJunction = const_cast<MSJunction*>(myLane->getEdge().getFromJunction()); // junctionGraph is initialized after the whole network is loaded
#ifdef HAVE_INTERNAL_LANES
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
    std::cout << " link " << myIndex << " to " << getViaLaneOrLane()->getID() << " internalLane=" << (lane == 0 ? "NULL" : lane->getID()) << " has foes: " << toString(foeLanes) << "\n";
#endif
    if (lane != 0) {
        const bool beforeInternalJunction = lane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().isInternal();
        assert(lane->getIncomingLanes().size() == 1);
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
                std::vector<SUMOReal> intersections1 = lane->getShape().intersectsAtLengths2D((*it_lane)->getShape());
#ifdef MSLink_DEBUG_CROSSING_POINTS
                //std::cout << " intersections1=" << toString(intersections1) << "\n";
#endif
                bool haveIntersection = true;
                if (intersections1.size() == 0) {
                    intersections1.push_back(-10000.0); // disregard this foe (using maxdouble leads to nasty problems down the line)
                    haveIntersection = false;
                } else if (intersections1.size() > 1) {
                    std::sort(intersections1.begin(), intersections1.end());
                }
                std::vector<SUMOReal> intersections2 = (*it_lane)->getShape().intersectsAtLengths2D(lane->getShape());
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
                    // also length/geometry factor
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
                std::vector<SUMOReal> intersections1 = lane->getShape().intersectsAtLengths2D(sibling->getShape());
#ifdef MSLink_DEBUG_CROSSING_POINTS
                //std::cout << " intersections1=" << toString(intersections1) << "\n";
#endif
                if (intersections1.size() > 0) {
                    std::sort(intersections1.begin(), intersections1.end());
                    if (intersections1.back() > NUMERICAL_EPS) {
                        // siblings share a common shape up to the last crossing point so intersections are identical and only need to be computed once
                        myLengthsBehindCrossing.push_back(std::make_pair(
                                                              lane->getLength() - intersections1.back(),
                                                              sibling->getLength() - intersections1.back()));
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
        }
    }
#else
    UNUSED_PARAMETER(internalLaneBefore);
#endif
    if (MSGlobals::gLateralResolution > 0) {
        // check for links with the same origin lane and the same destination edge
        const MSEdge* myTarget = &myLane->getEdge();
        const MSLinkCont& predLinks = myLaneBefore->getLinkCont();
        for (MSLinkCont::const_iterator it = predLinks.begin(); it != predLinks.end(); ++it) {
            const MSEdge* target = &((*it)->getLane()->getEdge());
            if (*it != this && target == myTarget) {
                mySublaneFoeLinks.push_back(*it);
            }
        }
    }
}


std::pair<SUMOReal, SUMOReal>
getLastIntersections(const MSLane* lane, const MSLane* foe);

void
MSLink::setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime, const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed,
                       const bool setRequest, const SUMOTime arrivalTimeBraking, const SUMOReal arrivalSpeedBraking, const SUMOTime waitingTime, SUMOReal dist) {
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, approaching->getVehicleType().getLength());
    myApproachingVehicles.insert(std::make_pair(approaching,
                                 ApproachingVehicleInformation(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, setRequest,
                                         arrivalTimeBraking, arrivalSpeedBraking, waitingTime, dist)));
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
MSLink::getLeaveTime(const SUMOTime arrivalTime, const SUMOReal arrivalSpeed,
                     const SUMOReal leaveSpeed, const SUMOReal vehicleLength) const {
    return arrivalTime + TIME2STEPS((getLength() + vehicleLength) / MAX2((SUMOReal)0.5 * (arrivalSpeed + leaveSpeed), NUMERICAL_EPS));
}


bool
MSLink::opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength,
               SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime, SUMOReal posLat,
               std::vector<const SUMOVehicle*>* collectFoes) const {
    if (haveRed()) {
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
                                     impatience, decel, waitingTime)) {
                        //std::cout << SIMTIME << " blocked by " << foe->getID() << " arrival=" << arrivalTime << " foeArrival=" << i->second.arrivalTime << "\n";
                        if (collectFoes == 0) {
                            //std::cout << " link=" << getViaLaneOrLane()->getID() << " blocked by sublaneFoe=" << foe->getID() << " foeLink=" << foeLink->getViaLaneOrLane()->getID() << " posLat=" << posLat << "\n";
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
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->haveRed()) {
                continue;
            }
        }
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, myLane == (*i)->getLane(),
                                impatience, decel, waitingTime, collectFoes)) {
            return false;
        }
    }
    if (collectFoes != 0 && collectFoes->size() > 0) {
        return false;
    }
    return true;
}


bool
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                      bool sameTargetLane, SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
                      std::vector<const SUMOVehicle*>* collectFoes) const {
    for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
        if (blockedByFoe(i->first, i->second, arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, sameTargetLane,
                         impatience, decel, waitingTime)) {
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
MSLink::blockedByFoe(const SUMOVehicle* veh, const ApproachingVehicleInformation& avi, SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                     bool sameTargetLane, SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime) const {
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
    const SUMOTime lookAhead = myState == LINKSTATE_ZIPPER ? myLookaheadTimeZipper : myLookaheadTime;
    if (avi.leavingTime < arrivalTime) {
        // ego wants to be follower
        if (sameTargetLane && (arrivalTime - avi.leavingTime < lookAhead
                               || unsafeMergeSpeeds(avi.leaveSpeed, arrivalSpeed,
                                       veh->getVehicleType().getCarFollowModel().getMaxDecel(), decel))) {
            return true;
        }
    } else if (foeArrivalTime > leaveTime) {
        // ego wants to be leader.
        if (sameTargetLane && (foeArrivalTime - leaveTime < lookAhead
                               || unsafeMergeSpeeds(leaveSpeed, avi.arrivalSpeedBraking,
                                       decel, veh->getVehicleType().getCarFollowModel().getMaxDecel()))) {
            return true;
        }
    } else {
        // even without considering safeHeadwayTime there is already a conflict
        return true;
    }
    return false;
}


bool
MSLink::maybeOccupied(MSLane* lane) {
    MSVehicle* veh = lane->getLastAnyVehicle();
    SUMOReal distLeft = 0;
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
MSLink::hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed, SUMOReal decel) const {
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
#ifdef HAVE_INTERNAL_LANES
    if (myInternalLane == 0 || myAmCont) {
        return false;
    } else {
        MSLane* pred = myInternalLane->getLogicalPredecessorLane();
        if (pred->getEdge().getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL) {
            return false;
        } else {
            MSLane* pred2 = pred->getLogicalPredecessorLane();
            assert(pred2 != 0);
            MSLink* predLink = MSLinkContHelper::getConnectingLink(*pred2, *pred);
            assert(predLink != 0);
            return predLink->havePriority();
        }
    }
#else
    return false;
#endif
}


void
MSLink::writeApproaching(OutputDevice& od, const std::string fromLaneID) const {
    if (myApproachingVehicles.size() > 0) {
        od.openTag("link");
        od.writeAttr(SUMO_ATTR_FROM, fromLaneID);
#ifdef HAVE_INTERNAL_LANES
        const std::string via = getViaLane() == 0 ? "" : getViaLane()->getID();
#else
        const std::string via = "";
#endif
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


SUMOReal
MSLink::getInternalLengthsAfter() const {
    SUMOReal len = 0.;
#ifdef HAVE_INTERNAL_LANES
    MSLane* lane = myInternalLane;

    while (lane != 0 && lane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        len += lane->getLength();
        lane = lane->getLinkCont()[0]->getViaLane();
    }
#endif
    return len;
}


#ifdef HAVE_INTERNAL_LANES
MSLane*
MSLink::getViaLane() const {
    return myInternalLane;
}


bool
MSLink::isExitLink() const {
    /// XXX this only works in networks with internal lanes
    return getInternalLaneBefore() != 0 && myLane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_NORMAL;
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
MSLink::getLeaderInfo(const MSVehicle* ego, SUMOReal dist, std::vector<const MSPerson*>* collectBlockers) const {
    LinkLeaders result;
    if (ego->getLaneChangeModel().isOpposite()) {
        // ignore link leaders
        return result;
    }
    //gDebugFlag1 = true;
    // this link needs to start at an internal lane (either an exit link or between two internal lanes)
    if (fromInternalLane()) {
        //if (gDebugFlag1) std::cout << SIMTIME << " getLeaderInfo link=" << getViaLaneOrLane()->getID() << "\n";
        // this is an exit link
        for (int i = 0; i < (int)myFoeLanes.size(); ++i) {
            const MSLane* foeLane = myFoeLanes[i];
            // distance from the querying vehicle to the crossing point with foeLane
            const SUMOReal distToCrossing = dist - myLengthsBehindCrossing[i].first;
            const bool sameTarget = (myLane == foeLane->getLinkCont()[0]->getLane());
            const bool sameSource = (myInternalLaneBefore != 0 && myInternalLaneBefore->getLogicalPredecessorLane() == foeLane->getLogicalPredecessorLane());
            const SUMOReal crossingWidth = (sameTarget || sameSource) ? 0 : foeLane->getWidth();
            const SUMOReal foeCrossingWidth = (sameTarget || sameSource) ? 0 : myInternalLaneBefore->getWidth();
            if (gDebugFlag1) {
                std::cout << " distToCrossing=" << distToCrossing << " foeLane=" << foeLane->getID() << "\n";
            }
            if (distToCrossing + crossingWidth < 0) {
                continue; // vehicle is behind the crossing point, continue with next foe lane
            }
            const SUMOReal foeDistToCrossing = foeLane->getLength() - myLengthsBehindCrossing[i].second;
            // it is not sufficient to return the last vehicle on the foeLane because ego might be its leader
            // therefore we return all vehicles on the lane
            //
            // special care must be taken for continuation lanes. (next lane is also internal)
            // vehicles on these lanes should always block (gap = -1)
            const bool contLane = (foeLane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL);
            // vehicles on cont. lanes or on internal lanes with the same target as this link can never be ignored
            const bool cannotIgnore = contLane || sameTarget || sameSource;
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
                if (!cannotIgnore && !foeLane->getLinkCont()[0]->getApproaching(leader).willPass && leader->isFrontOnLane(foeLane) && !isOpposite) {
                    continue;
                }
                if (cannotIgnore || leader->getWaitingTime() < MSGlobals::gIgnoreJunctionBlocker) {
                    // compute distance between vehicles on the the superimposition of both lanes
                    // where the crossing point is the common point
                    SUMOReal gap;
                    if ((contLane && !sameSource) || isOpposite) {
                        gap = -1; // always break for vehicles which are on a continuation lane or for opposite-direction vehicles
                    } else {
                        const SUMOReal leaderBack = leader->getBackPositionOnLane(foeLane);
                        const SUMOReal leaderBackDist = foeDistToCrossing - leaderBack;
                        if (gDebugFlag1) {
                            std::cout << " distToCrossing=" << distToCrossing << " leader back=" << leaderBack << " backDist=" << leaderBackDist << "\n";
                        }
                        if (leaderBackDist + foeCrossingWidth < 0) {
                            // leader is completely past the crossing point
                            // or there is no crossing point
                            continue; // next vehicle
                        }
                        gap = distToCrossing - leaderBackDist - (sameTarget ? ego->getVehicleType().getMinGap() : 0);
                    }
                    if (gDebugFlag1) {
                        std::cout << " leader=" << leader->getID() << " contLane=" << contLane << " cannotIgnore=" << cannotIgnore << "\n";
                    }
                    // if the foe is already moving off the intersection, we may
                    // advance up to the crossing point unless we have the same target
                    const bool stopAsap = leader->isFrontOnLane(foeLane) ? cannotIgnore : sameTarget;
                    result.push_back(LinkLeader(leader, gap, stopAsap ? -1 : distToCrossing));
                }

            }
            // check for crossing pedestrians (keep driving if already on top of the crossing
            const SUMOReal distToPeds = distToCrossing - MSPModel::SAFETY_GAP;
            if (distToPeds >= -MSPModel::SAFETY_GAP && MSPModel::getModel()->blockedAtDist(foeLane, foeDistToCrossing, collectBlockers)) {
                result.push_back(LinkLeader((MSVehicle*)0, -1, distToPeds));
            }
        }
    }
    return result;
}
#endif


MSLane*
MSLink::getViaLaneOrLane() const {
#ifdef HAVE_INTERNAL_LANES
    if (myInternalLane != 0) {
        return myInternalLane;
    }
#endif
    return myLane;
}


const MSLane*
MSLink::getLaneBefore() const {
#ifdef HAVE_INTERNAL_LANES
    // XXX this branch is superfluous
    if (myInternalLaneBefore != 0) {
        if (myLaneBefore != myInternalLaneBefore) {
            throw ProcessError("lane before mismatch!");
        }
    }
#endif
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
    MSLane* after = getViaLaneOrLane()->getParallelLane(direction);
    if (before != 0 && after != 0) {
        return MSLinkContHelper::getConnectingLink(*before, *after);
    } else {
        return 0;
    }
}

void
MSLink::passedJunction(const MSVehicle* vehicle) {
    if (myJunction != 0) {
        myJunction->passedJunction(vehicle);
    }
}


bool
MSLink::isLeader(const MSVehicle* ego, const MSVehicle* foe) {
    if (myJunction != 0) {
        return myJunction->isLeader(ego, foe);
    } else {
        // unregulated junction
        return false;
    }
}

const MSLane*
MSLink::getInternalLaneBefore() const {
#ifdef HAVE_INTERNAL_LANES
    return myInternalLaneBefore;
#else
    return 0;
#endif
}


SUMOReal
MSLink::getZipperSpeed(const MSVehicle* ego, const SUMOReal dist, SUMOReal vSafe,
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
    const SUMOReal secondsToArrival = STEPS2TIME(arrivalTime - now);
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
    const SUMOReal vSafeOrig = vSafe;
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
        const SUMOReal gap = dist - foe->getVehicleType().getLength() - ego->getVehicleType().getMinGap() - avi.dist;
        const SUMOReal follow = ego->getCarFollowModel().followSpeed(
                                    ego, ego->getSpeed(), gap, foe->getSpeed(), foe->getCarFollowModel().getMaxDecel());
        // speed adaption to follow the foe can be spread over secondsToArrival
        const SUMOReal followInTime = vSafeOrig + (follow - vSafeOrig) / MAX2((SUMOReal)1, secondsToArrival / TS);
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
MSLink::couldBrakeForLeader(SUMOReal followDist, SUMOReal leaderDist, const MSVehicle* follow, const MSVehicle* leader) {
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

