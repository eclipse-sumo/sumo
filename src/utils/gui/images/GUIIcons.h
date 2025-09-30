/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIIcons.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Andreas Gaubatz
/// @date    2004
///
// An enumeration of icons used by the gui applications
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUIIcon
 * @brief An enumeration of icons used by the gui applications
 */
enum class GUIIcon {

    /// @name SUMO Logos
    /// @{
    SUMO,
    SUMO_MINI,
    SUMO_LOGO,
    /// @}

    /// @name other SUMO programs
    /// @{
    NETEDIT,
    NETEDIT_MINI,
    NETGENERATE,
    /// @}

    /// @name netgenerate networks
    /// @{
    NETGENERATE_GRID,
    NETGENERATE_SPIDER,
    NETGENERATE_RANDOMGRID,
    NETGENERATE_RANDOM,
    /// @}

    /// @name general Interface icons
    /// @{
    EMPTY,
    NEW_NET,
    RELOAD,
    CLOSE,
    HELP,
    HALL_OF_FAME,
    CLEARMESSAGEWINDOW,
    GITHUB,
    /// @}

    /// @name netedit view icons
    /// @{
    VIEWDEFAULT,
    VIEWJUPEDSIM,
    /// @}

    /// @brief open icons
    /// @{
    OPEN,
    OPEN_SUMOCONFIG,
    OPEN_NETEDITCONFIG,
    OPEN_NET,
    OPEN_SHAPES,
    OPEN_ADDITIONALS,
    OPEN_MEANDATAS,
    OPEN_TLSPROGRAMS,
    OPEN_BMPDIALOG,
    /// @{

    /// @brief save icons
    /// @{
    SAVE,
    SAVE_DATABASE,
    SAVE_SUMOCONFIG,
    SAVE_NETEDITCONFIG,
    SAVE_MULTIPLE,
    SAVE_NETWORKELEMENTS,
    SAVE_ADDITIONALELEMENTS,
    SAVE_JUPEDSIMELEMENTS,
    SAVE_DEMANDELEMENTS,
    SAVE_DATAELEMENTS,
    SAVE_MEANDATAELEMENTS,
    /// @}

    /// @brief basic operations
    /// @{
    CUT,
    COPY,
    PASTE,
    /// @}


    /// @name simulation control icons
    /// @{
    START,
    STOP,
    STEP,
    /// @}

    /// @name select icons
    /// @{
    SELECT,
    UNSELECT,
    /// @}

    /// @name join icons
    /// @{
    JOIN,
    DISJOIN,
    /// @}

    /// @name simulation view icons
    /// @{
    MICROVIEW,
    OSGVIEW,
    /// @}

    /// @name simulation view icons (other)
    /// @{
    RECENTERVIEW,
    ALLOWROTATION,
    /// @}

    /// @name locate objects icons
    /// @{
    LOCATE,
    LOCATEJUNCTION,
    LOCATEEDGE,
    LOCATEWALKINGAREA,
    LOCATEVEHICLE,
    LOCATEROUTE,
    LOCATESTOP,
    LOCATEPERSON,
    LOCATECONTAINER,
    LOCATETLS,
    LOCATEADD,
    LOCATEPOI,
    LOCATEPOLY,
    /// @}

    /// @name tools
    /// @{
    TOOL_PYTHON,
    TOOL_EMISSIONS,
    TOOL_CITYBRAIN,
    TOOL_GTFS,
    TOOL_VISSIM,
    TOOL_VISUM,
    TOOL_IMPORT,
    TOOL_OUTPUT,
    TOOL_TURNDEFS,
    TOOL_XML,
    /// @}

    /// @name green and yellow objects icons
    /// @{
    GREENCONTAINER,
    GREENEDGE,
    GREENVEHICLE,
    GREENPERSON,
    YELLOWCONTAINER,
    YELLOWEDGE,
    YELLOWVEHICLE,
    YELLOWPERSON,
    /// @}

    /// @name options icons
    /// @{
    COLORWHEEL,
    REMOVEDB,
    SHOWTOOLTIPS_VIEW,
    SHOWTOOLTIPS_MENU,
    EDITVIEWPORT,
    ZOOMSTYLE,
    FULL_SCREEN,
    /// @}

    /// @name app icons
    /// @{
    APP_TRACKER,
    APP_FINDER,
    APP_BREAKPOINTS,
    APP_TLSTRACKER,
    APP_TABLE,
    APP_SELECTOR,
    /// @}

