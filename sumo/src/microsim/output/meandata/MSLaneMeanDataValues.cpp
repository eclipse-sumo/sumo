//---------------------------------------------------------------------------//
//                        MSLaneMeanDataValues.cpp -
//  Storage for a lane's mean values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include "MSLaneMeanDataValues.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
MSLaneMeanDataValues::addVehicleData(double contTimesteps,
                                     unsigned discreteTimesteps,
                                     double speedSum,
                                     double speedSquareSum,
                                     bool hasFinishedEntireLane,
                                     bool hasLeftLane,
                                     bool hasEnteredLane,
                                     double travelTimesteps)
{
    if ( hasFinishedEntireLane ) {
        nVehEntireLane       += hasFinishedEntireLane;
        traveltimeStepSum    += travelTimesteps;
        assert( hasLeftLane );
    }

    assert(contTimesteps>=0);
    nVehContributed      += 1;
    nVehLeftLane         += hasLeftLane;
    nVehEnteredLane      += hasEnteredLane;
    contTimestepSum      += contTimesteps;
    discreteTimestepSum  += discreteTimesteps;
    speedSum             += speedSum;
    speedSquareSum       += speedSquareSum;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
