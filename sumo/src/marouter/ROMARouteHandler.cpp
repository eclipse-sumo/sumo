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

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice_String.h>
#include <router/RONet.h>
#include <router/ROLane.h>
#include "ROMARouteHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROMARouteHandler::ROMARouteHandler(RONet& net) :
    SUMORouteHandler(""),
    myNet(net) {
}


ROMARouteHandler::~ROMARouteHandler() {
}


void
ROMARouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    SUMORouteHandler::myStartElement(element, attrs);
    switch (element) {
        case SUMO_TAG_FLOW:
            myActiveRouteProbability = DEFAULT_VEH_PROB;
            break;
        case SUMO_TAG_TRIP:
        case SUMO_TAG_VEHICLE:
            myActiveRouteProbability = DEFAULT_VEH_PROB;
            break;
        default:
            break;
    }
}


void
ROMARouteHandler::parseEdges(const std::string& desc, ConstROEdgeVector& into,
                           const std::string& rid) {
    if (desc[0] == BinaryFormatter::BF_ROUTE) {
        std::istringstream in(desc, std::ios::binary);
        char c;
        in >> c;
        FileHelpers::readEdgeVector(in, into, rid);
    } else {
        for (StringTokenizer st(desc); st.hasNext();) {
            const std::string id = st.next();
            const ROEdge* edge = myNet.getEdge(id);
            if (edge == 0) {
                WRITE_ERROR("The edge '" + id + "' within the route " + rid + " is not known.");
            } else {
                into.push_back(edge);
            }
        }
    }
}


/****************************************************************************/
