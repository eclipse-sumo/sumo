#include "MSLCM_Sparmann.h"


MSLCM_Sparmann::MSLCM_Sparmann(MSVehicle &v, bool pkw)
    : MSAbstractLaneChangeModel(v)
{
}

MSLCM_Sparmann::~MSLCM_Sparmann()
{
}

int
MSLCM_Sparmann::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
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

/*
    double sdxp_V =
  */
                 throw 1;
}


int
MSLCM_Sparmann::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
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

throw 1;
}
