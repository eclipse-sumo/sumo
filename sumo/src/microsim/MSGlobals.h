/****************************************************************************/
/// @file    MSGlobals.h
/// @author  Daniel Krajzewicz
/// @date    late summer 2003
/// @version $Id$
///
// Some static variables for faster access
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
#ifndef MSGlobals_h
#define MSGlobals_h
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

    /// Information whether a state has been loaded
    static bool gStateLoaded;


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

