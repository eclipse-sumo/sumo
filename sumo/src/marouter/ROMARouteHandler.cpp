/****************************************************************************/
/// @file    ROMARouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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

#include <utils/xml/SUMOVehicleParserHelper.h>
#include <od/ODMatrix.h>
#include "ROMARouteHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROMARouteHandler::ROMARouteHandler(ODMatrix& matrix) :
    SUMOSAXHandler(""),
    myMatrix(matrix) {
}


ROMARouteHandler::~ROMARouteHandler() {
}


void
ROMARouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    SUMOVehicleParameter* parameter = 0;
    if (element == SUMO_TAG_TRIP || element == SUMO_TAG_VEHICLE) {
        SUMOVehicleParameter* parameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        myMatrix.add(parameter->id, parameter->depart, parameter->fromTaz, parameter->toTaz, parameter->vtypeid);
    }
}


/****************************************************************************/
