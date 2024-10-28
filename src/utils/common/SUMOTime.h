/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    Fri, 29.04.2005
///
// Variables, methods, and tools for internal time representation
/****************************************************************************/
#pragma once
#include <config.h>
#include <limits>
#include <string>
#include "UtilExceptions.h"


// ===========================================================================
// type definitions
// ===========================================================================
typedef long long int SUMOTime;
#define SUMOTime_MAX (std::numeric_limits<SUMOTime>::max() - 1000)
#define SUMOTime_MIN std::numeric_limits<SUMOTime>::min()
#define SUMOTime_MAX_PERIOD (SUMOTime_MAX - SUMOTime_MAX % DELTA_T)

// the step length in ms
extern SUMOTime DELTA_T;

// the step length in seconds as double
#define TS (static_cast<double>(DELTA_T)/1000.)

// x*deltaT
#define SPEED2DIST(x) ((x)*TS)
// x/deltaT
#define DIST2SPEED(x) ((x)/TS)
// x*deltaT*deltaT
#define ACCEL2DIST(x) ((x)*TS*TS)
// x*deltaT
#define ACCEL2SPEED(x) ((x)*TS)
// x*deltaT
#define SPEED2ACCEL(x) ((x)/TS)

#define STEPS2TIME(x) (static_cast<double>(x)/1000.)
// static cast to long long int truncates so we must pad away from 0 for correct rounding
#define TIME2STEPS(x) (static_cast<SUMOTime>((x) * 1000. + ((x) >= 0 ? 0.5 : -0.5)))
#define STEPFLOOR(x) (static_cast<SUMOTime>((x)/DELTA_T)*DELTA_T)
#define STEPS2MS(x) (x)

#define SIMSTEP MSNet::getInstance()->getCurrentTimeStep()
#define SIMTIME STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())

// ===========================================================================
// method declarations
// ===========================================================================

/// @brief convert string to SUMOTime
SUMOTime string2time(const std::string& r);

/// @brief convert SUMOTime to string (independently of global format setting)
std::string time2string(SUMOTime t, bool humanReadable);

/// @brief convert SUMOTime to string (using the global format setting)
std::string time2string(SUMOTime t);

/// @brief convert ms to string for log output
std::string elapsedMs2string(long long int t);

/// @brief check if given SUMOTime is multiple of the step length
bool checkStepLengthMultiple(const SUMOTime t, const std::string& error = "", SUMOTime deltaT = DELTA_T, SUMOTime begin = 0);

/// @brief check the valid SUMOTime range of double input and throw an error if out of bounds
void checkTimeBounds(const double time);