    /// @name decision icons
    /// @{
    YES,
    NO,
    /// @}

    /// @name flags icons
    /// @{
    FLAG,
    FLAG_PLUS,
    FLAG_MINUS,
    /// @}

    /// @name windows icons
    /// @{
    WINDOWS_CASCADE,
    WINDOWS_TILE_VERT,
    WINDOWS_TILE_HORI,
    /// @}

    /// @name manipulation icons
    /// @{
    MANIP,
    CAMERA,
    /// @}

    /// @name graph icons
    /// @{
    EXTRACT,
    DILATE,
    ERODE,
    OPENING,
    CLOSING,
    CLOSE_GAPS,
    ERASE_STAINS,
    SKELETONIZE,
    RARIFY,
    CREATE_GRAPH,
    EYEDROP,
    PAINTBRUSH1X,
    PAINTBRUSH2X,
    PAINTBRUSH3X,
    PAINTBRUSH4X,
    PAINTBRUSH5X,
    RUBBER1X,
    RUBBER2X,
    RUBBER3X,
    RUBBER4X,
    RUBBER5X,
    EDITGRAPH,
    /// @}

    /// @name other tools
    /// @{
    EXT,
    CUT_SWELL,
    TRACKER,
    /// @}

    /// @name Netedit icons
    /// @{
    UNDO,
    REDO,
    UNDOLIST,
    LOCK,
    UNLOCK,
    LOCK_SELECTED,
    UNLOCK_SELECTED,
    ADD,
    REMOVE,
    BIGARROWLEFT,
    BIGARROWRIGHT,
    FRONTELEMENT,
    SIMPLIFYNETWORK,
    COMPUTEPATHMANAGER,
    COLLAPSE,
    UNCOLLAPSE,
    EXTEND,
    CURRENT,
    SEARCH,
    ALLOW,
    /// @}

    /// @name Netedit common mode specific icons
    /// @{
    COMMONMODE_CHECKBOX_TOGGLEGRID,
    COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE,
    COMMONMODE_CHECKBOX_SPREADVEHICLE,
    COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS,
    /// @}

    /// @name Netedit network mode specific icons
    /// @{
    NETWORKMODE_CHECKBOX_SELECTEDGES,
    NETWORKMODE_CHECKBOX_SHOWCONNECTIONS,
    NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS,
    NETWORKMODE_CHECKBOX_MERGEAUTOMATICALLY,
    NETWORKMODE_CHECKBOX_BUBBLES,
    NETWORKMODE_CHECKBOX_ELEVATION,
    NETWORKMODE_CHECKBOX_CHAIN,
    NETWORKMODE_CHECKBOX_TWOWAY,
    NETWORKMODE_CHECKBOX_HIDECONNECTIONS,
    NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS,
    NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS,
    NETWORKMODE_CHECKBOX_APPLYTOALLPHASES,
    /// @}

    /// @name Netedit demand mode specific icons
    /// @{
    DEMANDMODE_CHECKBOX_HIDESHAPES,
    DEMANDMODE_CHECKBOX_SHOWTRIPS,
    DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS,
    DEMANDMODE_CHECKBOX_SHOWPERSONPLANS,
    DEMANDMODE_CHECKBOX_LOCKPERSON,
    DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS,
    DEMANDMODE_CHECKBOX_LOCKCONTAINER,
    DEMANDMODE_CHECKBOX_SHOWOVERLAPPEDROUTES,
    /// @}

    /// @name Netedit data mode specific icons
    /// @{
    DATAMODE_CHECKBOX_SHOWADDITIONALS,
    DATAMODE_CHECKBOX_SHOWSHAPES,
    DATAMODE_CHECKBOX_TAZRELDRAWING,
    DATAMODE_CHECKBOX_TAZDRAWFILL,
    DATAMODE_CHECKBOX_TAZRELONLYFROM,
    DATAMODE_CHECKBOX_TAZRELONLYTO,
    /// @}

    /// @name arrows
    /// @{
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    /// @}

    /// @name lane icons
    /// @{
    LANE_PEDESTRIAN,
    LANE_BUS,
    LANE_BIKE,
    LANEGREENVERGE,
    /// @}

    /// @name netedit supermode icons
    /// @{
    SUPERMODENETWORK,
    SUPERMODEDEMAND,
    SUPERMODEDATA,
    /// @}

