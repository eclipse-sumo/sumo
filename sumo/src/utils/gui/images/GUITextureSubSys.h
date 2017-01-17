/****************************************************************************/
/// @file    GUITextureSubSys.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id$
///
// A class to manage gifs of SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUITextureSubSys_h
#define GUITextureSubSys_h


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
#include "GUITexturesHelper.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GUITextureSubSys {
public:
    /**@brief Initiate GUITextureSubSys for textures
     * @param[in] a FOX Toolkit APP
     */
    static void initTextures(FXApp* a);

    /**@brief returns a texture previously defined in the enum GUITexture
     * @param[in] GUITexture code of texture to use
     */
    static GUIGlID getTexture(GUITexture which);

    /**@brief Reset textures
     * @note Necessary to avoid problems with textures (ej: white empty)
     */
    static void resetTextures();

    /// @brief close GUITextureSubSys
    static void close();

private:
    /// @brief constructor private because is called by the static function init(FXApp* a
    GUITextureSubSys(FXApp* a);

    /// @brief destructor
    ~GUITextureSubSys();

    /// @pointer to Fox App
    FXApp* myApp;

    /// @brief instance of GUITextureSubSys
    static GUITextureSubSys* myInstance;

    /// @brief vector with the Gifs
    std::map<GUITexture, std::pair<bool, GUIGlID> > myTextures;
};


#endif

/****************************************************************************/

