//---------------------------------------------------------------------------//
//                        ROCompleteRouteDef.cpp -
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
// Revision 1.4  2003/03/20 17:40:58  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
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
#include <deque>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "RORouter.h"
#include "ROCompleteRouteDef.h"

using namespace std;

ROCompleteRouteDef::ROCompleteRouteDef(const std::string &id,
                                       const ROEdgeVector &edges)
    : RORouteDef(id), _edges(edges)
{
}


ROCompleteRouteDef::~ROCompleteRouteDef()
{
}


ROEdge *
ROCompleteRouteDef::getFrom() const
{
    return _edges.getFirst();
}


ROEdge *
ROCompleteRouteDef::getTo() const
{
    return _edges.getLast();
}


RORoute *
ROCompleteRouteDef::buildCurrentRoute(RORouter &router, long begin)
{
    return new RORoute(_id, 0, 1, _edges);
}


void
ROCompleteRouteDef::addAlternative(RORoute *current, long begin)
{
    _startTime = begin;
    delete current;
}


void
ROCompleteRouteDef::xmlOutCurrent(std::ostream &res, bool isPeriodical) const
{
    res << "   <route id=\"" << _id << "\"";
    if(isPeriodical) {
        res << " multi_ref=\"x\"";
    }
    res << ">" << _edges
        << "</route>" << endl;
}


void
ROCompleteRouteDef::xmlOutAlternatives(std::ostream &altres) const
{
    altres << "   <routealt id=\"" << _id
        << "\" last=\"0\">" << endl;
    altres << "      <route cost=\"" << _edges.recomputeCosts(_startTime)
        << "\" propability=\"1\">";
    altres << _edges;
    altres << "</route>" << endl;
    altres << "   </routealt>" << endl;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROCompleteRouteDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:


