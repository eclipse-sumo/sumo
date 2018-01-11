/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
                             double accel, double decel, double emergencyDecel, double apparentDecel,
                             double headwayTime, double delta,
                             double internalStepping) :
    MSCFModel(vtype, accel, decel, emergencyDecel, apparentDecel, headwayTime), myDelta(delta),
    myAdaptationFactor(1.), myAdaptationTime(0.),
    myIterations(MAX2(1, int(TS / internalStepping + .5))),
    myTwoSqrtAccelDecel(double(2 * sqrt(accel * decel))) {
}


MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype,
                             double accel, double decel, double emergencyDecel, double apparentDecel,
                             double headwayTime,
                             double adaptationFactor, double adaptationTime,
                             double internalStepping) :
    MSCFModel(vtype, accel, decel, emergencyDecel, apparentDecel, headwayTime), myDelta(4.),
    myAdaptationFactor(adaptationFactor), myAdaptationTime(adaptationTime),
    myIterations(MAX2(1, int(TS / internalStepping + .5))),
    myTwoSqrtAccelDecel(double(2 * sqrt(accel * decel))) {
}


MSCFModel_IDM::~MSCFModel_IDM() {}


double
MSCFModel_IDM::finalizeSpeed(MSVehicle* const veh, double vPos) const {
    const double vNext = MSCFModel::finalizeSpeed(veh, vPos);
    if (myAdaptationFactor != 1.) {
        VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        vars->levelOfService += (vNext / veh->getLane()->getVehicleMaxSpeed(veh) - vars->levelOfService) / myAdaptationTime * TS;
    }
    return vNext;
}


double
MSCFModel_IDM::followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double /*predMaxDecel*/) const {
    return _v(veh, gap2pred, speed, predSpeed, veh->getLane()->getVehicleMaxSpeed(veh));
}


double
MSCFModel_IDM::stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred) const {
    if (gap2pred < 0.01) {
        return 0;
    }
    return _v(veh, gap2pred, speed, 0, veh->getLane()->getVehicleMaxSpeed(veh), false);
}


/// @todo update interactionGap logic to IDM
double
MSCFModel_IDM::interactionGap(const MSVehicle* const veh, double vL) const {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const double acc = myAccel * (1. - pow(veh->getSpeed() / veh->getLane()->getVehicleMaxSpeed(veh), myDelta));
    const double vNext = veh->getSpeed() + acc;
    const double gap = (vNext - vL) * (veh->getSpeed() + vL) / (2 * myDecel) + vL;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


double
MSCFModel_IDM::_v(const MSVehicle* const veh, const double gap2pred, const double egoSpeed,
                  const double predSpeed, const double desSpeed, const bool respectMinGap) const {
// this is more or less based on http://www.vwi.tu-dresden.de/~treiber/MicroApplet/IDM.html
// and http://arxiv.org/abs/cond-mat/0304337
// we assume however constant speed for the leader
    double headwayTime = myHeadwayTime;
    if (myAdaptationFactor != 1.) {
        const VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        headwayTime *= myAdaptationFactor + vars->levelOfService * (1. - myAdaptationFactor);
    }
    double newSpeed = egoSpeed;
    double gap = gap2pred;
    for (int i = 0; i < myIterations; i++) {
        const double delta_v = newSpeed - predSpeed;
        double s = MAX2(0., newSpeed * headwayTime + newSpeed * delta_v / myTwoSqrtAccelDecel);
        if (respectMinGap) {
            s += myType->getMinGap();
        }
        const double acc = myAccel * (1. - pow(newSpeed / desSpeed, myDelta) - (s * s) / (gap * gap));
        newSpeed += ACCEL2SPEED(acc) / myIterations;
        //TODO use more realistic position update which takes accelerated motion into account
        gap -= MAX2(0., SPEED2DIST(newSpeed - predSpeed) / myIterations);
    }
//    return MAX2(getSpeedAfterMaxDecel(egoSpeed), newSpeed);
    return MAX2(0., newSpeed);
}


MSCFModel*
MSCFModel_IDM::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_IDM(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, myHeadwayTime, myDelta, TS / myIterations);
}
