/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSCFModel_Wiedemann.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    June 2011
///
// The psycho-physical model of Wiedemann
// references:
// Andre Stebens - Traffic simulation with the Wiedemann model
// Werner - Integration von Fahrzeugfolge- und Fahrstreifenwechselmodellen in die Nachtfahrsimulation LucidDrive
// Olstam, Tapani - Comparison of Car-following models
// Higgs, B. et.al - Analysis of theWiedemann car following model over different speeds using naturalistic data.
// Ahmed, H.U.; Huang, Y.; Lu, P. - A Review of Car-Following Models and Modeling Tools forHuman and Autonomous-Ready Driving Behaviors in Micro-Simulation

/****************************************************************************/
#include <config.h>

#include <cmath>
#include "MSCFModel_Wiedemann.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <utils/common/RandHelper.h>

//#define DEBUG_V

// ===========================================================================
// static members
// ===========================================================================

// magic constant proposed by Wiedemann (based on real world measurements)
const double MSCFModel_Wiedemann::D_MAX = 150;

#define B_MIN_MULT 0
#define B_MIN_ADD  -1
#define PRED_DECEL_MULT 0.5
#define PRED_DECEL_MULT_EMERGENCY 0.55

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Wiedemann::MSCFModel_Wiedemann(const MSVehicleType* vtype) :
    MSCFModel(vtype),
    mySecurity(vtype->getParameter().getCFParam(SUMO_ATTR_CF_WIEDEMANN_SECURITY, 0.5)),
    myEstimation(vtype->getParameter().getCFParam(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION, 0.5)),
    myAX(vtype->getLength() + 1. + 2. * mySecurity),
    myCX(25. *(1. + mySecurity + myEstimation)),
    myMinAccel(0.2 * myAccel),
    myMaxApproachingDecel((myDecel + myEmergencyDecel) / 2) {
    // Wiedemann does not drive very precise and may violate minGap on occasion
    myCollisionMinGapFactor = vtype->getParameter().getCFParam(SUMO_ATTR_COLLISION_MINGAP_FACTOR, 0.1);
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
MSCFModel_Wiedemann::followSpeed(const MSVehicle* const veh, double /* speed */, double gap2pred, double predSpeed, double /*predMaxDecel*/, const MSVehicle* const pred, const CalcReason /*usage*/) const {
    return _v(veh, predSpeed, gap2pred, pred != nullptr ? pred->getAcceleration() : 0);
}


double
MSCFModel_Wiedemann::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel, const CalcReason /*usage*/) const {
    /* Wiedemann does not handle approaching junctions or stops very well:
     * regime approaching() fails when dv = 0 (i.e. a vehicle inserted with speed 0 does not accelerate to reach a stop)
     * for dv ~ 0 the standard decision tree will switch to following() which
     * does a lousy job of closing in on a stop / junction
     * hence we borrow from Krauss here
     */
    return MIN2(maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs()), maxNextSpeed(speed, veh));
}


double
MSCFModel_Wiedemann::interactionGap(const MSVehicle* const, double vL) const {
    UNUSED_PARAMETER(vL);
    return D_MAX;
}


MSCFModel*
MSCFModel_Wiedemann::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Wiedemann(vtype);
}


double
MSCFModel_Wiedemann::getSecureGap(const MSVehicle* const veh, const MSVehicle* const pred, const double speed, const double leaderSpeed, const double leaderMaxDecel) const {
    const double bx = (1 + 7 * mySecurity) * sqrt(speed);
    const double abx = myAX + bx - myType->getLength(); // abx is the brutto gap
    return MAX2(abx, MSCFModel::getSecureGap(veh, pred, speed, leaderSpeed, leaderMaxDecel));
}


