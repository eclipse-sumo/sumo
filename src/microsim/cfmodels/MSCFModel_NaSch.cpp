/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2026 German Aerospace Center (DLR) and others.
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
/// @file    MSCFModel_NaSch.cpp
/// @author  Jerry Lin
/// @date    Thu, 13 Aug 2026
///
// The Nagel-Schreckenberg (1992) cellular automaton car-following model
/****************************************************************************/
#include <config.h>

#include <limits>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSCFModel_NaSch.h"
#include <utils/common/RandHelper.h>

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_NaSch::MSCFModel_NaSch(const MSVehicleType* vtype, double dawdle) :
    MSCFModel(vtype),
    myDawdle(dawdle),
    // The classic NaSch cell length is the distance covered by accelerating
    // for one simulation step, i.e. the "accel" attribute doubles as the
    // cell size (in meters) when using the default step-length of 1s,
    // see https://github.com/eclipse-sumo/sumo/issues/12182
    myCellSpeed(ACCEL2SPEED(myAccel)) {
}


MSCFModel_NaSch::~MSCFModel_NaSch() {}


double
MSCFModel_NaSch::roundToCell(double speed) const {
    if (speed <= 0. || myCellSpeed <= 0.) {
        return 0.;
    }
    return floor(speed / myCellSpeed + NUMERICAL_EPS) * myCellSpeed;
}


double
MSCFModel_NaSch::vsafe(const MSVehicle* const veh, double speed, double gap) const {
    if (gap < NUMERICAL_EPS) {
        return 0.;
    }
    // NaSch rule 1 (acceleration) followed by rule 2 (slowing down): unlike the
    // Krauss-derived models, the leader's speed does not enter the computation at
    // all -- the gap alone caps how far ego may move this step, see issue #12182
    // ("replace leaderSpeed with gap").
    const double vAccel = maxNextSpeed(speed, veh);
    const double vGapLimited = MIN2(vAccel, DIST2SPEED(gap));
    return roundToCell(MAX2(0., vGapLimited));
}


double
MSCFModel_NaSch::followSpeed(const MSVehicle* const veh, double speed, double gap2pred,
                             double /*predSpeed*/, double /*predMaxDecel*/, const MSVehicle* const /*pred*/, const CalcReason /*usage*/) const {
    return vsafe(veh, speed, gap2pred);
}


double
MSCFModel_NaSch::stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred, double /*decel*/, const CalcReason /*usage*/) const {
    return vsafe(veh, speed, gap2pred);
}


double
MSCFModel_NaSch::patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const {
    // NaSch rule 3 (randomization): with probability sigma, dawdle by exactly
    // one cell -- not a continuously distributed amount as in the Krauss dawdle.
    if (vMax > 0. && myDawdle > 0. && RandHelper::rand(veh->getRNG()) < myDawdle) {
        return MAX2(vMin, roundToCell(vMax - myCellSpeed));
    }
    return vMax;
}


double
MSCFModel_NaSch::minNextSpeedEmergency(double speed, const MSVehicle* const /*veh*/) const {
    // NaSch rule 4 (movement) permits vehicles to stop dead within a single
    // step whenever the gap requires it, i.e. emergency deceleration is not
    // bounded, see issue #12182 ("do not bound emergency decel").
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return 0.;
    }
    return -std::numeric_limits<double>::max();
}


MSCFModel*
MSCFModel_NaSch::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_NaSch(vtype, myDawdle);
}


/****************************************************************************/
