/****************************************************************************/
/// @file    GUITextures.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id$
///
// An enumeration of gifs used by the gui applications
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


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUITexture
 * @brief An enumeration of gifs used by the gui applications
 */
enum GUITexture {
    GNETEXTURE_E1 = 0,
    GNETEXTURE_E2,
    GNETEXTURE_E3,
    GNETEXTURE_E3SELECTED,
    GNETEXTURE_EMPTY,
    GNETEXTURE_EMPTYSELECTED,
    GNETEXTURE_ENTRY,
    GNETEXTURE_EXIT,
    GNETEXTURE_LOCK,
    GNETEXTURE_LOCKSELECTED,
    GNETEXTURE_NOTMOVING,
    GNETEXTURE_NOTMOVINGSELECTED,
    GNETEXTURE_REROUTER,
    GNETEXTURE_REROUTERSELECTED,
    GNETEXTURE_ROUTEPROBE,
    GNETEXTURE_ROUTEPROBESELECTED,
    GNETEXTURE_TLS,
    GNETEXTURE_VAPORIZER,
    GNETEXTURE_VAPORIZERSELECTED,
    GNETEXTURE_VARIABLESPEEDSIGN,
    GNETEXTURE_VARIABLESPEEDSIGNSELECTED,
    GNETEXTURE_LANEBIKE,
    GNETEXTURE_LANEBUS,
    GNETEXTURE_LANEPEDESTRIAN,
    GIF_MAX
};


#endif

/****************************************************************************/

