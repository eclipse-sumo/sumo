/****************************************************************************/
/// @file    GUIRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIRoute.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include "GUIRouteHandler.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSGlobals.h>
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIRouteHandler::GUIRouteHandler(const std::string &file,
                                 bool addVehiclesDirectly)
        : MSRouteHandler(file, addVehiclesDirectly)
{}


GUIRouteHandler::~GUIRouteHandler() throw()
{}


void
GUIRouteHandler::closeRoute() throw(ProcessError)
{
    if (myActiveRoute.size()==0) {
        if (myActiveRouteID[0]!='!') {
            throw ProcessError("Route '" + myActiveRouteID + "' has no edges.");
        } else {
            throw ProcessError("Vehicle's '" + myActiveRouteID.substr(1) + "' route has no edges.");
        }
    }
    const MSRoute *route =
        new GUIRoute(myColor, myActiveRouteID, myActiveRoute, myVehicleParameter==0||myVehicleParameter->repetitionNumber>=1);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {

        delete route;
#ifdef HAVE_MESOSIM
        if (!MSGlobals::gStateLoaded) {
#endif
            if (myActiveRouteID[0]!='!') {
                throw ProcessError("Another route with the id '" + myActiveRouteID + "' exists.");
            } else {
                if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id)==0) {
                    throw ProcessError("Another route for vehicle '" + myActiveRouteID.substr(1) + "' exists.");
                } else {
                    throw ProcessError("A vehicle with id '" + myActiveRouteID.substr(1) + "' already exists.");
                }
            }
#ifdef HAVE_MESOSIM
        } else {
            route = static_cast<const GUIRoute*>(MSRoute::dictionary(myActiveRouteID));
        }
#endif
    }
    myActiveRouteID = "";
}


void
GUIRouteHandler::openRoute(const SUMOSAXAttributes &attrs)
{
    myColor = RGBColor::parseColor(attrs.getStringSecure(SUMO_ATTR_COLOR, RGBColor::DEFAULT_COLOR_STRING));
    MSRouteHandler::openRoute(attrs);
}


/****************************************************************************/
