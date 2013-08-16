/****************************************************************************/
/// @file    ROVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
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

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORoute.h"
#include "ROHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicle::ROVehicle(const SUMOVehicleParameter& pars,
                     RORouteDef* route, const SUMOVTypeParameter* type)
    : myParameter(pars), myType(type), myRoute(route) {}


ROVehicle::~ROVehicle() {}


void
ROVehicle::saveAllAsXML(OutputDevice& os, OutputDevice* const altos,
                        OutputDevice* const typeos, bool withExitTimes) const {
    // check whether the vehicle's type was saved before
    if (myType != 0 && !myType->saved) {
        // ... save if not
        if (typeos != 0) {
            myType->write(*typeos);
        } else {
            myType->write(os);
            if (altos != 0) {
                myType->write(*altos);
            }
        }
        myType->saved = true;
    }

    // write the vehicle (new style, with included routes)
    myParameter.write(os, OptionsCont::getOptions());
    if (altos != 0) {
        myParameter.write(*altos, OptionsCont::getOptions());
    }

    // check whether the route shall be saved
    if (!myRoute->isSaved()) {
        myRoute->writeXMLDefinition(os, this, false, withExitTimes);
        if (altos != 0) {
            myRoute->writeXMLDefinition(*altos, this, true, withExitTimes);
        }
    }
    myParameter.writeStops(os);
    if (altos != 0) {
        myParameter.writeStops(*altos);
    }
    os.closeTag();
    if (altos != 0) {
        altos->closeTag();
    }
}


SUMOReal
ROVehicle::getMaxSpeed() const {
    return myType->maxSpeed;
}


ROVehicle*
ROVehicle::copy(const std::string& id, unsigned int depTime,
                RORouteDef* newRoute) const {
    SUMOVehicleParameter pars(myParameter);
    pars.id = id;
    pars.depart = depTime;
    return new ROVehicle(pars, newRoute, myType);
}


/****************************************************************************/

