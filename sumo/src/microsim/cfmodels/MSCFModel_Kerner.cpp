/****************************************************************************/
/// @file    MSCFModel_Kerner.cpp
/// @author  Daniel Krajzewicz
/// @date    03.04.2010
/// @version $Id$
///
// car-following model by B. Kerner
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "MSCFModel_Kerner.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Kerner::MSCFModel_Kerner(const MSVehicleType* vtype, SUMOReal accel,
                                   SUMOReal decel, SUMOReal tau, SUMOReal k, SUMOReal phi) throw()
        : MSCFModel(vtype, decel), myAccel(accel), myTau(tau), myK(k), myPhi(phi) {

    myTauDecel = decel * myTau;
}


MSCFModel_Kerner::~MSCFModel_Kerner() throw() {}


SUMOReal
MSCFModel_Kerner::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vSafe-oldV));
    //
    SUMOReal vNext =
        veh->getLaneChangeModel().patchSpeed(
            MAX2((SUMOReal) 0, oldV-(SUMOReal)ACCEL2SPEED(myDecel)), //!!! reverify
            vSafe,
            MIN3(vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV)),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    return MIN4(vNext, vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV));
}


SUMOReal
MSCFModel_Kerner::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return MIN2(_v(speed, maxNextSpeed(speed), gap, predSpeed), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Kerner::ffeV(const MSVehicle * const veh, SUMOReal gap, SUMOReal predSpeed) const throw() {
    SUMOReal speed = veh->getSpeed();
    return MIN2(_v(speed, maxNextSpeed(speed), gap, predSpeed), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Kerner::ffeV(const MSVehicle * const veh, const MSVehicle *pred) const throw() {
    SUMOReal speed = veh->getSpeed();
    return MIN2(_v(speed, maxNextSpeed(speed), veh->gap2pred(*pred), pred->getSpeed()), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Kerner::ffeS(const MSVehicle * const veh, SUMOReal gap) const throw() {
    SUMOReal speed = veh->getSpeed();
    return MIN2(_v(speed, maxNextSpeed(speed), gap, 0), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Kerner::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed()), veh->getLane().getMaxSpeed());
    SUMOReal gap = (vNext - vL) *
                   ((veh->getSpeed() + vL) * myInverseTwoDecel + myTau) +
                   vL * myTau;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel_Kerner::_v(SUMOReal speed, SUMOReal vfree, SUMOReal gap, SUMOReal predSpeed) const throw() {
    if (predSpeed==0&&gap<0.01) {
        return 0;
    }
    // !!! in the following, the prior step is not considered!!!
    SUMOReal G = MAX2((SUMOReal) 0, (SUMOReal)(SPEED2DIST(myK*speed)+myPhi/myAccel*speed*(speed-predSpeed)));
    SUMOReal vcond = gap>G ? speed+ACCEL2SPEED(myAccel) : speed+MAX2(ACCEL2SPEED(-myDecel), MIN2(ACCEL2SPEED(myAccel), predSpeed-speed));
    SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel + sqrt(myTauDecel*myTauDecel + (predSpeed*predSpeed) + (2. * myDecel * gap)));
    SUMOReal va = MAX2((SUMOReal) 0, MIN3(vfree, vsafe, vcond)) + RandHelper::rand();
    SUMOReal v = MAX2((SUMOReal) 0, MIN4(vfree, va, speed+ACCEL2SPEED(myAccel), vsafe));
    return v;
}


MSCFModel *
MSCFModel_Kerner::duplicate(const MSVehicleType *vtype) const throw() {
    return new MSCFModel_Kerner(vtype, myAccel, myDecel, myTau, myK, myPhi);
}



//void MSCFModel::saveState(std::ostream &os) {}

