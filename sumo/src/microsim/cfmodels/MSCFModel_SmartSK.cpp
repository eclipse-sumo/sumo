/****************************************************************************/
/// @file    MSCFModel_SmartSK.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// A smarter SK
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include "MSCFModel_SmartSK.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_SmartSK::MSCFModel_SmartSK(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
                                     SUMOReal dawdle, SUMOReal headwayTime,
                                     SUMOReal tmp1, SUMOReal tmp2, SUMOReal tmp3, SUMOReal tmp4, SUMOReal tmp5)
// check whether setting these variables here with default values is ''good'' SUMO design
//        SUMOReal tmp1=0.0, SUMOReal tmp2=5.0, SUMOReal tmp3=0.0, SUMOReal tmp4, SUMOReal tmp5)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle), myTauDecel(decel* headwayTime),
      myTmp1(tmp1), myTmp2(tmp2), myTmp3(tmp3), myTmp4(tmp4), myTmp5(tmp5) {
    // the variable tmp1 is the acceleration delay time, e.g. two seconds (or something like this).
    // for use in the upate process, a rule like if (v<myTmp1) vsafe = 0; is needed.
    // To have this, we have to transform myTmp1 (which is a time) into an equivalent speed. This is done by the
    // using the vsafe formula and computing:
    // v(t=myTmp1) = -myTauDecel + sqrt(myTauDecel*myTauDecel + accel*(accel + decel)*t*t + accel*decel*t*TS);
    SUMOReal t = myTmp1;
    myS2Sspeed = -myTauDecel + sqrt(myTauDecel * myTauDecel + accel * (accel + decel) * t * t + accel * decel * t * TS);
    std::cout << "# s2s-speed: " << myS2Sspeed << std::endl;
    if (myS2Sspeed > 5.0) {
        myS2Sspeed = 5.0;
    }
//	SUMOReal maxDeltaGap = -0.5*ACCEL2DIST(myDecel + myAccel);
    maxDeltaGap = -0.5 * (myDecel + myAccel) * TS * TS;
    std::cout << "# maxDeltaGap = " << maxDeltaGap << std::endl;
    myTmp2 = TS / myTmp2;
    myTmp3 = sqrt(TS) * myTmp3;
}

MSCFModel_SmartSK::~MSCFModel_SmartSK() {}

#include <map>

SUMOReal
MSCFModel_SmartSK::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const SUMOReal vMin = getSpeedAfterMaxDecel(oldV);
    const SUMOReal vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
#ifdef _DEBUG
    if (vMin > vMax) {
        WRITE_WARNING("Vehicle's '" + veh->getID() + "' maximum speed is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    }
#endif
    updateMyHeadway(veh);
    SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();
#ifdef _DEBUG
    if (vars->ggOld.size() > 1) {
        std::cout << "# more than one entry in ggOld list. Speed is " << vPos << ", corresponding dist is " << vars->ggOld[(int) vPos] << "\n";
        for (std::map<int, SUMOReal>::iterator I = vars->ggOld.begin(); I != vars->ggOld.end(); I++) {
            std::cout << "# " << (*I).first << ' ' << (*I).second << std::endl;
        }
    }
#endif

    vars->gOld = vars->ggOld[(int) vPos];
    vars->ggOld.clear();
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}

SUMOReal
MSCFModel_SmartSK::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal /*predMaxDecel*/) const {
    SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();

//	if (((gap - vars->gOld) < maxDeltaGap) && (speed>=5.0) && gap>=5.0) {
    if ((gap - vars->gOld) < maxDeltaGap) {
        SUMOReal tTauTest = gap / speed;
// allow  headway only to decrease only, never to increase. Increase is handled automatically by the headway dynamics in moveHelper()!!!
        if ((tTauTest < vars->myHeadway) && (tTauTest > TS)) {
            vars->myHeadway = tTauTest;
        }
    }

    SUMOReal vsafe = _vsafe(veh, gap, predSpeed);
    if ((speed <= 0.0) && (vsafe < myS2Sspeed)) {
        vsafe = 0;
    }

    SUMOReal vNew = MAX2(getSpeedAfterMaxDecel(speed), MIN2(vsafe, maxNextSpeed(speed, veh)));
    // there must be a better place to do the following assignment!!!
    vars->gOld = gap;
    vars->ggOld[(int)vNew] = gap;
    return vNew;
}

SUMOReal
MSCFModel_SmartSK::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();

//	if (((gap - vars->gOld) < maxDeltaGap) && (speed>=5.0) && gap>=5.0) {
    if ((gap - vars->gOld) < maxDeltaGap) {
        SUMOReal tTauTest = gap / speed;
// allow  headway only to decrease only, never to increase. Increase is handled automatically by the headway dynamics in moveHelper()!!!
        if ((tTauTest < vars->myHeadway) && (tTauTest > TS)) {
            vars->myHeadway = tTauTest;
        }
    }

    return MAX2(getSpeedAfterMaxDecel(speed), MIN2(_vsafe(veh, gap, 0), maxNextSpeed(speed, veh)));
}


SUMOReal
MSCFModel_SmartSK::dawdle(SUMOReal speed) const {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_SmartSK::_vsafe(const MSVehicle* const veh, SUMOReal gap, SUMOReal predSpeed) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();
    // this is the most obvious change to the normal SK: the model uses the variable vars->myHeadway instead of the constant
    // myHeadwayTime as the "reaction time" tau
    SUMOReal bTau = myDecel * (vars->myHeadway);
    SUMOReal vsafe = (SUMOReal)(-1. * bTau
                                + sqrt(
                                    bTau * bTau
                                    + (predSpeed * predSpeed)
                                    + (2. * myDecel * gap)
                                ));
    assert(vsafe >= 0);
    return vsafe;
}


MSCFModel*
MSCFModel_SmartSK::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_SmartSK(vtype, myAccel, myDecel, myDawdle, myHeadwayTime,
                                 myTmp1, myTmp2, myTmp3, myTmp4, myTmp5);
}