    /// @name Netedit Network modes icons
    /// @{
    MODEADDITIONAL,
    MODECONNECTION,
    MODECREATEEDGE,
    MODECROSSING,
    MODETAZ,
    MODEDELETE,
    MODEINSPECT,
    MODEMOVE,
    MODESELECT,
    MODETLS,
    MODESHAPE,
    MODEPROHIBITION,
    MODEWIRE,
    MODEDECAL,
    /// @}

    /// @name Netedit Demand modes icons
    /// @{
    MODEROUTE,
    MODEROUTEDISTRIBUTION,
    MODEVEHICLE,
    MODETYPE,
    MODETYPEDISTRIBUTION,
    MODESTOP,
    MODEPERSON,
    MODEPERSONPLAN,
    MODECONTAINER,
    MODECONTAINERPLAN,
    /// @}

    /// @name Netedit Edge modes icons
    /// @{
    MODEEDGEDATA,
    MODEEDGERELDATA,
    MODETAZRELDATA,
    MODEMEANDATA,
    /// @}

    /// @name Netedit processing icons
    /// @{
    COMPUTEJUNCTIONS,
    CLEANJUNCTIONS,
    JOINJUNCTIONS,
    COMPUTEDEMAND,
    CLEANROUTES,
    JOINROUTES,
    ADJUSTPERSONPLANS,
    OPTIONS,
    /// @}

    /// @name network elements icons
    /// @{
    JUNCTION,
    EDGETYPE,
    LANETYPE,
    EDGE,
    LANE,
    CONNECTION,
    PROHIBITION,
    CROSSING,
    WALKINGAREA,
    /// @}

    /// @name additional elements icons
    /// @{
    BUSSTOP,
    TRAINSTOP,
    ACCESS,
    CONTAINERSTOP,
    CHARGINGSTATION,
    E1,
    E2,
    E3,
    E3ENTRY,
    E3EXIT,
    E1INSTANT,
    REROUTER,
    ROUTEPROBE,
    VAPORIZER,
    VARIABLESPEEDSIGN,
    CALIBRATOR,
    PARKINGAREA,
    PARKINGSPACE,
    REROUTERINTERVAL,
    VSSSTEP,
    CLOSINGREROUTE,
    CLOSINGLANEREROUTE,
    DESTPROBREROUTE,
    PARKINGZONEREROUTE,
    ROUTEPROBREROUTE,
    /// @}

    /// @name poly elements icons
    /// @{
    TRACTION_SUBSTATION,
    OVERHEADWIRE,
    OVERHEADWIRE_CLAMP,
    /// @}

    /// @name shape elements icons
    /// @{
    POLY,
    POI,
    POILANE,
    POIGEO,
    /// @}

    /// @name poi icons
    /// @{
    POIICON_PIN,
    POIICON_NATURE,
    POIICON_HOTEL,
    POIICON_FUEL,
    POIICON_CHARGING_STATION,
    /// @}

    /// @name juPedSim elements
    /// @{
    JPS,
    JPS_WALKABLEAREA,
    JPS_OBSTACLE,
    /// @}

    /// @name TAZ elements icons
    /// @{
    TAZ,
    TAZEDGE,
    /// @}


