/****************************************************************************/
/// @file    RORDLoader_SUMORoutes.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SAX-handler for SUMO-routes
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/GfxConvHelper.h>
#include "RORouteDef.h"
#include "RORouteDef_Complete.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "RORDLoader_SUMORoutes.h"
#include "ROEdgeVector.h"
#include "RONet.h"

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
RORDLoader_SUMORoutes::RORDLoader_SUMORoutes(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin,
        SUMOTime end,
        const std::string &file)
        : RORDLoader_SUMOBase(vb, net, begin, end, "precomputed sumo routes", file)
{}


RORDLoader_SUMORoutes::~RORDLoader_SUMORoutes()
{}


void
RORDLoader_SUMORoutes::startRoute(const Attributes &attrs)
{
    try {
        mySkipCurrent = false;
        if (myAmInEmbeddedMode) {
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
RORDLoader_SUMORoutes::myCharacters(int element, const std::string &/*name*/,
                                    const std::string &chars)
{
    if (element==SUMO_TAG_ROUTE&&myCurrentRouteName.length()!=0) {
        // parse the list of edges
        ROEdgeVector list;
        StringTokenizer st(chars);
        bool ok = st.size()>1;
        while (ok&&st.hasNext()) { // !!! too slow !!!
            string id = st.next();
            ROEdge *edge = _net.getEdge(id);
            if (edge!=0) {
                list.add(edge);
            } else {
                getErrorHandlerMarkInvalid()->inform(
                    "The route '" + myCurrentRouteName + "' contains the unknown edge '"
                    + id + "'.");
                ok = false;
            }
        }
        // ... after reading the route ...
        if (ok) {
            // build the route if everything's ok
            RORouteDef_Complete *route =
                new RORouteDef_Complete(myCurrentRouteName, myCurrentColor, list);
            _net.addRouteDef(route);
        } else {
            // report problems otherwise
            if (myCurrentRouteName.length()>0) {
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
    switch (element) {
    case SUMO_TAG_ROUTE:
        if (!myAmInEmbeddedMode) {
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



/****************************************************************************/

