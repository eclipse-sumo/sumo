/****************************************************************************/
/// @file    GUIGlobals.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Some global variables (yep)
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
#ifndef GUIGlobals_h
#define GUIGlobals_h
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

#include <vector>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <gui/GUIAddWeightsStorage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;


// ===========================================================================
// class definitions
// ===========================================================================
struct GUISimInfo
{
    GUISimInfo(GUINet &netArg) : net(netArg)
    { }
    GUINet &net;
};


// ===========================================================================
// global variables declarations
// ===========================================================================
/// the window shall be closed when the simulation has ended
extern bool gQuitOnEnd;

/// Information whether aggregated views are allowed
extern bool gAllowAggregated;

/// Information whether time-floating views are allowed
extern bool gAllowAggregatedFloating;

/// Information whether not to display the information about a simulation's end
extern bool gSuppressEndInfo;

/// Structure holding the used network
extern GUISimInfo *gSimInfo;


/// A (global?!!!) value about how fast values shall be replaced within aggregated views
extern SUMOReal gAggregationRememberingFactor;




/// Storage for additional weights
extern GUIAddWeightsStorage gAddWeightsStorage;

/// List of breakpoint
extern std::vector<int> gBreakpoints;


#endif

/****************************************************************************/