    /// @name Netedit Demand elements icons
    /// @{
    ROUTE,
    ROUTEREF,
    ROUTEDISTRIBUTION,
    VTYPE,
    VTYPEREF,
    VTYPE_DEFAULT,
    VTYPE_BIKE,
    VTYPE_TAXI,
    VTYPE_RAIL,
    VTYPE_PEDESTRIAN,
    VTYPE_CONTAINER,
    VTYPEDISTRIBUTION,
    VEHICLE,
    TRIP,
    TRIP_JUNCTIONS,
    TRIP_TAZS,
    FLOW,
    FLOW_JUNCTIONS,
    FLOW_TAZS,
    ROUTEFLOW,
    STOPELEMENT,
    WAYPOINT,
    PERSON,
    PERSONFLOW,
    PERSONTRIP_EDGE,
    PERSONTRIP_BUSSTOP,
    PERSONTRIP_TRAINSTOP,
    PERSONTRIP_CONTAINERSTOP,
    PERSONTRIP_CHARGINGSTATION,
    PERSONTRIP_PARKINGAREA,
    PERSONTRIP_JUNCTION,
    PERSONTRIP_TAZ,
    WALK_EDGES,
    WALK_EDGE,
    WALK_BUSSTOP,
    WALK_TRAINSTOP,
    WALK_CONTAINERSTOP,
    WALK_CHARGINGSTATION,
    WALK_PARKINGAREA,
    WALK_ROUTE,
    WALK_JUNCTION,
    WALK_TAZ,
    RIDE_EDGE,
    RIDE_BUSSTOP,
    RIDE_TRAINSTOP,
    RIDE_CONTAINERSTOP,
    RIDE_CHARGINGSTATION,
    RIDE_PARKINGAREA,
    RIDE_JUNCTION,
    RIDE_TAZ,
    CONTAINER,
    CONTAINERFLOW,
    TRANSPORT_EDGE,
    TRANSPORT_BUSSTOP,
    TRANSPORT_TRAINSTOP,
    TRANSPORT_CONTAINERSTOP,
    TRANSPORT_CHARGINGSTATION,
    TRANSPORT_PARKINGAREA,
    TRANSPORT_JUNCTION,
    TRANSPORT_TAZ,
    TRANSHIP_EDGES,
    TRANSHIP_EDGE,
    TRANSHIP_BUSSTOP,
    TRANSHIP_TRAINSTOP,
    TRANSHIP_CONTAINERSTOP,
    TRANSHIP_CHARGINGSTATION,
    TRANSHIP_PARKINGAREA,
    TRANSHIP_JUNCTION,
    TRANSHIP_TAZ,
    /// @}

    /// @name Netedit data elements icons
    /// @{
    DATASET,
    DATAINTERVAL,
    EDGEDATA,
    EDGERELDATA,
    TAZRELDATA,
    MEANDATAEDGE,
    MEANDATALANE,
    /// @}

    /// @name Netedit icons used in GNETLSTable
    /// @{
    TLSPHASEDEFAULT,
    TLSPHASECOPY,
    TLSPHASEALLRED,
    TLSPHASEALLYELLOW,
    TLSPHASEALLGREEN,
    TLSPHASEALLGREENPRIORITY,
    /// @}

    /// @name vehicle Class icons (big, used in vType Dialog)
    /// @{
    VCLASS_IGNORING,
    VCLASS_PRIVATE,
    VCLASS_EMERGENCY,
    VCLASS_AUTHORITY,
    VCLASS_ARMY,
    VCLASS_VIP,
    VCLASS_PASSENGER,
    VCLASS_HOV,
    VCLASS_TAXI,
    VCLASS_BUS,
    VCLASS_COACH,
    VCLASS_DELIVERY,
    VCLASS_TRUCK,
    VCLASS_TRAILER,
    VCLASS_TRAM,
    VCLASS_RAIL_URBAN,
    VCLASS_RAIL,
    VCLASS_RAIL_ELECTRIC,
    VCLASS_RAIL_FAST,
    VCLASS_MOTORCYCLE,
    VCLASS_MOPED,
    VCLASS_BICYCLE,
    VCLASS_PEDESTRIAN,
    VCLASS_EVEHICLE,
    VCLASS_SHIP,
    VCLASS_CONTAINER,
    VCLASS_CABLE_CAR,
    VCLASS_SUBWAY,
    VCLASS_AIRCRAFT,
    VCLASS_WHEELCHAIR,
    VCLASS_SCOOTER,
    VCLASS_DRONE,
    VCLASS_CUSTOM1,
    VCLASS_CUSTOM2,
    /// @}

    /// @name small vehicle Class icons (used in comboBox)
    /// @{
    VCLASS_SMALL_IGNORING,
    VCLASS_SMALL_PRIVATE,
    VCLASS_SMALL_EMERGENCY,
    VCLASS_SMALL_AUTHORITY,
    VCLASS_SMALL_ARMY,
    VCLASS_SMALL_VIP,
    VCLASS_SMALL_PASSENGER,
    VCLASS_SMALL_HOV,
    VCLASS_SMALL_TAXI,
    VCLASS_SMALL_BUS,
    VCLASS_SMALL_COACH,
    VCLASS_SMALL_DELIVERY,
    VCLASS_SMALL_TRUCK,
    VCLASS_SMALL_TRAILER,
    VCLASS_SMALL_TRAM,
    VCLASS_SMALL_RAIL_URBAN,
    VCLASS_SMALL_RAIL,
    VCLASS_SMALL_RAIL_ELECTRIC,
    VCLASS_SMALL_RAIL_FAST,
    VCLASS_SMALL_MOTORCYCLE,
    VCLASS_SMALL_MOPED,
    VCLASS_SMALL_BICYCLE,
    VCLASS_SMALL_PEDESTRIAN,
    VCLASS_SMALL_EVEHICLE,
    VCLASS_SMALL_SHIP,
    VCLASS_SMALL_CONTAINER,
    VCLASS_SMALL_CABLE_CAR,
    VCLASS_SMALL_SUBWAY,
    VCLASS_SMALL_AIRCRAFT,
    VCLASS_SMALL_WHEELCHAIR,
    VCLASS_SMALL_SCOOTER,
    VCLASS_SMALL_DRONE,
    VCLASS_SMALL_CUSTOM1,
    VCLASS_SMALL_CUSTOM2,
    /// @}

