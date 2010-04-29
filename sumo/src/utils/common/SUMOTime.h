/****************************************************************************/
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Variables, methods, and tools for internal time representation
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

#include <string>
#include "UtilExceptions.h"


// ===========================================================================
// type definitions
// ===========================================================================
#ifndef HAVE_SUBSECOND_TIMESTEPS

typedef int SUMOTime;
// the step length in s
#define DELTA_T 1
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

#define STEPS2TIME(x) ((SUMOReal)x)
#define TIME2STEPS(x) (x)
#define TMOD(x,y) (x%y)

#else

typedef int SUMOTime;
extern SUMOTime DELTA_T;

#define TS ((SUMOReal) DELTA_T/1000.)

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

#define STEPS2TIME(x) ((SUMOReal) ((x)/1000.))
#define TIME2STEPS(x) ((SUMOTime) ((x)*1000))
#define TMOD(x,y) (x%y) 
//(fmod(x,y))

#endif


// ===========================================================================
// method declarations
// ===========================================================================
SUMOTime string2time(const std::string &r) throw(EmptyData, NumberFormatException);
std::string time2string(SUMOTime t) throw();


#endif

/****************************************************************************/

