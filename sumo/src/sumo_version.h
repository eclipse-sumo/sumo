#ifndef sumo_version_h
#define sumo_version_h
//---------------------------------------------------------------------------//
//                        sumo_version.h -
//  A string containing the current program version
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 17 Jun 2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.2  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2003/06/18 10:57:09  dkrajzew
// quite unsane specification within a cpp file only is now split on a cpp and a h file
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
extern const char *version;

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
