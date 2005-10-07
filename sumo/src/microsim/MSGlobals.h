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
// Revision 1.10  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/07/12 12:24:01  dkrajzew
// made checking for accidents optional
//
// Revision 1.7  2005/05/04 08:25:29  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.5  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
    static bool gOmitEmptyEdgesOnDump;

    /** Information how long the simulation shall wait until it recognizes
        a vehicle as a grid lock participant */
    static size_t gTimeToGridlock;

    /// Information whether the simulation regards internal lanes
    static bool gUsingInternalLanes;

    /** minimum velocity a lane must allow in order to let vehicles being teleported
        if being on this lane and the lane may not be used for further route */
    static SUMOReal gMinLaneVMax4FalseLaneTeleport;

    /** maximum velocity a vehicle must have in order to be teleported if being
        on a false lane */
    static SUMOReal gMaxVehV4FalseLaneTeleport;

    /** maximum distance a vehicle must have to the false lane's en in order
        to be teleported if being on a false lane */
    static SUMOReal gMinVehDist4FalseLaneTeleport;

    /** information whether the network shall check for collisions */
    static bool gCheck4Accidents;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

