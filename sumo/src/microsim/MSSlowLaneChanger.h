#ifndef MSSlowLaneChanger_H
#define MSSlowLaneChanger_H
/***************************************************************************
                          MSSlowLaneChanger.h  -  Handles lane-changes within
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
// Revision 1.3  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:20:54  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:49:30  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLaneChanger.h"
#include "MSAbstractLaneChangeModel.h"
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSSlowLaneChanger
 * Class which performs the lane changing on a single, associated lane.
 */
class MSSlowLaneChanger : public MSLaneChanger
{
public:
    /// Destructor.
    virtual ~MSSlowLaneChanger();

    /// Constructor
    MSSlowLaneChanger( MSEdge::LaneCont* lanes );

    /** Find a new candidate and try to change it. */
    bool change();

    /** Returns true if the target's lane is an allowed lane
        for the candidate's vehicle . */
    bool candiOnAllowed( ChangerIt target );

    /** Returns true if change to the right (left for people driving on
        the "wrong" side ;-) ) is possible & diserable. */
    bool change2right();

    /** Returns true if change to the left (right for people driving on
        the "wrong" side ;-) ) is possible & diserable. */
    bool change2left();

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

    bool change2RightPossible();
    bool change2LeftPossible();



private:
    /// Default constructor.
    MSSlowLaneChanger();

    /// Copy constructor.
    MSSlowLaneChanger( const MSSlowLaneChanger& );

    /// Assignment operator.
    MSSlowLaneChanger& operator=( const MSSlowLaneChanger& );

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
#ifndef DISABLE_INLINE
#include "MSSlowLaneChanger.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:
