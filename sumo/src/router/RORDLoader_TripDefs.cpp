/****************************************************************************/
/// @file    RORDLoader_TripDefs.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for loading trip definitions
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "RORoute.h"
#include "RONet.h"
#include "RORouteDef.h"
#include "RORDLoader_TripDefs.h"
#include "ROVehicle.h"
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORDLoader_TripDefs::RORDLoader_TripDefs(RONet& net,
        SUMOTime begin, SUMOTime end,
        bool emptyDestinationsAllowed, bool withTaz,
        const std::string& fileName)
    : ROTypedXMLRoutesLoader(net, begin, end, fileName),
      myEmptyDestinationsAllowed(emptyDestinationsAllowed),
      myWithTaz(withTaz), myCurrentVehicleType(0),
      myParameter(0) {}


RORDLoader_TripDefs::~RORDLoader_TripDefs() {}


void
RORDLoader_TripDefs::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    // check whether a trip definition shall be parsed
    if (element == SUMO_TAG_TRIP) {
        bool ok = true;
        // get the vehicle id, the edges, the speed and position and
        //  the departure time and other information
        std::string id = getVehicleID(attrs);
        myCurrentDepart = attrs.getSUMOTimeReporting(SUMO_ATTR_DEPART, id.c_str(), ok);
        if (myWithTaz) {
            myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM_TAZ, id, false);
            myEndEdge = getEdge(attrs, "destination", SUMO_ATTR_TO_TAZ, id, myEmptyDestinationsAllowed);
        } else {
            myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, id, false);
            myEndEdge = getEdge(attrs, "destination", SUMO_ATTR_TO, id, myEmptyDestinationsAllowed);
        }
        myParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true);
        myParameter->id = id;
        // recheck attributes
        if (!ok) {
            return;
        }
        if (myCurrentDepart < 0) {
            WRITE_ERROR("The departure time must be positive.");
            return;
        }
    }
    // check whether a vehicle type shall be parsed
    if (element == SUMO_TAG_VTYPE) {
        myCurrentVehicleType = SUMOVehicleParserHelper::beginVTypeParsing(attrs, getFileName());
    } else if (myCurrentVehicleType != 0) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVehicleType, element, attrs);
    }
}


std::string
RORDLoader_TripDefs::getVehicleID(const SUMOSAXAttributes& attrs) {
    // try to get the id, do not report an error if not given or empty...
    bool ok = true;
    std::string id = attrs.getOptStringReporting(SUMO_ATTR_ID, 0, ok, "", false);
    // get a valid vehicle id
    if (id == "") {
        return myIdSupplier.getNext();
    } else {
        return id;
    }
}


ROEdge*
RORDLoader_TripDefs::getEdge(const SUMOSAXAttributes& attrs,
                             const std::string& purpose,
                             SumoXMLAttr which, const std::string& vid,
                             bool emptyAllowed) {
    UNUSED_PARAMETER(purpose);
    bool ok = true;
    std::string id = attrs.getStringReporting(which, 0, ok, !emptyAllowed);
    if (which == SUMO_ATTR_FROM_TAZ) {
        id += "-source";
    }
    if (which == SUMO_ATTR_TO_TAZ) {
        id += "-sink";
    }
    ROEdge* e = myNet.getEdge(id);
    if (e == 0 && !emptyAllowed) {
        WRITE_ERROR("The edge '" + id + "' is not known.\n Vehicle id='" + vid + "'.");
    }
    return e;
}


void
RORDLoader_TripDefs::myEndElement(int element) {
    if (element == SUMO_TAG_TRIP && !MsgHandler::getErrorInstance()->wasInformed()) {
        if (myCurrentDepart < myBegin || myCurrentDepart >= myEnd) {
            delete myParameter;
            return;
        }
        RGBColor* col = myParameter->wasSet(VEHPARS_COLOR_SET) ? new RGBColor(myParameter->color) : 0;
        RORouteDef* route = new RORouteDef(myParameter->id, 0, true);
        std::vector<const ROEdge*> edges;
        edges.push_back(myBeginEdge);
        edges.push_back(myEndEdge);
        route->addLoadedAlternative(new RORoute(myParameter->id, 0, 1, edges, col));
        SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myParameter->vtypeid);
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        if (myNet.addRouteDef(route)) {
            myNextRouteRead = true;
            // build the vehicle
            ROVehicle* veh = new ROVehicle(*myParameter, route, type);
            myNet.addVehicle(myParameter->id, veh);
        } else {
            WRITE_ERROR("The vehicle '" + myParameter->id + "' occurs at least twice.");
            delete route;
        }
        delete myParameter;
        myParameter = 0;
    }
    if (element == SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVehicleType);
        myNet.addVehicleType(myCurrentVehicleType);
        myCurrentVehicleType = 0;
    }
}


/****************************************************************************/

