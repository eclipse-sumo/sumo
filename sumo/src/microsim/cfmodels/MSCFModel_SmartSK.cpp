/****************************************************************************/
/// @file    MSCFModel_SmartSK.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 05 Jun 2012
/// @version $Id:$
///
// A smarter SK
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "MSCFModel_SmartSK.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_SmartSK::MSCFModel_SmartSK(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
        SUMOReal dawdle, SUMOReal headwayTime, 
        SUMOReal tmp1, SUMOReal tmp2, SUMOReal tmp3, SUMOReal tmp4, SUMOReal tmp5)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle), myTauDecel(decel* headwayTime),
    myTmp1(tmp1), myTmp2(tmp2), myTmp3(tmp3), myTmp4(tmp4), myTmp5(tmp5)
{
}


MSCFModel_SmartSK::~MSCFModel_SmartSK() {}


SUMOReal
MSCFModel_SmartSK::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vSafe - oldV));
    const SUMOReal vMin = MAX2((SUMOReal) 0, oldV - ACCEL2SPEED(myDecel));
    const SUMOReal vMax = MIN3(veh->getLane()->getMaxSpeed(), maxNextSpeed(oldV), vSafe);
#ifdef _DEBUG
    if (vMin > vMax) {
        WRITE_WARNING("Vehicle's '" + veh->getID() + "' maximum speed is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    }
#endif
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}


SUMOReal
MSCFModel_SmartSK::followSpeed(const MSVehicle* const /*veh*/, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return MAX2(getSpeedAfterMaxDecel(speed), MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed)));
}


SUMOReal
MSCFModel_SmartSK::stopSpeed(const MSVehicle* const veh, SUMOReal gap) const {
    return MAX2(getSpeedAfterMaxDecel(veh->getSpeed()), MIN2(_vsafe(gap, 0), maxNextSpeed(veh->getSpeed())));
}


SUMOReal
MSCFModel_SmartSK::dawdle(SUMOReal speed) const {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_SmartSK::_vsafe(SUMOReal gap, SUMOReal predSpeed) const {
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
MSCFModel_SmartSK::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_SmartSK(vtype, myAccel, myDecel, myDawdle, myHeadwayTime,
        myTmp1, myTmp2, myTmp3, myTmp4, myTmp5);
}
