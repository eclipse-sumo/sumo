/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// The car-following model and parameter
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

#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSCFModel_Krauss.h"
#include "MSCFModel_IDM.h"
#include <cassert>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVTypeParameter.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::MSVehicleType(const std::string &id, SUMOReal length,
                             SUMOReal maxSpeed, SUMOReal accel,
                             SUMOReal decel, SUMOReal dawdle,
                             SUMOReal tau, SUMOReal prob,
                             SUMOReal speedFactor, SUMOReal speedDev,
                             SUMOVehicleClass vclass,
                             SUMOEmissionClass emissionClass,
                             SUMOVehicleShape shape,
                             SUMOReal guiWidth, SUMOReal guiOffset,
                             int cfModel, const std::string &lcModel,
                             const RGBColor &c) throw()
        : myID(id), myLength(length), myMaxSpeed(maxSpeed), myAccel(accel),
        myDecel(decel), myDawdle(dawdle), myTau(tau),
        myDefaultProbability(prob), mySpeedFactor(speedFactor),
        mySpeedDev(speedDev), myVehicleClass(vclass),
        myLaneChangeModel(lcModel),
        myEmissionClass(emissionClass), myColor(c),
        myWidth(guiWidth), myOffset(guiOffset), myShape(shape) {
    assert(myLength > 0);
    assert(getMaxSpeed() > 0);
    assert(myAccel > 0);
    assert(myDecel > 0);
    assert(myDawdle >= 0 && myDawdle <= 1);
}


MSVehicleType::~MSVehicleType() throw() {}


void
MSVehicleType::saveState(std::ostream &os) {
    FileHelpers::writeString(os, myID);
    FileHelpers::writeFloat(os, myLength);
    FileHelpers::writeFloat(os, getMaxSpeed());
    FileHelpers::writeFloat(os, myAccel);
    FileHelpers::writeFloat(os, myDecel);
    FileHelpers::writeFloat(os, myDawdle);
    FileHelpers::writeFloat(os, myTau);
    FileHelpers::writeInt(os, (int) myVehicleClass);
    FileHelpers::writeInt(os, (int) myEmissionClass);
    FileHelpers::writeInt(os, (int) myShape);
    FileHelpers::writeFloat(os, myWidth);
    FileHelpers::writeFloat(os, myOffset);
    FileHelpers::writeFloat(os, myDefaultProbability);
    FileHelpers::writeFloat(os, mySpeedFactor);
    FileHelpers::writeFloat(os, mySpeedDev);
    FileHelpers::writeFloat(os, myColor.red());
    FileHelpers::writeFloat(os, myColor.green());
    FileHelpers::writeFloat(os, myColor.blue());
    FileHelpers::writeInt(os, myCarFollowModel->getModelID());
    FileHelpers::writeString(os, myLaneChangeModel);
    //myCarFollowModel->saveState(os);
}


SUMOReal
MSVehicleType::get(const std::map<std::string, SUMOReal> &from, const std::string &name, SUMOReal defaultValue) throw() {
    std::map<std::string, SUMOReal>::const_iterator i = from.find(name);
    if (i==from.end()) {
        return defaultValue;
    }
    return (*i).second;
}


MSVehicleType *
MSVehicleType::build(SUMOVTypeParameter &from) throw(ProcessError) {
    MSVehicleType *vtype = new MSVehicleType(
        from.id, from.length, from.maxSpeed,
        get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
        get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
        get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
        get(from.cfParameter, "tau", DEFAULT_VEH_TAU),
        from.defaultProbability, from.speedFactor, from.speedDev, from.vehicleClass, from.emissionClass,
        from.shape, from.width, from.offset, from.cfModel, from.lcModel, from.color);
    MSCFModel *model = 0;
    if (from.cfModel==SUMO_TAG_CF_IDM) {
        model = new MSCFModel_IDM(vtype,
                                  get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                  get(from.cfParameter, "timeHeadWay", 1.5),
                                  get(from.cfParameter, "minGap", 5.));
    } else {
        model = new MSCFModel_Krauss(vtype,
                                     get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                     get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
    }
    vtype->myCarFollowModel = model;
    return vtype;
}


/****************************************************************************/

