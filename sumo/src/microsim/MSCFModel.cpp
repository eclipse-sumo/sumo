/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
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

#include "MSCFModel.h"
#include "MSVehicleType.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSAbstractLaneChangeModel.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel::MSCFModel(const MSVehicleType* vtype, SUMOReal decel) throw()
        : myType(vtype), myDecel(decel) {
    myInverseTwoDecel = SUMOReal(1) / (SUMOReal(2) * decel);
}


MSCFModel::~MSCFModel() throw() {}


SUMOReal
MSCFModel::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vSafe-oldV));
    const SUMOReal vMin = MAX2((SUMOReal) 0, oldV - ACCEL2SPEED(myDecel));
    const SUMOReal vMax = MIN3(lane->getMaxSpeed(), maxNextSpeed(oldV), vSafe);
    assert(vMin<=vMax);
    return veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
}


void
MSCFModel::leftVehicleVsafe(const MSVehicle * const ego, const MSVehicle * const neigh, SUMOReal &vSafe) const throw() {
    if (neigh!=0&&neigh->getSpeed()>60./3.6) {
        SUMOReal mgap = MAX2((SUMOReal) 0, neigh->getPositionOnLane()-neigh->getVehicleType().getLength()-ego->getPositionOnLane());
        SUMOReal nVSafe = ffeV(ego, mgap, neigh->getSpeed());
        if (mgap-neigh->getSpeed()>=0) {
            vSafe = MIN2(vSafe, nVSafe);
        }
    }
}


SUMOReal
MSCFModel::maxNextSpeed(SUMOReal speed) const throw() {
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(getMaxAccel(speed)), myType->getMaxSpeed());
}


SUMOReal
MSCFModel::brakeGap(SUMOReal speed) const throw() {
    /* one possiblity to speed this up is to precalculate speedReduction * steps * (steps+1) / 2
       for small values of steps (up to 10 maybe) and store them in an array */
    const SUMOReal speedReduction = ACCEL2SPEED(getMaxDecel());
    const int steps = int(speed / speedReduction);
    return SPEED2DIST(steps * speed - speedReduction * steps *(steps+1) / 2) + speed * getTau();
}


void MSCFModel::saveState(std::ostream&/*os*/) {}

