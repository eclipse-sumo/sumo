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
      myAdaptationFactor(1.), myAdaptationTime(0.),
      myIterations(MAX2(1, int(TS / internalStepping + .5))),
      myTwoSqrtAccelDecel(SUMOReal(2 * sqrt(accel * decel))) {
}


MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype,
                             SUMOReal accel, SUMOReal decel,
                             SUMOReal headwayTime,
                             SUMOReal adaptationFactor, SUMOReal adaptationTime,
                             SUMOReal internalStepping)
    : MSCFModel(vtype, accel, decel, headwayTime), myDelta(4.),
      myAdaptationFactor(adaptationFactor), myAdaptationTime(adaptationTime),
      myIterations(MAX2(1, int(TS / internalStepping + .5))),
      myTwoSqrtAccelDecel(SUMOReal(2 * sqrt(accel * decel))) {
}


MSCFModel_IDM::~MSCFModel_IDM() {}


SUMOReal
MSCFModel_IDM::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal vNext = MSCFModel::moveHelper(veh, vPos);
    if (myAdaptationFactor != 1.) {
        VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        vars->levelOfService += (vNext / veh->getLane()->getVehicleMaxSpeed(veh) - vars->levelOfService) / myAdaptationTime * TS;
    }
    return vNext;
}


SUMOReal
MSCFModel_IDM::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return _v(veh, gap2pred, speed, predSpeed, veh->getLane()->getVehicleMaxSpeed(veh));
}


SUMOReal
MSCFModel_IDM::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const {
    if (gap2pred < 0.01) {
        return 0;
    }
    return _v(veh, gap2pred, speed, 0, veh->getLane()->getVehicleMaxSpeed(veh), false);
}


/// @todo update interactionGap logic to IDM
SUMOReal
MSCFModel_IDM::interactionGap(const MSVehicle* const veh, SUMOReal vL) const {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const SUMOReal acc = myAccel * (1. - pow(veh->getSpeed() / veh->getLane()->getVehicleMaxSpeed(veh), myDelta));
    const SUMOReal vNext = veh->getSpeed() + acc;
    const SUMOReal gap = (vNext - vL) * (veh->getSpeed() + vL) / (2 * myDecel) + vL;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel_IDM::_v(const MSVehicle* const veh, const SUMOReal gap2pred, const SUMOReal egoSpeed,
                  const SUMOReal predSpeed, const SUMOReal desSpeed, const bool respectMinGap) const {
// this is more or less based on http://www.vwi.tu-dresden.de/~treiber/MicroApplet/IDM.html
// and http://arxiv.org/abs/cond-mat/0304337
// we assume however constant speed for the leader
    SUMOReal headwayTime = myHeadwayTime;
    if (myAdaptationFactor != 1.) {
        const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        headwayTime *= myAdaptationFactor + vars->levelOfService * (1. - myAdaptationFactor);
    }
    SUMOReal newSpeed = egoSpeed;
    SUMOReal gap = gap2pred;
    for (int i = 0; i < myIterations; i++) {
        const SUMOReal delta_v = newSpeed - predSpeed;
        SUMOReal s = MAX2(SUMOReal(0), newSpeed * headwayTime + newSpeed * delta_v / myTwoSqrtAccelDecel);
        if (respectMinGap) {
            s += myType->getMinGap();
        }
        const SUMOReal acc = myAccel * (1. - pow(newSpeed / desSpeed, myDelta) - (s * s) / (gap * gap));
        newSpeed += ACCEL2SPEED(acc) / myIterations;
        //TODO use more realistic position update which takes accelerated motion into account
        gap -= MAX2(SUMOReal(0), SPEED2DIST(newSpeed - predSpeed) / myIterations);
    }
//    return MAX2(getSpeedAfterMaxDecel(egoSpeed), newSpeed);
    return MAX2(SUMOReal(0), newSpeed);
}


MSCFModel*
MSCFModel_IDM::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_IDM(vtype, myAccel, myDecel, myHeadwayTime, myDelta, TS / myIterations);
}
