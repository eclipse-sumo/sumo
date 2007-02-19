/****************************************************************************/
/// @file    ROJTREdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The builder for jp-edges
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/RONet.h>
#include "ROJTREdge.h"
#include "ROJTREdgeBuilder.h"

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
ROJTREdgeBuilder::ROJTREdgeBuilder()
{}


ROJTREdgeBuilder::~ROJTREdgeBuilder()
{}


ROEdge *
ROJTREdgeBuilder::buildEdge(const std::string &name)
{
    myNames.push_back(name);
    return new ROJTREdge(name, getCurrentIndex());
}


void
ROJTREdgeBuilder::setTurningDefinitions(RONet &net,
                                        const std::vector<SUMOReal> &turn_defs)
{
    for (vector<string>::iterator i=myNames.begin(); i!=myNames.end(); i++) {
        ROJTREdge *edge = static_cast<ROJTREdge*>(net.getEdge((*i)));
        edge->setTurnDefaults(turn_defs);
    }
}



/****************************************************************************/

