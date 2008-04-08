/****************************************************************************/
/// @file    RORouteDef_Complete.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete route definition (with all passed edges being known)
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <deque>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "RORouteDef_Complete.h"

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
RORouteDef_Complete::RORouteDef_Complete(const std::string &id,
        const std::string &color,
        const std::vector<const ROEdge*> &edges) throw()
        : RORouteDef(id, color), myEdges(edges)
{}


RORouteDef_Complete::~RORouteDef_Complete() throw()
{}


const ROEdge * const
RORouteDef_Complete::getFrom() const
{
    return myEdges[0];
}


const ROEdge * const
RORouteDef_Complete::getTo() const
{
    return myEdges[myEdges.size()-1];
}


RORoute *
RORouteDef_Complete::buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                       SUMOTime , const ROVehicle &) const
{
    return new RORoute(myID, 0, 1, myEdges);
}


void
RORouteDef_Complete::addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin)
{
    myStartTime = begin;
    delete current;
}


RORouteDef *
RORouteDef_Complete::copy(const std::string &id) const
{
    return new RORouteDef_Complete(id, myColor, myEdges);
}


const std::vector<const ROEdge*> &
RORouteDef_Complete::getCurrentEdgeVector() const
{
    return myEdges;
}


/****************************************************************************/

