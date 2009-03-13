/****************************************************************************/
/// @file    Helper_ConvexHull.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "Position2D.h"
#include "Position2DVector.h"
#include <vector>

// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.


// Assume that a class is already given for the object:
//    Position2D with coordinates {SUMOReal x, y;}
//===================================================================


// isLeft(): test if a Position2D is Left|On|Right of an infinite line.
//    Input:  three Position2Ds P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles


inline SUMOReal
isLeft(const Position2D &P0,
       const Position2D &P1,
       const Position2D &P2) {
    return (P1.x() - P0.x())*(P2.y() - P0.y()) - (P2.x() - P0.x())*(P1.y() - P0.y());
}


Position2DVector
simpleHull_2D(const Position2DVector &V);


#endif

/****************************************************************************/

