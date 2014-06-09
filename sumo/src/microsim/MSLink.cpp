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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSLink.h"
#include "MSLane.h"
#include "MSPerson.h"
#include "MSEdge.h"
#include "MSGlobals.h"
#include "MSVehicle.h"
#include "MSPModel.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define MSLink_DEBUG_CROSSING_POINTS

// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);


// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane, LinkDirection dir, LinkState state, SUMOReal length) :
    myLane(succLane),
    myIndex(-1),
    myState(state),
    myDirection(dir),
    myLength(length),
    myHasFoes(false),
    myAmCont(false)
#else
MSLink::MSLink(MSLane* succLane, MSLane* via, LinkDirection dir, LinkState state, SUMOReal length) :
    myLane(succLane),
    myIndex(-1),
    myState(state),
    myDirection(dir),
    myLength(length),
    myHasFoes(false),
    myAmCont(false),
    myJunctionInlane(via)
#endif
{}


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
    myFoeLanes = foeLanes;
#ifdef MSLink_DEBUG_CROSSING_POINTS
    std::cout << " link " << myIndex << " to " << getViaLaneOrLane()->getID() << " has foes: " << toString(foeLanes) << "\n";
#endif
#ifdef HAVE_INTERNAL_LANES
    MSLane* lane = 0;
    if (internalLaneBefore != 0) {
        // this is an exit link. compute crossing points with all foeLanes
        lane = internalLaneBefore;
        //} else if (myLane->getEdge().isCrossing()) {
        //    // this is the link to a pedestrian crossing. compute crossing points with all foeLanes
        //    // @note not currently used by pedestrians
        //    lane = myLane;
    }
    if (lane != 0) {
        for (std::vector<MSLane*>::const_iterator it_lane = myFoeLanes.begin(); it_lane != myFoeLanes.end(); ++it_lane) {
            if (myLane == (*it_lane)->getLinkCont()[0]->getLane() && !lane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().isInternal()) {
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
                //std::cout << " number of intersections1=" << intersections1.size() << "\n";
                if (intersections1.size() == 0) {
                    intersections1.push_back(10000.0); // disregard this foe (using maxdouble leads to nasty problems down the line)
                } else if (intersections1.size() > 1) {
                    std::sort(intersections1.begin(), intersections1.end());
                }
                std::vector<SUMOReal> intersections2 = (*it_lane)->getShape().intersectsAtLengths2D(lane->getShape());
                //std::cout << " number of intersections2=" << intersections2.size() << "\n";
                if (intersections2.size() == 0) {
                    intersections2.push_back(0);
                } else if (intersections2.size() > 1) {
                    std::sort(intersections2.begin(), intersections2.end());
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
    }
#else
    UNUSED_PARAMETER(internalLaneBefore);
#endif
}


void
MSLink::setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime, const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed,
                       const bool setRequest, const SUMOTime arrivalTimeBraking, const SUMOReal arrivalSpeedBraking, const SUMOTime waitingTime) {
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, approaching->getVehicleType().getLength());
    myApproachingVehicles.insert(std::make_pair(approaching,
                                 ApproachingVehicleInformation(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, setRequest,
                                         arrivalTimeBraking, arrivalSpeedBraking, waitingTime)));
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
        return ApproachingVehicleInformation(-1000, -1000, 0, 0, false, -1000, 0, 0);
    }
}


SUMOTime
MSLink::getLeaveTime(const SUMOTime arrivalTime, const SUMOReal arrivalSpeed,
                     const SUMOReal leaveSpeed, const SUMOReal vehicleLength) const {
    return arrivalTime + TIME2STEPS((getLength() + vehicleLength) / MAX2((SUMOReal)0.5 * (arrivalSpeed + leaveSpeed), NUMERICAL_EPS));
}


bool
MSLink::opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength,
               SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
               std::vector<const SUMOVehicle*>* collectFoes) const {
    if (myState == LINKSTATE_TL_RED) {
        return false;
    }
    if (myAmCont && MSGlobals::gUsingInternalLanes) {
        return true;
    }
    if ((myState == LINKSTATE_STOP || myState == LINKSTATE_ALLWAY_STOP) && waitingTime == 0) {
        return false;
    }
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, vehicleLength);
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->getState() == LINKSTATE_TL_RED) {
                continue;
            }
        }
