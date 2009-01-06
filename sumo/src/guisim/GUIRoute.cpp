/****************************************************************************/
/// @file    GUIRoute.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// A vehicle route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "GUIRoute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIRoute::GUIRoute(const RGBColor &c, const std::string &id,
                   const MSEdgeVector &edges,
                   bool multipleReferenced) throw()
        : MSRoute(id, edges, multipleReferenced), myColor(c)
{}


GUIRoute::~GUIRoute() throw()
{}


const RGBColor &
GUIRoute::getColor() const
{
    return myColor;
}



/****************************************************************************/

