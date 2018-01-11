/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <cmath>
#include <limits>

/* avoiding compiler warning unreferenced parameter */
#define UNUSED_PARAMETER(x)  ((void)(x))

/// @brief the maximum number of connections across an intersection
#define  SUMO_MAX_CONNECTIONS 256

class RGBColor;

/* -------------------------------------------------------------------------
 * some constant defaults used by SUMO
 * ----------------------------------------------------------------------- */
const double SUMO_const_laneWidth = (double) 3.2;
const double SUMO_const_halfLaneWidth = (double) 1.6;
const double SUMO_const_quarterLaneWidth = (double) 0.8;
const double SUMO_const_laneOffset = (double) .1;
const double SUMO_const_laneWidthAndOffset = (double) 3.3;
const double SUMO_const_halfLaneAndOffset = (double)(3.2 / 2. + .1);

/// @brief the speed threshold at which vehicles are considered as halting
const double SUMO_const_haltingSpeed = (double) 0.1;

const double INVALID_DOUBLE = std::numeric_limits<double>::max();


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

/// @brief discrds mantissa bits beyond the given number
double truncate(double x, int fractionBits);

/// @brief round to the given number of mantissa bits beyond the given number
double roundBits(double x, int fractionBits);

#endif

/****************************************************************************/

