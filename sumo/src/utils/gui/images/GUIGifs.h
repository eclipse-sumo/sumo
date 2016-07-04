/****************************************************************************/
/// @file    GUIGifs.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id: GUIGifs.h 20975 2016-06-15 13:02:40Z palcraft $
///
// An enumeration of gifs used by the gui applications
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
#ifndef GUIGifs_h
#define GUIGifs_h


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
 * @enum GUIGif
 * @brief An enumeration of gifs used by the gui applications
 */
enum GUIGif {
    GNELOGO_E1 = 0,
    GNELOGO_E2,
    GNELOGO_E3,
    GNELOGO_E3SELECTED,
    GNELOGO_EMPTY,
    GNELOGO_EMPTY_SELECTED,
    GNELOGO_ENTRY,
    GNELOGO_EXIT,
    GNELOGO_LOCK,
    GNELOGO_LOCKSELECTED,
    GNELOGO_REROUTER,
    GNELOGO_REROUTERSELECTED,
    GNELOGO_ROUTEPROBE,
    GNELOGO_ROUTEPROBESELECTED,
    GNELOGO_TLS,
    GNELOGO_VAPORIZER,
    GNELOGO_VAPORIZERSELECTED,
    GNELOGO_VARIABLESPEEDSIGNAL,
    GNELOGO_VARIABLESPEEDSIGNALSELECTED,
    GIF_MAX
};


#endif

/****************************************************************************/

