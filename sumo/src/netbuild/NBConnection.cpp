/****************************************************************************/
/// @file    NBConnection.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The class holds a description of a connection between two edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
// static members
// ===========================================================================
const int NBConnection::InvalidTlIndex = -1;
const NBConnection NBConnection::InvalidConnection("invalidFrom", 0, "invalidTo", 0);

// ===========================================================================
// method definitions
// ===========================================================================
NBConnection::NBConnection(NBEdge* from, NBEdge* to) :
    myFrom(from), myTo(to),
    myFromID(from->getID()), myToID(to->getID()),
    myFromLane(-1), myToLane(-1),
    myTlIndex(InvalidTlIndex) {
}


NBConnection::NBConnection(const std::string& fromID, NBEdge* from,
                           const std::string& toID, NBEdge* to) :
    myFrom(from), myTo(to),
    myFromID(fromID), myToID(toID),
    myFromLane(-1), myToLane(-1),
    myTlIndex(InvalidTlIndex) {
}


NBConnection::NBConnection(NBEdge* from, int fromLane,
                           NBEdge* to, int toLane, int tlIndex) :
    myFrom(from), myTo(to),
    myFromLane(fromLane), myToLane(toLane),
    myTlIndex(tlIndex) {
    /* @todo what should we assert here?
    assert(myFromLane<0||from->getNumLanes()>(int) myFromLane);
    assert(myToLane<0||to->getNumLanes()>(int) myToLane);
    */
    myFromID = from->getID();
    myToID = to != 0 ? to->getID() : "";
}


NBConnection::~NBConnection() {}


NBConnection::NBConnection(const NBConnection& c) :
    myFrom(c.myFrom), myTo(c.myTo),
    myFromID(c.myFromID), myToID(c.myToID),
    myFromLane(c.myFromLane), myToLane(c.myToLane),
    myTlIndex(c.myTlIndex) {
}


NBEdge*
NBConnection::getFrom() const {
    return myFrom;
}


NBEdge*
NBConnection::getTo() const {
    return myTo;
}


bool
NBConnection::replaceFrom(NBEdge* which, NBEdge* by) {
    if (myFrom == which) {
        myFrom = by;
        if (myFrom != 0) {
            myFromID = myFrom->getID();
        } else {
            myFromID = "invalidFrom";
        }
        return true;
    }
    return false;
}


bool
NBConnection::replaceFrom(NBEdge* which, int whichLane,
                          NBEdge* by, int byLane) {
    if (myFrom == which && (myFromLane == whichLane || myFromLane < 0 || whichLane < 0)) {
        myFrom = by;
        if (myFrom != 0) {
            myFromID = myFrom->getID();
        } else {
            myFromID = "invalidFrom";
        }
        if (byLane >= 0) {
            myFromLane = byLane;
        }
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge* which, NBEdge* by) {
    if (myTo == which) {
        myTo = by;
        if (myTo != 0) {
            myToID = myTo->getID();
        } else {
            myToID = "invalidTo";
        }
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge* which, int whichLane,
                        NBEdge* by, int byLane) {
    if (myTo == which && (myToLane == whichLane || myFromLane < 0 || whichLane < 0)) {
        myTo = by;
        if (myTo != 0) {
            myToID = myTo->getID();
        } else {
            myToID = "invalidTo";
        }
        if (byLane >= 0) {
            myToLane = byLane;
        }
        return true;
    }
    return false;
}


bool
operator<(const NBConnection& c1, const NBConnection& c2) {
    if (c1.myFromID   != c2.myFromID) {
        return c1.myFromID   < c2.myFromID;
    }
    if (c1.myToID     != c2.myToID) {
        return c1.myToID     < c2.myToID;
    }
    if (c1.myFromLane != c2.myFromLane) {
        return c1.myFromLane < c2.myFromLane;
    }
    return c1.myToLane < c2.myToLane;
}


bool
NBConnection::operator==(const NBConnection& c) const {
    return (myFrom    == c.myFrom     && myTo    == c.myTo &&
            myFromID  == c.myFromID   && myToID  == c.myToID &&
            myFromLane == c.myFromLane && myToLane == c.myToLane &&
            myTlIndex == c.myTlIndex);
}


bool
NBConnection::check(const NBEdgeCont& ec) {
    myFrom = checkFrom(ec);
    myTo = checkTo(ec);
    return myFrom != 0 && myTo != 0;
}


NBEdge*
NBConnection::checkFrom(const NBEdgeCont& ec) {
    NBEdge* e = ec.retrieve(myFromID);
    // ok, the edge was not changed
    if (e == myFrom) {
        return myFrom;
    }
    // try to get the edge
    return ec.retrievePossiblySplit(myFromID, myToID, true);
}


NBEdge*
NBConnection::checkTo(const NBEdgeCont& ec) {
    NBEdge* e = ec.retrieve(myToID);
    // ok, the edge was not changed
    if (e == myTo) {
        return myTo;
    }
    // try to get the edge
    return ec.retrievePossiblySplit(myToID, myFromID, false);
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


void
NBConnection::shiftLaneIndex(NBEdge* edge, int offset) {
    if (myFrom == edge) {
        myFromLane += offset;
    } else if (myTo == edge) {
        myToLane += offset;
    }
}


std::ostream&
operator<<(std::ostream& os, const NBConnection& c) {
    os
            << "Con(from=" << Named::getIDSecure(c.getFrom())
            << " fromLane=" << c.getFromLane()
            << " to=" << Named::getIDSecure(c.getTo())
            << " toLane=" << c.getToLane()
            << " tlIndex=" << c.getTLIndex()
            << ")";
    return os;
}



/****************************************************************************/

