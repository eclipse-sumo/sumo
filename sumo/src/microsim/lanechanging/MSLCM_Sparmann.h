#ifndef MSLCM_Sparmann_h
#define MSLCM_Sparmann_h

#include <microsim/MSAbstractLaneChangeModel.h>



class MSLCM_Sparmann : public MSAbstractLaneChangeModel {
public:
    MSLCM_Sparmann(MSVehicle &v);

    virtual ~MSLCM_Sparmann();

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const MSVehicle * const leader,
        const MSVehicle * const neighLead,
        const MSVehicle * const neighFollow,
        const MSLane &neighLane,
        int bestLaneOffset, double bestDist,
        double currentDist);

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const MSVehicle * const leader,
        const MSVehicle * const neighLead,
        const MSVehicle * const neighFollow,
        const MSLane &neighLane,
//        bool congested, bool predInteraction,
        int bestLaneOffset, double bestDist,
        double currentDist);

    virtual void inform(LaneChangeAction lca, MSVehicle *sender) { }


};


#endif
