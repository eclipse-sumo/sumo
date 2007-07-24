/****************************************************************************/
/// @file    MEGlobals.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEGlobals.cpp 96 2007-06-06 07:40:46Z behr_mi $
///
// Declarations of global mesoscopic simulation variables
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
// compiler pragmas
// ===========================================================================
#pragma warning(disable: 4786)


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// global variable definitions
// ===========================================================================
SUMOReal segment_speed = 19.6f;
SUMOReal segment_tauff = 1.4f;//1.2f;//1.4f;
SUMOReal segment_taufj = 1.4f;//1.2f;//1.4f;
SUMOReal segment_taujf = 2.0f;//1.8f;//2.0;
SUMOReal segment_taujj = 2.0f;//1.8f;//2.0;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


