#ifndef MSAbstractLaneChangeModel_h
#define MSAbstractLaneChangeModel_h
//---------------------------------------------------------------------------//
//                        MSAbstractLaneChangeModel.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/22 13:45:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:22:18  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLaneChanger.h"

/* =========================================================================
 * used enumeration
 * ======================================================================= */
enum LaneChangeAction {
    LCA_NONE = 0,
    LCA_URGENT = 1,
    LCA_SPEEDGAIN = 2,
    LCA_LEFT = 4,
    LCA_RIGHT = 8,

    LCA_BLOCKEDBY_LEADER = 16,
    LCA_BLOCKEDBY_FOLLOWER = 32,
    LCA_OVERLAPPING = 64,

    LCA_MAX = 128,
};

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSAbstractLaneChangeModel {
public:
    class MSLCMessager {
    public:
        MSLCMessager(MSVehicle *leader,  MSVehicle *neighLead,
            MSVehicle *neighFollow)
            : myLeader(leader), myNeighLeader(neighLead),
            myNeighFollower(neighFollow) { }

        ~MSLCMessager() { }

        void *informLeader(void *info, MSVehicle *sender) {
            assert(myLeader!=0);
            return myLeader->getLaneChangeModel().inform(info, sender);
        }

        void *informNeighLeader(void *info, MSVehicle *sender) {
            assert(myNeighLeader!=0);
            return myNeighLeader->getLaneChangeModel().inform(info, sender);
        }

        void *informNeighFollower(void *info, MSVehicle *sender) {
            assert(myNeighFollower!=0);
            return myNeighFollower->getLaneChangeModel().inform(info, sender);
        }
    private:
        MSVehicle *myLeader;
        MSVehicle *myNeighLeader;
        MSVehicle *myNeighFollower;

    };


    MSAbstractLaneChangeModel(MSVehicle &v)
        : myVehicle(v), myState(0) { }

    virtual ~MSAbstractLaneChangeModel() { }
/*
    virtual void init(int bestLaneOffset, SUMOReal bestDist,
        SUMOReal currentDist) = 0;
*/
    /*
    /// Called if the vehicle has changed the lane
    virtual void changed(int offset) = 0;
*/
    /// Called if the vehicle is on a lane on which it has to change lanes
//    virtual bool forcedChangeNecessary(const ChangeElem &vehSurround) = 0;

    int getState() const { return myState; }

    void setState(int state) { myState = state; }

    virtual void prepareStep() { }

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
//        bool congested, bool predInteraction,
        int bestLaneOffset, SUMOReal bestDist, SUMOReal neighDist,
        SUMOReal currentDist,
        MSVehicle **lastBlocked) = 0;

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
//        bool congested, bool predInteraction,
        int bestLaneOffset, SUMOReal bestDist, SUMOReal neighDist,
        SUMOReal currentDist,
        MSVehicle **lastBlocked) = 0;

    virtual void *inform(void *info, MSVehicle *sender) = 0;

    virtual SUMOReal patchSpeed(SUMOReal min, SUMOReal wanted, SUMOReal max,
        SUMOReal vsafe) = 0;

    virtual void changed() = 0;

protected:
    virtual bool congested(const MSVehicle * const neighLeader) {
        if(neighLeader==0) {
            return false;
        }
        // Congested situation are relevant only on highways (maxSpeed > 70km/h)
        // and congested on German Highways means that the vehicles have speeds
        // below 60km/h. Overtaking on the right is allowed then.
        if ( ( myVehicle.getLane().maxSpeed() <= 70.0 / 3.6 ) ||
             ( neighLeader->getLane().maxSpeed() <= 70.0 / 3.6 ) ) {

            return false;
        }
        if ( myVehicle.congested() && neighLeader->congested() ) {
            return true;
        }
        return false;
    }

    virtual bool predInteraction(const MSVehicle * const leader) {
        if(leader==0) {
            return false;
        }
        // let's check it on highways only
        if(leader->speed()<(80.0*3.6)) {
            return false;
        }
        SUMOReal gap = leader->pos() - leader->length() - myVehicle.pos();
        return gap < myVehicle.interactionGap( &myVehicle.getLane(), *leader );
    }



protected:
    MSVehicle &myVehicle;
    int myState;

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
