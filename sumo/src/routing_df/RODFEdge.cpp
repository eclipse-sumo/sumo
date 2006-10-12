/***************************************************************************
                          RODFEdge.cpp
    An edge within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2006/10/12 10:14:30  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.3  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
//
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



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


