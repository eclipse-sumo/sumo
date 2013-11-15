/****************************************************************************/
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by D. Krajzewicz et al. between 2004 and 2013
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
#ifndef MSLCM_LC2013_h
#define MSLCM_LC2013_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSAbstractLaneChangeModel.h>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_LC2013
 * @brief A lane change model developed by D. Krajzewicz et al. between 2004 and 2013
 */
class MSLCM_LC2013 : public MSAbstractLaneChangeModel {
public:

    enum MyLCAEnum {
        LCA_AMBLOCKINGLEADER = 1 << 16,
        LCA_AMBLOCKINGFOLLOWER = 1 << 17,                          
        LCA_MRIGHT = 1 << 18,                                     
        LCA_MLEFT = 1 << 19,                                      
        // !!! never set LCA_UNBLOCK = 1 << 20,                   
        LCA_AMBLOCKINGFOLLOWER_DONTBRAKE = 1 << 21,               
        // !!! never used LCA_AMBLOCKINGSECONDFOLLOWER = 1 << 22,
        LCA_CHANGE_TO_HELP = 1 << 23,
        // !!! never read LCA_KEEP1 = 1 << 24,                   
        // !!! never used LCA_KEEP2 = 1 << 25,                  
        LCA_AMBACKBLOCKER = 1 << 26,                            
        LCA_AMBACKBLOCKER_STANDING = 1 << 27                    
    };

    MSLCM_LC2013(MSVehicle& v);

    virtual ~MSLCM_LC2013();

    /** @brief Called to examine whether the vehicle wants to change 
     * using the given laneOffset. 
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    int wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked);

    void* inform(void* info, MSVehicle* sender);

    /** @brief Called to adapt the speed in order to allow a lane change.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    SUMOReal patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                                const MSCFModel& cfModel);
    /** helper function which contains the actual logic */
    SUMOReal _patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                                const MSCFModel& cfModel);

    void changed();

    void prepareStep();


protected:

    // @brief helper function for doing the actual work
    int _wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked);




    void informBlocker(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                       int blocked, int dir,
                       const std::pair<MSVehicle*, SUMOReal>& neighLead,
                       const std::pair<MSVehicle*, SUMOReal>& neighFollow);

    inline bool amBlockingLeader() {
        return (myOwnState & LCA_AMBLOCKINGLEADER) != 0;
    }
    inline bool amBlockingFollower() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER) != 0;
    }
    inline bool amBlockingFollowerNB() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0;
    }
    inline bool amBlockingFollowerPlusNB() {
        return (myOwnState & (LCA_AMBLOCKINGFOLLOWER | LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)) != 0;
    }
    inline bool currentDistDisallows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / (abs(laneOffset)) < lookForwardDist;
    }
    inline bool currentDistAllows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / abs(laneOffset) > lookForwardDist;
    }

    /// @brief information regarding save velocity (unused) and state flags of the ego vehicle
    typedef std::pair<SUMOReal, int> Info;



protected:
    /// @brief a value for tracking the probability that a change to the offset with the same sign is beneficial
    SUMOReal mySpeedGainProbability;

    SUMOReal myLeadingBlockerLength;
    SUMOReal myLeftSpace;

    std::vector<SUMOReal> myVSafes;
    bool myDontBrake;

    /* @brief acceleration during this step in m/s. This is used to extrapolate 
     * future maneuvers when deciding whether blockers should slow down */
    SUMOReal myLastAccel;

};


#endif

/****************************************************************************/

