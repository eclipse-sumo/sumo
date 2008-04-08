/****************************************************************************/
/// @file    GUIRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <guisim/GUIVehicleType.h>
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
                                 MSVehicleControl &vc,
                                 bool addVehiclesDirectly,
                                 int incDUABase,
                                 int incDUAStage)
        : MSRouteHandler(file, vc, addVehiclesDirectly, incDUABase, incDUAStage)
{}


GUIRouteHandler::~GUIRouteHandler() throw()
{}


void
GUIRouteHandler::addVehicleType(const SUMOSAXAttributes &attrs)
{
    RGBColor col =
        RGBColor::parseColor(
            attrs.getStringSecure(SUMO_ATTR_COLOR, "1,1,0"));
    // !!! unsecure
    // get the id, report an error if not given or empty...
    string id;
    if(attrs.setIDFromAttribues("vtype", id)) {
        try {
            addParsedVehicleType(id,
                                 attrs.getFloatSecure(SUMO_ATTR_LENGTH, DEFAULT_VEH_LENGTH),
                                 attrs.getFloatSecure(SUMO_ATTR_MAXSPEED, DEFAULT_VEH_MAXSPEED),
                                 attrs.getFloatSecure(SUMO_ATTR_ACCEL, DEFAULT_VEH_A),
                                 attrs.getFloatSecure(SUMO_ATTR_DECEL, DEFAULT_VEH_B),
                                 attrs.getFloatSecure(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA),
                                 attrs.getFloatSecure(SUMO_ATTR_TAU, DEFAULT_VEH_TAU),
                                 SUMOVehicleParserHelper::parseVehicleClass(attrs, "vtype", id),
                                 col,
                                 attrs.getFloatSecure(SUMO_ATTR_PROB, 1.));
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing attribute in a vehicletype-object.");
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("One of an vehtype's attributes must be numeric but is not.");
        }
    }
}


void
GUIRouteHandler::addParsedVehicleType(const string &id, const SUMOReal length,
                                      const SUMOReal maxspeed, const SUMOReal bmax,
                                      const SUMOReal dmax, const SUMOReal sigma,
                                      SUMOReal tau,
                                      SUMOVehicleClass vclass,
                                      const RGBColor &c, SUMOReal prob)
{
    myCurrentVehicleType =
        new GUIVehicleType(c, id, length, maxspeed, bmax, dmax, sigma, tau, vclass);
    if (!MSNet::getInstance()->getVehicleControl().addVType(myCurrentVehicleType, prob)) {
        delete myCurrentVehicleType;
        myCurrentVehicleType = 0;
        if (!MSGlobals::gStateLoaded) {
            throw ProcessError("Another vehicle type with the id '" + id + "' exists.");
        }
    }
}


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
    GUIRoute *route =
        new GUIRoute(myColor, myActiveRouteID, myActiveRoute, myVehicleParameter==0||myVehicleParameter->repetitionNumber>=1);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {

        delete route;
        if (!MSGlobals::gStateLoaded) {
            if (myActiveRouteID[0]!='!') {
                throw ProcessError("Another route with the id '" + myActiveRouteID + "' exists.");
            } else {
                if (myVehicleControl.getVehicle(myVehicleParameter->id)==0) {
                    throw ProcessError("Another route for vehicle '" + myActiveRouteID.substr(1) + "' exists.");
                } else {
                    throw ProcessError("A vehicle with id '" + myActiveRouteID.substr(1) + "' already exists.");
                }
            }
        } else {
            route = static_cast<GUIRoute*>(MSRoute::dictionary(myActiveRouteID));
        }
    }
}


void
GUIRouteHandler::openRoute(const SUMOSAXAttributes &attrs)
{
    myColor = RGBColor::parseColor(attrs.getStringSecure(SUMO_ATTR_COLOR, "1,1,0"));
    MSRouteHandler::openRoute(attrs);
}


/****************************************************************************/
