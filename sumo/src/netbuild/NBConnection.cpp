//---------------------------------------------------------------------------//
//                        NBConnection.cpp -
//  The class holds a description of a connection between two edges
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
// Revision 1.6  2003/12/04 13:05:42  dkrajzew
// some work for joining vissim-edges
//
// Revision 1.5  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.3  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.2  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <sstream>
#include <iostream>
#include <cassert>
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include "NBConnection.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NBConnection::NBConnection(NBEdge *from, NBEdge *to)
    : myFrom(from), myTo(to), myFromLane(-1), myToLane(-1)
{
    myFromID = from->getID();
    myToID = to->getID();
}


NBConnection::NBConnection(const std::string &fromID, NBEdge *from,
                           const std::string &toID, NBEdge *to)
    : myFrom(from), myTo(to), myFromID(fromID), myToID(toID),
    myFromLane(-1), myToLane(-1)
{
}


NBConnection::NBConnection(NBEdge *from, int fromLane,
                           NBEdge *to, int toLane)
    : myFrom(from), myTo(to), myFromLane(fromLane), myToLane(toLane)
{
    assert(myFromLane<0||from->getNoLanes()>(size_t) myFromLane);
    assert(myToLane<0||to->getNoLanes()>(size_t) myToLane);
    myFromID = from->getID();
    myToID = to->getID();
}


NBConnection::~NBConnection()
{
}


NBConnection::NBConnection(const NBConnection &c)
    : myFrom(c.myFrom), myTo(c.myTo),
    myFromID(c.myFromID), myToID(c.myToID),
    myFromLane(c.myFromLane), myToLane(c.myToLane)
{
}


NBEdge *
NBConnection::getFrom() const
{
    return myFrom;
}


NBEdge *
NBConnection::getTo() const
{
    return myTo;
}


bool
NBConnection::replaceFrom(NBEdge *which, NBEdge *by)
{
    if(myFrom==which) {
        myFrom = by;
        myFromID = myFrom->getID();
        return true;
    }
    return false;
}


bool
NBConnection::replaceFrom(NBEdge *which, int whichLane,
                          NBEdge *by, int byLane)
{
    if(myFrom==which&&(myFromLane==(int) whichLane||myFromLane<0)) {
        myFrom = by;
        myFromID = myFrom->getID();
        myFromLane = byLane;
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge *which, NBEdge *by)
{
    if(myTo==which) {
        myTo = by;
        myToID = myTo->getID();
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge *which, int whichLane,
                        NBEdge *by, int byLane)
{
    if(myTo==which&&(myToLane==(int) whichLane||myFromLane<0)) {
        myTo = by;
        myToID = myTo->getID();
        myToLane = byLane;
        return true;
    }
    return false;
}


bool
operator<(const NBConnection &c1, const NBConnection &c2)
{
    return
        std::pair<NBEdge*, NBEdge*>(c1.getFrom(), c1.getTo())
        <
        std::pair<NBEdge*, NBEdge*>(c2.getFrom(), c2.getTo());
}



bool
NBConnection::check()
{
    myFrom = checkFrom();
    myTo = checkTo();
    return myFrom!=0 && myTo!=0;
}


NBEdge *
NBConnection::checkFrom()
{
    NBEdge *e = NBEdgeCont::retrieve(myFromID);
    // ok, the edge was not changed
    if(e==myFrom) {
        return myFrom;
    }
    // try to get the edge
    return NBEdgeCont::retrievePossiblySplitted(myFromID, myToID, true);
}


NBEdge *
NBConnection::checkTo()
{
    NBEdge *e = NBEdgeCont::retrieve(myToID);
    // ok, the edge was not changed
    if(e==myTo) {
        return myTo;
    }
    // try to get the edge
    return NBEdgeCont::retrievePossiblySplitted(myToID, myFromID, false);
}


std::string
NBConnection::getID() const
{
    stringstream str;
    str << myFromID << "_" << myFromLane << "->" << myToID << "_" << myToLane;
    return str.str();
}


int
NBConnection::getFromLane() const
{
    return myFromLane;
}


int
NBConnection::getToLane() const
{
    return myToLane;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBConnection.icc"
//#endif

// Local Variables:
// mode:C++
// End:


