/****************************************************************************/
/// @file    ROPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "RORouteDef.h"
#include "ROPerson.h"
#include "RORoute.h"
#include "ROVehicle.h"
#include "ROHelper.h"
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROPerson::ROPerson(const SUMOVehicleParameter& pars)
    : myParameter(pars), myRoutingSuccess(false) {
}


ROPerson::~ROPerson() {}


void
ROPerson::addRide(ROEdge* to, std::vector<std::string> lines){
}


void
ROPerson::addWalk(const SUMOReal duration, const SUMOReal speed, ConstROEdgeVector edges) {
}


void
ROPerson::addStop(const SUMOVehicleParameter::Stop& stopPar, const RONet& net) {
    const ROEdge* stopEdge = net.getEdge(stopPar.lane.substr(0, stopPar.lane.rfind("_")));
    if (stopEdge == 0) {
        // warn here?
        return;
    }
    myPlan.push_back(Stop(stopPar));
}


void
ROPerson::saveAsXML(OutputDevice& os) const {
    // write the person
    myParameter.write(os, OptionsCont::getOptions());

    for (std::map<std::string, std::string>::const_iterator j = myParameter.getMap().begin(); j != myParameter.getMap().end(); ++j) {
        os.openTag(SUMO_TAG_PARAM);
        os.writeAttr(SUMO_ATTR_KEY, (*j).first);
        os.writeAttr(SUMO_ATTR_VALUE, (*j).second);
        os.closeTag();
    }
    os.closeTag();
}


//SUMOReal
//ROPerson::getMaxSpeed() const {
//    return myType->maxSpeed;
//}


/****************************************************************************/

