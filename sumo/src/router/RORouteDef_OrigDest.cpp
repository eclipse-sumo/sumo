//---------------------------------------------------------------------------//
//                        RORouteDef_OrigDest.cpp -
//  A route where only the origin and the destination edges are known
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
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:53:07  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/12/16 12:26:52  dkrajzew
// debugging
//
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
// Revision 1.9  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.6  2003/04/10 15:47:01  dkrajzew
// random routes are now being prunned to avoid some stress with turning
//  vehicles
//
// Revision 1.5  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.4  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
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
#include <iostream>
#include <cassert>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "RORouteDef_OrigDest.h"
#include "ROAbstractRouter.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
RORouteDef_OrigDest::RORouteDef_OrigDest(const std::string &id,
                                         const RGBColor &color,
                                         ROEdge *from, ROEdge *to,
                                         bool removeFirst)
    : RORouteDef(id, color), _from(from), _to(to), _current(0),
    myRemoveFirst(removeFirst)
{
}


RORouteDef_OrigDest::~RORouteDef_OrigDest()
{
    delete _current;
}


ROEdge *
RORouteDef_OrigDest::getFrom() const
{
    return _from;
}


ROEdge *
RORouteDef_OrigDest::getTo() const
{
    return _to;
}


RORoute *
RORouteDef_OrigDest::buildCurrentRoute(ROAbstractRouter &router, SUMOTime begin,
		bool continueOnUnbuild, ROVehicle &veh,
		ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever)
{
    ROEdgeVector rv = router.compute(_from, _to, begin, continueOnUnbuild, retriever);
    if(myRemoveFirst&&rv.size()>1) {
        rv.removeEnds();
    }
    return new RORoute(_id, 0, 1, rv);
}


void
RORouteDef_OrigDest::addAlternative(RORoute *current, SUMOTime begin)
{
    _current = current;
    _startTime = begin;
}


void
RORouteDef_OrigDest::xmlOutCurrent(std::ostream &res,
                                   bool isPeriodical) const
{
    _current->xmlOut(res, isPeriodical);
}


void
RORouteDef_OrigDest::xmlOutAlternatives(std::ostream &altres) const
{
    altres << "   <routealt id=\"" << _current->getID()
        << "\" last=\"0\">" << endl;
    altres << "      <route cost=\"" << _current->recomputeCosts(_startTime)
        << "\" probability=\"1\">";
    _current->xmlOutEdges(altres);
    altres << "</route>" << endl;
    altres << "   </routealt>" << endl;
}


RORouteDef *
RORouteDef_OrigDest::copy(const std::string &id) const
{
    return new RORouteDef_OrigDest(id, myColor, _from, _to,
        myRemoveFirst);
}


const ROEdgeVector &
RORouteDef_OrigDest::getCurrentEdgeVector() const
{
    return _current->getEdgeVector();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


