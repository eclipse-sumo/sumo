/***************************************************************************
                          MSGlobals.cpp  -
    Some static variables for faster access
                             -------------------
    project              : SUMO
    begin                : late summer 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSGlobals.h"


/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
bool MSGlobals::myOmitEmptyEdgesOnDump;

bool MSGlobals::myUsingInternalLanes;

size_t MSGlobals::myTimeToGridlock = 300;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
