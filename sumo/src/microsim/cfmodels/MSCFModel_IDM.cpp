/****************************************************************************/
/// @file    MSCFModel_IDM.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The Intelligent Driver Model (IDM) car-following model
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

#include "MSCFModel_IDM.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype,
                             SUMOReal accel, SUMOReal decel,
                             SUMOReal headwayTime, SUMOReal delta,
                             SUMOReal internalStepping)
    : MSCFModel(vtype, accel, decel, headwayTime), myDelta(delta),
      myAdaptationFactor(1.), myAdaptationTime(0.), myExpFactor(0),
      myIterations(MAX2(1, int(TS / internalStepping + .5))),
      myTwoSqrtAccelDecel(SUMOReal(2 * sqrt(accel* decel))) {
}


MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype,
                             SUMOReal accel, SUMOReal decel,
                             SUMOReal headwayTime,
                             SUMOReal adaptationFactor, SUMOReal adaptationTime,
                             SUMOReal internalStepping)
    : MSCFModel(vtype, accel, decel, headwayTime), myDelta(4.),
      myAdaptationFactor(adaptationFactor), myAdaptationTime(adaptationTime),
      myExpFactor(exp(-TS / adaptationTime)),
      myIterations(MAX2(1, int(TS / internalStepping + .5))),
      myTwoSqrtAccelDecel(SUMOReal(2 * sqrt(accel* decel))) {
}


MSCFModel_IDM::~MSCFModel_IDM() {}


SUMOReal
MSCFModel_IDM::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal vNext = MSCFModel::moveHelper(veh, vPos);
    if (myExpFactor > 0.) {
        VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        vars->levelOfService *= myExpFactor;
        vars->levelOfService += vNext / desiredSpeed(veh) * myAdaptationTime * (1. - myExpFactor);
    }
    return vNext;
}


SUMOReal
MSCFModel_IDM::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return _v(veh, gap2pred, speed, predSpeed, desiredSpeed(veh));
}


SUMOReal
MSCFModel_IDM::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const {
    if (gap2pred < 0.01) {
        return 0;
    }
    return _v(veh, gap2pred, speed, 0, desiredSpeed(veh));
}


/// @todo update interactionGap logic to IDM
SUMOReal
MSCFModel_IDM::interactionGap(const MSVehicle* const veh, SUMOReal vL) const {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal acc = myAccel * (1. - pow(veh->getSpeed() / desiredSpeed(veh), myDelta));
    SUMOReal vNext = veh->getSpeed() + acc;
    SUMOReal gap = (vNext - vL) * (veh->getSpeed() + vL) / (2 * myDecel) + vL;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel_IDM::_v(const MSVehicle* const veh, SUMOReal gap2pred, SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal desSpeed) const {
    SUMOReal headwayTime = myHeadwayTime;
    if (myExpFactor > 0.) {
        const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        headwayTime *= myAdaptationFactor + vars->levelOfService * (1. - myAdaptationFactor);
    }
    for (int i = 0; i < myIterations; i++) {
        const SUMOReal delta_v = egoSpeed - predSpeed;
        const SUMOReal s = myType->getMinGap() + MAX2(SUMOReal(0), egoSpeed * headwayTime + egoSpeed * delta_v / myTwoSqrtAccelDecel);
        const SUMOReal acc = myAccel * (1. - pow(egoSpeed / desSpeed, myDelta) - (s * s) / (gap2pred * gap2pred));
        egoSpeed += ACCEL2SPEED(acc) / myIterations;
        gap2pred -= MAX2(SUMOReal(0), SPEED2DIST(egoSpeed - predSpeed) / myIterations);
    }
    return MAX2(SUMOReal(0), egoSpeed);
}


MSCFModel*
MSCFModel_IDM::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_IDM(vtype, myAccel, myDecel, myHeadwayTime, myDelta, TS / myIterations);
}
