#ifndef MSSourceLane_H
#define MSSourceLane_H
/***************************************************************************
                          MSSourceLane.h  -
                          Same as MSLane, but with another emission
                          behaviour
                             -------------------
    begin                : Mon, 25 Nov 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.4  2005/05/04 07:55:29  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.3  2004/07/02 09:58:08  dkrajzew
// MeanData refactored (moved to microsim/output); numerical id for online routing added
//
// Revision 1.2  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSModel;
class MSSourceLaneChanger;
class MSEmitter;
class MSLink;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSSourceLane
 * Source lanes are lanes which are not accessable for vehicles which come from
 * other lanes. As some constraints change for this lane type, it is an extra
 * class but derived from MSLane.
 */
class MSSourceLane : public MSLane
{
public:
    /// Destructor.
    virtual ~MSSourceLane();

    /** Use this constructor only. Later use initialize to complete
        lane initialization. */
    MSSourceLane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge* edge, size_t numericalID, const Position2DVector &shape);

    /// Emit vehicle with speed 0 into lane if possible.
    virtual bool emit( MSVehicle& newVeh );

protected:
    /** Tries to emit veh into lane. there are four kind of possible
        emits that have to be handled differently: The line is empty,
        emission as last veh (in driving direction) (front insert),
        as first veh (back insert) and between a follower and a leader.
        True is returned for successful emission.
        Use this when the lane is empty */
    bool emitTry( MSVehicle& veh );

    /** Use this, when there is only a vehicle in front of the vehicle
        to insert */
    bool emitTry( MSVehicle& veh, VehCont::iterator leaderIt );

    /** Checks if there is enough space for emission and sets vehicle-state
        if there is. Common code used by emitTry-methods. Returns true if
        emission is possible. */
/*    bool enoughSpace( MSVehicle& veh,
                      double followPos, double leaderPos, double safeSpace );
*/
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/


#endif

// Local Variables:
// mode:C++
// End:
