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
#include "MSRouteHandler.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
MSRouteHandler::MSRouteHandler(bool verbose, bool warn,
                               const std::string &file,
                               bool addVehiclesDirectly)
    : SUMOSAXHandler("sumo-network/routes", warn, verbose, file),
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
MSRouteHandler::myStartElement(int element, const std::string &name,
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
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            addParsedVehicleType(id,
                getFloat(attrs, SUMO_ATTR_LENGTH),
                getFloat(attrs, SUMO_ATTR_MAXSPEED),
                getFloat(attrs, SUMO_ATTR_ACCEL),
                getFloat(attrs, SUMO_ATTR_DECEL),
                getFloat(attrs, SUMO_ATTR_SIGMA));
        } catch (XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("vehicletype", id));
        } catch (EmptyData) {
            SErrorHandler::add(
                "Error in description: missing attribute in a vehicletype-object.");
        } catch (NumberFormatException) {
            SErrorHandler::add(
                "Error in description: one of an vehtype's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a vehicle-object.");
    }
}


void
MSRouteHandler::addParsedVehicleType(const string &id, const float length,
                                    const float maxspeed, const float bmax,
                                    const float dmax, const float sigma)
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
        SErrorHandler::add(
            "Error in description: missing id of a route-object.");
        return;
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("route", "(ID_UNKNOWN!)"));
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
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a vehicle-object.");
        return;
    }
    // now try to build the rest of the vehicle
    MSVehicle *vehicle = 0;
    try {
        vehicle = addParsedVehicle(id,
            getString(attrs, SUMO_ATTR_TYPE),
            getString(attrs, SUMO_ATTR_ROUTE),
            getInt(attrs, SUMO_ATTR_DEPART));
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing attribute in a vehicle-object.");
    } catch(XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
    } catch(XMLIdAlreadyUsedException &e) {
        SErrorHandler::add(e.getMessage("vehicle", id));
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
                                 const string &routeid, const long depart)
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
        MSNet::getInstance()->buildNewVehicle(id, route, depart, vtype, 0);
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
        SErrorHandler::add("Empty route (" + name + ")");
        return;
    }
    while(st.hasNext()) {
        string set = st.next();
        MSEdge *edge = MSEdge::dictionary(set);
        if(edge==0) {
            SErrorHandler::add(
                string("The edge '") + set + string("' within route '")
                + m_ActiveId + string("' is not known."));
            SErrorHandler::add(" The route can not be build.");
            return;
        }
        m_pActiveRoute->push_back(edge);
    }
}


// ----------------------------------

void
MSRouteHandler::myEndElement(int element, const std::string &name)
{
    switch(element) {
    case SUMO_TAG_ROUTE:
        try {
            closeRoute();
        } catch (XMLListEmptyException &e) {
            SErrorHandler::add(e.getMessage("route", ""));
        } catch (XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("route", ""));
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
