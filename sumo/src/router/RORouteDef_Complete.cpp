//---------------------------------------------------------------------------//
//                        RORouteDef_Complete.cpp -
//  A complete route definition (with all passed edges being known)
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
// Revision 1.3  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.7  2003/11/11 08:04:45  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.6  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.5  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.4  2003/03/20 17:40:58  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <deque>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROAbstractRouter.h"
#include "RORouteDef_Complete.h"


/* =========================================================================
 * using namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORouteDef_Complete::RORouteDef_Complete(const std::string &id,
                                         const RGBColor &color,
                                         const ROEdgeVector &edges)
    : RORouteDef(id, color), _edges(edges)
{
}


RORouteDef_Complete::~RORouteDef_Complete()
{
}


ROEdge *
RORouteDef_Complete::getFrom() const
{
    return _edges.getFirst();
}


ROEdge *
RORouteDef_Complete::getTo() const
{
    return _edges.getLast();
}


RORoute *
RORouteDef_Complete::buildCurrentRoute(ROAbstractRouter &router,
        long begin, bool continueOnUnbuild, ROVehicle &veh,
        ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever)
{
    return new RORoute(_id, 0, 1, _edges);
}


void
RORouteDef_Complete::addAlternative(RORoute *current, long begin)
{
    _startTime = begin;
    delete current;
}


void
RORouteDef_Complete::xmlOutCurrent(std::ostream &res, bool isPeriodical) const
{
    res << "   <route id=\"" << _id << "\"";
    if(isPeriodical) {
        res << " multi_ref=\"x\"";
    }
    res << ">" << _edges << "</route>" << endl;
}


void
RORouteDef_Complete::xmlOutAlternatives(std::ostream &altres) const
{
    altres << "   <routealt id=\"" << _id
        << "\" last=\"0\">" << endl;
    altres << "      <route cost=\"" << _edges.recomputeCosts(_startTime)
        << "\" propability=\"1\">";
    altres << _edges;
    altres << "</route>" << endl;
    altres << "   </routealt>" << endl;
}


RORouteDef *
RORouteDef_Complete::copy(const std::string &id) const
{
    return new RORouteDef_Complete(id, myColor, _edges);
}


const ROEdgeVector &
RORouteDef_Complete::getCurrentEdgeVector() const
{
    return _edges;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


