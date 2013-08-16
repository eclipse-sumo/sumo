/****************************************************************************/
/// @file    MSCFModel_Daniel1.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// The original Krauss (1998) car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_Daniel1.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Daniel1::MSCFModel_Daniel1(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
                                     SUMOReal dawdle, SUMOReal headwayTime,
                                     SUMOReal tmp1, SUMOReal tmp2, SUMOReal tmp3, SUMOReal tmp4, SUMOReal tmp5)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle), myTauDecel(decel* headwayTime),
      myTmp1(tmp1), myTmp2(tmp2), myTmp3(tmp3), myTmp4(tmp4), myTmp5(tmp5) {
}


MSCFModel_Daniel1::~MSCFModel_Daniel1() {}


SUMOReal
MSCFModel_Daniel1::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const SUMOReal vMin = getSpeedAfterMaxDecel(oldV);
    const SUMOReal vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
#ifdef _DEBUG
    if (vMin > vMax) {
        WRITE_WARNING("Vehicle's '" + veh->getID() + "' maximum speed is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    }
#endif
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}


SUMOReal
MSCFModel_Daniel1::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_Daniel1::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    return MIN2(_vsafe(gap, 0), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_Daniel1::dawdle(SUMOReal speed) const {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_Daniel1::_vsafe(SUMOReal gap, SUMOReal predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel
                                + sqrt(
                                    myTauDecel * myTauDecel
                                    + (predSpeed * predSpeed)
                                    + (2. * myDecel * gap)
                                ));
    assert(vsafe >= 0);
    return vsafe;
}


MSCFModel*
MSCFModel_Daniel1::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Daniel1(vtype, myAccel, myDecel, myDawdle, myHeadwayTime,
                                 myTmp1, myTmp2, myTmp3, myTmp4, myTmp5);
}
