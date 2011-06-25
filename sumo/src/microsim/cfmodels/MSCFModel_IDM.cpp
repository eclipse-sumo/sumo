/****************************************************************************/
/// @file    MSCFModel_IDM.cpp
/// @author  Tobias Mayer
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The Intelligent Driver Model (IDM) car-following model
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
                             SUMOReal tau, SUMOReal delta,
                             SUMOReal internalStepping)
        : MSCFModel(vtype, accel, decel, tau), myDelta(delta),
        myTwoSqrtAccelDecel(SUMOReal(2*sqrt(accel*decel))), myIterations(MAX2(1, int(TS/internalStepping + .5))) {
}


MSCFModel_IDM::~MSCFModel_IDM() {}


SUMOReal
MSCFModel_IDM::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const {
    return _updateSpeed(gap2pred, speed, predSpeed, desiredSpeed(veh));
}


SUMOReal
MSCFModel_IDM::ffeS(const MSVehicle * const veh, SUMOReal gap2pred) const {
    if (gap2pred<0.01) {
        return 0;
    }
    return _updateSpeed(gap2pred, veh->getSpeed(), 0, desiredSpeed(veh));
}


/// @todo update interactionGap logic to IDM
SUMOReal
MSCFModel_IDM::interactionGap(const MSVehicle * const veh, SUMOReal vL) const {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal acc = myAccel * (1. - pow(veh->getSpeed()/desiredSpeed(veh), myDelta));
    SUMOReal vNext = veh->getSpeed() + acc;
    SUMOReal gap = (vNext - vL) * (veh->getSpeed() + vL) / (2*myDecel) + vL;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}

 
SUMOReal
MSCFModel_IDM::_updateSpeed(SUMOReal gap2pred, SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal desSpeed) const {
    gap2pred += myType->getMinGap();
    for (int i = 0; i < myIterations; i++) {
        const SUMOReal delta_v = egoSpeed - predSpeed;
        const SUMOReal s = myType->getMinGap() + MAX2(SUMOReal(0), egoSpeed*myTau + egoSpeed*delta_v/myTwoSqrtAccelDecel);
        const SUMOReal acc = myAccel * (1. - pow(egoSpeed / desSpeed, myDelta) - (s*s)/(gap2pred*gap2pred));
        egoSpeed += ACCEL2SPEED(acc)/myIterations;
        gap2pred -= SPEED2DIST(egoSpeed)/myIterations;
    }
    return MAX2(SUMOReal(0), egoSpeed);
}


MSCFModel *
MSCFModel_IDM::duplicate(const MSVehicleType *vtype) const {
    return new MSCFModel_IDM(vtype, myAccel, myDecel, myTau, myDelta, TS/myIterations);
}
