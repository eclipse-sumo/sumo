/****************************************************************************/
/// @file    GUIGlObjectGlobals.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Some global variables (yep)
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
#ifndef GUIGlObjectGlobals_h
#define GUIGlObjectGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObjectStorage.h>


// ===========================================================================
// variable definitions
// ===========================================================================
/** @brief A container for numerical ids of objects
    in order to make them grippable by openGL */
extern GUIGlObjectStorage gIDStorage;


extern GUIGlObject *gNetWrapper;


#endif

/****************************************************************************/

