//---------------------------------------------------------------------------//
//                        RORoute.cpp -
//  A build route
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
// Revision 1.14  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.13  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.8  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.6  2004/04/02 11:25:34  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added
//
// Revision 1.5  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.4  2003/09/05 15:23:23  dkrajzew
// umlaute conversion added
//
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:05  dkrajzew
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
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORoute.h"

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
RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const ROEdgeVector &route)
    : Named(StringUtils::convertUmlaute(id)), _costs(costs),
    _probability(prop), _route(route)
{
}


RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const std::vector<const ROEdge*> &route)
    : Named(StringUtils::convertUmlaute(id)), _costs(costs),
    _probability(prop), _route(route)
{
}


RORoute::~RORoute()
{
}


void
RORoute::add(ROEdge *edge)
{
    _route.add(edge);
}


void
RORoute::xmlOut(std::ostream &os, bool isPeriodical) const
{
    os << "   <route id=\"" << _id << "\"";
    if(isPeriodical) {
        os << " multi_ref=\"x\"";
    }
    os << ">";
    os << _route;
    os << "</route>" << endl;
}

void
RORoute::xmlOutEdges(std::ostream &os) const
{
    os << _route;
}


SUMOReal
RORoute::getCosts() const
{
    return _costs;
}


void
RORoute::setCosts(SUMOReal costs)
{
    _costs = costs;
}


SUMOReal
RORoute::recomputeCosts(SUMOTime begin) const
{
    return _route.recomputeCosts(begin);
}


const ROEdge *
RORoute::getFirst() const
{
    return _route.getFirst();
}


const ROEdge *
RORoute::getLast() const
{
    return _route.getLast();
}



SUMOReal
RORoute::getProbability() const
{
    return _probability;
}


bool
RORoute::equals(RORoute *ro) const
{
    return _route.equals(ro->_route);
}


size_t
RORoute::size() const
{
    return _route.size();
}


void
RORoute::setProbability(SUMOReal prop)
{
    _probability = prop;
}


void
RORoute::pruneFirst()
{
    _route.removeFirst();
}


const ROEdgeVector &
RORoute::getEdgeVector() const
{
    return _route;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


