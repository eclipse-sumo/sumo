/****************************************************************************/
/// @file    GUIGlobalSelection.h
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// A global holder of selected objects
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
#ifndef GUIGlobalSelection_h
#define GUIGlobalSelection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUISelectedStorage.h"


// ===========================================================================
// global variable declarations
// ===========================================================================
/** @brief A global holder of selected objects
 *
 * @todo Check whether this should be replaced by a Singleton
 */
extern GUISelectedStorage gSelected;


#endif

/****************************************************************************/

