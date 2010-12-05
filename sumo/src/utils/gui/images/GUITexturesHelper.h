/****************************************************************************/
/// @file    GUITexturesHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2006
/// @version $Id$
///
// Global storage for textures; manages and draws them
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
#ifndef GUITexturesHelper_h
#define GUITexturesHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


// ===========================================================================
// variable declarations
// ===========================================================================
/// determines whether the textures shall be displayed within the gui
extern bool gAllowTextures;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITexturesHelper
 * @brief Global storage for textures; manages and draws them
 */
class GUITexturesHelper {
public:
    /// Draws a named texture as a box with the given size
    static void drawTexturedBox(unsigned int which, SUMOReal size);

    /// Draws a named texture as a rectangle with the given sizes
    static void drawTexturedBox(unsigned int which,
                                SUMOReal sizeX1, SUMOReal sizeY1, SUMOReal sizeX2, SUMOReal sizeY2);

    /// Adds a texture to use
    static unsigned int add(FXImage *i);

};


#endif

/****************************************************************************/
