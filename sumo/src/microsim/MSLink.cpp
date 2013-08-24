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
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir),  myLength(length) {}
#else
MSLink::MSLink(MSLane* succLane, MSLane* via,
               LinkDirection dir, LinkState state, SUMOReal length)
    :
    myLane(succLane),
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir), myLength(length),
    myJunctionInlane(via) {}
#endif


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(unsigned int requestIdx, unsigned int respondIdx, bool isCrossing, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes) {
    myRequestIdx = requestIdx;
    myRespondIdx = respondIdx;
    myIsCrossing = isCrossing;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    myFoeLanes = foeLanes;
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
            od.writeAttr("arrivalTime", time2string(avi.arrivalTime));
            od.writeAttr("leaveTime", time2string(avi.leavingTime));
            od.writeAttr("arrivalSpeed", toString(avi.arrivalSpeed));
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
MSLink::getLeaderInfo(SUMOReal dist) const {
    LinkLeaders result;
    if (MSGlobals::gUsingInternalLanes && myJunctionInlane == 0) {
        // this is an exit link

        for (std::vector<MSLane*>::const_iterator it_lane = myFoeLanes.begin(); it_lane != myFoeLanes.end(); ++it_lane) {
            // it is not sufficient to return the last vehicle on the foeLane because ego might be its leader
            // therefore we return all vehicles on the lane
            //
            // special care must be taken for continuation lanes. (next lane is also internal)
            // vehicles on these lanes should always block (gap = -1)
            const bool contLane = ((*it_lane)->getLinkCont()[0]->getViaLaneOrLane()->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL);
            const MSLane::VehCont& vehicles = (*it_lane)->getVehiclesSecure();
            (*it_lane)->releaseVehicles();
            for (MSLane::VehCont::const_iterator it_veh = vehicles.begin(); it_veh != vehicles.end(); ++it_veh) {
                MSVehicle* leader = *it_veh;
                // XXX apply viaLane/foeLane specific distance offset
                // to account for the fact that the crossing point has different distances from the lane ends
                result.push_back(std::make_pair(leader,
                                                contLane ? -1 :
                                                dist - ((*it_lane)->getLength() - leader->getPositionOnLane()) - leader->getVehicleType().getLength()));

            }
            // XXX partial occupates should be ignored if they do not extend past the crossing point
            MSVehicle* leader = (*it_lane)->getPartialOccupator();
            if (leader != 0) {
                result.push_back(std::make_pair(leader,
                                                contLane ? -1 :
                                                dist - ((*it_lane)->getLength() - (*it_lane)->getPartialOccupatorEnd())));
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


unsigned int
MSLink::getRespondIndex() const {
    return myRespondIdx;
}



/****************************************************************************/

