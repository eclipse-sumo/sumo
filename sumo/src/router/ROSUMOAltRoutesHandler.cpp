//---------------------------------------------------------------------------//
//                        ROSUMOAltRoutesHandler.cpp -
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
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/GfxConvHelper.h>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteAlternativesDef.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "ROSUMOAltRoutesHandler.h"
#include "ROEdgeVector.h"
#include "RORoute.h"
#include "RONet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;



/* =========================================================================
 * method definitions
 * ======================================================================= */
ROSUMOAltRoutesHandler::ROSUMOAltRoutesHandler(RONet &net,
                                               double gawronBeta,
                                               double gawronA,
                                               const std::string &file)
    : ROSUMOHandlerBase(net, "precomputed sumo route alternatives", file),
    _currentAlternatives(0),
    _gawronBeta(gawronBeta), _gawronA(gawronA)
{
}


ROSUMOAltRoutesHandler::~ROSUMOAltRoutesHandler()
{
}


void ROSUMOAltRoutesHandler::myStartElement(int element,
                                            const std::string &name,
                                            const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_ROUTEALT:
        startAlternative(attrs);
        break;
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
ROSUMOAltRoutesHandler::startRoute(const Attributes &attrs)
{
    if(_currentAlternatives==0) {
        MsgHandler::getErrorInstance()->inform(
            "Route declaration without an alternatives container occured.");
        return;
    }
    // try to get the costs
    _cost = getFloatSecure(attrs, SUMO_ATTR_COST, -1);
    if(_cost<0) {
        MsgHandler::getErrorInstance()->inform(
            string("Invalid cost in alternative for route '")
            + _currentAlternatives->getID() + string("'."));
        return;
    }
    // try to get the propability
    _prob = getFloatSecure(attrs, SUMO_ATTR_PROP, -1);
    if(_prob<0) {
        MsgHandler::getErrorInstance()->inform(
            string("Invalid propability in alternative for route '")
            + _currentAlternatives->getID() + string("'."));
        return;
    }
}


void
ROSUMOAltRoutesHandler::myCharacters(int element,
                                     const std::string &name,
                                     const std::string &chars)
{
    // process routes only, all other elements do
    //  not have embedded characters
    if(element!=SUMO_TAG_ROUTE) {
        return;
    }
    // check whether the costs and the propability are valid
    if(_cost<0||_prob<0) {
        return; // !!! should not happen (no costs and no propability)
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
            MsgHandler::getErrorInstance()->inform(
                string("The route '") + _currentAlternatives->getID() +
                string("' contains the unknown edge '") + id +
                string("'."));
            ok = false;
        }
    }
    if(ok) {
        _currentAlternatives->addLoadedAlternative(
            new RORoute(_currentAlternatives->getID(), _cost, _prob, *list));
    }
    delete list;
}


void
ROSUMOAltRoutesHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_ROUTEALT) {
        endAlternative();
    }
}


ROTypedRoutesLoader *
ROSUMOAltRoutesHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROSUMOAltRoutesHandler(_net, _gawronBeta, _gawronA, file);
}


void
ROSUMOAltRoutesHandler::startAlternative(const Attributes &attrs)
{
    // try to get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing route alternative name.");
        return;
    }
    // try to get the index of the last element
    int index = getIntSecure(attrs, SUMO_ATTR_LAST, -1);
    if(index<0) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing or non-numeric index of a route alternative (id='")
            + id + string("'."));
        return;
    }
    // try to get the start time
/*    int time = getLongSecure(attrs, SUMO_ATTR_DEPART, -1);
    if(time<0) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing or non-numeric departure time of a route alternative (id='")
            + id + string("'."));
        return;
    }*/
    // !!!
    // build the alternative cont
    _currentAlternatives = new RORouteAlternativesDef(id, index,
        _gawronBeta, _gawronA);
}


void
ROSUMOAltRoutesHandler::endAlternative()
{
    _net.addRouteDef(_currentAlternatives);
    _currentAlternatives = 0;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROSUMOAltRoutesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


