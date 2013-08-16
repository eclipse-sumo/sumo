/****************************************************************************/
/// @file    GUIIcons.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Andreas Gaubatz
/// @date    2004
/// @version $Id$
///
// An enumeration of icons used by the gui applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIIcons_h
#define GUIIcons_h


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
 * @enum GUIIcon
 * @brief An enumeration of icons used by the gui applications
 */
enum GUIIcon {
    ICON_APP = 0,
    ICON_EMPTY,
    ICON_OPEN_CONFIG,
    ICON_OPEN_NET,
    ICON_RELOAD,
    ICON_SAVE,
    ICON_CLOSE,

    ICON_START,
    ICON_STOP,
    ICON_STEP,

    ICON_MICROVIEW,
    ICON_LAGGRVIEW,

    ICON_RECENTERVIEW,
    ICON_ALLOWROTATION,

    ICON_LOCATE,
    ICON_LOCATEJUNCTION,
    ICON_LOCATEEDGE,
    ICON_LOCATEVEHICLE,
    ICON_LOCATETLS,
    ICON_LOCATEADD,
    ICON_LOCATESHAPE,

    ICON_COLORWHEEL,
    ICON_SAVEDB,
    ICON_REMOVEDB,
    ICON_SHOWTOOLTIPS,
    ICON_EDITVIEWPORT,
    ICON_ZOOMSTYLE,

    ICON_APP_TRACKER,
    ICON_APP_FINDER,
    ICON_APP_BREAKPOINTS,
    ICON_APP_TLSTRACKER,
    ICON_APP_TABLE,
    ICON_APP_SELECTOR,


    ICON_YES,
    ICON_NO,

    ICON_FLAG,
    ICON_FLAG_PLUS,
    ICON_FLAG_MINUS,

    ICON_WINDOWS_CASCADE,
    ICON_WINDOWS_TILE_VERT,
    ICON_WINDOWS_TILE_HORI,

    ICON_MANIP,
    ICON_CAMERA,

    ICON_EXTRACT,
    ICON_DILATE,
    ICON_ERODE,
    ICON_OPENING,
    ICON_CLOSING,
    ICON_CLOSE_GAPS,
    ICON_ERASE_STAINS,
    ICON_SKELETONIZE,
    ICON_RARIFY,
    ICON_CREATE_GRAPH,
    ICON_OPEN_BMP_DIALOG,
    ICON_EYEDROP,
    ICON_PAINTBRUSH1X,
    ICON_PAINTBRUSH2X,
    ICON_PAINTBRUSH3X,
    ICON_PAINTBRUSH4X,
    ICON_PAINTBRUSH5X,
    ICON_RUBBER1X,
    ICON_RUBBER2X,
    ICON_RUBBER3X,
    ICON_RUBBER4X,
    ICON_RUBBER5X,
    ICON_EDITGRAPH,

    ICON_EXT,
    ICON_CUT_SWELL,

    ICON_UNDO,
    ICON_REDO,
    ICON_NETEDIT,

    ICON_MAX

};


#endif

/****************************************************************************/

