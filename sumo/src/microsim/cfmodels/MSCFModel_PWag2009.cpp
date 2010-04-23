/****************************************************************************/
/// @file    MSCFModel_PWag2009.cpp
/// @author  Daniel Krajzewicz
/// @date    03.04.2010
/// @version $Id$
///
// Scalable model based on Krauﬂ by Peter Wagner
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
#include "MSCFModel_PWag2009.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_PWag2009::MSCFModel_PWag2009(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
								   SUMOReal dawdle, SUMOReal tau) throw()
        : MSCFModel(vtype, decel), myAccel(accel), myDawdle(dawdle), myTau(tau) {

    myTauDecel = decel * tau;
	myDecelDivTau = decel / tau;
}


MSCFModel_PWag2009::~MSCFModel_PWag2009() throw() {}


SUMOReal
MSCFModel_PWag2009::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vSafe-oldV));
	//
    SUMOReal vNext = vSafe;
    vNext =
        veh->getLaneChangeModel().patchSpeed(
            MAX2((SUMOReal) 0, oldV-(SUMOReal)ACCEL2SPEED(myDecel)), //!!! reverify
            vNext,
            MIN3(vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV)),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    return MIN4(vNext, vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV));
}


SUMOReal
MSCFModel_PWag2009::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return _v(speed, gap, predSpeed, maxNextSpeed(speed));
}


SUMOReal
MSCFModel_PWag2009::ffeV(const MSVehicle * const veh, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return _v(veh->getSpeed(), gap, predSpeed, maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_PWag2009::ffeV(const MSVehicle * const veh, const MSVehicle *pred) const throw() {
    return _v(veh->getSpeed(), veh->gap2pred(*pred), pred->getSpeed(), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_PWag2009::ffeS(const MSVehicle * const veh, SUMOReal gap) const throw() {
    return _v(veh->getSpeed(), gap, 0, maxNextSpeed(veh->getSpeed()));
}


SUMOReal 
MSCFModel_PWag2009::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
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


bool
MSCFModel_PWag2009::hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw() {
    if (gap<0) {
        return false;
    }
    SUMOReal vSafe = _v(speed, gap, predSpeed, maxNextSpeed(speed));
    SUMOReal vNext = MIN3(maxNextSpeed(speed), laneMaxSpeed, vSafe);
    return (vNext>=getSpeedAfterMaxDecel(speed) && gap>= SPEED2DIST(speed));
}


SUMOReal
MSCFModel_PWag2009::dawdle(SUMOReal speed) const throw() {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


SUMOReal 
MSCFModel_PWag2009::_v(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal vmax) const throw() {
    if (predSpeed==0&&gap<0.01) {
        return 0;
    }
	SUMOReal vsafe = -myTauDecel + sqrt(myTauDecel * myTauDecel + predSpeed * predSpeed + 2.0 * myDecel * gap);
	SUMOReal asafe = vsafe-speed;
	SUMOReal apref = asafe;
	if(RandHelper::rand()>.5) {
		apref = myDecelDivTau * (gap+(predSpeed-speed)*myTau-speed*myTau) / (speed+myTauDecel);
		apref += RandHelper::rand((SUMOReal)-1., (SUMOReal)1.);
		if(apref>asafe) apref = asafe;
	}
	return MAX2((SUMOReal)0, speed+apref);//ACCEL2SPEED(apref);
}



//void MSCFModel::saveState(std::ostream &os) {}

