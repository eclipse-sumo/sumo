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


NBConnection::NBConnection(NBEdge *from, size_t fromLane, NBEdge *to, size_t toLane)
    : myFrom(from), myTo(to), myFromLane(fromLane), myToLane(toLane)
{
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


