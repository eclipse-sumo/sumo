//---------------------------------------------------------------------------//
//                        RODFEdge.cpp -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.2  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
// Revision 1.1  2006/03/08 12:51:29  dkrajzew
// further work on the dfrouter
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

#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "RODFEdge.h"

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
RODFEdge::RODFEdge(const std::string &id, int index)
    : ROEdge(id, index)
{
}


RODFEdge::~RODFEdge()
{
}


void
RODFEdge::addFollower(ROEdge *s)
{
    ROEdge::addFollower(s);
}


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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


