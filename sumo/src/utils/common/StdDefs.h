/****************************************************************************/
/// @file    StdDefs.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef StdDefs_h
#define StdDefs_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

/* avoiding compiler warning unreferenced parameter */
#define UNUSED_PARAMETER(x)  ((void)(x))

/* -------------------------------------------------------------------------
 * some constant defaults used by SUMO
 * ----------------------------------------------------------------------- */
const SUMOReal SUMO_const_laneWidth = (SUMOReal) 3.2;
const SUMOReal SUMO_const_halfLaneWidth = (SUMOReal) 1.6;
const SUMOReal SUMO_const_quarterLaneWidth = (SUMOReal) 0.8;
const SUMOReal SUMO_const_laneOffset = (SUMOReal) .1;
const SUMOReal SUMO_const_laneWidthAndOffset = (SUMOReal) 3.3;
const SUMOReal SUMO_const_halfLaneAndOffset = (SUMOReal)(3.2/2.+.1);


/* -------------------------------------------------------------------------
 * templates for mathematical functions missing in some c++-implementations
 * ----------------------------------------------------------------------- */
template<typename T>
inline T
MIN2(T a, T b) {
    return a<b?a:b;
}

template<typename T>
inline T
MAX2(T a, T b) {
    return a>b?a:b;
}


template<typename T>
inline T
MIN3(T a, T b, T c) {
    return MIN2(c, a<b?a:b);
}


template<typename T>
inline T
MAX3(T a, T b, T c) {
    return MAX2(c, a>b?a:b);
}


template<typename T>
inline T
MIN4(T a, T b, T c, T d) {
    return MIN2(MIN2(a,b),MIN2(c,d));
}


template<typename T>
inline T
MAX4(T a, T b, T c, T d) {
    return MAX2(MAX2(a,b),MAX2(c,d));
}


#endif

/****************************************************************************/

