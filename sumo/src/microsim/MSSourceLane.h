/****************************************************************************/
/// @file    MSSourceLane.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// Same as MSLane, but with another emission
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
#ifndef MSSourceLane_h
#define MSSourceLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

// ===========================================================================
// class declarations
// ===========================================================================
class MSModel;
class MSSourceLaneChanger;
class MSLink;


// ===========================================================================
// class definitions
// ===========================================================================
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
    MSSourceLane(/*MSNet &net, */std::string id, SUMOReal maxSpeed,
                                 SUMOReal length, MSEdge* edge, size_t numericalID,
                                 const Position2DVector &shape,
                                 const std::vector<SUMOVehicleClass> &allowed,
                                 const std::vector<SUMOVehicleClass> &disallowed);


    /** @brief Emits the given vehicle
     *
     * @param[in] v The vehicle to emit
     * @param[in] isReinsertion Whether the vehicle was already emitted
     * @return Whether the vehicle could be emitted
     * @see MSLane::emit
     * @deprecated This method should be unnecessary with complete specification implementation
     */
    virtual bool emit(MSVehicle& newVeh, bool isReinsertion=false) throw();

protected:
    /** Tries to emit veh into lane. there are four kind of possible
        emits that have to be handled differently: The line is empty,
        emission as last veh (in driving direction) (front insert),
        as first veh (back insert) and between a follower and a leader.
        True is returned for successful emission.
        Use this when the lane is empty */
    bool emitTry(MSVehicle& veh);

    /** Use this, when there is only a vehicle in front of the vehicle
        to insert */
    bool emitTry(MSVehicle& veh, VehCont::iterator leaderIt);

    /** Checks if there is enough space for emission and sets vehicle-state
        if there is. Common code used by emitTry-methods. Returns true if
        emission is possible. */
    /*    bool enoughSpace( MSVehicle& veh,
                          SUMOReal followPos, SUMOReal leaderPos, SUMOReal safeSpace );
    */
};


#endif

/****************************************************************************/

