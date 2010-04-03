/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
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

#include "MSCFModel.h"
#include "MSVehicleType.h"
#include "MSVehicle.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel::MSCFModel(const MSVehicleType* vtype, SUMOReal decel) throw()
        : myType(vtype), myDecel(decel) {
    myInverseTwoDecel = SUMOReal(1) / (SUMOReal(2) * decel);
}


MSCFModel::~MSCFModel() throw() {}


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
    return speed * speed * myInverseTwoDecel + speed * getTau();
}


void MSCFModel::saveState(std::ostream &os) {}

