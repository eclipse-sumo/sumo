/****************************************************************************/
/// @file    MSDebugHelper.cpp
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


SUMOTime debug_globaltime;


#ifdef ABS_DEBUG
SUMOTime debug_searchedtime = 18193;
std::string debug_searched1 = "-----";
std::string debug_searched2 = "-----";
std::string debug_searchedJunction = "-----";
#endif


/****************************************************************************/

