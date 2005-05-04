//---------------------------------------------------------------------------//
//                        ROJPEdgeBuilder.cpp -
//      The builder for jp-edges
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
// Revision 1.3  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/07/02 09:40:36  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing (lane index added)
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <router/RONet.h>
#include "ROJPEdge.h"
#include "ROJPEdgeBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROJPEdgeBuilder::ROJPEdgeBuilder()
{
}


ROJPEdgeBuilder::~ROJPEdgeBuilder()
{
}


ROEdge *
ROJPEdgeBuilder::buildEdge(const std::string &name)
{
    myNames.push_back(name);
    return new ROJPEdge(name, getCurrentIndex());
}


void
ROJPEdgeBuilder::setTurningDefinitions(RONet &net,
                                       const std::vector<float> &turn_defs)
{
    for(vector<string>::iterator i=myNames.begin(); i!=myNames.end(); i++) {
        ROJPEdge *edge = static_cast<ROJPEdge*>(net.getEdge((*i)));
        edge->setTurnDefaults(turn_defs);
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

