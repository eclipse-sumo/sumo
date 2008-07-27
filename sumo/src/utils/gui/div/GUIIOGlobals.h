/****************************************************************************/
/// @file    GUIIOGlobals.h
/// @author  Daniel Krajzewicz
/// @date    2004-11-23
/// @version $Id$
///
// The folder used as last
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
#ifndef GUIIOGlobals_h
#define GUIIOGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// global variable declarations
// ===========================================================================
/** @brief The folder used as last
 *
 * This value is loaded and stored within the registry on startup/shutdown
 *  of the application. It is changed after a file was loaded/saved.
 */
extern FXString gCurrentFolder;


#endif

/****************************************************************************/

