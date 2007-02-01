/****************************************************************************/
/// @file    MSDebugHelper.h
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
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
#ifndef MSDebugHelper_h
#define MSDebugHelper_h
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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

