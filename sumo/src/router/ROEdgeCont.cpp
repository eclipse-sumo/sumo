//---------------------------------------------------------------------------//
//                        ROEdgeCont.cpp -
//  A container for routable edges
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
// Revision 1.6  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.5  2004/01/12 15:39:35  dkrajzew
// reproduces changes to NamedObjectsMap
//
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include "ROEdge.h"
#include "ROEdgeCont.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROEdgeCont::ROEdgeCont()
{
}


ROEdgeCont::~ROEdgeCont()
{
}


void
ROEdgeCont::postloadInit()
{
    const std::vector<ROEdge*> &v = getVector();
    for(size_t i=0; i!=v.size(); i++) {
        v[i]->postloadInit(i); // !!! for_each
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


