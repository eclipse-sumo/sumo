#ifndef MSNoLogicJunction_H
#define MSNoLogicJunction_H
/***************************************************************************
                          MSNoLogicJunction.h  -  Junction that needs no
                          logic, e.g. for exits.
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
// Revision 1.7  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/08/02 12:09:39  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.5  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.4  2003/07/07 08:19:53  dkrajzew
// As by now GUINoLogicJunctions exist, we have to make the destructor virtual
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added for
//  container; global file include; junction extended by position information
//  (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/06/18 10:59:53  croessel
// Removed some ^M.
//
// Revision 1.2  2002/06/07 14:45:17  dkrajzew
// Added MSNoLogicJunction ,,pro forma,,. The code may compile but the
// class has no real functionality...
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 16:02:21  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.2  2001/12/20 14:37:12  croessel
// using namespace std replaced by std:
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
#include <string>
#include <vector>
#include <bitset>
#include "MSJunction.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSNoLogicJunction
 * This junctions let all vehicles past through so they only should be used on
 * junctions where incoming vehicles are no foes to each other (may drive
 * simultaneously).
 */
class MSNoLogicJunction  : public MSJunction
{
public:
    /// Destructor.
    virtual ~MSNoLogicJunction();

    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > LaneCont;

    /** Use this constructor only. */
    MSNoLogicJunction( std::string id, const Position2D &position,
        LaneCont incoming, LaneCont internal );

    /** Here, do nothing. */
    bool clearRequests();

    /** @brief does nothing
        Implementation of MSJunction */
    bool setAllowed() { return true; };

    /** Initialises the junction after the net was completely loaded */
    void postloadInit();

private:
    /** Lanes incoming to the junction */
    LaneCont myIncomingLanes;

    /** The junctions internal lanes */
    LaneCont myInternalLanes;

private:
    /// Invalidated copy constructor.
    MSNoLogicJunction( const MSNoLogicJunction& );

    /// Invalidated assignment operator.
    MSNoLogicJunction& operator=( const MSNoLogicJunction& );

    /** @brief a dump container
        Request-setting vehicles may write into this container and responds
        are read from it. As responds are always true, this container is
        set to true for all links.
        This dump is also used as the mask for incoming non-first vehicles */
    static std::bitset<64> myDump;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
