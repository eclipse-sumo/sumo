#include "MSLCM_DK2002.h"


MSLCM_DK2002::MSLCM_DK2002(MSVehicle &v)
    : MSAbstractLaneChangeModel(v)
{
}

MSLCM_DK2002::~MSLCM_DK2002()
{
}

int
MSLCM_DK2002::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                 int blocked,
                                 const MSVehicle * const leader,
                                 const MSVehicle * const neighLead,
                                 const MSVehicle * const neighFollow,
                                 const MSLane &neighLane,
                                 int bestLaneOffset, double bestDist,
                                 double currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset<0) {
        return LCA_RIGHT|LCA_URGENT;
    }
    // no further process to the left
    if(bestLaneOffset>0) {
        return 0;
    }

    // security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return 0;
    }

    // higher speed
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead->pos() - neighLead->length() - myVehicle.pos(),
                neighLead->speed());
    }
    if(leader==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
    }
    if(thisLaneVSafe+0.1<neighLaneVSafe) {
        return LCA_RIGHT|LCA_SPEEDGAIN;
    }
    return 0;
}


int
MSLCM_DK2002::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                int blocked,
                                const MSVehicle * const leader,
                                const MSVehicle * const neighLead,
                                const MSVehicle * const neighFollow,
                                const MSLane &neighLane,
                                int bestLaneOffset, double bestDist,
                                double currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset>0) {
        return LCA_LEFT|LCA_URGENT;
    }
    // no further process to the right
    if(bestLaneOffset<0) {
        return 0;
    }

    // security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return 0;
    }

    // higher speed
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead->pos() - neighLead->length() - myVehicle.pos(),
                neighLead->speed());
    }
    if(leader==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
    }
    if(thisLaneVSafe+0.1<neighLaneVSafe) {
        return LCA_LEFT|LCA_SPEEDGAIN;
    }
    return 0;
}
