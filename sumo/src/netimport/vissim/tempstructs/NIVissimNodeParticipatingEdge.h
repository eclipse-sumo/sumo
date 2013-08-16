/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
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
#ifndef NIVissimNodeParticipatingEdge_h
#define NIVissimNodeParticipatingEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <string>
#include <utils/geom/Position.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimNodeParticipatingEdge {
public:
    NIVissimNodeParticipatingEdge(int edgeid,
                                  SUMOReal frompos, SUMOReal topos);
    ~NIVissimNodeParticipatingEdge();
    int getID() const;
    bool positionLiesWithin(SUMOReal pos) const;
    SUMOReal getFromPos() const;
    SUMOReal getToPos() const;
private:
    int myEdgeID;
    SUMOReal myFromPos, myToPos;
};


#endif

/****************************************************************************/

