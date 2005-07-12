//---------------------------------------------------------------------------//
//                        GUIHelpingJunction.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.3  2005/07/12 12:00:09  dkrajzew
// level 3 warnings removed; code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <microsim/MSJunction.h>
#include "GUIHelpingJunction.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
void
GUIHelpingJunction::fill(std::vector<GUIJunctionWrapper*> &list,
                         GUIGlObjectStorage &idStorage)
{
    size_t size = MSJunction::dictSize();
    list.reserve(size);
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        GUIJunctionWrapper *wrapper =
            (*i).second->buildJunctionWrapper(idStorage);
        list.push_back(wrapper);
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
