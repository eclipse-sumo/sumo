/****************************************************************************/
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Variables, methods, and tools for internal time representation
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
#ifndef SUMOTime_h
#define SUMOTime_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <climits>
#include <string>
#include "UtilExceptions.h"


// ===========================================================================
// type definitions
// ===========================================================================
typedef int SUMOTime;
#define SUMOTime_MAX INT_MAX
#define SUMOTIME_MAXSTRING "2147483" // INT_MAX / 1000

#ifndef HAVE_SUBSECOND_TIMESTEPS
// the step length in s
#define DELTA_T 1

#define TS (static_cast<SUMOReal>(1.))

// x*deltaT
#define SPEED2DIST(x) (x)
// x/deltaT
#define DIST2SPEED(x) (x)
// x*deltaT*deltaT
#define ACCEL2DIST(x) (x)
// x*deltaT
#define ACCEL2SPEED(x) (x)
// x/deltaT
#define SPEED2ACCEL(x) (x)

#define STEPS2TIME(x) (static_cast<SUMOReal>(x))
#define TIME2STEPS(x) (static_cast<SUMOTime>(x))

#else

// the step length in s
extern SUMOTime DELTA_T;

#define TS (static_cast<SUMOReal>(DELTA_T/1000.))

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

#define STEPS2TIME(x) (static_cast<SUMOReal>((x)/1000.))
#define TIME2STEPS(x) (static_cast<SUMOTime>((x)*1000))

#endif


// ===========================================================================
// method declarations
// ===========================================================================
SUMOTime string2time(const std::string &r) throw(EmptyData, NumberFormatException);
std::string time2string(SUMOTime t) throw();


#endif

/****************************************************************************/

