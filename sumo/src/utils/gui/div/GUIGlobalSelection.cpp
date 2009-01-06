/****************************************************************************/
/// @file    GUIGlobalSelection.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A global holder of selected objects
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIGlobalSelection.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUISelectedStorage.h>

#include <algorithm>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// global variables definitions
// ===========================================================================
GUISelectedStorage gSelected;



/****************************************************************************/

