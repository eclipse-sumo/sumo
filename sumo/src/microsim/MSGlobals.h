#ifndef MSGlobals_h
#define MSGlobals_h
/***************************************************************************
                          MSGlobals.h  -
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
// $Log$
// Revision 1.5  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cstddef>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSGlobals
 * This class holds some static variables, filled mostly with values coming
 *  from the command line or the simulation configuration file.
 * They are stored herein to allow a faster access than from the options
 *  container.
 */
class MSGlobals
{
public:
    /// Information whether empty edges shall be written on dump
    static bool myOmitEmptyEdgesOnDump;

    /** Information how long the simulation shall wait until it recognizes
        a vehicle as a grid lock participant */
    static size_t myTimeToGridlock;

    /// Information whether the simulation regards internal lanes
    static bool myUsingInternalLanes;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

