#include "MSLCM_DK2004.h"
#include <iostream>

using namespace std;


std::string id_leader("760");
std::string id_vehicle("1663");
std::string id_follower("129");
std::string id_pfollower("1131");
size_t stime = 21939;

//#define GUI_DEBUG

#ifdef GUI_DEBUG
#include <gui/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#endif


MSLCM_DK2004::MSLCM_DK2004(MSVehicle &v)
    : MSAbstractLaneChangeModel(v),
    myChangeProbability(0),
    myVSafe(0), myBlockingLeader(0), myBlockingFollower(0)
{
}

MSLCM_DK2004::~MSLCM_DK2004()
{
    changed();
}

int
MSLCM_DK2004::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                 int blocked,
                                 const std::pair<MSVehicle*, double> &leader,
                                 const std::pair<MSVehicle*, double> &neighLead,
                                 const std::pair<MSVehicle*, double> &neighFollow,
                                 const MSLane &neighLane,
                                 int bestLaneOffset, double bestDist,
                                 double neighDist,
                                 double currentDist)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int bla = 0;
    }
#endif
    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    // -------- forced changing
    if( bestLaneOffset<0&&currentDistDisallows(currentDist, bestLaneOffset)) {
        informBlocker(msgPass, blocked, LCA_MRIGHT, neighLead, neighFollow);
        return ret|LCA_RIGHT|LCA_URGENT|blocked;
    }
    // left lanes are better...
    if( bestLaneOffset>0&&currentDistDisallows(currentDist, bestLaneOffset)&&currentDist!=neighDist) {
        return ret;
    }
    // the current lane is better
    if(neighDist<LOOK_FORWARD_DIST&&bestLaneOffset==0) {
        return ret;
    }
    // --------

    // -------- make place on current lane if blocking follower
    if( amBlockingFollowerPlusNB()
        &&
        (currentDistAllows(neighDist, bestLaneOffset)||neighDist>=currentDist)
        /*&&
        (myState&LCA_MRIGHT)!=0 */) {

        return ret|LCA_RIGHT|LCA_URGENT|LCA_KEEP1;
    }
    // --------


    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }
    // --------

    // -------- higher speed
    if(congested( neighLead.first )||predInteraction(leader.first)) {
        return ret;
    }
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead.first == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead.second, neighLead.first->speed());
    }
    if(leader.first==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader.second, leader.first->speed());
    }

    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    if(thisLaneVSafe>neighLaneVSafe) {
        // this lane is better
        if(myChangeProbability<0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // right lane is better
        myChangeProbability -=
            ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }
    if(myChangeProbability<-1.0) {
        return ret | LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // --------
    return ret;
}


int
MSLCM_DK2004::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                int blocked,
                                const std::pair<MSVehicle*, double> &leader,
                                const std::pair<MSVehicle*, double> &neighLead,
                                const std::pair<MSVehicle*, double> &neighFollow,
                                const MSLane &neighLane,
                                int bestLaneOffset, double bestDist,
                                double neighDist,
                                double currentDist)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int bla = 0;
    }
#endif
    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    // -------- forced changing
    if( bestLaneOffset>0&&currentDistDisallows(currentDist, bestLaneOffset)) {
        informBlocker(msgPass, blocked, LCA_MLEFT, neighLead, neighFollow);
        return ret|LCA_LEFT|LCA_URGENT|blocked;
    }
    // left lanes are better...
    if( bestLaneOffset<0&&currentDistDisallows(currentDist, bestLaneOffset)&&currentDist!=neighDist) {
        return ret;
    }
    // the current lane is better
    if( neighDist<LOOK_FORWARD_DIST&&bestLaneOffset==0) {
        return ret;
    }
    // --------

    // -------- make place on current lane if blocking follower
    if( amBlockingFollowerPlusNB()
        &&
        (currentDistAllows(neighDist, bestLaneOffset)||neighDist>=currentDist) ) {

        return ret|LCA_LEFT|LCA_URGENT|LCA_KEEP1;
    }
    // --------

    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }

    // -------- higher speed
    if(congested( neighLead.first )) {
        return ret;
    }
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead.first == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead.second, neighLead.first->speed());
    }
    if(leader.first==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader.second, leader.first->speed());
    }
    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    if(thisLaneVSafe>neighLaneVSafe) {
        // this lane is better
        if(myChangeProbability>0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // right lane is better
        myChangeProbability +=
            ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }
    if(myChangeProbability>1.0) {
        return ret | LCA_LEFT|LCA_SPEEDGAIN;
    }
    // --------
    return ret;
}


double
MSLCM_DK2004::patchSpeed(double min, double wanted, double max, double vsafe)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int bla = 0;
    }
