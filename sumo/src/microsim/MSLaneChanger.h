#ifndef MSLaneChanger_H
#define MSLaneChanger_H
/***************************************************************************
                          MSLaneChanger.h  -  Handles lane-changes within
                          the edge's lanes.
                             -------------------
    begin                : Fri, 01 Feb 2002
    copyright            : (C) 2002 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.7  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.4  2004/08/02 12:07:01  dkrajzew
// first steps towards a lane-changing model API
//
// Revision 1.3  2003/10/15 11:40:59  dkrajzew
// false rules removed; initial state for further tests
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.2  2002/04/18 12:18:39  croessel
// Bug-fix: Problem was that a right and a left vehicle could change to a
// middle lane, even though they were overlapping. Solution: Introduction
// of hoppedVeh in ChangeElem and method overlapWithHopped().
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.1  2002/03/21 11:31:53  croessel
// Changed onAllowed to candiOnAllowed. Now the changeCandidate checks
// if a given lane is suitable for him. In change2right/left it is
// checked, that the target is a valid lane.
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/05 11:53:02  croessel
// Initial commit.
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSLane.h"
#include "MSEdge.h"
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    MSLaneChanger( MSEdge::LaneCont* lanes );

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
    virtual bool vehInChanger();

    /** Returns a pointer to the changer-element-iterator vehicle, or 0 if
        there is none. */
    virtual MSVehicle* veh( ChangerIt ce );

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
    virtual bool candiOnAllowed( ChangerIt target );

    virtual int change2right(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    virtual int change2left(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

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
        int blocked, int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    /** Returns true, if candidate has an advantage by changing to the
        left. */
    virtual int advan2left(
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &rLead,
        const std::pair<MSVehicle*, SUMOReal> &rFollow,
        int blocked, int bestLaneOffset, SUMOReal bestDist, SUMOReal currentDist);

    /** Returns true if candidate overlaps with a vehicle, that
        already changed the lane.*/
    virtual bool overlapWithHopped( ChangerIt target );
/*
    virtual bool change2RightPossible();
    virtual bool change2LeftPossible();
	*/
    std::pair<int, SUMOReal> getChangePreference();

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
    MSLaneChanger( const MSLaneChanger& );

    /// Assignment operator.
    MSLaneChanger& operator=( const MSLaneChanger& );
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
#ifndef DISABLE_INLINE
#include "MSLaneChanger.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:
