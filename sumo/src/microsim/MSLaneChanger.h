/****************************************************************************/
/// @file    MSLaneChanger.h
/// @author  Christian Roessel
/// @date    Fri, 01 Feb 2002
/// @version $Id: $
///
// the edge's lanes.
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
#ifndef MSLaneChanger_h
#define MSLaneChanger_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLane.h"
#include "MSEdge.h"
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneChanger
 * Class which performs the lane changing on a single, associated lane.
 */
class MSLaneChanger
{
public:
    /// Destructor.
    virtual ~MSLaneChanger();

    /// Constructor
    MSLaneChanger(MSEdge::LaneCont* lanes);

    /// Start lane-change-process for all vehicles on the edge'e lanes.
    virtual void laneChange();

public:
    /** Structure used for lane-change. For every lane you have to
        know four vehicles, the change-candidate veh and it's follower
        and leader. Further, information about the last vehicle that changed
        into this lane is needed */
    struct ChangeElem
    {
        /// the vehicle following the current vehicle
        MSVehicle*                follow;
        /// the vehicle in front of the current vehicle
        MSVehicle*                lead;
        /// the lane the vehicle is on
        MSLane*                   lane;
        /// the regarded vehicle
        MSLane::VehCont::reverse_iterator veh;
        /// last vehicle that changed into this lane
        MSVehicle*                hoppedVeh;
        /// the vehicle that really wants to change to this lane
        MSVehicle*                lastBlocked;

        SUMOReal dens;

    };

public:
    /** @brief The list of changers;
        For each lane, a ChangeElem is being build */
    typedef std::vector< ChangeElem > Changer;

    /// the iterator moving over the ChangeElems
    typedef Changer::iterator         ChangerIt;

protected:
    /// Initialize the changer before looping over all vehicles.
    virtual void initChanger();

    /** @brief Check if there is a single change-candidate in the changer.
        Returns true if there is one. */
    virtual bool vehInChanger()
    {
        // If there is at least one valid vehicle under the veh's in myChanger
        // return true.
        for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
            if (veh(ce) != 0) {
                return true;
            }
        }
        return false;
    }

    /** Returns a pointer to the changer-element-iterator vehicle, or 0 if
        there is none. */
    virtual MSVehicle* veh(ChangerIt ce)
    {
        // If ce has a valid vehicle, return it. Otherwise return 0.
        if (ce->veh != ce->lane->myVehicles.rend()) {
            return *(ce->veh);
        }
        return 0;
    }


    /** Find a new candidate and try to change it. */
    virtual bool change();

    /** After the possible change, update the changer. */
    virtual void updateChanger(bool vehHasChanged);

    /** During lane-change a temporary vehicle container is filled within
        the lanes (bad pratice to modify foreign members, I know). Swap
        this container with the real one. */
    virtual void updateLanes();

    /** @brief Find current candidate.
        If there is none, myChanger.end() is returned. */
    ChangerIt findCandidate();

    /** Returns true if the target's lane is an allowed lane
        for the candidate's vehicle . */
    virtual bool candiOnAllowed(ChangerIt target)
    {
        assert(veh(myCandi) != 0);
        return veh(myCandi)->onAllowed(target->lane);
    }

    virtual int change2right(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        const std::vector<std::vector<MSVehicle::LaneQ> > &preb);
    //int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    virtual int change2left(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        const std::vector<std::vector<MSVehicle::LaneQ> > &preb);
    //int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    /** If candidate isn't on an allowed lane, we need to find target-
        lane that takes it closer to an allowed one. */
    virtual ChangerIt findTarget();

    /** Returns true if change to target-lane is allowed. */
//    bool change2target( ChangerIt target );

    virtual void setOverlap(const std::pair<MSVehicle*, SUMOReal> &neighLead,
                            const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                            /*const ChangerIt &target,*/ int &blocked);

    virtual void setIsSafeChange(const std::pair<MSVehicle*, SUMOReal> &neighLead,
                                 const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                                 const ChangerIt &target, int &blocked);

    /** Returns true if candidate will be influenced by it's leader. */
    //bool predInteraction();

    /** Returns true, if candidate has an advantage by changing to the
        right. */
    virtual int advan2right(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        int blocked,
        const std::vector<std::vector<MSVehicle::LaneQ> > &preb);
    //int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    /** Returns true, if candidate has an advantage by changing to the
        left. */
    virtual int advan2left(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        int blocked,
        const std::vector<std::vector<MSVehicle::LaneQ> > &preb);
    //int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    /** Returns true if candidate overlaps with a vehicle, that
        already changed the lane.*/
    virtual bool overlapWithHopped(ChangerIt target)
    {
        MSVehicle *v1 = target->hoppedVeh;
        MSVehicle *v2 = veh(myCandi);
        if (v1!=0 && v2!=0) {
            return MSVehicle::overlap(v1, v2);
        }
        return false;
    }

    /*
        virtual bool change2RightPossible();
        virtual bool change2LeftPossible();
    	*/
    std::pair<MSVehicle *, SUMOReal> getRealThisLeader(const ChangerIt &target);
    std::pair<MSVehicle *, SUMOReal> getRealFollower(const ChangerIt &target);
    std::pair<MSVehicle *, SUMOReal> getRealRightFollower();
    std::pair<MSVehicle *, SUMOReal> getRealLeftFollower();
    std::pair<MSVehicle *, SUMOReal> getRealLeader(const ChangerIt &target);
    std::pair<MSVehicle *, SUMOReal> getRealRightLeader();
    std::pair<MSVehicle *, SUMOReal> getRealLeftLeader();


protected:
    /// Container for ChangeElemements, one for every lane in the edge.
    Changer   myChanger;

    /** Change-candidate. Last of the vehicles in changer. Only this one
        will try to change. Every vehicle on the edge will be a candidate
        once in the change-process. */
    ChangerIt myCandi;

private:
    /// Default constructor.
    MSLaneChanger();

    /// Copy constructor.
    MSLaneChanger(const MSLaneChanger&);

    /// Assignment operator.
    MSLaneChanger& operator=(const MSLaneChanger&);
};


#endif

/****************************************************************************/

