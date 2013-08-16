/****************************************************************************/
/// @file    SUMORouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser for routes during their loading
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

#include <string>
#include <map>
#include <vector>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include "SUMORouteHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMORouteHandler::SUMORouteHandler(const std::string& file) :
    SUMOSAXHandler(file),
    myVehicleParameter(0),
    myLastDepart(-1),
    myActiveRouteColor(0),
    myCurrentVType(0),
    myBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myEndDefault(string2time(OptionsCont::getOptions().getString("end"))) {
}


SUMORouteHandler::~SUMORouteHandler() {
}


SUMOTime
SUMORouteHandler::getLastDepart() const {
    return myLastDepart;
}


bool
SUMORouteHandler::checkLastDepart() {
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        if (myVehicleParameter->depart < myLastDepart) {
            WRITE_WARNING("Route file should be sorted by departure time, ignoring '" + myVehicleParameter->id + "'!");
            return false;
        }
    }
    return true;
}


void
SUMORouteHandler::registerLastDepart() {
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        myLastDepart = myVehicleParameter->depart;
    }
    // else: we don't know when this vehicle will depart. keep the previous known depart time
}


void
SUMORouteHandler::myStartElement(int element,
                                 const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_VEHICLE:
            delete myVehicleParameter;
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
            break;
        case SUMO_TAG_PERSON:
            delete myVehicleParameter;
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
            break;
        case SUMO_TAG_FLOW:
            delete myVehicleParameter;
            myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes(attrs, myBeginDefault, myEndDefault);
            break;
        case SUMO_TAG_VTYPE:
            myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs, getFileName());
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            openVehicleTypeDistribution(attrs);
            break;
        case SUMO_TAG_ROUTE:
            openRoute(attrs);
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            openRouteDistribution(attrs);
            break;
        case SUMO_TAG_STOP:
            addStop(attrs);
            break;
        case SUMO_TAG_TRIP: {
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true);
            if (myVehicleParameter->id == "") {
                //@todo warn about deprecation of missing trip ids
                myVehicleParameter->id = myIdSupplier.getNext();
            }
            myVehicleParameter->setParameter |= VEHPARS_FORCE_REROUTE;
            myActiveRouteID = "!" + myVehicleParameter->id;
            break;
        }
        case SUMO_TAG_INTERVAL: {
            bool ok;
            myBeginDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok);
            myEndDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_END, 0, ok);
            break;
        }
        case SUMO_TAG_PARAM:
            addParam(attrs);
            break;
        default:
            break;
    }
}


void
SUMORouteHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_ROUTE:
            closeRoute();
            break;
        case SUMO_TAG_PERSON:
            closePerson();
            delete myVehicleParameter;
            myVehicleParameter = 0;
            break;
        case SUMO_TAG_VEHICLE:
            if (myVehicleParameter->repetitionNumber > 0) {
                myVehicleParameter->repetitionNumber++; // for backwards compatibility
                // it is a flow, thus no break here
            } else {
                closeVehicle();
                delete myVehicleParameter;
                myVehicleParameter = 0;
                break;
            }
        case SUMO_TAG_FLOW:
            closeFlow();
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            closeVehicleTypeDistribution();
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            closeRouteDistribution();
            break;
        case SUMO_TAG_VTYPE:
            SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVType);
            break;
        case SUMO_TAG_INTERVAL:
            myBeginDefault = string2time(OptionsCont::getOptions().getString("begin"));
            myEndDefault = string2time(OptionsCont::getOptions().getString("end"));
            break;
        case SUMO_TAG_TRIP:
            delete myVehicleParameter;
            myVehicleParameter = 0;
        default:
            break;
    }
}


bool
SUMORouteHandler::checkStopPos(SUMOReal& startPos, SUMOReal& endPos, const SUMOReal laneLength,
                               const SUMOReal minLength, const bool friendlyPos) {
    if (minLength > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < minLength || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if (startPos < 0 || startPos > endPos - minLength) {
        if (!friendlyPos) {
            return false;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > endPos - minLength) {
            startPos = endPos - minLength;
        }
    }
    return true;
}


void
SUMORouteHandler::addParam(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, 0, ok);
    std::string val = attrs.get<std::string>(SUMO_ATTR_VALUE, 0, ok);
    if (myVehicleParameter != 0) {
        myVehicleParameter->addParameter(key, val);
    } else if (myCurrentVType != 0) {
        myCurrentVType->addParameter(key, val);
    }
}



/****************************************************************************/
