//---------------------------------------------------------------------------//
//                        RORDLoader_SUMOAlt.cpp -
//  A SAX-handler for SUMO-route-alternatives
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
// Revision 1.10  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.9  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be
//  derived for an online-routing
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
// Revision 1.10  2003/10/22 05:22:54  dkrajzew
// missing inclusion added
//
// Revision 1.9  2003/10/21 14:45:00  dkrajzew
// error checking improved
//
// Revision 1.8  2003/10/14 14:17:29  dkrajzew
// forgotten setting of information about an read route added
//
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
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/common/ToString.h>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteDef_Alternatives.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "RORDLoader_SUMOAlt.h"
#include "ROEdgeVector.h"
#include "RORoute.h"
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
RORDLoader_SUMOAlt::RORDLoader_SUMOAlt(ROVehicleBuilder &vb, RONet &net,
                                       SUMOTime begin, SUMOTime end,
                                       SUMOReal gawronBeta, SUMOReal gawronA,
                                       int maxRouteNumber,
                                       const std::string &file)
    : RORDLoader_SUMOBase(vb, net, begin, end,
        "precomputed sumo route alternatives", file),
    myCurrentAlternatives(0),
    myGawronBeta(gawronBeta), myGawronA(gawronA), myMaxRouteNumber(maxRouteNumber)
{
}


RORDLoader_SUMOAlt::~RORDLoader_SUMOAlt()
{
}


void
RORDLoader_SUMOAlt::myStartElement(int element,
                                   const std::string &name,
                                   const Attributes &attrs)
{
    RORDLoader_SUMOBase::myStartElement(element, name, attrs);
    switch(element) {
    case SUMO_TAG_ROUTEALT:
        startAlternative(attrs);
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMOAlt::startRoute(const Attributes &attrs)
{
    mySkipCurrent = false;
    if(myCurrentAlternatives==0) {
        getErrorHandlerMarkInvalid()->inform(
            "Route declaration without an alternatives container occured.");
        mySkipCurrent = true;
        return;
    }
    // try to get the costs
    try {
        myCost = getFloat(attrs, SUMO_ATTR_COST);
    } catch (NumberFormatException) {
        getErrorHandlerMarkInvalid()->inform(
            string("Invalid cost in alternative for route '")
            + myCurrentAlternatives->getID() + string("' (")
            + getString(attrs, SUMO_ATTR_COST)//toString<SUMOReal>(myCost)
            + string(")."));
        mySkipCurrent = true;
        return;
    } catch (EmptyData) {
        getErrorHandlerMarkInvalid()->inform(
            string("Missing cost in alternative for route '")
            + myCurrentAlternatives->getID() + string("'."));
        mySkipCurrent = true;
        return;
    }
    if(myCost<0) {
        getErrorHandlerMarkInvalid()->inform(
            string("Invalid cost in alternative for route '")
            + myCurrentAlternatives->getID() + string("' (")
            + toString<SUMOReal>(myCost)
            + string(")."));
        mySkipCurrent = true;
        return;
    }
    // try to get the probability
    try {
        myProbability = getFloatSecure(attrs, SUMO_ATTR_PROB, -10000);
    } catch (NumberFormatException) {
        getErrorHandlerMarkInvalid()->inform(
            string("Invalid probability in alternative for route '")
            + myCurrentAlternatives->getID() + string("' (")
            + toString<SUMOReal>(myProbability)
            + string(")."));
        mySkipCurrent = true;
        return;
    } catch (EmptyData) {
        getErrorHandlerMarkInvalid()->inform(
            string("Missing probability in alternative for route '")
            + myCurrentAlternatives->getID() + string("'."));
        mySkipCurrent = true;
        return;
    }
    if(myProbability<0) {
        getErrorHandlerMarkInvalid()->inform(
            string("Invalid probability in alternative for route '")
            + myCurrentAlternatives->getID() + string("' (")
            + toString<SUMOReal>(myProbability)
            + string(")."));
        mySkipCurrent = true;
        return;
    }
}


void
RORDLoader_SUMOAlt::myCharacters(int element, const std::string &name,
                                 const std::string &chars)
{
    // process routes only, all other elements do
    //  not have embedded characters
    if(element!=SUMO_TAG_ROUTE) {
        return;
    }
    // check whether the costs and the probability are valid
    if(myCost<0||myProbability<0||mySkipCurrent) {
        return;
    }
    // build the list of edges
    ROEdgeVector *list = new ROEdgeVector();
    bool ok = true;
    StringTokenizer st(chars);
    while(ok&&st.hasNext()) { // !!! too slow !!!
        string id = st.next();
        ROEdge *edge = _net.getEdge(id);
        if(edge!=0) {
            list->add(edge);
        } else {
            getErrorHandlerMarkInvalid()->inform(
                string("The route '") + myCurrentAlternatives->getID() +
                string("' contains the unknown edge '") + id +
                string("'."));
            ok = false;
        }
    }
    if(ok) {
        myCurrentAlternatives->addLoadedAlternative(
            new RORoute(myCurrentAlternatives->getID(), myCost, myProbability, *list));
    }
    delete list;
}


void
RORDLoader_SUMOAlt::myEndElement(int element, const std::string &name)
{
    RORDLoader_SUMOBase::myEndElement(element, name);
    switch(element) {
    case SUMO_TAG_ROUTEALT:
        if(mySkipCurrent) {
            return;
        }
        if(!myAmInEmbeddedMode) {
            myHaveNextRoute = true;
        }
        endAlternative();
        break;
    case SUMO_TAG_VEHICLE:
        myHaveNextRoute = true;
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMOAlt::startAlternative(const Attributes &attrs)
{
    // try to get the id
    string id;
    try {
        mySkipCurrent = false;
        if(myAmInEmbeddedMode) {
            id = getStringSecure(attrs, SUMO_ATTR_ID, "!" + myActiveVehicleID);
        } else {
            id = getString(attrs, SUMO_ATTR_ID);
        }
    } catch (EmptyData) {
        getErrorHandlerMarkInvalid()->inform("Missing route alternative name.");
        return;
    }
    // try to get the index of the last element
    int index = getIntSecure(attrs, SUMO_ATTR_LAST, -1);
    if(index<0) {
        getErrorHandlerMarkInvalid()->inform(string("Missing or non-numeric index of a route alternative (id='") + id + string("'."));
        return;
    }
    myCurrentColor = parseColor(*this, attrs, "route", myCurrentRouteName);
    // try to get the color
//!!!    myCurrentColor = parseColor(attrs, "route", id);
    // try to get the start time
/*    SUMOTime time = getLongSecure(attrs, SUMO_ATTR_DEPART, -1);
    if(time<0) {
        getErrorHandlerMarkInvalid()->inform(
            string("Missing or non-numeric departure time of a route alternative (id='")
            + id + string("'."));
        return;
    }*/
    // !!!
    // build the alternative cont
    myCurrentAlternatives = new RORouteDef_Alternatives(id, myCurrentColor,
        index, myGawronBeta, myGawronA, myMaxRouteNumber);
}


void
RORDLoader_SUMOAlt::endAlternative()
{
    _net.addRouteDef(myCurrentAlternatives);
    myCurrentAlternatives = 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


