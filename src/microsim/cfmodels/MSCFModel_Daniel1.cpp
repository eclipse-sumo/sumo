/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_Daniel1.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// The original Krauss (1998) car-following model and parameter
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
#include "MSCFModel_Daniel1.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Daniel1::MSCFModel_Daniel1(const MSVehicleType* vtype,  double accel,
                                     double decel, double emergencyDecel, double apparentDecel,
                                     double dawdle, double headwayTime,
                                     double tmp1, double tmp2, double tmp3, double tmp4, double tmp5) :
    MSCFModel(vtype, accel, decel, emergencyDecel, apparentDecel, headwayTime),
    myDawdle(dawdle), myTauDecel(decel * headwayTime),
    myTmp1(tmp1), myTmp2(tmp2), myTmp3(tmp3), myTmp4(tmp4), myTmp5(tmp5) {
}


MSCFModel_Daniel1::~MSCFModel_Daniel1() {}


double
MSCFModel_Daniel1::finalizeSpeed(MSVehicle* const veh, double vPos) const {
    const double oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const double vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const double vMin = getSpeedAfterMaxDecel(oldV);
    const double vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
#ifdef _DEBUG
    if (vMin > vMax) {
        WRITE_WARNING("Maximum speed of vehicle '" + veh->getID() + "' is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    }
#endif
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}


double
MSCFModel_Daniel1::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double /*predMaxDecel*/) const {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed, veh));
}


double
MSCFModel_Daniel1::stopSpeed(const MSVehicle* const veh, const double speed, double gap) const {
    return MIN2(_vsafe(gap, 0), maxNextSpeed(speed, veh));
}


double
MSCFModel_Daniel1::dawdle(double speed) const {
    return MAX2(0., speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
double MSCFModel_Daniel1::_vsafe(double gap, double predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    double vsafe = (double)(-1. * myTauDecel
                            + sqrt(
                                myTauDecel * myTauDecel
                                + (predSpeed * predSpeed)
                                + (2. * myDecel * gap)
                            ));
    assert(vsafe >= 0);
    return vsafe;
}


MSCFModel*
MSCFModel_Daniel1::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Daniel1(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, myDawdle, myHeadwayTime,
                                 myTmp1, myTmp2, myTmp3, myTmp4, myTmp5);
}
