/****************************************************************************/
/// @file    MSCFModel_Krauss.cpp
/// @author  Tobias Mayer
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, with acceleration decrease and faster start
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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_Krauss.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal tau) throw()
        : MSCFModel(vtype, decel), myAccel(accel), myDawdle(dawdle), myTau(tau), myTauDecel(decel*tau) {
}


MSCFModel_Krauss::~MSCFModel_Krauss() throw() {}


SUMOReal
MSCFModel_Krauss::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
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
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const /*veh*/, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const veh, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const veh, const MSVehicle *pred) const throw() {
    return MIN2(_vsafe(veh->gap2pred(*pred), pred->getSpeed()), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::ffeS(const MSVehicle * const veh, SUMOReal gap) const throw() {
    return MIN2(_vsafe(gap, 0), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed()), veh->getLane()->getMaxSpeed());
    SUMOReal gap = (vNext - vL) *
                   ((veh->getSpeed() + vL) * myInverseTwoDecel + myTau) +
                   vL * myTau;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel_Krauss::dawdle(SUMOReal speed) const throw() {
    // generate random number out of [0,1]
    SUMOReal random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(myDawdle * speed * random);
    } else {
        speed -= ACCEL2SPEED(myDawdle * getMaxAccel(speed) * random);
    }
    return MAX2(SUMOReal(0), speed);
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel_Krauss::_vsafe(SUMOReal gap, SUMOReal predSpeed) const throw() {
    if (predSpeed==0&&gap<0.01) {
        return 0;
    }
    const SUMOReal speedReduction = ACCEL2SPEED(myDecel); // !!! TODO use the deceleration of the leader
    const int predSteps = int(predSpeed / speedReduction);
    const SUMOReal leaderContrib = 2. * myDecel * (gap + SPEED2DIST(predSteps * predSpeed - speedReduction * predSteps * (predSteps+1) / 2));
    return (SUMOReal)(-myTauDecel + sqrt(myTauDecel*myTauDecel + leaderContrib));
}


MSCFModel *
MSCFModel_Krauss::duplicate(const MSVehicleType *vtype) const throw() {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myDawdle, myTau);
}


//void MSCFModel::saveState(std::ostream &os) {}

