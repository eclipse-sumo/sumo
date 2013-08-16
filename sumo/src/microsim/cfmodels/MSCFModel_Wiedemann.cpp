/****************************************************************************/
/// @file    MSCFModel_Wiedemann.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    June 2011
/// @version $Id$
///
// The psycho-physical model of Wiedemann
// references:
// Andre Stebens - Traffic simulation with the Wiedemann model
// Werner - Integration von Fahrzeugfolge- und Fahrstreifenwechselmodellen in die Nachtfahrsimulation LucidDrive
// Olstam, Tapani - Comparison of Car-following models
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

#include <cmath>
#include "MSCFModel_Wiedemann.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// static members
// ===========================================================================

// magic constant proposed by Wiedemann (based on real world measurements)
const SUMOReal MSCFModel_Wiedemann::D_MAX = 150;


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Wiedemann::MSCFModel_Wiedemann(const MSVehicleType* vtype,
        SUMOReal accel, SUMOReal decel,
        SUMOReal security, SUMOReal estimation) :
    MSCFModel(vtype, accel, decel, 1.0),
    mySecurity(security),
    myEstimation(estimation),
    myAX(vtype->getLength() + 1. + 2. * security),
    myCX(25. *(1. + security + estimation)),
    myMinAccel(0.2 * myAccel) { // +noise?
}


MSCFModel_Wiedemann::~MSCFModel_Wiedemann() {}


SUMOReal
MSCFModel_Wiedemann::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal vNext = MSCFModel::moveHelper(veh, vPos);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    vars->accelSign = vNext > veh->getSpeed() ? 1. : -1.;
    return vNext;
}


SUMOReal
MSCFModel_Wiedemann::followSpeed(const MSVehicle* const veh, SUMOReal /* speed */, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    return _v(veh, predSpeed, gap2pred);
}


SUMOReal
MSCFModel_Wiedemann::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    /* Wiedemann does not handle approaching junctions or stops very well:
     * regime approaching() fails when dv = 0 (i.e. a vehicle inserted with speed 0 does not accelerate to reach a stop)
     * for dv ~ 0 the standard decision tree will switch to following() which
     * does a lousy job of closing in on a stop / junction
     * hence we borrow from Krauss here
     */
    return MAX2(getSpeedAfterMaxDecel(speed), MIN2(krauss_vsafe(gap, 0), maxNextSpeed(speed, veh)));
}


SUMOReal
MSCFModel_Wiedemann::interactionGap(const MSVehicle* const , SUMOReal vL) const {
    UNUSED_PARAMETER(vL);
    return D_MAX;
}


MSCFModel*
MSCFModel_Wiedemann::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Wiedemann(vtype, myAccel, myDecel, mySecurity, myEstimation);
}


SUMOReal
MSCFModel_Wiedemann::_v(const MSVehicle* veh, SUMOReal predSpeed, SUMOReal gap) const {
    const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const SUMOReal dx = gap + myType->getLength(); // wiedemann uses brutto gap
    const SUMOReal v = veh->getSpeed();
    const SUMOReal vpref = veh->getMaxSpeed();
    const SUMOReal dv = v - predSpeed;
    const SUMOReal bx = myAX + (1 + 7 * mySecurity) * sqrt(v); // Harding propose a factor of  *.8 here
    const SUMOReal ex = 2 - myEstimation; // + RandHelper::randNorm(0.5, 0.15)
    const SUMOReal sdx = myAX + ex * (bx - myAX); /// the distance at which we drift out of following
    const SUMOReal sdv_root = (dx - myAX) / myCX;
    const SUMOReal sdv = sdv_root * sdv_root;
    const SUMOReal cldv = sdv * ex * ex;
    const SUMOReal opdv = cldv * (-1 - 2 * RandHelper::randNorm(0.5, 0.15));
    // select the regime, get new acceleration, compute new speed based
    SUMOReal accel;
    if (dx <= bx) {
        accel = emergency(dv, dx);
    } else if (dx < sdx) {
        if (dv > cldv) {
            accel = approaching(dv, dx, bx);
        } else if (dv > opdv) {
            accel = following(vars->accelSign);
        } else {
            accel = fullspeed(v, vpref, dx, bx);
        }
    } else {
        if (dv > sdv && dx < D_MAX) { //@note other versions have an disjunction instead of conjunction
            accel = approaching(dv, dx, bx);
        } else {
            accel = fullspeed(v, vpref, dx, bx);
        }
    }
    // since we have hard constrainst on accel we may as well use them here
    accel = MAX2(MIN2(accel, myAccel), -myDecel);
    const SUMOReal vNew = MAX2(SUMOReal(0), v + ACCEL2SPEED(accel)); // don't allow negative speeds
    return vNew;
}


SUMOReal
MSCFModel_Wiedemann::fullspeed(SUMOReal v, SUMOReal vpref, SUMOReal dx, SUMOReal bx) const {
    SUMOReal bmax = 0.2 + 0.8 * myAccel * (7 - sqrt(v));
    // if veh just drifted out of a 'following' process the acceleration is reduced
    SUMOReal accel = dx <= 2 * bx ? MIN2(myMinAccel, bmax * (dx - bx) / bx) : bmax;
    if (v > vpref) {
        accel = - accel;
    }
    return accel;
}


SUMOReal
MSCFModel_Wiedemann::following(SUMOReal sign) const {
    return myMinAccel * sign;
}


SUMOReal
MSCFModel_Wiedemann::approaching(SUMOReal dv, SUMOReal dx, SUMOReal bx) const {
    // there is singularity in the formula. we do the sanity check outside
    return 0.5 * dv * dv / (bx - dx); // + predAccel at t-reaction_time if this is value is above a treshold
}


SUMOReal
MSCFModel_Wiedemann::emergency(SUMOReal /* dv */, SUMOReal /* dx */) const {
    /* emergency according to A.Stebens
    // wiedemann assumes that dx will always be larger than myAX (sumo may
    // violate this assumption when crashing (-:
    if (dx > myAX) {
        SUMOReal accel = 0.5 * dv * dv / (myAX - dx); // + predAccel at t-reaction_time if this is value is above a treshold
        // one would assume that in an emergency accel must be negative. However the
        // wiedemann formula allows for accel = 0 whenever dv = 0
        assert(accel <= 0);
        return accel;
    } else {
        return = -myDecel;
    }
    */

    // emergency according to C.Werner
    return -myDecel;
}


SUMOReal
MSCFModel_Wiedemann::krauss_vsafe(SUMOReal gap, SUMOReal predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    const SUMOReal tauDecel = myDecel * myHeadwayTime;
    const SUMOReal speedReduction = ACCEL2SPEED(myDecel);
    const int predSteps = int(predSpeed / speedReduction);
    const SUMOReal leaderContrib = 2. * myDecel * (gap + SPEED2DIST(predSteps * predSpeed - speedReduction * predSteps * (predSteps + 1) / 2));
    return (SUMOReal)(-tauDecel + sqrt(tauDecel * tauDecel + leaderContrib));
}
