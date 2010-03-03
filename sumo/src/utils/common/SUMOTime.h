/****************************************************************************/
/// @file    SUMOTime.h
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
// x*deltaT
#define STEP2TIME(x) (x)
#define STEPS2SECONDS(x) (x)
#define TIME2STEPS(x) (x)
#define TMOD(x,y) (x%y)

#else

typedef SUMOReal SUMOTime;
extern SUMOReal DELTA_T;

// x*deltaT
#define SPEED2DIST(x) ((x)*DELTA_T)
// x/deltaT
#define DIST2SPEED(x) ((x)/DELTA_T)
// x*deltaT*deltaT
#define ACCEL2DIST(x) ((x)*DELTA_T*DELTA_T)
// x*deltaT
#define ACCEL2SPEED(x) ((x)*DELTA_T)
// x*deltaT
#define SPEED2ACCEL(x) ((x)/DELTA_T)
// x*deltaT
#define STEP2TIME(x) ((int) ((x)*DELTA_T))
#define STEPS2SECONDS(x) ((int) ((x)*DELTA_T))
#define TIME2STEPS(x) ((int) ((x)/DELTA_T))
#define TMOD(x,y) (fmod(x,y))

#endif



#endif

/****************************************************************************/

