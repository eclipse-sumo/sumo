//---------------------------------------------------------------------------//
//                        GUINetHandler.cpp -
//  The XML-Handler for building networks within the gui-version derived
//      from NLNetHandler
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.24  2005/10/07 11:37:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.23  2005/09/22 13:39:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.22  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.21  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.20  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.19  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.18  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.17  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.16  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics
//
// Revision 1.15  2004/01/12 14:59:51  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.14  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.13  2003/12/05 10:37:23  dkrajzew
// made the code a little bit more pretty
//
// Revision 1.12  2003/12/04 13:25:52  dkrajzew
// handling of internal links added; documentation added;
//  some dead code removed
//
// Revision 1.11  2003/11/17 07:13:48  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.10  2003/10/08 14:48:56  dkrajzew
// new usage of MSAgentbased... impemented
//
// Revision 1.9  2003/10/02 14:51:20  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.8  2003/10/01 11:13:13  dkrajzew
// agent-based tl-logic allocation added
//
// Revision 1.7  2003/09/24 09:54:11  dkrajzew
// bug on building induct loops of an actuated tls within the gui patched
//
// Revision 1.6  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.5  2003/07/16 15:21:16  dkrajzew
// conversion tools splitted and relocated to avoid mandatory inclusion
//  of unused files
//
// Revision 1.4  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry
//  implemented
//
// Revision 1.3  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/13 15:59:00  dkrajzew
// unnecessary output of build edges id removed
//
// Revision 1.1  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <netload/NLNetHandler.h>
//#include <netload/NLContainer.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
//#include "GUIContainer.h"
#include "GUIEdgeControlBuilder.h"
#include "GUIJunctionControlBuilder.h"
#include "GUIDetectorBuilder.h"
#include "GUINetHandler.h"
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetHandler::GUINetHandler(const std::string &file,
                             MSNet &net,
                             NLDetectorBuilder &detBuilder,
                             NLTriggerBuilder &triggerBuilder,
                             NLEdgeControlBuilder &edgeBuilder,
                             NLJunctionControlBuilder &junctionBuilder,
                             NLGeomShapeBuilder &shapeBuilder)
    : NLNetHandler(file, net, detBuilder, triggerBuilder,
        edgeBuilder, junctionBuilder, shapeBuilder)
{
}


GUINetHandler::~GUINetHandler()
{
}


void
GUINetHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    NLNetHandler::myStartElement(element, name, attrs);
}


void
GUINetHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    NLNetHandler::myCharacters(element, name, chars);
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case SUMO_TAG_SHAPE:
            addJunctionShape(chars);
            break;
        default:
            break;
        }
    }
}


void
GUINetHandler::addJunctionShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIJunctionControlBuilder&>(myJunctionControlBuilder).addJunctionShape(shape);
}


void
GUINetHandler::addVehicleType(const Attributes &attrs)
{
    RGBColor col =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1,1,0"));
    // !!! unsecure
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            addParsedVehicleType(id,
                getFloat(attrs, SUMO_ATTR_LENGTH),
                getFloat(attrs, SUMO_ATTR_MAXSPEED),
                getFloat(attrs, SUMO_ATTR_ACCEL),
                getFloat(attrs, SUMO_ATTR_DECEL),
                getFloat(attrs, SUMO_ATTR_SIGMA),
                col);
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("vehicletype", id));
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: missing attribute in a vehicletype-object.");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: one of an vehtype's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a vehicle-object.");
    }
}


void
GUINetHandler::addParsedVehicleType(const string &id, const SUMOReal length,
                                    const SUMOReal maxspeed, const SUMOReal bmax,
                                    const SUMOReal dmax, const SUMOReal sigma,
                                    const RGBColor &c)
{
    GUIVehicleType *vtype =
        new GUIVehicleType(c, id, length, maxspeed, bmax, dmax, sigma);
    if(!MSVehicleType::dictionary(id, vtype)) {
        throw XMLIdAlreadyUsedException("VehicleType", id);
    }
}


void
GUINetHandler::openRoute(const Attributes &attrs)
{
    myColor =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1,1,0"));
    MSRouteHandler::openRoute(attrs);
}


void
GUINetHandler::closeRoute()
{
    int size = m_pActiveRoute->size();
    if(size==0) {
        throw XMLListEmptyException();
    }
    GUIRoute *route =
        new GUIRoute(myColor, m_ActiveId, *m_pActiveRoute, m_IsMultiReferenced);
    m_pActiveRoute->clear();
    if(!MSRoute::dictionary(m_ActiveId, route)) {
        delete route;
        throw XMLIdAlreadyUsedException("route", m_ActiveId);
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

