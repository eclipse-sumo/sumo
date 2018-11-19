/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUICursors.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2018
/// @version $Id$
///
// An enumeration of icons used by the gui applications
/****************************************************************************/
#ifndef GUICursors_h
#define GUICursors_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUICursor
 * @brief An enumeration of icons used by the gui applications
 */
enum GUICursor {
    /// @name general Interface icons
    /// @{
    CURSOR_SUMO,
    CURSOR_SUMO_MINI,
    CURSOR_SUMO_LOGO,
    CURSOR_EMPTY,
    CURSOR_OPEN_CONFIG,
    CURSOR_OPEN_NET,
    CURSOR_OPEN_SHAPES,
    CURSOR_OPEN_ADDITIONALS,
    CURSOR_OPEN_TLSPROGRAMS,
    CURSOR_RELOAD,
    CURSOR_SAVE,
    CURSOR_CLOSE,
    CURSOR_HELP,
    /// @}

    /// @name simulation control icons
    /// @{
    CURSOR_START,
    CURSOR_STOP,
    CURSOR_STEP,
    /// @}

    /// @name simulation view icons
    /// @{
    CURSOR_MICROVIEW,
    CURSOR_LAGGRVIEW,
    /// @}

    /// @name simulation view icons (other)
    /// @{
    CURSOR_RECENTERVIEW,
    CURSOR_ALLOWROTATION,
    /// @}

    /// @name locate objects icons
    /// @{
    CURSOR_LOCATE,
    CURSOR_LOCATEJUNCTION,
    CURSOR_LOCATEEDGE,
    CURSOR_LOCATEVEHICLE,
    CURSOR_LOCATEPERSON,
    CURSOR_LOCATETLS,
    CURSOR_LOCATEADD,
    CURSOR_LOCATEPOI,
    CURSOR_LOCATEPOLY,
    /// @}

    /// @name green and yellow objects icons
    /// @{
    CURSOR_GREENEDGE,
    CURSOR_GREENVEHICLE,
    CURSOR_GREENPERSON,
    CURSOR_YELLOWEDGE,
    CURSOR_YELLOWVEHICLE,
    CURSOR_YELLOWPERSON,
    /// @}

    /// @name options icons
    /// @{
    CURSOR_COLORWHEEL,
    CURSOR_SAVEDB,
    CURSOR_REMOVEDB,
    CURSOR_SHOWTOOLTIPS,
    CURSOR_EDITVIEWPORT,
    CURSOR_ZOOMSTYLE,
    /// @}

    /// @name app icons
    /// @{
    CURSOR_APP_TRACKER,
    CURSOR_APP_FINDER,
    CURSOR_APP_BREAKPOINTS,
    CURSOR_APP_TLSTRACKER,
    CURSOR_APP_TABLE,
    CURSOR_APP_SELECTOR,
    /// @}

    /// @name decision icons
    /// @{
    CURSOR_YES,
    CURSOR_NO,
    /// @}

    /// @name flags icons
    /// @{
    CURSOR_FLAG,
    CURSOR_FLAG_PLUS,
    CURSOR_FLAG_MINUS,
    /// @}

    /// @name windows icosn
    /// @{
    CURSOR_WINDOWS_CASCADE,
    CURSOR_WINDOWS_TILE_VERT,
    CURSOR_WINDOWS_TILE_HORI,
    /// @}

    /// @name manipulation icons
    /// @{
    CURSOR_MANIP,
    CURSOR_CAMERA,
    /// @}

    /// @name graph icons
    /// @{
    CURSOR_EXTRACT,
    CURSOR_DILATE,
    CURSOR_ERODE,
    CURSOR_OPENING,
    CURSOR_CLOSING,
    CURSOR_CLOSE_GAPS,
    CURSOR_ERASE_STAINS,
    CURSOR_SKELETONIZE,
    CURSOR_RARIFY,
    CURSOR_CREATE_GRAPH,
    CURSOR_OPEN_BMP_DIALOG,
    CURSOR_EYEDROP,
    CURSOR_PAINTBRUSH1X,
    CURSOR_PAINTBRUSH2X,
    CURSOR_PAINTBRUSH3X,
    CURSOR_PAINTBRUSH4X,
    CURSOR_PAINTBRUSH5X,
    CURSOR_RUBBER1X,
    CURSOR_RUBBER2X,
    CURSOR_RUBBER3X,
    CURSOR_RUBBER4X,
    CURSOR_RUBBER5X,
    CURSOR_EDITGRAPH,
    /// @}

    /// @name other tools
    /// @{
    CURSOR_EXT,
    CURSOR_CUT_SWELL,
    /// @}

    /// @name net edit icons
    /// @{
    CURSOR_UNDO,
    CURSOR_REDO,
    CURSOR_NETEDIT,
    CURSOR_NETEDIT_MINI,
    CURSOR_LOCK,
    CURSOR_ADD,
    CURSOR_REMOVE,
    CURSOR_NETEDITARROWLEFT,
    CURSOR_NETEDITARROWRIGHT,
    /// @}

