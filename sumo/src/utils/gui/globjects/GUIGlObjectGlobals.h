/****************************************************************************/
/// @file    GUIGlObjectGlobals.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id: $
///
// Some global variables (yep)
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
#ifndef GUIGlObjectGlobals_h
#define GUIGlObjectGlobals_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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

