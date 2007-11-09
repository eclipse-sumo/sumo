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
#include <utils/common/TextHelpers.h>
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
RORouteDefCont::RORouteDefCont()
{}


RORouteDefCont::~RORouteDefCont()
{
    clear();
}


bool
RORouteDefCont::add(RORouteDef *def)
{
    string id = def->getID();
    // avoid usage of an id twice (from different files)
    if (known(id)) {
        if (def==get(id)) {
            delete def;
        } else {
            while (known(id)) {
                id = TextHelpers::version(id);
            }
        }
    }
    // insert the route into the structures
    NamedObjectCont<RORouteDef*>::add(id, def);
    myKnown.insert(idMap::value_type(std::pair<const ROEdge*, const ROEdge*>(def->getFrom(), def->getTo()), def)); // !!! not very well
    return true;
}


void
RORouteDefCont::writeXML(std::ostream &)
{}


bool
RORouteDefCont::known(ROEdge *from, ROEdge *to) const
{
    idMap::const_iterator i=myKnown.find(std::pair<ROEdge*, ROEdge*>(from, to));
    return i!=myKnown.end();
}

bool
RORouteDefCont::known(const std::string &name) const
{
    return get(name)!=0;
}


std::string
RORouteDefCont::getID(ROEdge *from, ROEdge *to) const
{
    idMap::const_iterator i=myKnown.find(std::pair<ROEdge*, ROEdge*>(from, to));
    if (i==myKnown.end())
        return "";
    return (*i).second->getID();
}

void
RORouteDefCont::clear()
{
    myKnown.clear();
    NamedObjectCont<RORouteDef*>::clear();
}



/****************************************************************************/