#endif
    // just to make sure to be notified about lane chaning end
    if(myVehicle.getLane().edge().nLanes()==1) {
        // remove chaning information if on a road with a single lane
        myState = 0;//(myState&0x0000ff00);
        changed();
    }

    // check whether the vehicle is blocked
    if((myState&LCA_URGENT)!=0) {
        // check whether the vehicle maybe has to be swapped with one of
        //  the blocking vehicles
        if( (myState&LCA_BLOCKEDBY_LEADER)!=0
            &&
            (myState&LCA_BLOCKEDBY_FOLLOWER)!=0) {

            return wanted;
        } else {
            if((myState&LCA_BLOCKEDBY_LEADER)!=0) {
                // if interacting with leader and not too slow
//                if(min==0) {
                    return (min+wanted)/2.0;
  //              }
    //            return min;
            }
            if((myState&LCA_BLOCKEDBY_FOLLOWER)!=0) {
                return (max+wanted)/2.0;
//                return max;
            }
        }
    }


    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if((myState&LCA_AMBLOCKINGFOLLOWER)!=0) {
        if(myVSafe<=0) {
            return (min+wanted)/2.0;
        }
        return MAX(min, MIN(myVSafe, wanted));
    }
    /*
    if((myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0&&vsafe>max) {
        myState &= 0xffff00ff;
        if(myVSafe>0) {
            return MAX(min, MIN(myVSafe, wanted));
        } else {
            return (max+wanted)/2.0;
//            return max;
        }

    }
    */
    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if((myState&LCA_AMBLOCKINGLEADER)!=0||(myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {
        return (max+wanted)/2.0;
//        return max;
    }
    return wanted;
}


void *
MSLCM_DK2004::inform(void *info, MSVehicle *sender)
{
    Info *pinfo = (Info*) info;
    if(pinfo->second==LCA_UNBLOCK) {
        myState &= 0xffff00ff;
        return (void*) true;
    }
    myState &= 0xffff00ff;
    myState |= pinfo->second;
    myVSafe = MIN(myVSafe, pinfo->first);
    return (void*) true;
}


void
MSLCM_DK2004::changed()
{
    if(MSNet::globaltime>16834) {
        int bla = 0;
    }
    if(myBlockingLeader!=0) {
        cout << myBlockingLeader->id() << endl;
        myBlockingLeader->quitRemindedLeft(this);
        myBlockingLeader->getLaneChangeModel().inform(
            (void*) &(Info(0, LCA_UNBLOCK)), &myVehicle);
        myBlockingLeader = 0;
    }
    if(myBlockingFollower!=0) {
        cout << myBlockingFollower->id() << endl;
        myBlockingFollower->quitRemindedLeft(this);
        myBlockingFollower->getLaneChangeModel().inform(
            (void*) &(Info(0, LCA_UNBLOCK)), &myVehicle);
        myBlockingFollower = 0;
    }
    myChangeProbability = 0;
    myState &= 0x00ff0000;
}


void
MSLCM_DK2004::informBlocker(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                            int &blocked,
                            int dir,
                            const std::pair<MSVehicle*, double> &neighLead,
                            const std::pair<MSVehicle*, double> &neighFollow)
{
    if((blocked&LCA_BLOCKEDBY_FOLLOWER)!=0) {
        assert(neighFollow.first!=0);
        MSVehicle *nv = neighFollow.first;
        double decelGap =
            neighFollow.second
            + myVehicle.speed() * MSNet::deltaT() * 2.0
            - MAX(nv->speed() - nv->decelAbility() * 2.0, 0);
        if(neighFollow.second>0&&decelGap>0&&nv->isSafeChange_WithDistance(decelGap, myVehicle, &nv->getLane())) {
            float vsafe = myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighFollow.second, neighFollow.first->speed());
            if(msgPass.informNeighFollower(
                (void*) &(Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER)), &myVehicle)) {

                setBlockingFollower(neighFollow.first);

            }
        } else {
            setBlockingFollower(neighFollow.first);
            float vsafe = neighFollow.second<=0
                ? 0
                : myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                    neighFollow.second, neighFollow.first->speed());
            msgPass.informNeighFollower(
                (void*) &(Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)), &myVehicle);
        }
    }
    if((blocked&LCA_BLOCKEDBY_LEADER)!=0) {
        if(neighLead.first!=0&&neighLead.second>0) {
            if(msgPass.informNeighLeader(
                (void*) &(Info(0, dir|LCA_AMBLOCKINGLEADER)), &myVehicle)!=0) {

                if(myBlockingLeader!=neighLead.first) {
                    if(myBlockingLeader!=0) {
                        cout << myBlockingLeader->id() << endl;
                        myBlockingLeader->quitRemindedLeft(this);
                        myBlockingLeader->getLaneChangeModel().inform(
                            (void*) &(Info(0, LCA_UNBLOCK)), &myVehicle);
                    }
                    myBlockingLeader = neighLead.first;
                    myBlockingLeader->quitRemindedEntered(this);
                }
            }
        }
    }
}


void
MSLCM_DK2004::setBlockingFollower(MSVehicle *v)
{
    if(myBlockingFollower==v) {
        return;
    }
    if(myBlockingFollower!=0) {
        cout << myBlockingFollower->id() << endl;
        myBlockingFollower->quitRemindedLeft(this);
        myBlockingFollower->getLaneChangeModel().inform(
            (void*) &(Info(0, LCA_UNBLOCK)), &myVehicle);
    }
    myBlockingFollower = v;
    myBlockingFollower->quitRemindedEntered(this);
}

double
MSLCM_DK2004::getProb() const
{
    return myChangeProbability;
}


void
MSLCM_DK2004::removeOnTripEnd( MSVehicle *veh )
{
    if(veh==myBlockingLeader) {
        myBlockingLeader = 0;
    }
    if(veh==myBlockingFollower) {
        myBlockingFollower = 0;
    }
}

    /*
void
MSLCM_DK2004::addToBlocking(MSVehicle *veh)
{
    std::vector<MSVehicle*>::iterator i =
        find(myBlockingVehicles.begin(), myBlockingVehicles.end(), veh);
    if(i==myBlockingVehicles.end()) {
        myBlockingVehicles.push_back(veh);
        veh->quitRemindedEntered(this);
    }
}
*/