double
MSCFModel_Wiedemann::_v(const MSVehicle* veh, double predSpeed, double gap, double predAccel) const {
    const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const double dx = gap + myType->getLength(); // wiedemann uses brutto gap
    const double v = veh->getSpeed();
    const double vpref = veh->getMaxSpeed();
    const double dv = v - predSpeed;
    // desired minimum following distance at low speed difference
    const double bx = (1 + 7 * mySecurity) * sqrt(v); // Harding propose a factor of  *.8 here
    const double abx = myAX + bx; // Harding propose a factor of  *.8 here
    const double ex = 2 - myEstimation; // + RandHelper::randNorm(0.5, 0.15)
    const double sdx = myAX + ex * bx; /// the distance at which we drift out of following
    const double sdv_root = (dx - myAX) / myCX;
    const double sdv = sdv_root * sdv_root;
    const double cldv = sdv * ex * ex;
    const double opdv = cldv * (-1 - 2 * RandHelper::randNorm(0.5, 0.15, veh->getRNG()));
    // D_MAX is too low to brake safely when driving at speeds above 36m/s
    const double dmax = MAX2(D_MAX, brakeGap(v, myDecel, 0));
    // select the regime, get new acceleration, compute new speed based
    double accel;
    int branch = 0;
    if (dx <= abx) {
        accel = emergency(dv, dx, predAccel, v, gap, abx, bx);
        branch = 1;
    } else if (dx < sdx) {
        if (dv > cldv) {
            accel = approaching(dv, dx, abx, predAccel);
            branch = 2;
        } else if (dv > opdv) {
            accel = following(vars->accelSign);
            branch = 3;
        } else {
            accel = fullspeed(v, vpref, dx, abx);
            branch = 4;
        }
    } else {
        if (dv > sdv && dx < dmax) { //@note other versions have an disjunction instead of conjunction
            accel = approaching(dv, dx, abx, predAccel);
            branch = 5;
        } else {
            accel = fullspeed(v, vpref, dx, abx);
            branch = 6;
        }
    }
    // since we have hard constraints on accel we may as well use them here
#ifdef DEBUG_V
    const double rawAccel = accel;
#endif
    accel = MAX2(MIN2(accel, myAccel), -myEmergencyDecel);
    const double vNew = MAX2(0., v + ACCEL2SPEED(accel)); // don't allow negative speeds
#ifdef DEBUG_V
    if (veh->isSelected() && !MSGlobals::gComputeLC) {
        std::cout << SIMTIME << " Wiedemann::_v veh=" << veh->getID()
                  << " v=" << v << " pV=" << predSpeed << " pA=" << predAccel << " gap=" << gap
                  << " dv=" << dv << " dx=" << dx << " ax=" << myAX << " bx=" << bx << " abx=" << abx
                  << " sdx=" << sdx << " sdv=" << sdv << " cldv=" << cldv << " opdv=" << opdv
                  << " branch=" << branch << " rawAccel=" << rawAccel
                  << " accel=" << accel << " vNew=" << vNew << "\n";
    }
#else
    UNUSED_PARAMETER(branch);
#endif
    return vNew;
}


double
MSCFModel_Wiedemann::fullspeed(double v, double vpref, double dx, double abx) const {
    // maximum acceleration is reduced with increasing speed
    double bmax = 0.2 + 0.8 * myAccel * (7 - sqrt(v));
    // if veh just drifted out of a 'following' process the acceleration is reduced
    double accel = dx <= 2 * abx ? MIN2(myMinAccel, bmax * (dx - abx) / abx) : bmax;
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
MSCFModel_Wiedemann::approaching(double dv, double dx, double abx, double predAccel) const {
    // there is singularity in the formula. we do the sanity check outside
    assert(abx < dx);
    // @note: the original model does not have a limit on maximum deceleration here.
    // We add this to avoid cascading emergency deceleration
    // also, the multiplier on predAccel is always 1 in the original model
    return MAX2(0.5 * dv * dv / (abx - dx) + predAccel * PRED_DECEL_MULT, -myMaxApproachingDecel);
}


double
MSCFModel_Wiedemann::emergency(double dv, double dx, double predAccel, double v, double gap, double abx, double bx) const {
    // wiedemann assumes that dx will always be larger than myAX (sumo may
    // violate this assumption when crashing (-:
    //
    // predAccel is called b_(n-1) in the literature and it's multipleir is always 1

    if (dx > myAX) {
        const double bmin = B_MIN_ADD + B_MIN_MULT * v;
        const double accel = (0.5 * dv * dv / (myAX - dx)
                              + predAccel * PRED_DECEL_MULT_EMERGENCY
                              + bmin * (abx - gap) / bx);
        return accel;
    } else {
        return -myEmergencyDecel;
    }

    // emergency according to C.Werner
    //return -myEmergencyDecel;
}
