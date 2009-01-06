/****************************************************************************/
/// @file    MSGlobals.h
/// @author  Daniel Krajzewicz
/// @date    late summer 2003
/// @version $Id$
///
// Some static variables for faster access
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSGlobals_h
#define MSGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <cstddef>


// ===========================================================================
// class declarations
// ===========================================================================
class MELoop;


// ===========================================================================
// class definitions
// ===========================================================================
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

    static SUMOReal gMinLaneChangeSight;
    static int gMinLaneChangeSightEdges;

    /** information whether the network shall check for collisions */
    static bool gCheck4Accidents;

#ifdef HAVE_MESOSIM
    /// Information whether a state has been loaded
    static bool gStateLoaded;
#endif

    /// Information whether c2x communication shall be used
    static bool gUsingC2C;
    /// the standard WLAN range
    static SUMOReal gLANRange;
    /// the Number of Sending possibility per TimeStep
    static SUMOReal gNumberOfSendingPos;
    /// the Number Of Infos per Paket
    static SUMOReal gInfoPerPaket;
    /// duration until which an information is kept
    static SUMOTime gLANRefuseOldInfosOffset;
    /// factor over which the real duration over an edge must lie over the estimated to be inserted
    static SUMOReal gAddInfoFactor;

#ifdef HAVE_MESOSIM
    /** Information whether mesosim shall be used */
    static bool gUseMesoSim;
    static MELoop *gMesoNet;

#endif

};


#endif

/****************************************************************************/

