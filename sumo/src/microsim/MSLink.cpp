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
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/iodevices/OutputDevice.h>
#include "MSNet.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSGlobals.h"
#include "MSVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);


// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane,
               LinkDirection dir, LinkState state,
               SUMOReal length)
    :
    myLane(succLane),
    myIndex(0),
    myState(state), myDirection(dir),  myLength(length) {}
#else
MSLink::MSLink(MSLane* succLane, MSLane* via,
               LinkDirection dir, LinkState state, SUMOReal length)
    :
    myLane(succLane),
    myIndex(0),
    myState(state), myDirection(dir), myLength(length),
    myJunctionInlane(via)
{}
#endif


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(unsigned int index, bool isCrossing, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes,
                              MSLane* internalLaneBefore) {
    myIndex = index;
    myIsCrossing = isCrossing;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    myFoeLanes = foeLanes;
#ifdef HAVE_INTERNAL_LANES
    if (internalLaneBefore != 0) {
        // this is an exit link. compute crossing points with all foeLanes
        for (std::vector<MSLane*>::const_iterator it_lane = myFoeLanes.begin(); it_lane != myFoeLanes.end(); ++it_lane) {
            if (myLane == (*it_lane)->getLinkCont()[0]->getLane()) {
                // this foeLane has the same target
                myLengthsBehindCrossing.push_back(std::make_pair(0, 0)); // dummy value, never used
            } else {
                std::vector<SUMOReal> intersections1 = internalLaneBefore->getShape().intersectsAtLengths2D((*it_lane)->getShape());
                //std::cout << " number of intersections1=" << intersections1.size() << "\n";
                if (intersections1.size() == 0) {
                    intersections1.push_back(0);
                } else if (intersections1.size() > 1) {
                    std::sort(intersections1.begin(), intersections1.end());
                }
                std::vector<SUMOReal> intersections2 = (*it_lane)->getShape().intersectsAtLengths2D(internalLaneBefore->getShape());
                //std::cout << " number of intersections2=" << intersections2.size() << "\n";
                if (intersections2.size() == 0) {
                    intersections2.push_back(0);
                } else if (intersections2.size() > 1) {
                    std::sort(intersections2.begin(), intersections2.end());
                }
                myLengthsBehindCrossing.push_back(std::make_pair(
                                                      internalLaneBefore->getLength() - intersections1.back(),
                                                      (*it_lane)->getLength() - intersections2.back()));
                //std::cout
                //    << " intersection of " << internalLaneBefore->getID()
                //    << " totalLength=" << internalLaneBefore->getLength()
                //    << " with " << (*it_lane)->getID()
                //    << " totalLength=" << (*it_lane)->getLength()
                //    << " dist1=" << myLengthsBehindCrossing.back().first
                //    << " dist2=" << myLengthsBehindCrossing.back().second
                //    << "\n";
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
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, approaching->getVehicleType().getLengthWithGap());
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
MSLink::getLeaveTime(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const {
    return arrivalTime + TIME2STEPS((getLength() + vehicleLength) / (0.5 * (arrivalSpeed + leaveSpeed)));
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
MSLink::getLeaderInfo(SUMOReal dist, SUMOReal minGap) const {
    LinkLeaders result;
    if (MSGlobals::gUsingInternalLanes && myJunctionInlane == 0 &&
            getLane()->getEdge().getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL) {
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
                            assert(!sameTarget);
                            continue; // next vehicle
                        }
                        gap = distToCrossing - leaderBackDist - (sameTarget ? minGap : 0);
                    }
                    result.push_back(std::make_pair(std::make_pair(leader, gap), sameTarget ? -1 : distToCrossing));
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
                            assert(!sameTarget);
                            continue; // next lane
                        }
                        gap = distToCrossing - leaderBackDist - (sameTarget ? minGap : 0);
                    }
                    result.push_back(std::make_pair(std::make_pair(leader, gap), sameTarget ? -1 : distToCrossing));
                }
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

