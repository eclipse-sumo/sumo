/****************************************************************************/
/// @file    RORouteDefCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for route definitions
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

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "RORouteDef_OrigDest.h"
#include "RORouteDefCont.h"

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
RORouteDefCont::RORouteDefCont() throw()
{}


RORouteDefCont::~RORouteDefCont() throw()
{
    clear();
}


bool
RORouteDefCont::add(RORouteDef *def) throw()
{
    string id = def->getID();
    // insert the route into the structures
    NamedObjectCont<RORouteDef*>::add(id, def);
    return true;
}


bool
RORouteDefCont::known(const std::string &name) const throw()
{
    return get(name)!=0;
}


void
RORouteDefCont::clear() throw()
{
    NamedObjectCont<RORouteDef*>::clear();
}



/****************************************************************************/

