#ifndef Helper_ConvexHull_h
#define Helper_ConvexHull_h


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
//    Position2D with coordinates {float x, y;}
//===================================================================


// isLeft(): test if a Position2D is Left|On|Right of an infinite line.
//    Input:  three Position2Ds P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles


inline double
isLeft(const Position2D &P0,
       const Position2D &P1,
       const Position2D &P2 )
{
    return (P1.x() - P0.x())*(P2.y() - P0.y()) - (P2.x() - P0.x())*(P1.y() - P0.y());
}


Position2DVector
simpleHull_2D(const Position2DVector &V);


#endif

