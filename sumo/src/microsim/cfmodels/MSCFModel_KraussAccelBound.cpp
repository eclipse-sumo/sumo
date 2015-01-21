/****************************************************************************/
/// @file    MSCFModel_KraussAccelBound.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 07 Mar 2014
/// @version $Id$
///
// Krauss car-following model, with PHEMlight-based acceleration limits
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/StdDefs.h>
#include <utils/emissions/PollutantsInterface.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_KraussAccelBound.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussAccelBound::MSCFModel_KraussAccelBound(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel_Krauss(vtype, accel, decel, dawdle, headwayTime) {
}


MSCFModel_KraussAccelBound::~MSCFModel_KraussAccelBound() {}



SUMOReal
MSCFModel_KraussAccelBound::maxNextSpeed(SUMOReal speed, const MSVehicle* const veh) const {
    const MSLane* const lane = veh->getLane();
    const SUMOReal gp = lane->interpolateLanePosToGeometryPos(veh->getPositionOnLane());
    const SUMOReal slope = lane->getShape().slopeDegreeAtOffset(gp);
    const SUMOReal aMax = PollutantsInterface::getMaxAccel(veh->getVehicleType().getEmissionClass(), speed, 0, slope);
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(aMax), veh->getVehicleType().getMaxSpeed());
}



MSCFModel*
MSCFModel_KraussAccelBound::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussAccelBound(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


//void MSCFModel::saveState(std::ostream &os) {}

