/****************************************************************************/
/// @file    MSCFModel_PWag2009.cpp
/// @author  Peter Wagner
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id$
///
// Scalable model based on Krauss by Peter Wagner
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
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
MSCFModel_PWag2009::MSCFModel_PWag2009(const MSVehicleType* vtype,  double accel, double decel,
                                       double emergencyDecel,
                                       double dawdle, double headwayTime, double tauLast, double apProb) :
    MSCFModel(vtype, accel, decel, emergencyDecel, decel, headwayTime), myDawdle(dawdle),
    myTauDecel(decel * headwayTime),
    myDecelDivTau(decel / headwayTime),
    myTauLastDecel(decel * tauLast),
    myActionPointProbability(apProb) {
}


MSCFModel_PWag2009::~MSCFModel_PWag2009() {}


double
MSCFModel_PWag2009::moveHelper(MSVehicle* const veh, double vPos) const {
    const double vNext = MSCFModel::moveHelper(veh, vPos);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    double apref = SPEED2ACCEL(vNext - veh->getSpeed());
    vars->aOld = apref;
    return vNext;
}

// in addition, the parameters myTauLast, probAP, and sigmaAcc are needed; sigmaAcc can use myDawdle
// myTauLast might use the current time-step size, but this yields eventually an extreme model, I would be
// more careful and set it to something around 0.3 or 0.4, which are among the shortest headways I have
// seen so far in data ...

double
MSCFModel_PWag2009::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double /*predMaxDecel*/) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    const double vsafe = -myTauLastDecel + sqrt(myTauLastDecel * myTauLastDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
    const double asafe = SPEED2ACCEL(vsafe - speed);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    double apref = vars->aOld;
    if (apref <= asafe && RandHelper::rand() <= myActionPointProbability * TS) {
        apref = myDecelDivTau * (gap + (predSpeed - speed) * myHeadwayTime - speed * myHeadwayTime) / (speed + myTauDecel);
        apref = MIN2(apref, myAccel);
        apref = MAX2(apref, -myDecel);
        apref += myDawdle * RandHelper::rand((double) - 1., (double)1.);
    }
    if (apref > asafe) {
        apref = asafe;
    }
    return MAX2(0., speed + ACCEL2SPEED(apref));
}

// uses the safe speed and preferred acceleration with the same NORMAL tau to compute stopSpeed
double
MSCFModel_PWag2009::stopSpeed(const MSVehicle* const /* veh */, const double speed, double gap) const {
    if (gap < 0.01) {
        return 0.;
    }
    const double vsafe = -myTauDecel + sqrt(myTauDecel * myTauDecel +  2.0 * myDecel * gap);
    const double asafe = SPEED2ACCEL(vsafe - speed);
//    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    double apref = myDecelDivTau * (gap  - 2 * speed * myHeadwayTime) / (speed + myTauDecel);
    if (apref <= asafe) {
        apref = MIN2(apref, myAccel);
        apref = MAX2(apref, -myDecel);
    } else {
        apref = asafe;
    }
    return MAX2(0., vsafe + ACCEL2SPEED(apref));
}

// this method should not do anything, since followSpeed() has taken care of dawdling already...
double
MSCFModel_PWag2009::dawdle(double speed) const {
    return speed;
//    return MAX2(0., speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}

// eventually, this method isn't needed anymore
//double
//MSCFModel_PWag2009::_v(const MSVehicle* const veh, double speed, double gap, double predSpeed) const {
//    if (predSpeed == 0 && gap < 0.01) {
//        return 0;
//    }
//    const double vsafe = -myTauLastDecel + sqrt(myTauLastDecel * myTauLastDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
//  const double asafe = SPEED2ACCEL(vsafe - speed);
//    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
//  double apref = vars->aOld;
//  if (apref <= asafe && RandHelper::rand() <= myActionPointProbability * TS) {
//    apref = myDecelDivTau * (gap + (predSpeed - speed) * myHeadwayTime - speed * myHeadwayTime) / (speed + myTauDecel);
//    if (apref>myAccel)
//      apref = myAccel;
//    if (apref<-myDecel)
//      apref = -myDecel;
//    apref += myDawdle * RandHelper::rand((double) - 1., (double)1.);
//  }
//  if (apref > asafe)
//    apref = asafe;
//    return MAX2(0, vsafe+ACCEL2SPEED(apref));
//}
//

MSCFModel*
MSCFModel_PWag2009::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_PWag2009(vtype, myAccel, myDecel, myEmergencyDecel, myDawdle, myHeadwayTime, myTauLastDecel / myDecel, myActionPointProbability);
}
