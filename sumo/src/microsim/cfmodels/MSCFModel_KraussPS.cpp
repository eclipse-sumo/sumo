/****************************************************************************/
/// @file    MSCFModel_KraussPS.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id: MSCFModel_KraussPS.cpp 13107 2012-12-02 13:57:34Z behrisch $
///
// Krauss car-following model, changing accel and speed by slope
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "MSCFModel_KraussPS.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussPS::MSCFModel_KraussPS(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel_Krauss(vtype, accel, decel, dawdle, headwayTime) {
}


MSCFModel_KraussPS::~MSCFModel_KraussPS() {}



SUMOReal 
MSCFModel_KraussPS::maxNextSpeed(SUMOReal speed, const MSVehicle * const veh) const {
    const SUMOReal lp = veh->getPositionOnLane();
    const MSLane *lane = veh->getLane();
    const SUMOReal gp = lane->interpolateLanePosToGeometryPos(lp);
    const SUMOReal slope = lane->getShape().slopeDegreeAtLengthPosition(gp);
    const SUMOReal cSlope = sin(slope);
	SUMOReal aMax = getMaxAccel();
    aMax = aMax - aMax*cSlope;
	SUMOReal vMax = myType->getMaxSpeed();
    vMax = vMax - vMax*cSlope;
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(aMax), vMax);
}



MSCFModel*
MSCFModel_KraussPS::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussPS(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


//void MSCFModel::saveState(std::ostream &os) {}

