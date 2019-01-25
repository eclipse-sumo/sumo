/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
    int edgeid, double frompos, double topos)
    : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos) {}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge() {}


int
NIVissimNodeParticipatingEdge::getID() const {
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(double pos) const {
    return
        (myFromPos < myToPos && myFromPos >= pos && myToPos <= pos)
        ||
        (myFromPos > myToPos && myFromPos <= pos && myToPos >= pos);
}

double
NIVissimNodeParticipatingEdge::getFromPos() const {
    return myFromPos;
}


double
NIVissimNodeParticipatingEdge::getToPos() const {
    return myToPos;
}



/****************************************************************************/

