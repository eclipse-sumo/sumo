/****************************************************************************/
/// @file    MSCFModel_PWag2009.cpp
/// @author  Peter Wagner
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id$
///
// Scalable model based on Krauﬂ by Peter Wagner
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSCFModel_PWag2009.h"
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_PWag2009::MSCFModel_PWag2009(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
                                       SUMOReal dawdle, SUMOReal headwayTime, SUMOReal tauLast, SUMOReal apProb)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle),
      myTauDecel(decel* headwayTime), myDecelDivTau(decel / headwayTime), myTauLastDecel(decel* tauLast),
      myActionPointProbability(apProb) {
}


MSCFModel_PWag2009::~MSCFModel_PWag2009() {}


SUMOReal
MSCFModel_PWag2009::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal vNext = MSCFModel::moveHelper(veh, vPos);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    SUMOReal apref = SPEED2ACCEL(vNext - veh->getSpeed());
    vars->aOld = apref;
    return vNext;
}

// in addition, the parameters myTauLast, probAP, and sigmaAcc are needed; sigmaAcc can use myDawdle
// myTauLast might use the current time-step size, but this yields eventually an extreme model, I would be
// more careful and set it to something around 0.3 or 0.4, which are among the shortest headways I have
// seen so far in data ...

SUMOReal
MSCFModel_PWag2009::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    const SUMOReal vsafe = -myTauLastDecel + sqrt(myTauLastDecel * myTauLastDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
    const SUMOReal asafe = SPEED2ACCEL(vsafe - speed);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    SUMOReal apref = vars->aOld;
    if (apref <= asafe && RandHelper::rand() <= myActionPointProbability * TS) {
        apref = myDecelDivTau * (gap + (predSpeed - speed) * myHeadwayTime - speed * myHeadwayTime) / (speed + myTauDecel);
        apref = MIN2(apref, myAccel);
        apref = MAX2(apref, -myDecel);
        apref += myDawdle * RandHelper::rand((SUMOReal) - 1., (SUMOReal)1.);
    }
    if (apref > asafe) {
        apref = asafe;
    }
    return MAX2((SUMOReal)0, speed + ACCEL2SPEED(apref));
}

// uses the safe speed and preferred acceleration with the same NORMAL tau to compute stopSpeed
SUMOReal
MSCFModel_PWag2009::stopSpeed(const MSVehicle* const /* veh */, const SUMOReal speed, SUMOReal gap) const {
    if (gap < 0.01) {
        return 0;
    }
    const SUMOReal vsafe = -myTauDecel + sqrt(myTauDecel * myTauDecel +  2.0 * myDecel * gap);
    const SUMOReal asafe = SPEED2ACCEL(vsafe - speed);
//    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    SUMOReal apref = myDecelDivTau * (gap  - 2 * speed * myHeadwayTime) / (speed + myTauDecel);
    if (apref <= asafe) {
        apref = MIN2(apref, myAccel);
        apref = MAX2(apref, -myDecel);
    } else {
        apref = asafe;
    }
    return MAX2((SUMOReal)0, vsafe + ACCEL2SPEED(apref));
}

// this method should not do anything, since followSpeed() has taken care of dawdling already...
SUMOReal
MSCFModel_PWag2009::dawdle(SUMOReal speed) const {
    return speed;
//    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}

// eventually, this method isn't needed anymore
//SUMOReal
//MSCFModel_PWag2009::_v(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const {
//    if (predSpeed == 0 && gap < 0.01) {
//        return 0;
//    }
//    const SUMOReal vsafe = -myTauLastDecel + sqrt(myTauLastDecel * myTauLastDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
//	const SUMOReal asafe = SPEED2ACCEL(vsafe - speed);
//    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
//	SUMOReal apref = vars->aOld;
//	if (apref <= asafe && RandHelper::rand() <= myActionPointProbability * TS) {
//		apref = myDecelDivTau * (gap + (predSpeed - speed) * myHeadwayTime - speed * myHeadwayTime) / (speed + myTauDecel);
//		if (apref>myAccel)
//			apref = myAccel;
//		if (apref<-myDecel)
//			apref = -myDecel;
//		apref += myDawdle * RandHelper::rand((SUMOReal) - 1., (SUMOReal)1.);
//	}
//	if (apref > asafe)
//		apref = asafe;
//    return MAX2((SUMOReal)0, vsafe+ACCEL2SPEED(apref));
//}
//

MSCFModel*
MSCFModel_PWag2009::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_PWag2009(vtype, myAccel, myDecel, myDawdle, myHeadwayTime, myTauLastDecel / myDecel, myActionPointProbability);
}
