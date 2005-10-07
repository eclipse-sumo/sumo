#ifndef GUIGlobals_h
#define GUIGlobals_h
//---------------------------------------------------------------------------//
//                        GUIGlobals.h -
//  Some global variables (yep)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.9  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 07:47:23  dkrajzew
// level 3 warnings removed
//
// Revision 1.5  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
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

#include <vector>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <gui/GUIAddWeightsStorage.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
struct GUISimInfo {
    GUISimInfo(GUINet &netArg) : net(netArg) { }
    GUINet &net;
};


/* =========================================================================
 * global variables declarations
 * ======================================================================= */
/// the window shall be closed when the simulation has ended
extern bool gQuitOnEnd;

/// Information whether aggregated views are allowed
extern bool gAllowAggregated;

/// Information whether time-SUMORealing views are allowed
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

