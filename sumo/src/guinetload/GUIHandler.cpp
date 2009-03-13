/****************************************************************************/
/// @file    GUIHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The XML-Handler for network loading (gui-version)
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
#include <iostream>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <netload/NLHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/RGBColor.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include "GUIEdgeControlBuilder.h"
#include "GUIJunctionControlBuilder.h"
#include "GUIDetectorBuilder.h"
#include "GUIHandler.h"
#include <guisim/GUIVehicle.h>
#include <guisim/GUIRoute.h>
#include <microsim/MSGlobals.h>
#include <utils/xml/SUMOVehicleParserHelper.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIHandler::GUIHandler(const std::string &file,
                       MSNet &net,
                       NLDetectorBuilder &detBuilder,
                       NLTriggerBuilder &triggerBuilder,
                       NLEdgeControlBuilder &edgeBuilder,
                       NLJunctionControlBuilder &junctionBuilder,
                       NLGeomShapeBuilder &shapeBuilder) throw()
        : NLHandler(file, net, detBuilder, triggerBuilder,
                    edgeBuilder, junctionBuilder, shapeBuilder) {}


GUIHandler::~GUIHandler() throw() {}


void
GUIHandler::myCharacters(SumoXMLTag element,
                         const std::string &chars) throw(ProcessError) {
    NLHandler::myCharacters(element, chars);
    switch (element) {
    case SUMO_TAG_SHAPE:
        addJunctionShape(chars);
        break;
    default:
        break;
    }
}


void
GUIHandler::addJunctionShape(const std::string &chars) {
    try {
        Position2DVector shape = GeomConvHelper::parseShape(chars);
        static_cast<GUIJunctionControlBuilder&>(myJunctionControlBuilder).addJunctionShape(shape);
        return;
    } catch (OutOfBoundsException &) {
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
        return;
    }
    MsgHandler::getErrorInstance()->inform("Could not parse shape of junction '" + myJunctionControlBuilder.getActiveID() + "'.");
}


MSRoute*
GUIHandler::buildRoute() throw() {
    return new GUIRoute(myColor, myActiveRouteID, myActiveRoute, myVehicleParameter==0||myVehicleParameter->repetitionNumber>=1);
}


void
GUIHandler::openRoute(const SUMOSAXAttributes &attrs) {
    myColor =
        RGBColor::parseColor(attrs.getStringSecure(SUMO_ATTR_COLOR, RGBColor::DEFAULT_COLOR_STRING));
    MSRouteHandler::openRoute(attrs);
}


/****************************************************************************/

