/****************************************************************************/
/// @file    GUITexturesHelper.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2006
/// @version $Id$
///
// Global storage for textures; manages and draws them
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2006-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITexturesHelper
 * @brief Global storage for textures; manages and draws them
 */
class GUITexturesHelper {
public:
    /// @brief return maximum number of pixels in x and y direction
    static int getMaxTextureSize();

    /// @brief Adds a texture to use
    static GUIGlID add(FXImage* i);

    /// @brief Draws a named texture as a box with the given size
    static void drawTexturedBox(int which, SUMOReal size);

    /// @brief Draws a named texture as a rectangle with the given sizes
    static void drawTexturedBox(int which, SUMOReal sizeX1, SUMOReal sizeY1, SUMOReal sizeX2, SUMOReal sizeY2);

    /**@brief return texture id for the given filename (initialize on first use)
     * @note return -1 on failure
     */
    static int getTextureID(const std::string& filename, const bool mirrorX = false);

    /// @brief clears loaded textures
    static void clearTextures();

    /// @brief switch texture drawing on and off
    static void allowTextures(const bool val) {
        myAllowTextures = val;
    }

    /// @brief ask whether texture drawing is enabled
    static bool texturesAllowed() {
        return myAllowTextures;
    }

private:
    /// @brief mapping from image paths to decals (initialization on first use)
    static std::map<std::string, int> myTextures;

    /// @brief whether textures are drawn
    static bool myAllowTextures;
};


#endif

/****************************************************************************/
