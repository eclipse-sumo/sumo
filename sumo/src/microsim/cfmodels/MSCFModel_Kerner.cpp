/****************************************************************************/
/// @file    MSCFModel_Kerner.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id$
///
// car-following model by B. Kerner
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSCFModel_Kerner.h"
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Kerner::MSCFModel_Kerner(const MSVehicleType* vtype, SUMOReal accel,
                                   SUMOReal decel, SUMOReal headwayTime, SUMOReal k, SUMOReal phi)
    : MSCFModel(vtype, accel, decel, headwayTime), myK(k), myPhi(phi),
      myTauDecel(decel* headwayTime) {
}


MSCFModel_Kerner::~MSCFModel_Kerner() {}


SUMOReal
MSCFModel_Kerner::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal vNext = MSCFModel::moveHelper(veh, vPos);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    vars->rand = RandHelper::rand();
    return vNext;
}


SUMOReal
MSCFModel_Kerner::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return MIN2(_v(veh, speed, maxNextSpeed(speed, veh), gap, predSpeed), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_Kerner::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    return MIN2(_v(veh, speed, maxNextSpeed(speed, veh), gap, 0), maxNextSpeed(speed, veh));
}


MSCFModel::VehicleVariables*
MSCFModel_Kerner::createVehicleVariables() const {
    VehicleVariables* ret = new VehicleVariables();
    ret->rand = RandHelper::rand();
    return ret;
}


SUMOReal
MSCFModel_Kerner::_v(const MSVehicle* const veh, SUMOReal speed, SUMOReal vfree, SUMOReal gap, SUMOReal predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    // !!! in the following, the prior step is not considered!!!
    SUMOReal G = MAX2((SUMOReal) 0, (SUMOReal)(SPEED2DIST(myK * speed) + myPhi / myAccel * speed * (speed - predSpeed)));
    SUMOReal vcond = gap > G ? speed + ACCEL2SPEED(myAccel) : speed + MAX2(ACCEL2SPEED(-myDecel), MIN2(ACCEL2SPEED(myAccel), predSpeed - speed));
    SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel + sqrt(myTauDecel * myTauDecel + (predSpeed * predSpeed) + (2. * myDecel * gap)));
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    SUMOReal va = MAX2((SUMOReal) 0, MIN3(vfree, vsafe, vcond)) + vars->rand;
    SUMOReal v = MAX2((SUMOReal) 0, MIN4(vfree, va, speed + ACCEL2SPEED(myAccel), vsafe));
    return v;
}


MSCFModel*
MSCFModel_Kerner::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Kerner(vtype, myAccel, myDecel, myHeadwayTime, myK, myPhi);
}