#endif
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, myLane == (*i)->getLane(),
                                impatience, decel, waitingTime, collectFoes)) {
            return false;
        }
    }
    return true;
}


bool
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                      bool sameTargetLane, SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
                      std::vector<const SUMOVehicle*>* collectFoes) const {
    for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
        if (!i->second.willPass) {
            continue;
        }
        if (myState == LINKSTATE_ALLWAY_STOP) {
            assert(waitingTime > 0);
            if (waitingTime > i->second.waitingTime) {
                continue;
            }
            if (waitingTime == i->second.waitingTime && arrivalTime < i->second.arrivalTime) {
                continue;
            }
        }
        const SUMOTime foeArrivalTime = (SUMOTime)((1.0 - impatience) * i->second.arrivalTime + impatience * i->second.arrivalTimeBraking);
        if (i->second.leavingTime < arrivalTime) {
            // ego wants to be follower
            if (sameTargetLane && (arrivalTime - i->second.leavingTime < myLookaheadTime
                                   || unsafeMergeSpeeds(i->second.leaveSpeed, arrivalSpeed,
                                           i->first->getVehicleType().getCarFollowModel().getMaxDecel(), decel))) {
                if (collectFoes == 0) {
                    return true;
                } else {
                    collectFoes->push_back(i->first);
                }
            }
        } else if (foeArrivalTime > leaveTime) {
            // ego wants to be leader.
            if (sameTargetLane && (foeArrivalTime - leaveTime < myLookaheadTime
                                   || unsafeMergeSpeeds(leaveSpeed, i->second.arrivalSpeedBraking,
                                           decel, i->first->getVehicleType().getCarFollowModel().getMaxDecel()))) {
                if (collectFoes == 0) {
                    return true;
                } else {
                    collectFoes->push_back(i->first);
                }
            }
        } else {
            // even without considering safeHeadwayTime there is already a conflict
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
MSLink::maybeOccupied(MSLane* lane) {
    MSVehicle* veh = lane->getLastVehicle();
    SUMOReal distLeft = 0;
    if (veh == 0) {
        veh = lane->getPartialOccupator();
        distLeft = lane->getLength() - lane->getPartialOccupatorEnd();
    } else {
        distLeft = lane->getLength() - veh->getPositionOnLane() + veh->getVehicleType().getLength();
    }
    if (veh == 0) {
        return false;
    } else {
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
    for (std::vector<MSLane*>::const_iterator i = myFoeLanes.begin(); i != myFoeLanes.end(); ++i) {
        if ((*i)->getVehicleNumber() > 0 || (*i)->getPartialOccupator() != 0) {
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
MSLink::setTLState(LinkState state, SUMOTime /*t*/) {
    myState = state;
}


MSLane*
MSLink::getLane() const {
    return myLane;
}


bool
MSLink::lastWasContMajor() const {
#ifdef HAVE_INTERNAL_LANES
    if (myJunctionInlane == 0 || myAmCont) {
        return false;
    } else {
        MSLane* pred = myJunctionInlane->getLogicalPredecessorLane();
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


#ifdef HAVE_INTERNAL_LANES
MSLane*
MSLink::getViaLane() const {
    return myJunctionInlane;
}


MSLink::LinkLeaders
MSLink::getLeaderInfo(SUMOReal dist, SUMOReal minGap, std::vector<const MSPerson*>* collectBlockers) const {
    LinkLeaders result;
    // this link needs to start at an internal lane (either an exit link or between two internal lanes)
    if (MSGlobals::gUsingInternalLanes && (
                (myJunctionInlane == 0 && getLane()->getEdge().getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL)
                || (myJunctionInlane != 0 && myJunctionInlane->getLogicalPredecessorLane()->getEdge().isInternal()))) {
        //std::cout << " getLeaderInfo link=" << getViaLaneOrLane()->getID() << "\n";
        // this is an exit link
        for (size_t i = 0; i < myFoeLanes.size(); ++i) {
            MSLane* foeLane = myFoeLanes[i];
            // distance from the querying vehicle to the crossing point with foeLane
            const SUMOReal distToCrossing = dist - myLengthsBehindCrossing[i].first;
            //std::cout << " distToCrossing=" << distToCrossing << " foeLane=" << foeLane->getID() << "\n";
            if (distToCrossing < 0) {
                continue; // vehicle is behind the crossing point, continue with next foe lane
            }
            const SUMOReal foeDistToCrossing = foeLane->getLength() - myLengthsBehindCrossing[i].second;
            // it is not sufficient to return the last vehicle on the foeLane because ego might be its leader
            // therefore we return all vehicles on the lane
            //
            // special care must be taken for continuation lanes. (next lane is also internal)
            // vehicles on these lanes should always block (gap = -1)
            const bool contLane = (foeLane->getLinkCont()[0]->getViaLaneOrLane()->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL);
            const bool sameTarget = (myLane == foeLane->getLinkCont()[0]->getLane());
            // vehicles on cont. lanes or on internal lanes with the same target as this link can never be ignored
            const bool cannotIgnore = contLane || sameTarget;
            const MSLane::VehCont& vehicles = foeLane->getVehiclesSecure();
            foeLane->releaseVehicles();
            for (MSLane::VehCont::const_iterator it_veh = vehicles.begin(); it_veh != vehicles.end(); ++it_veh) {
                MSVehicle* leader = *it_veh;
                if (cannotIgnore || leader->getWaitingTime() < MSGlobals::gIgnoreJunctionBlocker) {
                    // compute distance between vehicles on the the superimposition of both lanes
                    // where the crossing point is the common point
                    SUMOReal gap;
                    if (contLane) {
                        gap = -1; // always break for vehicles which are on a continuation lane
                    } else {
                        const SUMOReal leaderBack = leader->getPositionOnLane() - leader->getVehicleType().getLength();
                        const SUMOReal leaderBackDist = foeDistToCrossing - leaderBack;
                        //std::cout << " distToCrossing=" << distToCrossing << " leader backDist=" << leaderBackDist << "\n";
                        if (leaderBackDist < 0) {
                            // leader is completely past the crossing point
                            // or there is no crossing point
                            continue; // next vehicle
                        }
                        gap = distToCrossing - leaderBackDist - (sameTarget ? minGap : 0);
                    }
                    result.push_back(LinkLeader(leader, gap, cannotIgnore ? -1 : distToCrossing));
                }

            }
            MSVehicle* leader = foeLane->getPartialOccupator();
            if (leader != 0) {
                if (cannotIgnore || leader->getWaitingTime() < MSGlobals::gIgnoreJunctionBlocker) {
                    // compute distance between vehicles on the the superimposition of both lanes
                    // where the crossing point is the common point
                    SUMOReal gap;
                    if (contLane) {
                        gap = -1; // always break for vehicles which are on a continuation lane
                    } else {
                        const SUMOReal leaderBackDist = foeDistToCrossing - foeLane->getPartialOccupatorEnd();
                        //std::cout << " distToCrossing=" << distToCrossing << " leader (partialOccupator) backDist=" << leaderBackDist << "\n";
                        if (leaderBackDist < 0) {
                            // leader is completely past the crossing point
                            // or there is no crossing point
                            continue; // next lane
                        }
                        gap = distToCrossing - leaderBackDist - (sameTarget ? minGap : 0);
                    }
                    result.push_back(LinkLeader(leader, gap, sameTarget ? -1 : distToCrossing));
                }
            }
            // check for crossing pedestrians (keep driving if already on top of the crossing
            const SUMOReal distToPeds = distToCrossing - MSPModel::SAFETY_GAP - foeLane->getWidth() * 0.5;
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
    if (myJunctionInlane != 0) {
        return myJunctionInlane;
    }
#endif
    return myLane;
}


/****************************************************************************/

