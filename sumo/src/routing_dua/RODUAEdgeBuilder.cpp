//---------------------------------------------------------------------------//
//                        RODUAEdgeBuilder.cpp -
//      The builder for dua-edges
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2004/07/02 09:40:36  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing (lane index added)
//
// Revision 1.1  2004/01/26 06:08:38  dkrajzew
// initial commit for dua-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <router/ROEdge.h>
#include "RODUAEdgeBuilder.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
RODUAEdgeBuilder::RODUAEdgeBuilder()
{
}


RODUAEdgeBuilder::~RODUAEdgeBuilder()
{
}


ROEdge *
RODUAEdgeBuilder::buildEdge(const std::string &name)
{
    return new ROEdge(name, getCurrentIndex());
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

