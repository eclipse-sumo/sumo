/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
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


#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
    int edgeid, SUMOReal frompos, SUMOReal topos)
        : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos)
{}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge()
{}


int
NIVissimNodeParticipatingEdge::getID() const
{
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(SUMOReal pos) const
{
    return
        (myFromPos<myToPos && myFromPos>=pos && myToPos<=pos)
        ||
        (myFromPos>myToPos && myFromPos<=pos && myToPos>=pos);
}

SUMOReal
NIVissimNodeParticipatingEdge::getFromPos() const
{
    return myFromPos;
}


SUMOReal
NIVissimNodeParticipatingEdge::getToPos() const
{
    return myToPos;
}



/****************************************************************************/

