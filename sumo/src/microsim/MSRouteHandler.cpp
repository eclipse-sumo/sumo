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
// Revision 1.17  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.16  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.15  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.14  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.13  2004/08/02 12:15:43  dkrajzew
// now one does not have to wait until all routes are read if something has failed
//
// Revision 1.12  2004/07/02 09:26:24  dkrajzew
// classes prepared to be derived
//
// Revision 1.11  2004/02/02 16:17:30  dkrajzew
// missing return on broken routes added
//
// Revision 1.10  2004/01/26 07:47:15  dkrajzew
// patched the false specification of default colors
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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
#include "MSNet.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
int beginTime_3;

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
MSRouteHandler::init()
{
    myLastDepart = 0;
    myLastReadVehicle = 0;
}


SUMOTime
MSRouteHandler::getLastDepart() const
{
    return myLastDepart;
}


MSVehicle *
MSRouteHandler::retrieveLastReadVehicle()
{
    MSVehicle *v = myLastReadVehicle;
    myLastReadVehicle = 0;
    return v;
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
    // !!! unsecure
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
MSRouteHandler::addParsedVehicleType(const string &id, const SUMOReal length,
                                    const SUMOReal maxspeed, const SUMOReal bmax,
                                    const SUMOReal dmax, const SUMOReal sigma)
{
    MSVehicleType *vtype =
        new MSVehicleType(id, length, maxspeed, bmax, dmax, sigma);
    if(!MSVehicleType::dictionary(id, vtype)) {
        if(false) { //!!!
            throw XMLIdAlreadyUsedException("VehicleType", id);
        } else {
            delete vtype;
        }
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
            getStringSecure(attrs, SUMO_ATTR_COLOR, "1,1,0"));
    // !!! unsecure
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
        return;
    } catch(XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("", ""));
        return;
    } catch(XMLIdAlreadyUsedException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("vehicle", id));
        return;
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if(myAddVehiclesDirectly) {
        if(vehicle!=0) {
            MSNet::getInstance()->myEmitter->add(vehicle);
        }
    } else {
        myLastReadVehicle = vehicle;
    }
}


MSVehicle *
MSRouteHandler::addParsedVehicle(const string &id, const string &vtypeid,
                                 const string &routeid, const long &depart,
                                 int repNumber, int repOffset, RGBColor &c)
{
    if(MSVehicle::dictionary(id)==0&&depart<beginTime_3) { //!!! only if multiple vehicles allowed
        MSRoute *r = MSRoute::dictionary(routeid);
        if(r!=0&&!r->inFurtherUse()) {
            MSRoute::erase(routeid);
        }
    }

    if(MSVehicle::dictionary(id)!=0||depart<beginTime_3) { //!!! only if multiple vehicles allowed
        /*
        MSRoute *r = MSRoute::dictionary(routeid);
        if(r!=0&&lastUnadded!=) {
            if(!r->inFurtherUse()) {
                MSRoute::erase(routeid);
            }
        }
        */
        return 0;
    }
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
        if(false) { // !!!
            throw XMLIdAlreadyUsedException("vehicle", id);
        } else {
            delete vehicle;
            vehicle = 0;
            return vehicle;
        }
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
            throw ProcessError();
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
        if(false) {//!!!
            delete route;
            throw XMLIdAlreadyUsedException("route", m_ActiveId);
        } else {
            delete route;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
