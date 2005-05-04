#ifndef MSRightOfWayJunction_H
#define MSRightOfWayJunction_H
/***************************************************************************
                          MSRightOfWayJunction.h  -  Usual right-of-way
                          junction.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
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
// Revision 1.6  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2004/08/02 12:09:39  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.4  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.1  2002/02/21 18:49:46  croessel
// Deadlock-killer implemented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.6  2002/02/13 10:12:26  croessel
// Removed "class findCompetitor" from inside "class
// MSRightOfWayJunction".
//
// Revision 1.5  2002/02/01 11:52:28  croessel
// Removed function-adaptor findCompetitor from inside the class to the
// outside to please MSVC++.
//
// Revision 1.4  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.3  2001/12/20 14:39:15  croessel
// using namespace std replaced by std::
//
// Revision 1.2  2001/12/13 15:53:12  croessel
// In class InLane: changed Junction to RightOfWayJunction.
//
// Revision 1.1  2001/12/12 17:46:02  croessel
// Initial commit. Part of a new junction hierarchy.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSJunctionLogic;
class findCompetitor;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSRightOfWayJunction
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
class MSRightOfWayJunction : public MSLogicJunction
{
public:

    /// Destructor.
    virtual ~MSRightOfWayJunction();

    /** Use this constructor only. */
    MSRightOfWayJunction( std::string id, const Position2D &position,
        LaneCont incoming, LaneCont internal, MSJunctionLogic* logic );

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();

    /** Sets the information which vehicles may drive */
    virtual bool setAllowed();

protected:
    /// Search for deadlock-situations and eleminate them.
    virtual void deadlockKiller();

    /** the type of the junction (its logic) */
    MSJunctionLogic* myLogic;

private:
    /// Invalidated copy constructor.
    MSRightOfWayJunction( const MSRightOfWayJunction& );

    /// Invalidated assignment operator.
    MSRightOfWayJunction& operator=( const MSRightOfWayJunction& );

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
