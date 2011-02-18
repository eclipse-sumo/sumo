/****************************************************************************/
/// @file    MSLCM_DK2004.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A lane change model developed by D. Krajzewicz between 2004 and 2010
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLCM_DK2004_h
#define MSLCM_DK2004_h


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
// enumeration definition
// ===========================================================================
enum MyLCAEnum {
    LCA_AMBLOCKINGLEADER = 256, // 0
    LCA_AMBLOCKINGFOLLOWER = 512,// 1
    LCA_MRIGHT = 1024, // 2
    LCA_MLEFT = 2048,// 3
    // !!! never set LCA_UNBLOCK = 4096,// 4
    LCA_AMBLOCKINGFOLLOWER_DONTBRAKE = 8192,// 5
    // !!! never used LCA_AMBLOCKINGSECONDFOLLOWER = 16384, // 6
    // !!! never read LCA_KEEP1 = 65536,// 8
    // !!! never used LCA_KEEP2 = 131072,// 9
    LCA_AMBACKBLOCKER = 262144,// 10
    LCA_AMBACKBLOCKER_STANDING = 524288// 11

};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_DK2004
 * @brief A lane change model developed by D. Krajzewicz between 2004 and 2010
 */
class MSLCM_DK2004 : public MSAbstractLaneChangeModel {
public:
    MSLCM_DK2004(MSVehicle &v);

    virtual ~MSLCM_DK2004();

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked);

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked);

    virtual void *inform(void *info, MSVehicle *sender);

    /** @brief Called to adapt the speed in order to allow a lane change.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    virtual SUMOReal patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                                const MSCFModel &cfModel);

    virtual void changed();

    SUMOReal getProb() const;
    virtual void prepareStep();

    SUMOReal getChangeProbability() const {
        return myChangeProbability;
    }


protected:
    void informBlocker(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                       int &blocked, int dir,
                       const std::pair<MSVehicle*, SUMOReal> &neighLead,
                       const std::pair<MSVehicle*, SUMOReal> &neighFollow);

    inline bool amBlockingLeader() {
        return (myOwnState&LCA_AMBLOCKINGLEADER)!=0;
    }
    inline bool amBlockingFollower() {
        return (myOwnState&LCA_AMBLOCKINGFOLLOWER)!=0;
    }
    inline bool amBlockingFollowerNB() {
        return (myOwnState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0;
    }
    inline bool amBlockingFollowerPlusNB() {
        return (myOwnState&(LCA_AMBLOCKINGFOLLOWER|LCA_AMBLOCKINGFOLLOWER_DONTBRAKE))!=0;
    }
    inline bool currentDistDisallows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist/(abs(laneOffset))<lookForwardDist;
    }
    inline bool currentDistAllows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist/abs(laneOffset)>lookForwardDist;
    }

    typedef std::pair<SUMOReal, int> Info;



protected:
    SUMOReal myChangeProbability;

    SUMOReal myLeadingBlockerLength;
    SUMOReal myLeftSpace;

    std::vector<SUMOReal> myVSafes;
    bool myDontBrake;

};


#endif

/****************************************************************************/

