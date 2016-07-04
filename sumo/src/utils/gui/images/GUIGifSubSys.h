/****************************************************************************/
/// @file    GUIGifSubSys.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id: GUIGifSubSys.h 20433 2016-04-13 08:00:14Z behrisch $
///
// A class to manage gifs of SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGifSubSys_h
#define GUIGifSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include "GUIGifs.h"
#include "GUITexturesHelper.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GUIGifSubSys {
public:
    /// @brief Initiate GUIGifSubSys for textures
    /// @param[in] a FOX Toolkit APP
    static void init(FXApp* a);

    /// @brief returns a texture Gif previously defined in the enum GUIGif
    /// @param[in] GUIGif code of texture to use
    static GUIGlID getGif(GUIGif which);

    /// @brief Reset textures
    /// @note Necessary to avoid problems with textures (ej: white empty)
    static void reset();

    /// @brief close GUIGifSubSys
    static void close();

private:
    /// @brief constructor
    /// @note is private because is called by the static function init(FXApp* a)
    GUIGifSubSys(FXApp* a);

    /// @brief destructor
    ~GUIGifSubSys();

    /// @pointer to Fox App
    FXApp* myApp;

    /// @brief instance of GUIGifSubSys
    static GUIGifSubSys* myInstance;

    /// @brief vector with the Gifs
    std::map<GUIGif, std::pair<bool, GUIGlID> > myTextures;
};


#endif

/****************************************************************************/

