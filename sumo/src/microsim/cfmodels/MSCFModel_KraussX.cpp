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
#include <microsim/MSNet.h>
#include "MSCFModel_KraussX.h"


#define OVERBRAKING_THRESHOLD -3

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussX::MSCFModel_KraussX(const MSVehicleType* vtype, double accel, double decel,
                                     double emergencyDecel, double apparentDecel,
                                     double dawdle, double headwayTime,
                                     double tmp1, double tmp2):
    MSCFModel_Krauss(vtype, accel, decel, emergencyDecel, apparentDecel, dawdle, headwayTime),
    myTmp1(tmp1),
    myTmp2(tmp2) {
}


MSCFModel_KraussX::~MSCFModel_KraussX() {}


MSCFModel*
MSCFModel_KraussX::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussX(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, myDawdle, myHeadwayTime, myTmp1, myTmp2);
}


double
MSCFModel_KraussX::moveHelper(MSVehicle* const veh, double vPos) const {
    const double oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const double vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    double vMin, vNext;
    const double vMax = MIN3(veh->getMaxSpeedOnLane(), maxNextSpeed(oldV, veh), vSafe);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // we do not rely on never braking harder than maxDecel because TraCI or strange cf models may decide to do so
        vMin = MIN2(getSpeedAfterMaxDecel(oldV), vMax);
        const double vDawdle = dawdleX(oldV, vMin, vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vDawdle, vMax, *this);
        //std::cout << SIMTIME << " veh=" << veh->getID()
        //    << " vOld=" << oldV << " vPos=" << vPos << " vSafe=" << vSafe
        //    << " vMax=" << vMax << " vMin=" << vMin << " vDawdle=" << vDawdle << " vNext=" << vNext
        //    << "\n";
    } else {
        // for ballistic update, negative vnext must be allowed to
        // indicate a stop within the coming timestep (i.e., to attain negative values)
        vMin =  MIN2(minNextSpeed(oldV, veh), vMax);
        const double vDawdle = dawdleX(oldV, vMin, vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vDawdle, vMax, *this);
        // (Leo) moveHelper() is responsible for assuring that the next
        // velocity is chosen in accordance with maximal decelerations.
        // At this point vNext may also be negative indicating a stop within next step.
        // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
        // vNext can be a large negative value at this point. We cap vNext here.
        vNext = MAX2(vNext, vMin);
    }
    return vNext;
}


double
MSCFModel_KraussX::dawdleX(double vOld, double vMin, double vMax) const {
    double speed = vMax;
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        // in case of the ballistic update, negative speeds indicate
        // a desired stop before the completion of the next timestep.
        // We do not allow dawdling to overwrite this indication
        if (speed < 0) {
            return speed;
        }
    }
    // extra slow to start
    if (vOld < myAccel) {
        speed -= ACCEL2SPEED(myTmp1 * myAccel);
    }
    const double random = RandHelper::rand();
    speed -= ACCEL2SPEED(myDawdle * myAccel * random);
    speed = MAX2(vMin, speed);
    // overbraking
    if (vOld > vMax) {
        speed -= ACCEL2SPEED(myTmp2 * myAccel * random);
        //std::cout << " vMin=" << vMin << " vMax=" << vMax << "speed=" << speed << " d1=" << ACCEL2SPEED(myDawdle * myAccel * random) << " d2=" << ACCEL2SPEED(myTmp2 * myAccel * random) << " unexpectedDecel=" << (speed < vMin) << "\n";
        if (MSGlobals::gSemiImplicitEulerUpdate) {
            speed = MAX2(0.0, speed);
        }
    }
    return speed;
}



/****************************************************************************/
