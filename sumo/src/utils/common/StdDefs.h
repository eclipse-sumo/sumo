/****************************************************************************/
/// @file    StdDefs.h
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <string>

/* avoiding compiler warning unreferenced parameter */
#define UNUSED_PARAMETER(x)  ((void)(x))

/// @brief the maximum number of connections across an intersection
#define  SUMO_MAX_CONNECTIONS 256

class RGBColor;

/* -------------------------------------------------------------------------
 * some constant defaults used by SUMO
 * ----------------------------------------------------------------------- */
const SUMOReal SUMO_const_laneWidth = (SUMOReal) 3.2;
const SUMOReal SUMO_const_halfLaneWidth = (SUMOReal) 1.6;
const SUMOReal SUMO_const_quarterLaneWidth = (SUMOReal) 0.8;
const SUMOReal SUMO_const_laneOffset = (SUMOReal) .1;
const SUMOReal SUMO_const_laneWidthAndOffset = (SUMOReal) 3.3;
const SUMOReal SUMO_const_halfLaneAndOffset = (SUMOReal)(3.2 / 2. + .1);

/// @brief the speed threshold at which vehicles are considered as halting
const SUMOReal SUMO_const_haltingSpeed = (SUMOReal) 0.1;


/* -------------------------------------------------------------------------
 * templates for mathematical functions missing in some c++-implementations
 * ----------------------------------------------------------------------- */
template<typename T>
inline T
MIN2(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
inline T
MAX2(T a, T b) {
    return a > b ? a : b;
}


template<typename T>
inline T
MIN3(T a, T b, T c) {
    return MIN2(c, a < b ? a : b);
}


template<typename T>
inline T
MAX3(T a, T b, T c) {
    return MAX2(c, a > b ? a : b);
}


template<typename T>
inline T
MIN4(T a, T b, T c, T d) {
    return MIN2(MIN2(a, b), MIN2(c, d));
}


template<typename T>
inline T
MAX4(T a, T b, T c, T d) {
    return MAX2(MAX2(a, b), MAX2(c, d));
}


template<typename T>
inline T
ISNAN(T a) {
    volatile T d = a;
    return d != d;
}


/// the precision for floating point outputs
extern int gPrecision;
extern int gPrecisionGeo; // for lon,lat


/// @brief global utility flags for debugging
extern bool gDebugFlag1;
extern bool gDebugFlag2;
extern bool gDebugFlag3;
extern bool gDebugFlag4;

extern std::string gDebugSelectedVehicle;

#endif

/****************************************************************************/

