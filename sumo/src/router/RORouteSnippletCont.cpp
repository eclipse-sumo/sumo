/****************************************************************************/
/// @file    RORouteSnippletCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A container with route snipplets for faster computation
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

#include <map>
#include <utils/router/TextHelpers.h>
#include "ROEdgeVector.h"
#include "RORouteDef_OrigDest.h"
#include "RORouteSnippletCont.h"

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
RORouteSnippletCont::RORouteSnippletCont()
{}


RORouteSnippletCont::~RORouteSnippletCont()
{}


bool
RORouteSnippletCont::add(const ROEdgeVector &item)
{
    const ROEdge *from = item.getFirst();
    const ROEdge *to = item.getLast();
    MapType::iterator i=_known.find(MapType::key_type(from, to));
    if (i==_known.end()) {
        _known.insert(
            MapType::value_type(std::pair<const ROEdge*, const ROEdge*>(from, to), item));
        return true;
    }
    return false;
}


const ROEdgeVector &
RORouteSnippletCont::get(ROEdge *from, ROEdge *to) const
    {
        MapType::const_iterator i=_known.find(MapType::key_type(from, to));
        return (*i).second;
    }

bool
RORouteSnippletCont::knows(ROEdge * /*from*/, ROEdge * /*to*/) const
{
    return false; // !!! should not be used until a validation of net statistics has been made
    /*    MapType::const_iterator i=_known.find(MapType::key_type(from, to));
        return i!=_known.end();*/
}



/****************************************************************************/

