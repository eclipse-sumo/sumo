//---------------------------------------------------------------------------//
//                        RORDLoader_SUMORoutes.cpp -
//  A SAX-handler for SUMO-routes
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
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.7  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/GfxConvHelper.h>
#include "RORouteDef.h"
#include "RORouteDef_Complete.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "RORDLoader_SUMORoutes.h"
#include "ROEdgeVector.h"
#include "RONet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORDLoader_SUMORoutes::RORDLoader_SUMORoutes(RONet &net,
										   const std::string &file)
    : RORDLoader_SUMOBase(net, "precomputed sumo routes", file)
{
}


RORDLoader_SUMORoutes::~RORDLoader_SUMORoutes()
{
}

void RORDLoader_SUMORoutes::myStartElement(int element,
										  const std::string &name,
										  const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_ROUTE:
        startRoute(attrs);
        break;
    case SUMO_TAG_VEHICLE:
        startVehicle(attrs);
        break;
    case SUMO_TAG_VTYPE:
        startVehType(attrs);
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMORoutes::startRoute(const Attributes &attrs)
{
    try {
        _currentRoute = getString(attrs, SUMO_ATTR_ID);
        myCurrentColor = parseColor(attrs, "route", _currentRoute);
    } catch (EmptyData) {
        _currentRoute = "";
        MsgHandler::getErrorInstance()->inform("Missing id in route.");
    }
}



void
RORDLoader_SUMORoutes::myCharacters(int element, const std::string &name,
                                   const std::string &chars)
{
    if(element==SUMO_TAG_ROUTE&&_currentRoute.length()!=0) {
        ROEdgeVector list;
        StringTokenizer st(chars);
        bool ok = st.size()>1;
        while(ok&&st.hasNext()) { // !!! too slow !!!
            string id = st.next();
            ROEdge *edge = _net.getEdge(id);
            if(edge!=0) {
                list.add(edge);
            } else {
                MsgHandler::getErrorInstance()->inform(
                    string("The route '") + _currentRoute +
                    string("' contains the unknown edge '") + id +
                    string("'."));
                ok = false;
            }
        }
        if(ok) {
            RORouteDef_Complete *route =
                new RORouteDef_Complete(_currentRoute, myCurrentColor, list);
            _net.addRouteDef(route);
        } else {
            if(_currentRoute.length()>0) {
                MsgHandler::getErrorInstance()->inform(
                    string("Something is wrong with route '")
					+ _currentRoute + string("'."));
            } else {
                MsgHandler::getErrorInstance()->inform(
                    string("Invalid route occured."));
            }
        }
    }
}


void
RORDLoader_SUMORoutes::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_ROUTE) {
        _currentRoute = "";
        _nextRouteRead = true;
    }
}


ROAbstractRouteDefLoader *
RORDLoader_SUMORoutes::getAssignedDuplicate(const std::string &file) const
{
    return new RORDLoader_SUMORoutes(_net, file);
}


bool
RORDLoader_SUMORoutes::nextRouteRead()
{
    return _nextRouteRead;
}


void
RORDLoader_SUMORoutes::beginNextRoute()
{
    _nextRouteRead = false;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


