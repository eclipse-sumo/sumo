/****************************************************************************/
/// @file    MSCFModel_PWag2009.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id$
///
// Scalable model based on Krauﬂ by Peter Wagner
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
    const SUMOReal speed = veh->getSpeed();
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    // model should re-use acceleration from previous time-step:
    SUMOReal apref = vars->aOld;
    const SUMOReal asafe = SPEED2ACCEL(vNext - speed);
    if (apref <= asafe && RandHelper::rand() > myActionPointProbability * TS) {
        std::pair<MSVehicle* const, SUMOReal> l = veh->getLane()->getLeaderOnConsecutive(100., 0., speed, *veh, veh->getBestLanesContinuation(veh->getLane()));
        if (l.first) {
            apref = myDecelDivTau * (l.second + (l.first->getSpeed() - speed) * myHeadwayTime - speed * myHeadwayTime) / (speed + myTauDecel);
            apref += myDawdle * RandHelper::rand((SUMOReal) - 1., (SUMOReal)1.);
        }
    }
    if (apref > asafe) {
        apref = asafe;
    }
    vars->aOld = apref;  // save this value for the next time-step
    return MAX2((SUMOReal)0, speed + ACCEL2SPEED(apref));
}


SUMOReal
MSCFModel_PWag2009::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return _v(veh, speed, gap, predSpeed);
}


SUMOReal
MSCFModel_PWag2009::stopSpeed(const MSVehicle* const veh, SUMOReal gap) const {
    return _v(veh, veh->getSpeed(), gap, 0);
}


SUMOReal
MSCFModel_PWag2009::dawdle(SUMOReal speed) const {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}

// in addition, the parameters myTauLast, probAP, and sigmaAcc are needed; sigmaAcc can use myDawdle
// myTauLast might use the current time-step size, but this yields eventually an extreme model, I would be
// more careful and set it to something around 0.3 or 0.4, which are among the shortest headways I have
// seen so far in data ...
SUMOReal
MSCFModel_PWag2009::_v(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    const SUMOReal vsafe = -myTauLastDecel + sqrt(myTauLastDecel * myTauLastDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
    return MAX2((SUMOReal)0, vsafe);
}


MSCFModel*
MSCFModel_PWag2009::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_PWag2009(vtype, myAccel, myDecel, myDawdle, myHeadwayTime, myTauLastDecel / myDecel, myActionPointProbability);
}
