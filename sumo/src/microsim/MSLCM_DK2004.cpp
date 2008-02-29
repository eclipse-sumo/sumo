/****************************************************************************/
/// @file    MSLCM_DK2004.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id:MSLCM_DK2004.cpp 4683 2007-11-09 12:54:04Z dkrajzew $
///
//  missingDescription
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSLCM_DK2004.h"
#include <utils/common/RandHelper.h>
#include <iostream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// variable definitions
// ===========================================================================
// 80km/h will be the swell for dividing between long/short foresight
#define LOOK_FORWARD_SPEED_DIVIDER 14.

#define LOOK_FORWARD_FAR  15.
#define LOOK_FORWARD_NEAR 5.



#define JAM_FACTOR 2.
#define JAM_FACTOR2 1.


// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_DK2004::MSLCM_DK2004(MSVehicle &v)
        : MSAbstractLaneChangeModel(v),
        myChangeProbability(0),
        myVSafe(0), myBlockingLeader(0), myBlockingFollower(0),
        myUrgency(0)
{}

MSLCM_DK2004::~MSLCM_DK2004()
{
    changed();
}


int
MSLCM_DK2004::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                 int blocked,
                                 const std::pair<MSVehicle*, SUMOReal> &leader,
                                 const std::pair<MSVehicle*, SUMOReal> &neighLead,
                                 const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                                 const MSLane &neighLane,
                                 const std::vector<MSVehicle::LaneQ> &preb,
                                 /*
                                 int bestLaneOffset, SUMOReal bestDist,
                                 SUMOReal neighDist,
                                 SUMOReal currentDist,
                                 */
                                 MSVehicle **lastBlocked)
{
    MSVehicle::LaneQ curr, best;
    int bestLaneOffset = 0;
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    SUMOReal neighExtDist = 0;
    SUMOReal currExtDist = 0;
    int currIdx = 0;
    for (int p=0; p<(int) preb.size(); ++p) {
        if (preb[p].lane==&myVehicle.getLane()) {
            curr = preb[p];
            bestLaneOffset = curr.dir;
            currentDist = curr.length;
            currExtDist = curr.lane->length();//alllength;
            neighDist = preb[p-1].length;
            neighExtDist = preb[p-1].lane->length();//.alllength;
            best = preb[p+bestLaneOffset];
            currIdx = p;
        }
    }

    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    if (leader.first!=0
            &&
            (myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0
            &&
            (leader.first->getLaneChangeModel().getState()&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {

        myState &= (0xffffffff-LCA_AMBLOCKINGFOLLOWER_DONTBRAKE);
        if (myVehicle.getSpeed()>0.1) {
            myState |= LCA_AMBACKBLOCKER;
        } else {
            ret |= LCA_AMBACKBLOCKER;
            myVSafe = 0;
        }
    }

    // process information about the last blocked vehicle
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*lastBlocked)!=0) {
        SUMOReal gap = (*lastBlocked)->getPositionOnLane()-(*lastBlocked)->getLength()-myVehicle.getPositionOnLane();
        if (gap>0.1) {
            if (myVehicle.getSpeed()<myVehicle.decelAbility()) {
                if ((*lastBlocked)->getSpeed()<0.1) {
                    ret |= LCA_AMBACKBLOCKER_STANDING;
                    myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), (SUMOReal)(gap-0.1), (*lastBlocked)->getSpeed());
                    (*lastBlocked) = 0;
                } else {
                    ret |= LCA_AMBACKBLOCKER;
                    myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), (SUMOReal)(gap-0.1), (*lastBlocked)->getSpeed());
                    (*lastBlocked) = 0;
                }
            }
        }
    }

    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon
    SUMOReal rv = myVehicle.getSpeed() > LOOK_FORWARD_SPEED_DIVIDER
                  ? myVehicle.getSpeed() * (SUMOReal) LOOK_FORWARD_FAR
                  : myVehicle.getSpeed() * (SUMOReal) LOOK_FORWARD_NEAR;
    rv += myVehicle.getLength() * (SUMOReal) 2.;

    SUMOReal tdist = currentDist/*best.lane->length()*/-myVehicle.getPositionOnLane() - best.hindernisPos * (SUMOReal) JAM_FACTOR2;
    /*
    if(bestLaneOffset<0) {
        myChangeProbability += 1. / (tdist / rv);
    }
    */
    if (fabs(best.length-curr.length)>MIN2((SUMOReal) .1, best.lane->length()) && bestLaneOffset<0&&currentDistDisallows(tdist/*currentDist*/, bestLaneOffset, rv)) {
        informBlocker(msgPass, blocked, LCA_MRIGHT, neighLead, neighFollow);
        if (neighLead.second>0&&neighLead.second>leader.second) {
            myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed())
                      - (SUMOReal) 0.5;
        }
        myBlockingLeader = neighLead.first;
        myBlockingFollower = neighFollow.first;
        myUrgency = tdist;
        return ret|LCA_RIGHT|LCA_URGENT|blocked;
    }


    // the opposite lane-changing direction should be done than the one examined herein
    //  we'll check whether we assume we could change anyhow and get back in time...
    //
    // this rule prevents the vehicle from moving in opposite direction of the best lane
    //  unless the way till the end where the vehicle has to be on the best lane
    //  is long enough
    SUMOReal maxJam = MAX2(preb[currIdx-1].hindernisPos, preb[currIdx].hindernisPos);
    SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist-myVehicle.getPositionOnLane()-maxJam);
    if (bestLaneOffset>=0&&(currentDistDisallows(neighLeftPlace, bestLaneOffset+2, rv))) {
        // ...we will not change the lane if not
        return ret;
    }


    // if the current lane is the best and a lane-changing would cause a situation
    //  of which we assume we will not be able to return to the lane we have to be on...
    //
    // this rule prevents the vehicle from leaving the current, best lane when it is
    //  close to this lane's end
    if (currExtDist>neighExtDist&&(neighLeftPlace*2.<rv/*||currE[currIdx+1].length<currentDist*/)) {
        return ret;
    }

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    //
    // THIS RULE APPLIES ONLY TO CHANGING TO THE RIGHT LANE
    if (bestLaneOffset==0&&preb[currIdx-1].dir!=0&&myVehicle.getLane().maxSpeed()>80./3.6) {
        return ret;
    }

    /*
        if(bestLaneOffset==0&&(neighDist==0||curr.hindernisPos*JAM_FACTOR>=neighExtDist-curr.length)) {
            return ret;
        }
        */
    // --------

    // -------- make place on current lane if blocking follower
    if (amBlockingFollowerPlusNB()
            &&
            (currentDistAllows(neighDist, bestLaneOffset, rv)||neighDist>=currentDist)) {

        return ret|LCA_RIGHT|LCA_URGENT|LCA_KEEP1;
    }
    // --------


    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if ((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }
    // --------

    // -------- higher speed
    if ((congested(neighLead.first) && neighLead.second<20)||predInteraction(leader.first)) { //!!!
        return ret;
    }
    SUMOReal neighLaneVSafe, thisLaneVSafe;
    if (neighLead.first == 0) {
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), neighLane.length() - myVehicle.getPositionOnLane(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed());
    }
    if (leader.first==0) {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), leader.second, leader.first->getSpeed());
    }

    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().getMaxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().getMaxSpeed());
    if (thisLaneVSafe-neighLaneVSafe>5./3.6) {
        // ok, the current lane is faster than the right one...
        if (myChangeProbability<0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // ok, the right lane is faster than the current
        myChangeProbability -= (SUMOReal)
                               ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }

    // let's recheck the "Rechtsfahrgebot"
    SUMOReal vmax = MIN2(myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().getMaxSpeed());
    vmax -= (SUMOReal)(5./2.6);
    if (neighLaneVSafe>=vmax) {
#ifdef TRACI
        /* if there was a request by TraCI for changing to this lane 
        and holding it, this rule is ignored */
        if (myChangeRequest != REQUEST_HOLD) {
#endif
        myChangeProbability -= (SUMOReal)((neighLaneVSafe-vmax) / (vmax));
#ifdef TRACI
        }
#endif
    }

    if (myChangeProbability<-2&&neighDist/MAX2((SUMOReal) .1, myVehicle.getSpeed())>20.) {//./MAX2((SUMOReal) .1, myVehicle.getSpeed())) { // -.1
        return ret | LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // --------

	// If there is a request by TraCI, try to change the lane
	if (myChangeRequest == REQUEST_RIGHT) {
		return ret | LCA_RIGHT;
	}
    return ret;
}


int
MSLCM_DK2004::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                int blocked,
                                const std::pair<MSVehicle*, SUMOReal> &leader,
                                const std::pair<MSVehicle*, SUMOReal> &neighLead,
                                const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                                const MSLane &neighLane,
                                const std::vector<MSVehicle::LaneQ> &preb,
                                /*
                                int bestLaneOffset, SUMOReal bestDist,
                                SUMOReal neighDist,
                                SUMOReal currentDist,
                                */
                                MSVehicle **lastBlocked)
{
    MSVehicle::LaneQ curr, best;
    int bestLaneOffset = 0;
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    SUMOReal neighExtDist = 0;
    SUMOReal currExtDist = 0;
    int currIdx = 0;
    for (int p=0; p<(int) preb.size(); ++p) {
        if (preb[p].lane==&myVehicle.getLane()) {
            curr = preb[p];
            bestLaneOffset = curr.dir;
            currentDist = curr.length;
            currExtDist = curr.lane->length();//.alllength;
            neighDist = preb[p+1].length;
            neighExtDist = preb[p+1].lane->length();//.alllength;
            best = preb[p+bestLaneOffset];
            currIdx = p;
        }
    }
    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    // ?!!!
    if (leader.first!=0
            &&
            (myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0
            &&
            (leader.first->getLaneChangeModel().getState()&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {

        myState &= (0xffffffff-LCA_AMBLOCKINGFOLLOWER_DONTBRAKE);
        if (myVehicle.getSpeed()>0.1) {
            myState |= LCA_AMBACKBLOCKER;
        } else {
            ret |= LCA_AMBACKBLOCKER;
            myVSafe = 0;
        }
    }

    // process information about the last blocked vehicle
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*lastBlocked)!=0) {
        SUMOReal gap = (*lastBlocked)->getPositionOnLane()-(*lastBlocked)->getLength()-myVehicle.getPositionOnLane();
        if (gap>0.1) {
            if (myVehicle.getSpeed()<myVehicle.decelAbility()) {
                if ((*lastBlocked)->getSpeed()<0.1) {
                    ret |= LCA_AMBACKBLOCKER_STANDING;
                    myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), (SUMOReal)(gap-0.1), (*lastBlocked)->getSpeed());
                    (*lastBlocked) = 0;
                } else {
                    ret |= LCA_AMBACKBLOCKER;
                    myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), (SUMOReal)(gap-0.1), (*lastBlocked)->getSpeed());
                    (*lastBlocked) = 0;
                }
            }
        }
    }

    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon
    SUMOReal lv = myVehicle.getSpeed() > LOOK_FORWARD_SPEED_DIVIDER
                  ? myVehicle.getSpeed() * (SUMOReal) LOOK_FORWARD_FAR
                  : myVehicle.getSpeed() * (SUMOReal) LOOK_FORWARD_NEAR;
    lv += myVehicle.getLength() * (SUMOReal) 2.;


    SUMOReal tdist = currentDist/*best.lane->length()*/-myVehicle.getPositionOnLane() - best.hindernisPos * (SUMOReal) JAM_FACTOR2;
    /*
    if(bestLaneOffset>0) {
        myChangeProbability -= 1. / (tdist / lv);
    }
    */
    if (fabs(best.length-curr.length)>MIN2((SUMOReal) .1, best.lane->length()) && bestLaneOffset>0
            &&
            currentDistDisallows(tdist/*currentDist*/, bestLaneOffset, lv)) {
        informBlocker(msgPass, blocked, LCA_MLEFT, neighLead, neighFollow);
        if (neighLead.second>0&&neighLead.second>leader.second) {
            myVSafe = myVehicle.ffeV(myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed()) - (SUMOReal) 0.5;
        }
        myBlockingLeader = neighLead.first;
        myBlockingFollower = neighFollow.first;
        myUrgency = tdist;
        return ret|LCA_LEFT|LCA_URGENT|blocked;
    }

    // the opposite lane-changing direction should be rather done, not
    //  the one examined herein
    //  we'll check whether we assume we could change anyhow and get back in time...
    //
    // this rule prevents the vehicle from moving in opposite direction of the best lane
    //  unless the way till the end where the vehicle has to be on the best lane
    //  is long enough
    SUMOReal maxJam = MAX2(preb[currIdx+1].hindernisPos, preb[currIdx].hindernisPos);
    SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist-myVehicle.getPositionOnLane()-maxJam);
    if (bestLaneOffset<=0&&(currentDistDisallows(neighLeftPlace, bestLaneOffset-2, lv))) {
        // ...we will not change the lane if not
        return ret;
    }


    // if the current lane is the best and a lane-changing would cause a situation
    //  of which we assume we will not be able to return to the lane we have to be on...
    //
    // this rule prevents the vehicle from leaving the current, best lane when it is
    //  close to this lane's end
    if (currExtDist>neighExtDist&&(neighLeftPlace*2.<lv/*||currE[currIdx+1].length<currentDist*/)) {
        // ... let's not change the lane
        return ret;
    }

    /*
    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if(bestLaneOffset==0&&myVehicle.getLane().maxSpeed()>80./3.6) {
        return ret;
    }
    */


    /*
    // if the current lane is the
    if(bestLaneOffset==0&&(neighDist==0||curr.seenVehicles2*JAM_FACTOR>=neighExtDist-curr.length)) {
        return ret;
    }
    */
    // --------

    // -------- make place on current lane if blocking follower
    if (amBlockingFollowerPlusNB()
            &&
            (currentDistAllows(neighDist, bestLaneOffset, lv)||neighDist>=currentDist)) {

        return ret|LCA_LEFT|LCA_URGENT|LCA_KEEP1;
    }
    // --------

    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if ((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }

    // -------- higher speed
    /* !!! scheint nicht vernnftig zu funktionieren - Fahrzeuge bleiben auf der rechten Spur
       obwohl sie noch eine Zeitlang auf der linken fahren drften
    */
    if ((congested(neighLead.first) && neighLead.second<20)||predInteraction(leader.first)) { //!!!
//    if(congested( neighLead.first ) && neighLead.second<20) {//!!!
        return ret;
    }
    SUMOReal neighLaneVSafe, thisLaneVSafe;
    if (neighLead.first == 0) {
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), neighLane.length() - myVehicle.getPositionOnLane(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed());
    }
    if (leader.first==0) {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), leader.second, leader.first->getSpeed());
    }
    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().getMaxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().getMaxSpeed());
    if (thisLaneVSafe>neighLaneVSafe) {
        // this lane is better
        if (myChangeProbability>0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // right lane is better
        myChangeProbability += (SUMOReal)
                               ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }
    //if(myChangeProbability>2./MAX2((SUMOReal) .1, myVehicle.getSpeed())) { // .1
    if (myChangeProbability>.2&&neighDist/MAX2((SUMOReal) .1, myVehicle.getSpeed())>20.) { // .1
        return ret | LCA_LEFT|LCA_SPEEDGAIN|LCA_URGENT;
    }
    // --------

	// If there is a request by TraCI, try to change the lane
	if (myChangeRequest == REQUEST_LEFT) {
		return ret | LCA_LEFT;
	}

    return ret;
}


