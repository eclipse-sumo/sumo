/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    StdDefs.h
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include <string>
#include <cmath>
#include <limits>

/* avoiding compiler warning unreferenced parameter */
#define UNUSED_PARAMETER(x)  ((void)(x))

#ifdef _MSC_VER
#define FALLTHROUGH /* do nothing */
#elif __GNUC__ < 7
#define FALLTHROUGH /* do nothing */
#else
#define FALLTHROUGH __attribute__((fallthrough))
#endif

/// @brief the maximum number of connections across an intersection
#define  SUMO_MAX_CONNECTIONS 256

class RGBColor;

/* -------------------------------------------------------------------------
 * some constant defaults used by SUMO
 * ----------------------------------------------------------------------- */
const double SUMO_const_laneWidth = 3.2;
const double SUMO_const_halfLaneWidth = SUMO_const_laneWidth / 2;
const double SUMO_const_quarterLaneWidth = SUMO_const_laneWidth / 4;
const double SUMO_const_laneMarkWidth = 0.1;
const double SUMO_const_waitingPersonWidth = 0.8;
const double SUMO_const_waitingPersonDepth = 0.67;
const double SUMO_const_waitingContainerWidth = 2.5;
const double SUMO_const_waitingContainerDepth = 6.2;

/// @brief the speed threshold at which vehicles are considered as halting
const double SUMO_const_haltingSpeed = (double) 0.1;

/// @brief invalid int
const int INVALID_INT = std::numeric_limits<int>::max();

/// @brief invalid double
const double INVALID_DOUBLE = std::numeric_limits<double>::max();

/// @brief (M)ajor/(M)inor version for written networks and default version for loading
typedef std::pair<int, double> MMVersion;
const MMVersion NETWORK_VERSION(1, 20);

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


/// the precision for floating point outputs
extern int gPrecision;
extern int gPrecisionGeo; // for lon,lat
extern int gPrecisionRandom; // for randomized values (i.e. speedFactor)
extern bool gHumanReadableTime;
extern bool gSimulation; // whether the current application is sumo or sumo-gui (as opposed to a router)
extern bool gIgnoreUnknownVClass; // whether the unknown vehicle classes shall be ignored on loading (for upward compatibility)
extern double gWeightsRandomFactor; // randomization for edge weights
extern double gWeightsWalkOppositeFactor; // factor for walking against flow of traffic

/// the language for GUI elements and messages
extern std::string gLanguage;

/// the default size for GUI elements
extern int GUIDesignHeight;


/// @brief global utility flags for debugging
extern bool gDebugFlag1;
extern bool gDebugFlag2;
extern bool gDebugFlag3;
extern bool gDebugFlag4;
extern bool gDebugFlag5;
extern bool gDebugFlag6;

// synchronized output to stdout with << (i.e. DEBUGOUT(gDebugFlag1, SIMTIME << " var=" << var << "\n")
#define DEBUGOUT(cond, msg) if (cond) {std::ostringstream oss; oss << msg; std::cout << oss.str();}

/// @brief discrds mantissa bits beyond the given number
double truncate(double x, int fractionBits);

/// @brief round to the given number of mantissa bits beyond the given number
double roundBits(double x, int fractionBits);

/// @brief round to the given number of decimal digits
double roundDecimal(double x, int precision);

/** @brief Returns the number of instances of the current object that shall be emitted
 * given the number of loaded objects
 * considering that "frac" of all objects shall be emitted overall
 * @return the number of objects to create (something between 0 and ceil(frac))
 */
int getScalingQuota(double frac, int loaded);
