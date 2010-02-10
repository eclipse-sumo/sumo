/****************************************************************************/
/// @file    GUIGlobals.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Some global variables (yep)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;


// ===========================================================================
// global variables declarations
// ===========================================================================
/// the window shall be closed when the simulation has ended
extern bool gQuitOnEnd;

/// List of breakpoint
extern std::vector<int> gBreakpoints;


#endif

/****************************************************************************/

