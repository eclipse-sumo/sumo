/****************************************************************************/
/// @file    MSCFModel_Wiedemann.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// The psycho-physical model of Wiedemann
// references:
// Andre Stebens - Traffic simulation with the Wiedemann model
// Werner - Integration von Fahrzeugfolge- und Fahrstreifenwechselmodellen in die Nachtfahrsimulation LucidDrive
// Olstam, Tapani - Comparison of Car-following models
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
            SUMOReal security, SUMOReal estimation) throw() : 
    MSCFModel(vtype, decel),
    myAccel(accel),
    mySecurity(security),
    myEstimation(estimation),
    myAX(vtype->getLengthWithGap() - vtype->getMinGap() + 1 + 2 * security),
    myCX(25 * (1 + security + estimation)),
    myMinAccel(0.2 * myAccel) // +noise?
{
}


MSCFModel_Wiedemann::~MSCFModel_Wiedemann() throw() {}


SUMOReal
MSCFModel_Wiedemann::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const throw() {
    return _v(veh, predSpeed, gap2pred);
}


SUMOReal
MSCFModel_Wiedemann::ffeV(const MSVehicle * const veh, SUMOReal gap2pred, SUMOReal predSpeed) const throw() {
    return _v(veh, predSpeed, gap2pred);
}


SUMOReal
MSCFModel_Wiedemann::ffeV(const MSVehicle * const veh, const MSVehicle * const pred) const throw() {
    return _v(veh, pred->getSpeed(), veh->gap2pred(*pred));
}


SUMOReal
MSCFModel_Wiedemann::ffeS(const MSVehicle * const veh, SUMOReal gap2pred) const throw() {
    return _v(veh, myType->getMaxSpeed(), gap2pred);
}


SUMOReal 
MSCFModel_Wiedemann::interactionGap(const MSVehicle * const , SUMOReal vL) const throw() {
    UNUSED_PARAMETER(vL);
    return D_MAX;
}


MSCFModel *
MSCFModel_Wiedemann::duplicate(const MSVehicleType *vtype) const throw() {
    return new MSCFModel_Wiedemann(vtype, myAccel, myDecel, mySecurity, myEstimation);
}


SUMOReal 
MSCFModel_Wiedemann::_v(const MSVehicle *veh, SUMOReal predSpeed, SUMOReal dx) const {
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const SUMOReal v = veh->getSpeed();
    const SUMOReal vpref = veh->getMaxSpeed();
    const SUMOReal dv = v - predSpeed;
    const SUMOReal bx = myAX + (1 + 7 * mySecurity) * sqrt(v) *.8; // the 0.8 factor is a commonly used modification by Harding
    const SUMOReal ex = 2 - myEstimation; // + RandHelper::randNorm(0.5, 0.15)
    const SUMOReal sdx = myAX + ex * (bx - myAX); /// the distance at which we drift out of following
    const SUMOReal sdv_root = (dx - myAX) / myCX;
    const SUMOReal sdv = sdv_root * sdv_root;
    const SUMOReal cldv = sdv * ex * ex;
    const SUMOReal opdv = cldv * (-1 - 2* RandHelper::randNorm(0.5, 0.15));
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
        if (dv > sdv && dx > D_MAX) { //@note other versions have an disjunction instead of conjunction
            accel = fullspeed(v, vpref, dx, bx);
        } else {
            accel = approaching(dv, dx, bx);
        }
    }
    vars->accelSign = accel > 0 ? 1 : -1;
    return v + ACCEL2SPEED(accel);
}


SUMOReal 
MSCFModel_Wiedemann::fullspeed(SUMOReal v, SUMOReal vpref, SUMOReal dx, SUMOReal bx) const {
    SUMOReal bmax = 0.2 + 0.8 * myAccel * (7 - sqrt(v));
    // if veh just drifted out of a 'following' process the acceleration is reduced
    SUMOReal accel = dx<=2*bx ? MIN2(myMinAccel, bmax * (dx-bx) / bx) : bmax;
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
    SUMOReal accel = 0.5 * dv * dv / (bx - dx); // + predAccel at t-reaction_time if this is value is above a treshold
    // there are different ways to handel the singularity in the above formula ...
    // since we have hard constrainst on accel we may as well use them here
    return MAX2(MIN2(accel, myAccel), -myDecel);
}


SUMOReal 
MSCFModel_Wiedemann::emergency(SUMOReal dv, SUMOReal dx) const {
    SUMOReal accel = 0.5 * dv * dv / (myAX - dx); // + predAccel at t-reaction_time if this is value is above a treshold
    // since we have hard constrainst on accel we may as well use them here
    assert(accel > 0);
    return MAX2(accel, -myDecel);
}
