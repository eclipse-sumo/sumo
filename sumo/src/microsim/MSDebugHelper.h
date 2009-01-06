/****************************************************************************/
/// @file    MSDebugHelper.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
//  »missingDescription«
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
#ifndef MSDebugHelper_h
#define MSDebugHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/SUMOTime.h>


/// The current simulation time for debugging purposes
extern SUMOTime debug_globaltime;

/// ----------------- debug variables -------------

#ifdef ABS_DEBUG
extern SUMOTime debug_searchedtime;
extern std::string debug_searched1, debug_searched2, debug_searchedJunction;
#endif


#endif

/****************************************************************************/

