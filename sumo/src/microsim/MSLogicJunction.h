#ifndef MSLogicJunction_H
#define MSLogicJunction_H
/***************************************************************************
                          MSLogicJunction.h  -  Base class for junctions
                          with one ore more logics.
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
// Revision 1.7  2004/08/02 12:09:39  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.6  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.5  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.4  2003/03/03 14:56:21  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
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
// Revision 1.2  2002/04/18 10:51:22  croessel
// Introduced new method "bool driveRequest()" in class DriveBrakeRequest
// to let findCompetitor check, if a first car set a request.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/01/30 16:14:57  croessel
// Made the destructor virtual.
//
// Revision 1.3  2001/12/20 14:30:42  croessel
// using namespace std replaced by std::
//
// Revision 1.2  2001/12/19 16:32:32  croessel
// Changes due to new junction-hierarchy.
//
// Revision 1.1  2001/12/12 17:46:02  croessel
// Initial commit. Part of a new junction hierarchy.
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSJunction.h"
#include <bitset>
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSLogicJunction
 * A junction which may not let all vehicles through, but must perform any
 * kind of an operation to determine which cars are allowed to drive in this
 * step.
 */
class MSLogicJunction : public MSJunction
{
public:
    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > LaneCont;

    /// Destructor.
    virtual ~MSLogicJunction();

    /** @brief Container for vehicle requests.
        Each element of this container represents one particular
        link from one lane to another. */
    typedef std::bitset<64> Request;

    /** @brief Container for the request responds.
        The respond is link-bound */
    typedef std::bitset<64> Respond;

    /** @brief Container for junction-internal lane occupation
        Each element of this container represents one particular
        junction-internal lane */
    typedef std::bitset<64> InnerState;

    /// initialises the junction after the whole net has been loaded
    virtual void postloadInit();

protected:
    /// constructor; this class is virtual
    MSLogicJunction( std::string id, const Position2D &position,
        LaneCont incoming, LaneCont internal );

protected:
    /// list of incoming lanes
    LaneCont myIncomingLanes;

    /// list of incoming lanes
    LaneCont myInternalLanes;

    /** Current request. */
    Request myRequest;

    /** Current inner state */
    InnerState  myInnerState;

    /** Current respond */
    Respond myRespond;

private:
    /// Invalidated copy constructor.
    MSLogicJunction( const MSLogicJunction& );

    /// Invalidated assignment operator.
    MSLogicJunction& operator=( const MSLogicJunction& );

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
