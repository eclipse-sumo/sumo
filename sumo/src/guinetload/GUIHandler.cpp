/****************************************************************************/
/// @file    GUIHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The XML-Handler for building networks within the gui-version
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <netload/NLHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include "GUIEdgeControlBuilder.h"
#include "GUIJunctionControlBuilder.h"
#include "GUIDetectorBuilder.h"
#include "GUIHandler.h"
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>
#include <microsim/MSGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIHandler::GUIHandler(const std::string &file,
                       MSNet &net,
                       NLDetectorBuilder &detBuilder,
                       NLTriggerBuilder &triggerBuilder,
                       NLEdgeControlBuilder &edgeBuilder,
                       NLJunctionControlBuilder &junctionBuilder,
                       NLGeomShapeBuilder &shapeBuilder,
                       int incDUABase,
                       int incDUAStage)
        : NLHandler(file, net, detBuilder, triggerBuilder,
                    edgeBuilder, junctionBuilder, shapeBuilder, true, incDUABase, incDUAStage) // wants vehicle color
{}


GUIHandler::~GUIHandler()
{}


void
GUIHandler::myStartElement(SumoXMLTag element, const std::string &name,
                           const Attributes &attrs)
{
    NLHandler::myStartElement(element, name, attrs);
}


void
GUIHandler::myCharacters(SumoXMLTag element, const std::string &name,
                         const std::string &chars)
{
    NLHandler::myCharacters(element, name, chars);
    if (wanted(LOADFILTER_NET)) {
        switch (element) {
        case SUMO_TAG_SHAPE:
            addJunctionShape(chars);
            break;
        default:
            break;
        }
    }
}


void
GUIHandler::addJunctionShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIJunctionControlBuilder&>(myJunctionControlBuilder).addJunctionShape(shape);
}


void
GUIHandler::addVehicleType(const Attributes &attrs)
{
    RGBColor col =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1,1,0"));
    // !!! unsecure
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            addParsedVehicleType(id,
                                 getFloatSecure(attrs, SUMO_ATTR_LENGTH, DEFAULT_VEH_LENGTH),
                                 getFloatSecure(attrs, SUMO_ATTR_MAXSPEED, DEFAULT_VEH_MAXSPEED),
                                 getFloatSecure(attrs, SUMO_ATTR_ACCEL, DEFAULT_VEH_A),
                                 getFloatSecure(attrs, SUMO_ATTR_DECEL, DEFAULT_VEH_B),
                                 getFloatSecure(attrs, SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA),
                                 getFloatSecure(attrs, SUMO_ATTR_TAU, DEFAULT_VEH_TAU),
                                 parseVehicleClass(*this, attrs, "vehicle", id),
                                 col,
                                 getFloatSecure(attrs, SUMO_ATTR_PROB, 1.));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing attribute in a vehicletype-object.");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Error in description: one of an vehtype's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a vehicle-object.");
    }
}


void
GUIHandler::addParsedVehicleType(const string &id, const SUMOReal length,
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
            throw XMLIdAlreadyUsedException("VehicleType", id);
        }
    }
}


void
GUIHandler::openRoute(const Attributes &attrs)
{
    myColor =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1,1,0"));
    MSRouteHandler::openRoute(attrs);
}


void
GUIHandler::closeRoute()
{
    int size = myActiveRoute.size();
    if (size==0) {
        throw XMLListEmptyException("route", myActiveRouteID);
    }
    GUIRoute *route =
        new GUIRoute(myColor, myActiveRouteID, myActiveRoute, m_IsMultiReferenced);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {
        delete route;
        if (!MSGlobals::gStateLoaded) {
            throw XMLIdAlreadyUsedException("route", myActiveRouteID);
        }
    }
    if (myAmInEmbeddedMode) {
        myCurrentEmbeddedRoute = route;
    }
}



/****************************************************************************/

