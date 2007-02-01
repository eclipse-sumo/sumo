/****************************************************************************/
/// @file    RORouteDef_Complete.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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
#include <deque>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROAbstractRouter.h"
#include "RORouteDef_Complete.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef_Complete::RORouteDef_Complete(const std::string &id,
        const RGBColor &color,
        const ROEdgeVector &edges)
        : RORouteDef(id, color), _edges(edges)
{}


RORouteDef_Complete::~RORouteDef_Complete()
{}


const ROEdge * const
RORouteDef_Complete::getFrom() const
{
    return _edges.getFirst();
}


const ROEdge * const
RORouteDef_Complete::getTo() const
{
    return _edges.getLast();
}


RORoute *
RORouteDef_Complete::buildCurrentRoute(ROAbstractRouter &,
                                       SUMOTime , ROVehicle &) const
{
    return new RORoute(_id, 0, 1, _edges);
}


void
RORouteDef_Complete::addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin)
{
    _startTime = begin;
    delete current;
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



/****************************************************************************/