    /// @name lane icons
    /// @{
    CURSOR_LANEPEDESTRIAN,
    CURSOR_LANEBUS,
    CURSOR_LANEBIKE,
    CURSOR_LANEGREENVERGE,
    /// @}

    /// @name netedit modes icons
    /// @{
    CURSOR_MODEADDITIONAL,
    CURSOR_MODECONNECTION,
    CURSOR_MODECREATEEDGE,
    CURSOR_MODECROSSING,
    CURSOR_MODETAZ,
    CURSOR_MODEDELETE,
    CURSOR_MODEINSPECT,
    CURSOR_MODEMOVE,
    CURSOR_MODESELECT,
    CURSOR_MODETLS,
    CURSOR_MODEPOLYGON,
    CURSOR_MODEPROHIBITION,
    /// @}

    /// @name netedit processing icons
    /// @{
    CURSOR_CLEANJUNCTIONS,
    CURSOR_COMPUTEJUNCTIONS,
    CURSOR_JOINJUNCTIONS,
    CURSOR_OPTIONS,
    /// @}

    /// @name netedit netElements icons
    /// @{
    CURSOR_JUNCTION,
    CURSOR_EDGE,
    CURSOR_LANE,
    CURSOR_CONNECTION,
    CURSOR_PROHIBITION,
    CURSOR_CROSSING,
    /// @}

    /// @name netedit additional icons
    /// @{
    CURSOR_BUSSTOP,
    CURSOR_ACCESS,
    CURSOR_CONTAINERSTOP,
    CURSOR_CHARGINGSTATION,
    CURSOR_E1,
    CURSOR_E2,
    CURSOR_E3,
    CURSOR_E3ENTRY,
    CURSOR_E3EXIT,
    CURSOR_E1INSTANT,
    CURSOR_REROUTER,
    CURSOR_ROUTEPROBE,
    CURSOR_VAPORIZER,
    CURSOR_VARIABLESPEEDSIGN,
    CURSOR_CALIBRATOR,
    CURSOR_PARKINGAREA,
    CURSOR_PARKINGSPACE,
    CURSOR_REROUTERINTERVAL,
    CURSOR_VSSSTEP,
    CURSOR_CLOSINGREROUTE,
    CURSOR_CLOSINGLANEREROUTE,
    CURSOR_DESTPROBREROUTE,
    CURSOR_PARKINGZONEREROUTE,
    CURSOR_ROUTEPROBREROUTE,
    CURSOR_TAZ,
    CURSOR_TAZEDGE,
    /// @}

    /// @name netedit routeElements icons
    /// @{
    CURSOR_FLOW,
    CURSOR_ROUTE,
    CURSOR_VTYPE,
    /// @}

    /// @name vehicle Class icons
    /// @{
    CURSOR_VCLASS_IGNORING,
    CURSOR_VCLASS_PRIVATE,
    CURSOR_VCLASS_EMERGENCY,
    CURSOR_VCLASS_AUTHORITY,
    CURSOR_VCLASS_ARMY,
    CURSOR_VCLASS_VIP,
    CURSOR_VCLASS_PASSENGER,
    CURSOR_VCLASS_HOV,
    CURSOR_VCLASS_TAXI,
    CURSOR_VCLASS_BUS,
    CURSOR_VCLASS_COACH,
    CURSOR_VCLASS_DELIVERY,
    CURSOR_VCLASS_TRUCK,
    CURSOR_VCLASS_TRAILER,
    CURSOR_VCLASS_TRAM,
    CURSOR_VCLASS_RAIL_URBAN,
    CURSOR_VCLASS_RAIL,
    CURSOR_VCLASS_RAIL_ELECTRIC,
    CURSOR_VCLASS_MOTORCYCLE,
    CURSOR_VCLASS_MOPED,
    CURSOR_VCLASS_BICYCLE,
    CURSOR_VCLASS_PEDESTRIAN,
    CURSOR_VCLASS_EVEHICLE,
    CURSOR_VCLASS_SHIP,
    CURSOR_VCLASS_CUSTOM1,
    CURSOR_VCLASS_CUSTOM2,
    /// @}

    /// @name icons for status
    /// @{
    CURSOR_OK,
    CURSOR_ACCEPT,
    CURSOR_CANCEL,
    CURSOR_CORRECT,
    CURSOR_ERROR,
    CURSOR_RESET,
    CURSOR_WARNING,
    /// @}

    /// @name icons for grid
    /// @{
    CURSOR_GRID1,
    CURSOR_GRID2,
    CURSOR_GRID3,
    /// @}

    /// @brief max number of icons
    CURSOR_MAX
};


#endif

/****************************************************************************/

