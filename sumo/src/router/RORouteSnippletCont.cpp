//---------------------------------------------------------------------------//
//                        RORouteSnippletCont.cpp -
//  A container with route snipplets for faster computation
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
// Revision 1.8  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:53:07  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.3  2003/02/07 10:45:06  dkrajzew
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

#include <map>
#include <utils/router/TextHelpers.h>
#include "ROEdgeVector.h"
#include "RORouteDef_OrigDest.h"
#include "RORouteSnippletCont.h"

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
RORouteSnippletCont::RORouteSnippletCont()
{
}


RORouteSnippletCont::~RORouteSnippletCont()
{
}


bool
RORouteSnippletCont::add(const ROEdgeVector &item)
{
    const ROEdge *from = item.getFirst();
    const ROEdge *to = item.getLast();
    MapType::iterator i=_known.find(MapType::key_type(from, to));
    if(i==_known.end()) {
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
RORouteSnippletCont::knows(ROEdge *from, ROEdge *to) const
{
    return false; // !!! should not be used until a validation of net statistics has been made
/*    MapType::const_iterator i=_known.find(MapType::key_type(from, to));
    return i!=_known.end();*/
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


