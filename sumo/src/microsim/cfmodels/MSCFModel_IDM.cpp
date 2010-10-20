/****************************************************************************/
/// @file    MSCFModel_IDM.cpp
/// @author  Tobias Mayer
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The Intellignet Driver Model (IDM) car-following model
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

#include "MSCFModel_IDM.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// definitions
// ===========================================================================
#define DELTA_IDM 4.0


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype,
                             SUMOReal accel, SUMOReal decel,
                             SUMOReal timeHeadWay, SUMOReal mingap, SUMOReal tau) throw()
        : MSCFModel(vtype, decel),
        myAccel(accel), myTimeHeadWay(timeHeadWay), myMinSpace(mingap), myTau(tau) {

}


MSCFModel_IDM::~MSCFModel_IDM() throw() {}


SUMOReal
MSCFModel_IDM::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    vPos = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vPos-oldV));
    //
    SUMOReal vNext = vPos;
    return
        veh->getLaneChangeModel().patchSpeed(
            MAX2((SUMOReal) 0, veh->getSpeed()-(SUMOReal)ACCEL2SPEED(myDecel)), //!!! reverify
            vNext,
            MIN3(vNext, lane->getMaxSpeed(), maxNextSpeed(oldV)),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vNext, *this);
}


SUMOReal
MSCFModel_IDM::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const throw() {
    return _updateSpeed(gap2pred, speed, predSpeed, desiredSpeed(veh));
}


SUMOReal
MSCFModel_IDM::ffeV(const MSVehicle * const veh, SUMOReal gap2pred, SUMOReal predSpeed) const throw() {
    return _updateSpeed(gap2pred, veh->getSpeed(), predSpeed, desiredSpeed(veh));
}


SUMOReal
MSCFModel_IDM::ffeV(const MSVehicle * const veh, const MSVehicle * const pred) const throw() {
    return _updateSpeed(veh->gap2pred(*pred), veh->getSpeed(), pred->getSpeed(), desiredSpeed(veh));
}


SUMOReal
MSCFModel_IDM::ffeS(const MSVehicle * const veh, SUMOReal gap2pred) const throw() {
    SUMOReal desSpeed = desiredSpeed(veh);
    return _updateSpeed(gap2pred, veh->getSpeed(), desSpeed, desSpeed);
}


/// @todo update logic to IDM
SUMOReal
MSCFModel_IDM::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal acc = myAccel * (1. - pow((double)(veh->getSpeed()/desiredSpeed(veh)), (double) DELTA_IDM));
    SUMOReal vNext = veh->getSpeed() + acc;
    SUMOReal gap = (vNext - vL) *
                   ((veh->getSpeed() + vL) * myInverseTwoDecel) +
                   vL * 1;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


/// @todo update logic to IDM
SUMOReal
MSCFModel_IDM::_updateSpeed(SUMOReal gap2pred, SUMOReal mySpeed, SUMOReal predSpeed, SUMOReal desSpeed) const throw() {
    SUMOReal delta_v = mySpeed - predSpeed;
    SUMOReal s_star_raw = myMinSpace + mySpeed*myTimeHeadWay + (mySpeed*delta_v)/(2*sqrt(myAccel*myDecel));
    SUMOReal s_star = MAX2(s_star_raw, myMinSpace);
    SUMOReal acc = myAccel * (1. - pow((double)(mySpeed/desSpeed), (double) DELTA_IDM) - (s_star*s_star)/(gap2pred*gap2pred));
    SUMOReal vNext = mySpeed + ACCEL2SPEED(acc);
    return vNext;
}


MSCFModel *
MSCFModel_IDM::duplicate(const MSVehicleType *vtype) const throw() {
    return new MSCFModel_IDM(vtype, myAccel, myDecel, myTimeHeadWay, myMinSpace, myTau);
}



//void MSCFModel::saveState(std::ostream &os) {}

