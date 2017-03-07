/****************************************************************************/
/// @file    MSCFModel_KraussX.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, changing accel and speed by slope
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/MSVehicle.h>
#include "MSCFModel_KraussX.h"


#define OVERBRAKING_THRESHOLD -3

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussX::MSCFModel_KraussX(const MSVehicleType* vtype, double accel, double decel,
                                       double dawdle, double headwayTime, double tmp1): 
    MSCFModel_Krauss(vtype, accel, decel, dawdle, headwayTime),
    myTmp1(tmp1)
{
}


MSCFModel_KraussX::~MSCFModel_KraussX() {}


MSCFModel*
MSCFModel_KraussX::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussX(vtype, myAccel, myDecel, myDawdle, myHeadwayTime, myTmp1);
}


double
MSCFModel_KraussX::moveHelper(MSVehicle* const veh, double vPos) const {
    const double oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const double vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const double vMin = minNextSpeed(oldV, veh);
    // do not exceed max decel even if it is unsafe
    double vMax = MAX2(vMin,
                         MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe));
#ifdef _DEBUG
    //if (vMin > vMax) {
    //    WRITE_WARNING("Maximum speed of vehicle '" + veh->getID() + "' is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    //}
#endif

    const double vDawdle = MAX2(vMin, dawdle(oldV, vMax));

    double vNext = veh->getLaneChangeModel().patchSpeed(vMin, vDawdle, vMax, *this);

#ifdef DEBUG_EXECUTE_MOVE
    if DEBUG_COND {
    std::cout << "\nMOVE_HELPER\n"
    << "veh '" << veh->getID() << "' vMin=" << vMin
        << " vMax=" << vMax << " vDawdle=" << vDawdle
        << " vSafe" << vSafe << " vNext=" << vNext
        << "\n";
    }
#endif

    // (Leo) At this point vNext may also be negative indicating a stop within next step.
    // This would have resulted from a call to maximumSafeStopSpeed(), which does not
    // consider deceleration bounds. Therefore, we cap vNext here.
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        vNext = MAX2(vNext, veh->getSpeed() - ACCEL2SPEED(getMaxDecel()));
    }

    return vNext;
}


double
MSCFModel_KraussX::dawdle(double vOld, double speed) const {
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        // in case of the ballistic update, negative speeds indicate
        // a desired stop before the completion of the next timestep.
        // We do not allow dawdling to overwrite this indication
        if (speed < 0) {
            return speed;
        }
    }
    if (vOld < myAccel) {
        speed -= ACCEL2SPEED(myTmp1 * myAccel);
    }
    return MSCFModel_Krauss::dawdle(MAX2(0.0, speed));
}



/****************************************************************************/
