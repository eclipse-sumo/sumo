/****************************************************************************/
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// type definitions
// ===========================================================================
typedef int SUMOTime;


// ===========================================================================
// some defines
// ===========================================================================
// the step length in s
#define DELTA_T 1

// x*deltaT
#define SPEED2DIST(x) x

// x/deltaT
#define DIST2SPEED(x) x

// x*deltaT*deltaT
#define ACCEL2DIST(x) x

// x*deltaT
#define ACCEL2SPEED(x) x

// x*deltaT
#define STEP2TIME(x) x




#define GET_XML_SUMO_TIME getInt
#define GET_XML_SUMO_TIME_SECURE getIntSecure


// ===========================================================================
// type definitions
// ===========================================================================

//!!!
typedef unsigned long ExtSUMOTime;


#endif

/****************************************************************************/

