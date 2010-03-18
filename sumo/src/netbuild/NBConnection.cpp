/****************************************************************************/
/// @file    NBConnection.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The class holds a description of a connection between two edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <sstream>
#include <iostream>
#include <cassert>
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include "NBConnection.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBConnection::NBConnection(NBEdge *from, NBEdge *to)
        : myFrom(from), myTo(to), myFromLane(-1), myToLane(-1) {
    myFromID = from->getID();
    myToID = to->getID();
}


NBConnection::NBConnection(const std::string &fromID, NBEdge *from,
                           const std::string &toID, NBEdge *to)
        : myFrom(from), myTo(to), myFromID(fromID), myToID(toID),
        myFromLane(-1), myToLane(-1) {}


NBConnection::NBConnection(NBEdge *from, int fromLane,
                           NBEdge *to, int toLane)
        : myFrom(from), myTo(to), myFromLane(fromLane), myToLane(toLane) {
    assert(myFromLane<0||from->getNoLanes()>(size_t) myFromLane);
    assert(myToLane<0||to->getNoLanes()>(size_t) myToLane);
    myFromID = from->getID();
    myToID = to!=0 ? to->getID() : "";
}


NBConnection::~NBConnection() {}


NBConnection::NBConnection(const NBConnection &c)
        : myFrom(c.myFrom), myTo(c.myTo),
        myFromID(c.myFromID), myToID(c.myToID),
        myFromLane(c.myFromLane), myToLane(c.myToLane) {}


NBEdge *
NBConnection::getFrom() const {
    return myFrom;
}


NBEdge *
NBConnection::getTo() const {
    return myTo;
}


bool
NBConnection::replaceFrom(NBEdge *which, NBEdge *by) {
    if (myFrom==which) {
        myFrom = by;
        myFromID = myFrom->getID();
        return true;
    }
    return false;
}


bool
NBConnection::replaceFrom(NBEdge *which, int whichLane,
                          NBEdge *by, int byLane) {
    if (myFrom==which&&(myFromLane==(int) whichLane||myFromLane<0)) {
        myFrom = by;
        myFromID = myFrom->getID();
        myFromLane = byLane;
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge *which, NBEdge *by) {
    if (myTo==which) {
        myTo = by;
        myToID = myTo->getID();
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge *which, int whichLane,
                        NBEdge *by, int byLane) {
    if (myTo==which&&(myToLane==(int) whichLane||myFromLane<0)) {
        myTo = by;
        myToID = myTo->getID();
        myToLane = byLane;
        return true;
    }
    return false;
}


bool
operator<(const NBConnection &c1, const NBConnection &c2) {
    return
        std::pair<NBEdge*, NBEdge*>(c1.getFrom(), c1.getTo())
        <
        std::pair<NBEdge*, NBEdge*>(c2.getFrom(), c2.getTo());
}



bool
NBConnection::check(const NBEdgeCont &ec) {
    myFrom = checkFrom(ec);
    myTo = checkTo(ec);
    return myFrom!=0 && myTo!=0;
}


NBEdge *
NBConnection::checkFrom(const NBEdgeCont &ec) {
    NBEdge *e = ec.retrieve(myFromID);
    // ok, the edge was not changed
    if (e==myFrom) {
        return myFrom;
    }
    // try to get the edge
    return ec.retrievePossiblySplitted(myFromID, myToID, true);
}


NBEdge *
NBConnection::checkTo(const NBEdgeCont &ec) {
    NBEdge *e = ec.retrieve(myToID);
    // ok, the edge was not changed
    if (e==myTo) {
        return myTo;
    }
    // try to get the edge
    return ec.retrievePossiblySplitted(myToID, myFromID, false);
}


std::string
NBConnection::getID() const {
    std::stringstream str;
    str << myFromID << "_" << myFromLane << "->" << myToID << "_" << myToLane;
    return str.str();
}


int
NBConnection::getFromLane() const {
    return myFromLane;
}


int
NBConnection::getToLane() const {
    return myToLane;
}



/****************************************************************************/

