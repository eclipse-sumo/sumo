/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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


#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
    int edgeid, SUMOReal frompos, SUMOReal topos)
    : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos) {}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge() {}


int
NIVissimNodeParticipatingEdge::getID() const {
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(SUMOReal pos) const {
    return
        (myFromPos < myToPos && myFromPos >= pos && myToPos <= pos)
        ||
        (myFromPos > myToPos && myFromPos <= pos && myToPos >= pos);
}

SUMOReal
NIVissimNodeParticipatingEdge::getFromPos() const {
    return myFromPos;
}


SUMOReal
NIVissimNodeParticipatingEdge::getToPos() const {
    return myToPos;
}



/****************************************************************************/

