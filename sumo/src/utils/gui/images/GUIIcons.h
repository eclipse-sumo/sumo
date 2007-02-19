/****************************************************************************/
/// @file    GUIIcons.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Base class for additional objects (emitter, detectors etc.)
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
#ifndef GUIIcons_h
#define GUIIcons_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// enumerations
// ===========================================================================
enum GUIIcon {
    ICON_APP = 0,
    ICON_OPEN,
    ICON_RELOAD,
    ICON_SAVE,
    ICON_CLOSE,

    ICON_START,
    ICON_STOP,
    ICON_STEP,

    ICON_MICROVIEW,
    ICON_LAGGRVIEW,

    ICON_RECENTERVIEW,
    ICON_SHOWLEGEND,
    ICON_ALLOWROTATION,

    ICON_LOCATE,
    ICON_LOCATEJUNCTION,
    ICON_LOCATEEDGE,
    ICON_LOCATEVEHICLE,
    ICON_LOCATEADD,

    ICON_SHOWGRID,
    ICON_COLORWHEEL,
    ICON_SHOWTOOLTIPS,
    ICON_SHOWFULLGEOM,
    ICON_EDITVIEWPORT,

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

    ICON_SPEEDLIMIT_020KMH,
    ICON_SPEEDLIMIT_040KMH,
    ICON_SPEEDLIMIT_060KMH,
    ICON_SPEEDLIMIT_080KMH,
    ICON_SPEEDLIMIT_100KMH,
    ICON_SPEEDLIMIT_120KMH,
    ICON_SPEEDLIMIT_140KMH,
    ICON_SPEEDLIMIT_160KMH,
    ICON_SPEEDLIMIT_180KMH,

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

    ICON_MAX

};


#endif

/****************************************************************************/

