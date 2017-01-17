/****************************************************************************/
/// @file    Helper_ConvexHull.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef Helper_ConvexHull_h
#define Helper_ConvexHull_h


// ===========================================================================
// included modules
// ===========================================================================


#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Position.h"
#include "PositionVector.h"
#include <vector>

// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.


// Assume that a class is already given for the object:
//    Position with coordinates {SUMOReal x, y;}
//===================================================================


// isLeft(): test if a Position is Left|On|Right of an infinite line.
//    Input:  three Positions P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles


inline SUMOReal
isLeft(const Position& P0,
       const Position& P1,
       const Position& P2) {
    return (P1.x() - P0.x()) * (P2.y() - P0.y()) - (P2.x() - P0.x()) * (P1.y() - P0.y());
}


PositionVector
simpleHull_2D(const PositionVector& V);


#endif

/****************************************************************************/

