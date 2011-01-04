/****************************************************************************/
/// @file    Helper_ConvexHull.cpp
/// @author  unknown_author
/// @date    2004-11-23
/// @version $Id$
///
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "Helper_ConvexHull.h"


#include <utils/common/UtilExceptions.h>
#include <iostream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// Assume that a class is already given for the object:
//    Position2D with coordinates {SUMOReal x, y;}
Position2DVector
simpleHull_2D(const Position2DVector &V) {
    if (V.size()<3) {
        throw ProcessError();
    }
    // initialize a deque D[] from bottom to top so that the
    // 1st three vertices of V[] are a counterclockwise triangle
    int n = (int) V.size();
    std::vector<Position2D> D(2*n+1);
    int bot = n-2, top = bot+3;   // initial bottom and top deque indices
    D[bot] = D[top] = V[2];       // 3rd vertex is at both bot and top
    if (isLeft(V[0], V[1], V[2]) > 0) {
        D[bot+1] = V[0];
        D[bot+2] = V[1];          // ccw vertices are: 2,0,1,2
    } else {
        D[bot+1] = V[1];
        D[bot+2] = V[0];          // ccw vertices are: 2,1,0,2
    }

    // compute the hull on the deque D[]
    for (int i=3; i < n; i++) {   // process the rest of vertices
        // test if next vertex is inside the deque hull
        if (bot>=(int) D.size()||top-1>=(int) D.size()||i>=(int) V.size()) {
            throw ProcessError();
        }
        if ((isLeft(D[bot], D[bot+1], V[i]) > 0) &&
                (isLeft(D[top-1], D[top], V[i]) > 0))
            continue;         // skip an interior vertex

        // incrementally add an exterior vertex to the deque hull
        // get the rightmost tangent at the deque bot
        while (isLeft(D[bot], D[bot+1], V[i]) <= 0) {
            ++bot;                // remove bot of deque
            if (bot>=(int) D.size()) {
                throw ProcessError();
            }
        }
        if (bot==0) {
            throw ProcessError();
        }
        D[--bot] = V[i];          // insert V[i] at bot of deque

        if (top==0||top>=(int) D.size()) {
            throw ProcessError();
        }
        // get the leftmost tangent at the deque top
        while (isLeft(D[top-1], D[top], V[i]) <= 0) {
            --top;                // pop top of deque
            if (top==0||top>=(int) D.size()) {
                throw ProcessError();
            }
        }

        if (top+1>=(int) D.size()) {
            throw ProcessError();
        }
        D[++top] = V[i];          // push V[i] onto top of deque
    }

    // transcribe deque D[] to the output hull array H[]
    int h;        // hull vertex counter
    Position2DVector H;
    for (h=0; h <= (top-bot); h++) {
        if (bot + h>=(int) D.size()) {
            throw ProcessError();
        }
        H.push_back_noDoublePos(D[bot + h]);
    }
    return H;
}



/****************************************************************************/