    /// @name vehicle Shape icons
    /// @{
    VSHAPE_PEDESTRIAN,
    VSHAPE_BICYCLE,
    VSHAPE_MOPED,
    VSHAPE_MOTORCYCLE,
    VSHAPE_PASSENGER,
    VSHAPE_PASSENGER_SEDAN,
    VSHAPE_PASSENGER_HATCHBACK,
    VSHAPE_PASSENGER_WAGON,
    VSHAPE_PASSENGER_VAN,
    VSHAPE_TAXI,
    VSHAPE_DELIVERY,
    VSHAPE_TRUCK,
    VSHAPE_TRUCK_SEMITRAILER,
    VSHAPE_TRUCK_1TRAILER,
    VSHAPE_BUS,
    VSHAPE_BUS_COACH,
    VSHAPE_BUS_FLEXIBLE,
    VSHAPE_BUS_TROLLEY,
    VSHAPE_RAIL,
    VSHAPE_RAIL_CAR,
    VSHAPE_RAIL_CARGO,
    VSHAPE_E_VEHICLE,
    VSHAPE_ANT,
    VSHAPE_SHIP,
    VSHAPE_EMERGENCY,
    VSHAPE_FIREBRIGADE,
    VSHAPE_POLICE,
    VSHAPE_RICKSHAW,
    VSHAPE_SCOOTER,
    VSHAPE_UNKNOWN,
    VSHAPE_AIRCRAFT,
    /// @}

    /// @name icons for status
    /// @{
    OK,
    ACCEPT,
    CANCEL,
    CORRECT,
    INCORRECT,
    RESET,
    WARNING,
    DANGER,
    SUM,
    BACK,
    /// @}

    /// @name icons for dialogs
    /// @{
    INFORMATION_LARGE,
    INFORMATION_SMALL,
    QUESTION_LARGE,
    QUESTION_SMALL,
    ERROR_LARGE,
    ERROR_SMALL,
    WARNING_LARGE,
    WARNING_SMALL,
    /// @}

    /// @name icons for grid
    /// @{
    GRID,
    GRID1,
    GRID2,
    GRID3,
    /// @}

    /// @brief icons for geo tools
    /// @{
    GEOHACK,
    GOOGLEMAPS,
    OSM,
    /// @}

    /// @brief icons for file dialogs
    /// @{
    FILEDIALOG_BOOK_CLR,
    FILEDIALOG_BOOK_SET,
    FILEDIALOG_DIRUP_ICON,
    FILEDIALOG_FILE_COPY,
    FILEDIALOG_FILE_DELETE,
    FILEDIALOG_FILE_HIDDEN,
    FILEDIALOG_FILE_LINK,
    FILEDIALOG_FILE_MOVE,
    FILEDIALOG_FILE_SHOWN,
    FILEDIALOG_FOLDER_BIG,
    FILEDIALOG_FOLDER_NEW,
    FILEDIALOG_GOTO_HOME,
    FILEDIALOG_GOTO_WORK,
    FILEDIALOG_SHOW_BIGICONS,
    FILEDIALOG_SHOW_DETAILS,
    FILEDIALOG_SHOW_SMALLICONS,
    /// @}

    /// @brief icons for languages
    /// @{
    LANGUAGE_EN,
    LANGUAGE_DE,
    LANGUAGE_ES,
    LANGUAGE_PT,
    LANGUAGE_FR,
    LANGUAGE_IT,
    LANGUAGE_HU,
    LANGUAGE_TR,
    LANGUAGE_ZH,
    LANGUAGE_ZHT,
    LANGUAGE_JA
    /// @}
};