SUMOReal
MSLCM_DK2004::patchSpeed(SUMOReal min, SUMOReal wanted, SUMOReal max, SUMOReal /*vsafe*/)
{
    SUMOReal vSafe = myVSafe;
    int state = myState;
    myState = 0;
    myVSafe = -1;
    // just to make sure to be notified about lane chaning end
    if (myVehicle.getLane().getEdge()->nLanes()==1) {
        // remove chaning information if on a road with a single lane
        changed();
        return wanted;
    }

    // check whether the vehicle is blocked
    if ((state&LCA_URGENT)!=0) {
        if (vSafe>0) {
            return MIN2(max, MAX2(min, vSafe));
        }
        // check whether the vehicle maybe has to be swapped with one of
        //  the blocking vehicles
        if ((state&LCA_BLOCKEDBY_LEADER)!=0
                &&
                (state&LCA_BLOCKEDBY_FOLLOWER)!=0) {

            return (min+wanted)/(SUMOReal) 2.0;//wanted;
        } else {
            if ((state&LCA_BLOCKEDBY_LEADER)!=0) {
                // if interacting with leader and not too slow
                return (min+wanted)/(SUMOReal) 2.0;
            }
            if ((state&LCA_BLOCKEDBY_FOLLOWER)!=0) {
                return (max+wanted)/(SUMOReal) 2.0;
            }
        }
    }


    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if ((state&LCA_AMBLOCKINGFOLLOWER)!=0) {
        if (fabs(max-myVehicle.maxNextSpeed(myVehicle.getSpeed()))<0.001&&min==0) { // !!! was standing
            return 0;
        }
        if (myVSafe<=0) {
            return (min+wanted)/(SUMOReal) 2.0;
        }
        return min;//MAX2(min, MIN2(vSafe, wanted));
    }
    if ((state&LCA_AMBACKBLOCKER)!=0) {
        if (max<=myVehicle.maxNextSpeed(myVehicle.getSpeed())&&min==0) {// !!! was standing
            return MAX2(min, MIN2(vSafe, wanted));
        }
    }
    if ((state&LCA_AMBACKBLOCKER_STANDING)!=0) {
        return MAX2(min, MIN2(vSafe, wanted));
    }
    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state&LCA_AMBLOCKINGLEADER)!=0) {
        return (max+wanted)/(SUMOReal) 2.0;
    }
    /*
    if((state&LCA_AMBACKBLOCKER)!=0) {
        if(max<=myVehicle.accelAbility()&&min==0) {
            return MAX2(min, MIN2(vSafe, wanted));
        }
        if(myVSafe>(min+wanted)/2.0) {
            return MIN2(vSafe, wanted);
        }
        return (min+wanted)/2.0;
    }
    */
    if ((state&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {
        if (max<=myVehicle.maxNextSpeed(myVehicle.getSpeed())&&min==0) {// !!! was standing
            return wanted;
        }
        if (myVSafe>(min+wanted)/(SUMOReal) 2.0) {
            return MIN2(vSafe, wanted);
        }
        return (min+wanted)/(SUMOReal) 2.0;
    }
    return wanted;
}


void *
MSLCM_DK2004::inform(void *info, MSVehicle * /*sender*/)
{
    Info *pinfo = (Info*) info;
    if (pinfo->second==LCA_UNBLOCK) {
        myState &= 0xffff00ff;
        return (void*) true;
    }
    myState &= 0xffffffff;
    myState |= pinfo->second;
    if (pinfo->first>=0) {
        myVSafe = MIN2(myVSafe, pinfo->first);
    }
    delete pinfo;
    return (void*) true;
}


void
MSLCM_DK2004::changed()
{
    myChangeProbability = 0;
    myState = 0;
}


void
MSLCM_DK2004::informBlocker(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                            int &blocked,
                            int dir,
                            const std::pair<MSVehicle*, SUMOReal> &neighLead,
                            const std::pair<MSVehicle*, SUMOReal> &neighFollow)
{
    /*
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int blb = 0;
    }
    */
    if ((blocked&LCA_BLOCKEDBY_FOLLOWER)!=0) {
        assert(neighFollow.first!=0);
        MSVehicle *nv = neighFollow.first;
        SUMOReal decelGap =
            neighFollow.second
            + SPEED2DIST(myVehicle.getSpeed()) * (SUMOReal) 2.0
            - MAX2(nv->getSpeed() - ACCEL2SPEED(nv->decelAbility()) * (SUMOReal) 2.0, (SUMOReal) 0);
        if (neighFollow.second>0&&decelGap>0&&nv->hasSafeGap(nv->getSpeed(), decelGap, myVehicle.getSpeed(), nv->getLane().maxSpeed())) {//isSafeChange_WithDistance(decelGap, myVehicle, &nv->getLane())) {
            SUMOReal vsafe = myVehicle.ffeV(myVehicle.getSpeed(), neighFollow.second, neighFollow.first->getSpeed());
            msgPass.informNeighFollower(
                (void*) new Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER), &myVehicle);
        } else {
            SUMOReal vsafe = neighFollow.second<=0
                             ? 0
                             : myVehicle.ffeV(myVehicle.getSpeed(), neighFollow.second, neighFollow.first->getSpeed());
            msgPass.informNeighFollower(
                (void*) new Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER_DONTBRAKE), &myVehicle);
        }
    }
    if ((blocked&LCA_BLOCKEDBY_LEADER)!=0) {
        if (neighLead.first!=0&&neighLead.second>0) {
            msgPass.informNeighLeader(
                (void*) new Info(0, dir|LCA_AMBLOCKINGLEADER), &myVehicle);
        }
    }
}


void
MSLCM_DK2004::prepareStep()
{}


SUMOReal
MSLCM_DK2004::getProb() const
{
    return myChangeProbability;
}


/****************************************************************************/

