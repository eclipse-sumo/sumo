#include "MSLCM_Krauss.h"


MSLCM_Krauss::MSLCM_Krauss(MSVehicle &v)
    : MSAbstractLaneChangeModel(v)
{
}

MSLCM_Krauss::~MSLCM_Krauss()
{
}

int
MSLCM_Krauss::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
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

    // krauss: do not change lanes if congested or if following another vehicle
    if(congested( neighLead )||predInteraction(leader)) {
        return false;
    }

    // krauss: change for higher speed
    double thisLaneVSafe;
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
    if(thisLaneVSafe
        >=
        MIN2(myVehicle.getVehicleType().maxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}


int
MSLCM_Krauss::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
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

    // krauss: do not change lanes if congested
    if(congested( neighLead )) {
        return false;
    }
    // krauss: change for higher speed
    double thisLaneVSafe;
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
    if(thisLaneVSafe
        <
        MIN2(myVehicle.getVehicleType().maxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_LEFT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}
