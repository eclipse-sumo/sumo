/****************************************************************************/
/// @file    GUITextures.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An enumeration of internally used textures
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
#ifndef GUITextures_h
#define GUITextures_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSLink.h>


// ===========================================================================
// enumeration
// ===========================================================================
/**
 * @enum GUITexture
 * @brief An enumeration of internally used textures
 */
enum GUITexture {
    /// a straight arrow
    TEXTURE_LINKDIR_STRAIGHT = MSLink::LINKDIR_STRAIGHT,
    /// a turn around arrow
    TEXTURE_LINKDIR_TURN = MSLink::LINKDIR_TURN,
    /// a turn left arrow
    TEXTURE_LINKDIR_LEFT = MSLink::LINKDIR_LEFT,
    /// a turn right arrow
    TEXTURE_LINKDIR_RIGHT = MSLink::LINKDIR_RIGHT,
    /// a partially turn left arrow
    TEXTURE_LINKDIR_PARTLEFT = MSLink::LINKDIR_PARTLEFT,
    /// a partially turn right arrow
    TEXTURE_LINKDIR_PARTRIGHT = MSLink::LINKDIR_PARTRIGHT,
    /// maximum value
    TEXTURE_MAX

};


#endif

/****************************************************************************/

