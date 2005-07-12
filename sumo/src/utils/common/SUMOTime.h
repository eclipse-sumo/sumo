#ifndef SUMOTime_h
#define SUMOTime_h
//---------------------------------------------------------------------------//
//                        SUMOTime.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * type definitions
 * ======================================================================= */
typedef int SUMOTime;


/* =========================================================================
 * some defines
 * ======================================================================= */
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

/* =========================================================================
 * type definitions
 * ======================================================================= */

//!!!
typedef unsigned long ExtSUMOTime;

#endif
