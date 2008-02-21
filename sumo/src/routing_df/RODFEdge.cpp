/****************************************************************************/
/// @file    RODFEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// An edge within the DFROUTER
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

#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "RODFEdge.h"

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
RODFEdge::RODFEdge(const std::string &id, int index) throw()
        : ROEdge(id, index, false)
{}


RODFEdge::~RODFEdge() throw()
{}


void
RODFEdge::setFlows(const std::vector<FlowDef> &flows)
{
    myFlows = flows;
}


const std::vector<FlowDef> &
RODFEdge::getFlows() const
{
    return myFlows;
}


void
RODFEdge::setFromPosition(const Position2D &p)
{
    myFromPosition = p;
}


void
RODFEdge::setToPosition(const Position2D &p)
{
    myToPosition = p;
}


const Position2D &
RODFEdge::getFromPosition() const
{
    return myFromPosition;
}


const Position2D &
RODFEdge::getToPosition() const
{
    return myToPosition;
}



/****************************************************************************/

