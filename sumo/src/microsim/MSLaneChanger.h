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
    void laneChange();

protected:
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
        MSLane::VehCont::iterator veh;

	    /// last vehicle that changed into this lane
        MSVehicle*                hoppedVeh;
    };

    /** @brief The list of changers;
        For each lane, a ChangeElem is being build */
    typedef std::vector< ChangeElem > Changer;

    /// the iterator moving over the ChangeElems
    typedef Changer::iterator         ChangerIt;

    /// Initialize the changer before looping over all vehicles.
    void initChanger();

    /** @brief Check if there is a single change-candidate in the changer.
        Returns true if there is one. */
    bool vehInChanger();

    /** Returns a pointer to the changer-element-iterator vehicle, or 0 if
        there is none. */
    MSVehicle* veh( ChangerIt ce );

    /** Find a new candidate and try to change it. */
    void change();

    /** After the possible change, update the changer. */
    void updateChanger();

    /** During lane-change a temporary vehicle container is filled within
        the lanes (bad pratice to modify foreign members, I know). Swap
        this container with the real one. */
    virtual void updateLanes();

    /** @brief Find current candidate.
        If there is none, myChanger.end() is returned. */
    ChangerIt findCandidate();

    /** Returns true if the target's lane is an allowed lane
        for the candidate's vehicle . */
    bool candiOnAllowed( ChangerIt target );

    /** Returns true if change to the right (left for people driving on
        the "wrong" side ;-) ) is possible & diserable. */
    bool change2right();

    /** Returns true if change to the left (right for people driving on
        the "wrong" side ;-) ) is possible & diserable. */
    bool change2left();

    /** If candidate isn't on an allowed lane, we need to find target-
        lane that takes it closer to an allowed one. */
    ChangerIt findTarget();

    /** Returns true if change to target-lane is allowed. */
    bool change2target( ChangerIt target );

    /** Returns true if the two changer-element's vehicles overlap. */
    bool overlap( ChangerIt target );

    /** Returns true if there is a congested traffic situation on a
        highway (i.e. speed > 70/3.6 m/s). In a congested state overtaking
        on the right is allowed (in Germay). Congested means, that both
        vehicles have a speed less than 60/3.6 m/s. */
    bool congested( ChangerIt target );

    /** Returns true if the candidate is able to change collision-free to
        the target's lane. */
    bool safeChange( ChangerIt target );

    /** Returns true if candidate will be influenced by it's leader. */
    bool predInteraction();

    /** Returns true, if candidate has an advantage by changing to the
        right. */
    bool advan2right();

    /** Returns true, if candidate has an advantage by changing to the
        left. */
    bool advan2left();

    /** Returns true if candidate overlaps with a vehicle, that
        already changed the lane.*/
    bool overlapWithHopped( ChangerIt target );

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
