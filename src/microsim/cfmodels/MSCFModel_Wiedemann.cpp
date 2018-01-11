/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2011-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_Wiedemann.cpp
/// @author  Daniel Krajzewicz
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
const double MSCFModel_Wiedemann::D_MAX = 150;


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Wiedemann::MSCFModel_Wiedemann(const MSVehicleType* vtype,
        double accel, double decel, double emergencyDecel, double apparentDecel,
        double security, double estimation) :
    MSCFModel(vtype, accel, decel, emergencyDecel, apparentDecel, 1.0),
    mySecurity(security),
    myEstimation(estimation),
    myAX(vtype->getLength() + 1. + 2. * security),
    myCX(25. *(1. + security + estimation)),
    myMinAccel(0.2 * myAccel) { // +noise?
}


MSCFModel_Wiedemann::~MSCFModel_Wiedemann() {}


double
MSCFModel_Wiedemann::finalizeSpeed(MSVehicle* const veh, double vPos) const {
    const double vNext = MSCFModel::finalizeSpeed(veh, vPos);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    vars->accelSign = vNext > veh->getSpeed() ? 1. : -1.;
    return vNext;
}


double
MSCFModel_Wiedemann::followSpeed(const MSVehicle* const veh, double /* speed */, double gap2pred, double predSpeed, double /*predMaxDecel*/) const {
    return _v(veh, predSpeed, gap2pred);
}


double
MSCFModel_Wiedemann::stopSpeed(const MSVehicle* const veh, const double speed, double gap) const {
    /* Wiedemann does not handle approaching junctions or stops very well:
     * regime approaching() fails when dv = 0 (i.e. a vehicle inserted with speed 0 does not accelerate to reach a stop)
     * for dv ~ 0 the standard decision tree will switch to following() which
     * does a lousy job of closing in on a stop / junction
     * hence we borrow from Krauss here
     */
    return MAX2(getSpeedAfterMaxDecel(speed), MIN2(krauss_vsafe(gap, 0), maxNextSpeed(speed, veh)));
}


double
MSCFModel_Wiedemann::interactionGap(const MSVehicle* const , double vL) const {
    UNUSED_PARAMETER(vL);
    return D_MAX;
}


MSCFModel*
MSCFModel_Wiedemann::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Wiedemann(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, mySecurity, myEstimation);
}


double
MSCFModel_Wiedemann::_v(const MSVehicle* veh, double predSpeed, double gap) const {
    const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const double dx = gap + myType->getLength(); // wiedemann uses brutto gap
    const double v = veh->getSpeed();
    const double vpref = veh->getMaxSpeed();
    const double dv = v - predSpeed;
    const double bx = myAX + (1 + 7 * mySecurity) * sqrt(v); // Harding propose a factor of  *.8 here
    const double ex = 2 - myEstimation; // + RandHelper::randNorm(0.5, 0.15)
    const double sdx = myAX + ex * (bx - myAX); /// the distance at which we drift out of following
    const double sdv_root = (dx - myAX) / myCX;
    const double sdv = sdv_root * sdv_root;
    const double cldv = sdv * ex * ex;
    const double opdv = cldv * (-1 - 2 * RandHelper::randNorm(0.5, 0.15));
    // select the regime, get new acceleration, compute new speed based
    double accel;
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
    const double vNew = MAX2(0., v + ACCEL2SPEED(accel)); // don't allow negative speeds
    return vNew;
}


double
MSCFModel_Wiedemann::fullspeed(double v, double vpref, double dx, double bx) const {
    double bmax = 0.2 + 0.8 * myAccel * (7 - sqrt(v));
    // if veh just drifted out of a 'following' process the acceleration is reduced
    double accel = dx <= 2 * bx ? MIN2(myMinAccel, bmax * (dx - bx) / bx) : bmax;
    if (v > vpref) {
        accel = - accel;
    }
    return accel;
}


double
MSCFModel_Wiedemann::following(double sign) const {
    return myMinAccel * sign;
}


double
MSCFModel_Wiedemann::approaching(double dv, double dx, double bx) const {
    // there is singularity in the formula. we do the sanity check outside
    assert(bx < dx);
    return 0.5 * dv * dv / (bx - dx); // + predAccel at t-reaction_time if this is value is above a treshold
}


double
MSCFModel_Wiedemann::emergency(double /* dv */, double /* dx */) const {
    /* emergency according to A.Stebens
    // wiedemann assumes that dx will always be larger than myAX (sumo may
    // violate this assumption when crashing (-:
    if (dx > myAX) {
        double accel = 0.5 * dv * dv / (myAX - dx); // + predAccel at t-reaction_time if this is value is above a treshold
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



// XXX: This could be replaced by maximumSafeStopSpeed(), refs. #2575
double
MSCFModel_Wiedemann::krauss_vsafe(double gap, double predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    const double tauDecel = myDecel * myHeadwayTime;
    const double speedReduction = ACCEL2SPEED(myDecel);
    const int predSteps = int(predSpeed / speedReduction);
    const double leaderContrib = 2. * myDecel * (gap + SPEED2DIST(predSteps * predSpeed - speedReduction * predSteps * (predSteps + 1) / 2));
    return (double)(-tauDecel + sqrt(tauDecel * tauDecel + leaderContrib));
}
