/****************************************************************************/
/// @file    MSCFModel_Krauss.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, with acceleration decrease and faster start
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
#include "MSCFModel_Krauss.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel_KraussOrig1(vtype, accel, decel, dawdle, headwayTime) {
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


SUMOReal
MSCFModel_Krauss::followSpeed(const MSVehicle* const /*veh*/, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    return MAX2(getSpeedAfterMaxDecel(speed), MIN2(_vsafe(gap, predSpeed, predMaxDecel), maxNextSpeed(speed)));
}


SUMOReal
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, SUMOReal gap) const {
    return MAX2(getSpeedAfterMaxDecel(veh->getSpeed()), MIN2(_vsafe(gap, 0, 0), maxNextSpeed(veh->getSpeed())));
}


SUMOReal
MSCFModel_Krauss::dawdle(SUMOReal speed) const {
    // generate random number out of [0,1]
    SUMOReal random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(myDawdle * speed * random);
    } else {
        speed -= ACCEL2SPEED(myDawdle * myAccel * random);
    }
    return MAX2(SUMOReal(0), speed);
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel_Krauss::_vsafe(SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    if (predSpeed == 0) {
        if (gap < 0.01) {
            return 0;
        }
        return (SUMOReal)(-myTauDecel + sqrt(myTauDecel * myTauDecel + 2. * myDecel * gap));
    }
    if (predMaxDecel == 0) {
        return (SUMOReal)(-myTauDecel + sqrt(myTauDecel * myTauDecel + predSpeed * predSpeed + 2. * myDecel * gap));
    }
    const SUMOReal speedReduction = ACCEL2SPEED(predMaxDecel);
    const int predSteps = int(predSpeed / speedReduction);
    const SUMOReal leaderContrib = 2. * myDecel * (gap + SPEED2DIST(predSteps * predSpeed - speedReduction * predSteps * (predSteps + 1) / 2));
    return (SUMOReal)(-myTauDecel + sqrt(myTauDecel * myTauDecel + leaderContrib));
}


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


//void MSCFModel::saveState(std::ostream &os) {}

