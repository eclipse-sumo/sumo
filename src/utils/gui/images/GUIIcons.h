/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef GUIIcons_h
#define GUIIcons_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


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
    ICON_SUMO,
    ICON_SUMO_MINI,
    ICON_SUMO_LOGO,
    ICON_EMPTY,
    ICON_OPEN_CONFIG,
    ICON_OPEN_NET,
    ICON_OPEN_SHAPES,
    ICON_OPEN_ADDITIONALS,
    ICON_OPEN_TLSPROGRAMS,
    ICON_RELOAD,
    ICON_SAVE,
    ICON_CLOSE,
    ICON_HELP,
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
    ICON_OSGVIEW,
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
    ICON_LOCATEROUTE,
    ICON_LOCATESTOP,
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
    ICON_NETEDIT_MINI,
    ICON_LOCK,
    ICON_ADD,
    ICON_REMOVE,
    ICON_BIGARROWLEFT,
    ICON_BIGARROWRIGHT,
    /// @}

    /// @name arrows
    /// @{
    ICON_ARROW_UP,
    ICON_ARROW_DOWN,
    ICON_ARROW_LEFT,
    ICON_ARROW_RIGHT,
    /// @}

    /// @name lane icons
    /// @{
    ICON_LANEPEDESTRIAN,
    ICON_LANEBUS,
    ICON_LANEBIKE,
    ICON_LANEGREENVERGE,
    /// @}

    /// @name netedit supermode icons
    /// @{
    ICON_SUPERMODENETWORK,
    ICON_SUPERMODEDEMAND,
    /// @}

    /// @name NETEDIT Network modes icons
    /// @{
    ICON_MODEADDITIONAL,
    ICON_MODECONNECTION,
    ICON_MODECREATEEDGE,
    ICON_MODECROSSING,
    ICON_MODETAZ,
    ICON_MODEDELETE,
    ICON_MODEINSPECT,
    ICON_MODEMOVE,
    ICON_MODESELECT,
    ICON_MODETLS,
    ICON_MODEPOLYGON,
    ICON_MODEPROHIBITION,
    /// @}

    /// @name NETEDIT Demand modes icons
    /// @{
    ICON_MODEROUTE,
    ICON_MODEVEHICLE,
    ICON_MODEVEHICLETYPE,
    ICON_MODESTOP,
    ICON_MODEPERSONTYPE,
    ICON_MODEPERSON,
    ICON_MODEPERSONPLAN,
    /// @}

    /// @name NETEDIT processing icons
    /// @{
    ICON_COMPUTEJUNCTIONS,
    ICON_CLEANJUNCTIONS,
    ICON_JOINJUNCTIONS,
    ICON_COMPUTEDEMAND,
    ICON_CLEANROUTES,
    ICON_JOINROUTES,
    ICON_OPTIONS,
    /// @}

    /// @name NETEDIT netElements icons
    /// @{
    ICON_JUNCTION,
    ICON_EDGE,
    ICON_LANE,
    ICON_CONNECTION,
    ICON_PROHIBITION,
    ICON_CROSSING,
    /// @}

    /// @name NETEDIT additional icons
    /// @{
    ICON_BUSSTOP,
    ICON_ACCESS,
    ICON_CONTAINERSTOP,
    ICON_CHARGINGSTATION,
    ICON_E1,
    ICON_E2,
    ICON_E3,
    ICON_E3ENTRY,
    ICON_E3EXIT,
    ICON_E1INSTANT,
    ICON_REROUTER,
    ICON_ROUTEPROBE,
    ICON_VAPORIZER,
    ICON_VARIABLESPEEDSIGN,
    ICON_CALIBRATOR,
    ICON_PARKINGAREA,
    ICON_PARKINGSPACE,
    ICON_REROUTERINTERVAL,
    ICON_VSSSTEP,
    ICON_CLOSINGREROUTE,
    ICON_CLOSINGLANEREROUTE,
    ICON_DESTPROBREROUTE,
    ICON_PARKINGZONEREROUTE,
    ICON_ROUTEPROBREROUTE,
    ICON_TAZ,
    ICON_TAZEDGE,
    /// @}

    /// @name NETEDIT DemandElements icons
    /// @{
    ICON_ROUTE,
    ICON_VTYPE,
    ICON_PTYPE,
    ICON_VEHICLE,
    ICON_TRIP,
    ICON_FLOW,
    ICON_ROUTEFLOW,
    ICON_STOPELEMENT,
    ICON_PERSON,
    ICON_PERSONFLOW,
    ICON_PERSONTRIP_FROMTO,
    ICON_PERSONTRIP_BUSSTOP,
    ICON_WALK_EDGES,
    ICON_WALK_FROMTO,
    ICON_WALK_BUSSTOP,
    ICON_WALK_ROUTE,
    ICON_RIDE_FROMTO,
    ICON_RIDE_BUSSTOP,
    ICON_PERSON_BUSSTOP,
    ICON_PERSON_CONTAINERSTOP,
    ICON_PERSON_LANESTOP,
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

    /// @name vehicle Shape icons
    /// @{
    ICON_VSHAPE_PEDESTRIAN,
    ICON_VSHAPE_BICYCLE,
    ICON_VSHAPE_MOPED,
    ICON_VSHAPE_MOTORCYCLE,
    ICON_VSHAPE_PASSENGER,
    ICON_VSHAPE_PASSENGER_SEDAN,
    ICON_VSHAPE_PASSENGER_HATCHBACK,
    ICON_VSHAPE_PASSENGER_WAGON,
    ICON_VSHAPE_PASSENGER_VAN,
    ICON_VSHAPE_DELIVERY,
    ICON_VSHAPE_TRUCK,
    ICON_VSHAPE_TRUCK_SEMITRAILER,
    ICON_VSHAPE_TRUCK_1TRAILER,
    ICON_VSHAPE_BUS,
    ICON_VSHAPE_BUS_COACH,
    ICON_VSHAPE_BUS_FLEXIBLE,
    ICON_VSHAPE_BUS_TROLLEY,
    ICON_VSHAPE_RAIL,
    ICON_VSHAPE_RAIL_CAR,
    ICON_VSHAPE_RAIL_CARGO,
    ICON_VSHAPE_E_VEHICLE,
    ICON_VSHAPE_ANT,
    ICON_VSHAPE_SHIP,
    ICON_VSHAPE_EMERGENCY,
    ICON_VSHAPE_FIREBRIGADE,
    ICON_VSHAPE_POLICE,
    ICON_VSHAPE_RICKSHAW,
    ICON_VSHAPE_UNKNOWN,
    /// @}

    /// @name icons for status
    /// @{
    ICON_OK,
    ICON_ACCEPT,
    ICON_CANCEL,
    ICON_CORRECT,
    ICON_ERROR,
    ICON_RESET,
    ICON_WARNING,
    /// @}

    /// @name icons for grid
    /// @{
    ICON_GRID1,
    ICON_GRID2,
    ICON_GRID3,
    /// @}

    /// @brief max number of icons
    ICON_MAX
};


#endif

/****************************************************************************/

