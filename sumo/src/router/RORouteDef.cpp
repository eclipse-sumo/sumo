//---------------------------------------------------------------------------//
//                        RORouteDef.cpp -
//  Basic class for route definitions (not the computed routes)
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
#include <string>
#include <utils/common/Named.h>
#include <utils/common/SErrorHandler.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "RORouter.h"
#include "ReferencedItem.h"
#include "RORouteDef.h"

using namespace std;


RORouteDef::RORouteDef(const std::string &id)
    : ReferencedItem(), Named(id)
{
}

RORouteDef::~RORouteDef()
{
}


bool
RORouteDef::computeAndSave(RORouter &router, long begin,
                           std::ostream &res, std::ostream &altres)
{
    RORoute *current = buildCurrentRoute(router, begin);
    if(current->size()<2) {
        SErrorHandler::add(
            string("The route '") + _id
            + string("' is too short, propably ending at the starting edge."));
        SErrorHandler::add("Skipping...");
        delete current;
        return false;
    }
    addAlternative(current, begin);
    xmlOutCurrent(res);
//    current->xmlOut(res);
    xmlOutAlternatives(altres);
    return true;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORouteDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:


