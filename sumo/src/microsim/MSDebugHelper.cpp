/****************************************************************************/
/// @file    MSDebugHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


SUMOTime debug_globaltime;


#ifdef ABS_DEBUG
SUMOTime debug_searchedtime = 18193;
std::string debug_searched1 = "-----";
std::string debug_searched2 = "-----";
std::string debug_searchedJunction = "-----";
#endif


/****************************************************************************/

