/****************************************************************************/
/// @file    MSCFModel_IDM.cpp
/// @author  Tobias Mayer
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The IDM car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSAbstractLaneChangeModel.h"
#include <utils/common/RandHelper.h>


// ===========================================================================
// definitions
// ===========================================================================
#define DELTA_IDM 4.0


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_IDM::MSCFModel_IDM(const MSVehicleType* vtype, SUMOReal dawdle,
                             SUMOReal timeHeadWay, SUMOReal mingap) throw()
        : MSCFModel(vtype), myTimeHeadWay(timeHeadWay), myMinSpace(mingap) {

    myInverseTwoDecel = SUMOReal(1) / (SUMOReal(2) * vtype->getMaxDecel());
}


MSCFModel_IDM::~MSCFModel_IDM() throw() {}


SUMOReal 
MSCFModel_IDM::move(MSVehicle * const veh, const MSLane * const lane, const MSVehicle * const pred, const MSVehicle * const neigh) const throw() {
    SUMOReal nSpeed = ffeV(veh, pred);
    nSpeed = moveHelper(veh, lane, nSpeed);
    return nSpeed;
}


SUMOReal 
MSCFModel_IDM::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    SUMOReal nSpeed = vPos;
    nSpeed =
        veh->getLaneChangeModel().patchSpeed(
            MAX2((SUMOReal) 0, veh->getSpeed()-(SUMOReal)ACCEL2SPEED(myType->getMaxDecel())), //!!! reverify
            nSpeed,
            MIN3(nSpeed, lane->getMaxSpeed(), maxNextSpeed(veh->getSpeed())),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            nSpeed);
    return nSpeed;
}


void 
MSCFModel_IDM::leftVehicleVsafe(const MSVehicle * const ego, const MSVehicle * const neigh, SUMOReal &vSafe) const throw()
{
    if (neigh!=0&&neigh->getSpeed()>60./3.6) {
        SUMOReal mgap = MAX2((SUMOReal) 0, neigh->getPositionOnLane()-neigh->getVehicleType().getLength()-ego->getPositionOnLane());
        SUMOReal nVSafe = ffeV(ego, mgap, neigh->getSpeed());
        if (mgap-neigh->getSpeed()>=0) {
            vSafe = MIN2(vSafe, nVSafe);
        }
    }
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


SUMOReal 
MSCFModel_IDM::maxNextSpeed(SUMOReal speed) const throw() {
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(myType->getMaxAccel(speed)), myType->getMaxSpeed());
}


/// @todo update logic to IDM
SUMOReal 
MSCFModel_IDM::brakeGap(SUMOReal speed) const throw() {
    return speed * speed * myInverseTwoDecel;
}


/// @todo update logic to IDM
SUMOReal 
MSCFModel_IDM::approachingBrakeGap(SUMOReal speed) const throw() {
    return speed * speed * myInverseTwoDecel;
}


/// @todo update logic to IDM
SUMOReal 
MSCFModel_IDM::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal acc = myType->getMaxAccel() * (1. - pow((double)(veh->getSpeed()/desiredSpeed(veh)), (double) DELTA_IDM));
    SUMOReal vNext = veh->getSpeed() + acc;
    SUMOReal gap = (vNext - vL) *
                   ((veh->getSpeed() + vL) * myInverseTwoDecel) +
                   vL * 1;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, timeHeadWayGap(vNext));
}


/// @todo update logic to IDM
bool 
MSCFModel_IDM::hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw() {
    if(gap<0) {
        return false;
    }
    SUMOReal vSafe = _updateSpeed(speed, gap, predSpeed, laneMaxSpeed);
    SUMOReal vNext = MIN3(maxNextSpeed(speed), laneMaxSpeed, vSafe);
    return (vNext>=myType->getSpeedAfterMaxDecel(speed)
            &&
            gap   >= timeHeadWayGap(speed));
}

// ???
SUMOReal 
MSCFModel_IDM::safeEmitGap(SUMOReal speed) const throw() {
    SUMOReal vNextMin = myType->getSpeedAfterMaxDecel(speed); // ok, minimum next speed
    SUMOReal safeGap  = vNextMin * (speed * myInverseTwoDecel);
    return MAX2(safeGap, timeHeadWayGap(speed)) + ACCEL2DIST(myType->getMaxAccel(speed));
}


SUMOReal MSCFModel_IDM::decelAbility() const throw() {
    return ACCEL2SPEED(myType->getMaxDecel());
}


/**  */
SUMOReal 
MSCFModel_IDM::_updateSpeed(SUMOReal gap2pred, SUMOReal mySpeed, SUMOReal predSpeed, SUMOReal desSpeed) const throw() {
    SUMOReal a = myType->getMaxAccel();
    SUMOReal b = myType->getMaxDecel();
    SUMOReal delta_v = mySpeed - predSpeed;
    SUMOReal s_star_raw = myMinSpace + mySpeed*myTimeHeadWay + (mySpeed*delta_v)/(2*sqrt(a*b));
    SUMOReal s_star = MAX2(s_star_raw, myMinSpace);
    SUMOReal acc = a * (1. - pow((double)(mySpeed/desSpeed), (double) DELTA_IDM) - (s_star*s_star)/(gap2pred*gap2pred));
    SUMOReal vNext = mySpeed + acc;
    return vNext;
}


SUMOReal 
MSCFModel_IDM::desiredSpeed(const MSVehicle * const veh) const throw() {
    return MIN2(myType->getMaxSpeed(), veh->getLane().getMaxSpeed());
}



//void MSCFModel::saveState(std::ostream &os) {}

