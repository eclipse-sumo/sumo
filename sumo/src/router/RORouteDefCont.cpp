//---------------------------------------------------------------------------//
//                        RORouteDefCont.cpp -
//  A container for route definitions
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
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utils/router/TextHelpers.h>
#include "ROOrigDestRouteDef.h"
#include "RORouteDefCont.h"

using namespace std;

RORouteDefCont::RORouteDefCont()
{
}


RORouteDefCont::~RORouteDefCont()
{
    clear();
}


bool RORouteDefCont::add(std::string id, RORouteDef *def)
{
    // avoid usage of an id twice (from different files)
    if(known(id)) {
        if(def==get(id)) {
            delete def;
        } else {
            while(known(id)) {
                id = TextHelpers::version(id);
            }
        }
    }
    // insert the route into the structures
    NamedObjectCont<RORouteDef*>::add(string(id), def);
    _known.insert(idMap::value_type(std::pair<ROEdge*, ROEdge*>(def->getFrom(), def->getTo()), def)); // !!! not very well
    return true;
}


void
RORouteDefCont::writeXML(std::ostream &os) {
}

/*
std::string
RORouteDefCont::add(ROEdge *from, ROEdge *to) {
    // check whether a route like the given is already known
    if(known(from, to)) {
        return getID(from, to);
    }
    // get a valid id
    string id = _idSupplier.getNext();
    while(known(id)) {
        _idSupplier.getNext();
    }
    // add the route
    NamedObjectCont<RORouteDef*>::add(
        id, new ROOrigDestRouteDef(id, from, to));
    return id;
}
*/

bool
RORouteDefCont::known(ROEdge *from, ROEdge *to) const {
    idMap::const_iterator i=_known.find(std::pair<ROEdge*, ROEdge*>(from, to));
    return i!=_known.end();
}

bool
RORouteDefCont::known(const std::string &name) const {
    return get(name)!=0;
}


std::string
RORouteDefCont::getID(ROEdge *from, ROEdge *to) const {
    idMap::const_iterator i=_known.find(std::pair<ROEdge*, ROEdge*>(from, to));
    if(i==_known.end())
        return "";
    return (*i).second->getID();
}

void
RORouteDefCont::clear()
{
    _known.clear();
    NamedObjectCont<RORouteDef*>::clear();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORouteDefCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


