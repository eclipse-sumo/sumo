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
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <fx.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIAddWeightsStorage.h>
#include <gui/GUIGradientStorage.h>


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

/// the gui loads and starts a simulation at the beginning
extern bool gStartAtBegin;

/// Information whether aggregated views are allowed
extern bool gAllowAggregated;

/// Information whether time-floating views are allowed
extern bool gAllowAggregatedFloating;

/// Information whether to use textures
extern bool gAllowTextures;

/// Information whether not to display the information about a simulation's end
extern bool gSuppressEndInfo;

/// Structure holding the used network
extern GUISimInfo *gSimInfo;

/// The folder used as last
extern std::string gCurrentFolder;

/// A storage for available gradients
extern GUIGradientStorage *gGradients;

/// A (global?!!!) value about how fast values shall be replaced within aggregated views
extern float gAggregationRememberingFactor;

/// Pointer to the FX-Application
extern FXApp *gFXApp;


    /** @brief A container for numerical ids of objects
        in order to make them grippable by openGL */
extern GUIGlObjectStorage gIDStorage;

/// Storage for additional weights
extern GUIAddWeightsStorage gAddWeightsStorage;

/// List of breakpoint
extern std::vector<int> gBreakpoints;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

