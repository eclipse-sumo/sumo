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
    /// @name general Interface icons
    /// @{
    ICON_APP = 0,
    ICON_DLR,
    ICON_EMPTY,
    ICON_OPEN_CONFIG,
    ICON_OPEN_NET,
    ICON_OPEN_SHAPES,
    ICON_OPEN_ADDITIONALS,
    ICON_RELOAD,
    ICON_SAVE,
    ICON_CLOSE,
    /// @}

    /// @name simulation control icons
    /// @{
    ICON_START,
    ICON_STOP,
    ICON_STEP,
    /// @}

    /// @name simulation view icons
    /// @{
    ICON_MICROVIEW,
    ICON_LAGGRVIEW,
    /// @}

    /// @name simulation view icons (other)
    /// @{
    ICON_RECENTERVIEW,
    ICON_ALLOWROTATION,
    /// @}

    /// @name locate objects icons
    /// @{
    ICON_LOCATE,
    ICON_LOCATEJUNCTION,
    ICON_LOCATEEDGE,
    ICON_LOCATEVEHICLE,
    ICON_LOCATEPERSON,
    ICON_LOCATETLS,
    ICON_LOCATEADD,
    ICON_LOCATEPOI,
    ICON_LOCATEPOLY,
    /// @}

    /// @name green and yellow objects icons
    /// @{
    ICON_GREENEDGE,
    ICON_GREENVEHICLE,
    ICON_GREENPERSON,
    ICON_YELLOWEDGE,
    ICON_YELLOWVEHICLE,
    ICON_YELLOWPERSON,
    /// @}

    /// @name options icons
    /// @{
    ICON_COLORWHEEL,
    ICON_SAVEDB,
    ICON_REMOVEDB,
    ICON_SHOWTOOLTIPS,
    ICON_EDITVIEWPORT,
    ICON_ZOOMSTYLE,
    /// @}

    /// @name app icons
    /// @{
    ICON_APP_TRACKER,
    ICON_APP_FINDER,
    ICON_APP_BREAKPOINTS,
    ICON_APP_TLSTRACKER,
    ICON_APP_TABLE,
    ICON_APP_SELECTOR,
    /// @}

    /// @name decision icons
    /// @{
    ICON_YES,
    ICON_NO,
    /// @}

    /// @name flags icons
    /// @{
    ICON_FLAG,
    ICON_FLAG_PLUS,
    ICON_FLAG_MINUS,
    /// @}

    /// @name windows icosn
    /// @{
    ICON_WINDOWS_CASCADE,
    ICON_WINDOWS_TILE_VERT,
    ICON_WINDOWS_TILE_HORI,
    /// @}

    /// @name manipulation icons
    /// @{
    ICON_MANIP,
    ICON_CAMERA,
    /// @}

    /// @name graph icons
    /// @{
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
    /// @}

    /// @name other tools
    /// @{
    ICON_EXT,
    ICON_CUT_SWELL,
    /// @}

    /// @name net edit icons
    /// @{
    ICON_UNDO,
    ICON_REDO,
    ICON_NETEDIT,
    ICON_LOCK,
    ICON_ADD,
    ICON_REMOVE,
    ICON_NETEDITARROW,
    /// @}

    /// @name lane icons
    /// @{
    ICON_LANEPEDESTRIAN,
    ICON_LANEBUS,
    ICON_LANEBIKE,
    /// @}

    /// @name netedit modes icons
    /// @{
    ICON_MODEADDITIONAL,
    ICON_MODECONNECTION,
    ICON_MODECREATEEDGE,
    ICON_MODECROSSING,
    ICON_MODEDELETE,
    ICON_MODEINSPECT,
    ICON_MODEMOVE,
    ICON_MODESELECT,
    ICON_MODETLS,
    /// @}

    /// @name netedit processing icons
    /// @{
    ICON_CLEANJUNCTIONS,
    ICON_COMPUTEJUNCTIONS,
    ICON_JOINJUNCTIONS,
    ICON_OPTIONS,
    /// @}

    /// @name netedit netElements icons
    /// @{
    ICON_JUNCTION,
    ICON_EDGE,
    ICON_LANE,
    ICON_CONNECTION,
    ICON_PROHIBITION,
    ICON_CROSSING,
    /// @}

    /// @name netedit additional icons
    /// @{
    ICON_BUSSTOP,
    ICON_CONTAINERSTOP,
    ICON_CHARGINGSTATION,
    ICON_E1,
    ICON_E2,
    ICON_E3,
    ICON_E3ENTRY,
    ICON_E3EXIT,
    ICON_REROUTER,
    ICON_ROUTEPROBE,
    ICON_VAPORIZER,
    ICON_VARIABLESPEEDSIGN,
    ICON_CALIBRATOR,
    /// @}

    /// @name vehicle Class icons
    /// @{
    ICON_VCLASS_IGNORING,
    ICON_VCLASS_PRIVATE,
    ICON_VCLASS_EMERGENCY,
    ICON_VCLASS_AUTHORITY,
    ICON_VCLASS_ARMY,
    ICON_VCLASS_VIP,
    ICON_VCLASS_PASSENGER,
    ICON_VCLASS_HOV,
    ICON_VCLASS_TAXI,
    ICON_VCLASS_BUS,
    ICON_VCLASS_COACH,
    ICON_VCLASS_DELIVERY,
    ICON_VCLASS_TRUCK,
    ICON_VCLASS_TRAILER,
    ICON_VCLASS_TRAM,
    ICON_VCLASS_RAIL_URBAN,
    ICON_VCLASS_RAIL,
    ICON_VCLASS_RAIL_ELECTRIC,
    ICON_VCLASS_MOTORCYCLE,
    ICON_VCLASS_MOPED,
    ICON_VCLASS_BICYCLE,
    ICON_VCLASS_PEDESTRIAN,
    ICON_VCLASS_EVEHICLE,
    ICON_VCLASS_SHIP,
    ICON_VCLASS_CUSTOM1,
    ICON_VCLASS_CUSTOM2,
    /// @}

    /// @name icons for status
    /// @{
    ICON_ACCEPT,
    ICON_CANCEL,
    ICON_CORRECT,
    ICON_ERROR,
    ICON_RESET,
    ICON_WARNING,
    /// @}

    /// @brief max number of icons
    ICON_MAX
};


#endif

/****************************************************************************/

