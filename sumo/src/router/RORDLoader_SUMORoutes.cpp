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
// Revision 1.8  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.7  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.6  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
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
RORDLoader_SUMORoutes::RORDLoader_SUMORoutes(ROVehicleBuilder &vb, RONet &net,
                                             SUMOTime begin,
                                             SUMOTime end,
                                             const std::string &file)
    : RORDLoader_SUMOBase(vb, net, begin, end, "precomputed sumo routes", file)
{
}


RORDLoader_SUMORoutes::~RORDLoader_SUMORoutes()
{
}


void
RORDLoader_SUMORoutes::startRoute(const Attributes &attrs)
{
    try {
        mySkipCurrent = false;
        if(myAmInEmbeddedMode) {
            myCurrentRouteName = getStringSecure(attrs, SUMO_ATTR_ID, "!" + myActiveVehicleID);
        } else {
            myCurrentRouteName = getString(attrs, SUMO_ATTR_ID);
        }
        myCurrentColor = parseColor(*this, attrs, "route", myCurrentRouteName);
    } catch (EmptyData) {
        myCurrentRouteName = "";
        getErrorHandlerMarkInvalid()->inform("Missing id in route.");
    }
}



void
RORDLoader_SUMORoutes::myCharacters(int element, const std::string &name,
                                   const std::string &chars)
{
    if(element==SUMO_TAG_ROUTE&&myCurrentRouteName.length()!=0) {
        // parse the list of edges
        ROEdgeVector list;
        StringTokenizer st(chars);
        bool ok = st.size()>1;
        while(ok&&st.hasNext()) { // !!! too slow !!!
            string id = st.next();
            ROEdge *edge = _net.getEdge(id);
            if(edge!=0) {
                list.add(edge);
            } else {
                getErrorHandlerMarkInvalid()->inform(
                    "The route '" + myCurrentRouteName + "' contains the unknown edge '"
					+ id + "'.");
                ok = false;
            }
        }
        // ... after reading the route ...
        if(ok) {
            // build the route if everything's ok
            RORouteDef_Complete *route =
                new RORouteDef_Complete(myCurrentRouteName, myCurrentColor, list);
            _net.addRouteDef(route);
        } else {
            // report problems otherwise
            if(myCurrentRouteName.length()>0) {
                getErrorHandlerMarkInvalid()->inform("Something is wrong with route '" + myCurrentRouteName + "'.");
            } else {
                getErrorHandlerMarkInvalid()->inform("Invalid route occured.");
            }
        }
    }
}


void
RORDLoader_SUMORoutes::myEndElement(int element, const std::string &name)
{
    RORDLoader_SUMOBase::myEndElement(element, name);
    switch(element) {
    case SUMO_TAG_ROUTE:
        if(!myAmInEmbeddedMode) {
            myHaveNextRoute = true;
        }
        break;
    case SUMO_TAG_VEHICLE:
        myHaveNextRoute = true;
        break;
    default:
        break;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


