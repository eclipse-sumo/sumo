/***************************************************************************
                          MSRouteHandler.cpp
			  Parser and container for routes during their loading
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.9  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.8  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.7  2003/11/20 13:28:38  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.6  2003/11/20 13:27:42  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.5  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.4  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.1  2003/02/07 10:41:50  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include "MSRouteHandler.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSRouteHandler::MSRouteHandler(const std::string &file,
                               bool addVehiclesDirectly)
    : SUMOSAXHandler("sumo-network/routes", file),
    myLastDepart(0), myLastReadVehicle(0), m_pActiveRoute(0),
    myAddVehiclesDirectly(addVehiclesDirectly)
{
	m_pActiveRoute = new MSEdgeVector(); // !!! why a pointer
	m_pActiveRoute->reserve(100);
}


MSRouteHandler::~MSRouteHandler()
{
	delete m_pActiveRoute;
}


void
MSRouteHandler::myStartElement(int element, const std::string &,
                              const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_VEHICLE:
        addVehicle(attrs);
        break;
    case SUMO_TAG_VTYPE:
        addVehicleType(attrs);
        break;
    case SUMO_TAG_ROUTE:
        openRoute(attrs);
        break;
    default:
        break;
    }
}


void
MSRouteHandler::addVehicleType(const Attributes &attrs)
{
    RGBColor col =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1, 1, 1"));
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
MSRouteHandler::addParsedVehicleType(const string &id, const float length,
                                    const float maxspeed, const float bmax,
                                    const float dmax, const float sigma,
                                    RGBColor &c)
{
    MSVehicleType *vtype =
        new MSVehicleType(id, length, maxspeed, bmax, dmax, sigma);
    if(!MSVehicleType::dictionary(id, vtype)) {
        throw XMLIdAlreadyUsedException("VehicleType", id);
    }
}


void
MSRouteHandler::openRoute(const Attributes &attrs)
{
    // get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a route-object.");
        return;
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("route", "(ID_UNKNOWN!)"));
        return;
    }
    // get the information whether the route shall be deleted after
    // being passed
    bool multiReferenced = false;
    try {
        multiReferenced = getBool(attrs, SUMO_ATTR_MULTIR);
    } catch (...) {
    }
	m_ActiveId = id;
    m_IsMultiReferenced = multiReferenced;
}


void
MSRouteHandler::addVehicle(const Attributes &attrs)
{
    RGBColor col =
        GfxConvHelper::parseColor(
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1, 1, 1"));
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a vehicle-object.");
        return;
    }
    // try to get some optional values
    int repOffset = getIntSecure(attrs, SUMO_ATTR_PERIOD, -1);
    int repNumber = getIntSecure(attrs, SUMO_ATTR_REPNUMBER, -1);
    // now try to build the rest of the vehicle
    MSVehicle *vehicle = 0;
    try {
        vehicle = addParsedVehicle(id,
            getString(attrs, SUMO_ATTR_TYPE),
            getString(attrs, SUMO_ATTR_ROUTE),
            getInt(attrs, SUMO_ATTR_DEPART),
            repNumber, repOffset, col);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing attribute in a vehicle-object.");
    } catch(XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("", ""));
    } catch(XMLIdAlreadyUsedException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("vehicle", id));
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if(myAddVehiclesDirectly) {
        MSNet::getInstance()->myEmitter->add(vehicle);
    } else {
        myLastReadVehicle = vehicle;
    }
}


MSVehicle *
MSRouteHandler::addParsedVehicle(const string &id, const string &vtypeid,
                                 const string &routeid, const long &depart,
                                 int repNumber, int repOffset, RGBColor &c)
{
    MSVehicleType *vtype = MSVehicleType::dictionary(vtypeid);
    if(vtype==0) {
        throw XMLIdNotKnownException("vtype", vtypeid);
    }
    MSRoute *route = MSRoute::dictionary(routeid);
    if(route==0) {
        throw XMLIdNotKnownException("route", routeid);
    }
    MSVehicle *vehicle =
        MSNet::getInstance()->getVehicleControl().buildVehicle(id,
            route, depart, vtype, repNumber, repOffset, c);
    if(!MSVehicle::dictionary(id, vehicle)) {
        throw XMLIdAlreadyUsedException("vehicle", id);
    }
    myLastDepart = depart;
    return vehicle;
}


// ----------------------------------


void
MSRouteHandler::myCharacters(int element, const std::string &name,
                            const std::string &chars)
{
    switch(element) {
    case SUMO_TAG_ROUTE:
        addRouteElements(name, chars);
        break;
    default:
        break;
    }
}


void
MSRouteHandler::addRouteElements(const std::string &name,
                                const std::string &chars)
{
    StringTokenizer st(chars);
    if(st.size()==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Empty route (") + name + string(")"));
        return;
    }
    MSEdge *edge = 0;
    while(st.hasNext()) {
        string set = st.next();
        edge = MSEdge::dictionary(set);
        // check whether the edge exists
        if(edge==0) {
            MsgHandler::getErrorInstance()->inform(
                string("The edge '") + set + string("' within route '")
                + m_ActiveId + string("' is not known."));
            MsgHandler::getErrorInstance()->inform(
                " The route can not be build.");
            return;
        }
        m_pActiveRoute->push_back(edge);
    }
}


// ----------------------------------

void
MSRouteHandler::myEndElement(int element, const std::string &)
{
    switch(element) {
    case SUMO_TAG_ROUTE:
        try {
            closeRoute();
        } catch (XMLListEmptyException &e) {
            MsgHandler::getErrorInstance()->inform(
                e.getMessage("route", ""));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(
                e.getMessage("route", ""));
        }
        break;
    }
}

void
MSRouteHandler::closeRoute()
{
    int size = m_pActiveRoute->size();
    if(size==0) {
		throw XMLListEmptyException();
    }
	MSRoute *route = new MSRoute(m_ActiveId, *m_pActiveRoute, m_IsMultiReferenced);
    m_pActiveRoute->clear();
    if(!MSRoute::dictionary(m_ActiveId, route)) {
	    delete route;
        throw XMLIdAlreadyUsedException("route", m_ActiveId);
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSRouteHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:
