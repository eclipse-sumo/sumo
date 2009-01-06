/****************************************************************************/
/// @file    GUITexturesHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2006
/// @version $Id$
///
// Global storage for textures; manages and draws them
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
#include "GUITextures.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITexturesHelper
 * @brief Global storage for textures; manages and draws them
 */
class GUITexturesHelper
{
public:
    /// Initialises the textures-subsystem by setting the application
    static void init(FXApp *a);

    /// Draws the given link arrow as a box of the given size
    static void drawDirectionArrow(GUITexture which, SUMOReal size);

    /// Draws the given link arrow as a rectangle with the given sizes
    static void drawDirectionArrow(unsigned int which,
                                   SUMOReal sizeX1, SUMOReal sizeY1, SUMOReal sizeX2, SUMOReal sizeY2);

    /// Draws a named texture as a box with the given size
    static void drawTexturedBox(unsigned int which, SUMOReal size);

    /// Draws a named texture as a rectangle with the given sizes
    static void drawTexturedBox(unsigned int which,
                                SUMOReal sizeX1, SUMOReal sizeY1, SUMOReal sizeX2, SUMOReal sizeY2);

    /// Removes all allocated textures
    static void close();

    /// Adds a texture to use
    static unsigned int add(FXImage *i);

private:
    /// Builds previously added textures if not already done
    static void assignTextures();

private:
    /// Information whether the textures have been build
    static bool myWasInitialised;

    /// The application to use for building textures
    static FXApp *myApp;

    /// Storage for texture ids (!!! as a fixed-size container?)
    static GLuint myTextureIDs[TEXTURE_MAX];

    /// Storage for textures (!!! as a fixed-size container?)
    static FXImage *myTextures[TEXTURE_MAX];

};


#endif

/****************************************************************************/

