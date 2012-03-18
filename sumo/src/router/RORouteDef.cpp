/****************************************************************************/
/// @file    RORouteDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for a vehicle's route definition
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

#include <string>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "ROEdge.h"
#include "RORoute.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "ReferencedItem.h"
#include "RORouteDef.h"
#include "ROVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef::RORouteDef(const std::string& id, const RGBColor* const color) : 
    ReferencedItem(), 
    Named(StringUtils::convertUmlaute(id)),
    myColor(color),
    myPrecomputed(0), myLastUsed(0)
{}


RORouteDef::~RORouteDef() {
    for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        delete *i;
    }
    delete myColor;
}


const RGBColor*
RORouteDef::copyColorIfGiven() const {
    if (myColor == 0) {
        return 0;
    }
    return new RGBColor(*myColor);
}


RORoute*
RORouteDef::buildCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle& veh) const {
    if (myPrecomputed == 0) {
        preComputeCurrentRoute(router, begin, veh);
    }
    return myPrecomputed;
}


const ROEdge*
RORouteDef::getDestination() const {
    return myAlternatives[0]->getLast();
}


OutputDevice&
RORouteDef::writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                               bool asAlternatives, bool withExitTimes) const {
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, myLastUsed).closeOpener();
        for (size_t i = 0; i != myAlternatives.size(); i++) {
            myAlternatives[i]->writeXMLDefinition(dev, veh, true, withExitTimes);
        }
        dev.closeTag();
        return dev;
    } else {
        return myAlternatives[myLastUsed]->writeXMLDefinition(dev, veh, false, withExitTimes);
    }
}


/****************************************************************************/
