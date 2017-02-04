/****************************************************************************/
/// @file    GUIIconSubSys.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Andreas Gaubatz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Helper for icons loading and usage
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <cassert>
#include "GUIIcons.h"
#include "GUIIconSubSys.h"

#include "dlr.xpm"

#include "empty.xpm"
#include "filesave.xpm"
#include "fileopen.xpm"
#include "netopen.xpm"
#include "shapeopen.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "step.xpm"
#include "new_window.xpm"
#include "document.xpm"

#include "app_tracker.xpm"
#include "app_finder.xpm"
#include "app_breakpoints.xpm"
#include "app_tlstracker.xpm"
#include "app_table.xpm"
#include "app_selector.xpm"

#include "locate.xpm"
#include "locate_junction.xpm"
#include "locate_edge.xpm"
#include "locate_vehicle.xpm"
#include "locate_person.xpm"
#include "locate_tls.xpm"
#include "locate_add.xpm"
#include "locate_poi.xpm"
#include "locate_poly.xpm"

#include "green_edge.xpm"
#include "yellow_edge.xpm"
#include "green_vehicle.xpm"
#include "yellow_vehicle.xpm"
#include "green_person.xpm"
#include "yellow_person.xpm"

#include "colorwheel.xpm"
#include "savedb.xpm"
#include "removedb.xpm"
#include "recenter_view.xpm"
#include "allow_rotation.xpm"
#include "show_tooltips.xpm"
#include "magnify.xpm"
#include "zoomstyle.xpm"
#include "yes.xpm"
#include "no.xpm"

#include "extract_streets.xpm"
#include "dilate.xpm"
#include "erode.xpm"
#include "opening.xpm"
#include "closing.xpm"
#include "erase_stains.xpm"
#include "close_gaps.xpm"
#include "skeletonize.xpm"
#include "rarify.xpm"
#include "create_graph.xpm"
#include "open_bmp_dialog.xpm"
#include "eyedrop.xpm"
#include "rubber1x.xpm"
#include "rubber2x.xpm"
#include "rubber3x.xpm"
#include "rubber4x.xpm"
#include "rubber5x.xpm"
#include "paintbrush1x.xpm"
#include "paintbrush2x.xpm"
#include "paintbrush3x.xpm"
#include "paintbrush4x.xpm"
#include "paintbrush5x.xpm"
#include "edit_graph.xpm"


#include "flag.cpp"
#include "flag_plus.cpp"
#include "flag_minus.cpp"
#include "windows_cascade.xpm"
#include "windows_tile_vertically.xpm"
#include "windows_tile_horizontally.xpm"
#include "reload.xpm"
#include "manip.cpp"
#include "camera.xpm"

#include "undo.xpm"
#include "redo.xpm"
#include "netedit.xpm"
#include "lock.xpm"
#include "add.xpm"
#include "remove.xpm"
#include "neteditarrow.xpm"

#include "lanebike.xpm"
#include "lanebus.xpm"
#include "lanepedestrian.xpm"

#include "cut.xpm"
#include "ext.xpm"

#include "modeadditional.xpm"
#include "modeconnection.xpm"
#include "modecreateedge.xpm"
#include "modecrossing.xpm"
#include "modedelete.xpm"
#include "modeinspect.xpm"
#include "modemove.xpm"
#include "modeselect.xpm"
#include "modetrafficlight.xpm"

#include "computejunctions.xpm"
#include "cleanjunctions.xpm"
#include "joinjunctions.xpm"
#include "options.xpm"

#include "junction.xpm"
#include "edge.xpm"
#include "lane.xpm"
#include "connection.xpm"
#include "prohibition.xpm"
#include "crossing.xpm"

#include "busstop.xpm"
#include "chargingstation.xpm"
#include "containerstop.xpm"
#include "e1.xpm"
#include "e2.xpm"
#include "e3.xpm"
#include "e3entry.xpm"
#include "e3exit.xpm"
#include "rerouter.xpm"
#include "routeprobe.xpm"
#include "vaporizer.xpm"
#include "variablespeedsign.xpm"
#include "calibrator.xpm"

#include "vclass_ignoring.xpm"
#include "vclass_private.xpm"
#include "vclass_emergency.xpm"
#include "vclass_authority.xpm"
#include "vclass_army.xpm"
#include "vclass_vip.xpm"
#include "vclass_passenger.xpm"
#include "vclass_hov.xpm"
#include "vclass_taxi.xpm"
#include "vclass_bus.xpm"
#include "vclass_coach.xpm"
#include "vclass_delivery.xpm"
#include "vclass_truck.xpm"
#include "vclass_trailer.xpm"
#include "vclass_tram.xpm"
#include "vclass_rail_urban.xpm"
#include "vclass_rail.xpm"
#include "vclass_rail_electric.xpm"
#include "vclass_motorcycle.xpm"
#include "vclass_moped.xpm"
#include "vclass_bicycle.xpm"
#include "vclass_pedestrian.xpm"
#include "vclass_evehicle.xpm"
#include "vclass_ship.xpm"
#include "vclass_custom1.xpm"
#include "vclass_custom2.xpm"

#include "accept.xpm"
#include "cancel.xpm"
#include "correct.xpm"
#include "error.xpm"
#include "reset.xpm"
#include "warning.xpm"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variable definitions
// ===========================================================================

GUIIconSubSys* GUIIconSubSys::myInstance = 0;

// ===========================================================================
// member definitions
// ===========================================================================

GUIIconSubSys::GUIIconSubSys(FXApp* a) {
    // build icons
    myIcons[ICON_APP] = new FXXPMIcon(a, document_xpm);
    myIcons[ICON_DLR] = new FXXPMIcon(a, dlr_xpm);
    myIcons[ICON_EMPTY] = new FXXPMIcon(a, empty_xpm);
    myIcons[ICON_OPEN_CONFIG] = new FXXPMIcon(a, fileopen_xpm);
    myIcons[ICON_OPEN_NET] = new FXXPMIcon(a, netopen_xpm);
    myIcons[ICON_OPEN_SHAPES] = new FXXPMIcon(a, shapeopen_xpm);
    myIcons[ICON_OPEN_ADDITIONALS] = new FXXPMIcon(a, shapeopen_xpm);
    myIcons[ICON_RELOAD] = new FXXPMIcon(a, reload_xpm);
    myIcons[ICON_SAVE] = new FXXPMIcon(a, filesave_xpm);
    myIcons[ICON_CLOSE] = 0;//new FXXPMIcon(a, close);
    myIcons[ICON_START] = new FXXPMIcon(a, play_xpm);
    myIcons[ICON_STOP] = new FXXPMIcon(a, stop_xpm);
    myIcons[ICON_STEP] = new FXXPMIcon(a, step_xpm);
    myIcons[ICON_MICROVIEW] = new FXXPMIcon(a, new_window_xpm);
    myIcons[ICON_LAGGRVIEW] = new FXXPMIcon(a, new_window_xpm);
    myIcons[ICON_RECENTERVIEW] = new FXXPMIcon(a, recenter_view_xpm);
    myIcons[ICON_ALLOWROTATION] = new FXXPMIcon(a, allow_rotation_xpm);
    myIcons[ICON_EDITVIEWPORT] = new FXXPMIcon(a, magnify_xpm);
    myIcons[ICON_ZOOMSTYLE] = new FXXPMIcon(a, zoomstyle_xpm);

    myIcons[ICON_APP_TRACKER] = new FXXPMIcon(a, app_tracker_xpm);
    myIcons[ICON_APP_FINDER] = new FXXPMIcon(a, app_finder_xpm);
    myIcons[ICON_APP_BREAKPOINTS] = new FXXPMIcon(a, app_breakpoints_xpm);
    myIcons[ICON_APP_TLSTRACKER] = new FXXPMIcon(a, app_tlstracker_xpm);
    myIcons[ICON_APP_TABLE] = new FXXPMIcon(a, app_table_xpm);
    myIcons[ICON_APP_SELECTOR] = new FXXPMIcon(a, app_selector_xpm);

    myIcons[ICON_LOCATE] = new FXXPMIcon(a, locate_xpm);
    myIcons[ICON_LOCATEJUNCTION] = new FXXPMIcon(a, locate_junction_xpm);
    myIcons[ICON_LOCATEEDGE] = new FXXPMIcon(a, locate_edge_xpm);
    myIcons[ICON_LOCATEVEHICLE] = new FXXPMIcon(a, locate_vehicle_xpm);
    myIcons[ICON_LOCATEPERSON] = new FXXPMIcon(a, locate_person_xpm);
    myIcons[ICON_LOCATETLS] = new FXXPMIcon(a, locate_tls_xpm);
    myIcons[ICON_LOCATEADD] = new FXXPMIcon(a, locate_add_xpm);
    myIcons[ICON_LOCATEPOI] = new FXXPMIcon(a, locate_poi_xpm);
    myIcons[ICON_LOCATEPOLY] = new FXXPMIcon(a, locate_poly_xpm);

    myIcons[ICON_GREENEDGE] = new FXXPMIcon(a, green_edge_xpm);
    myIcons[ICON_GREENVEHICLE] = new FXXPMIcon(a, green_vehicle_xpm);
    myIcons[ICON_GREENPERSON] = new FXXPMIcon(a, green_person_xpm);
    myIcons[ICON_YELLOWEDGE] = new FXXPMIcon(a, yellow_edge_xpm);
    myIcons[ICON_YELLOWVEHICLE] = new FXXPMIcon(a, yellow_vehicle_xpm);
    myIcons[ICON_YELLOWPERSON] = new FXXPMIcon(a, yellow_person_xpm);

    myIcons[ICON_COLORWHEEL] = new FXXPMIcon(a, colorwheel_xpm);
    myIcons[ICON_SAVEDB] = new FXXPMIcon(a, savedb_xpm);
    myIcons[ICON_REMOVEDB] = new FXXPMIcon(a, removedb_xpm);
    myIcons[ICON_SHOWTOOLTIPS] = new FXXPMIcon(a, show_tooltips_xpm);
    myIcons[ICON_YES] = new FXXPMIcon(a, yes_xpm);
    myIcons[ICON_NO] = new FXXPMIcon(a, no_xpm);
    myIcons[ICON_FLAG] = new FXGIFIcon(a, flag);
    myIcons[ICON_FLAG_PLUS] = new FXGIFIcon(a, flag_plus);
    myIcons[ICON_FLAG_MINUS] = new FXGIFIcon(a, flag_minus);
    // window arrangements
    myIcons[ICON_WINDOWS_CASCADE] = new FXXPMIcon(a, windows_cascade_xpm);
    myIcons[ICON_WINDOWS_TILE_VERT] = new FXXPMIcon(a, windows_tile_vertically_xpm);
    myIcons[ICON_WINDOWS_TILE_HORI] = new FXXPMIcon(a, windows_tile_horizontally_xpm);
    // manipulate
    myIcons[ICON_MANIP] = new FXGIFIcon(a, manip);
    myIcons[ICON_CAMERA] = new FXXPMIcon(a, camera_xpm);
    myIcons[ICON_EXTRACT] = new FXXPMIcon(a, extract_streets_xpm);
    myIcons[ICON_DILATE] = new FXXPMIcon(a, dilate_xpm);
    myIcons[ICON_ERODE] = new FXXPMIcon(a, erode_xpm);
    myIcons[ICON_OPENING] = new FXXPMIcon(a, opening_xpm);
    myIcons[ICON_CLOSING] = new FXXPMIcon(a, closing_xpm);
    myIcons[ICON_ERASE_STAINS] = new FXXPMIcon(a, erase_stains_xpm);
    myIcons[ICON_CLOSE_GAPS] = new FXXPMIcon(a, close_gaps_xpm);
    myIcons[ICON_SKELETONIZE] = new FXXPMIcon(a, skeletonize_xpm);
    myIcons[ICON_RARIFY] = new FXXPMIcon(a, rarify_xpm);
    myIcons[ICON_CREATE_GRAPH] = new FXXPMIcon(a, create_graph_xpm);
    myIcons[ICON_OPEN_BMP_DIALOG] = new FXXPMIcon(a, open_bmp_dialog_xpm);
    myIcons[ICON_EYEDROP] = new FXXPMIcon(a, eyedrop_xpm);
    myIcons[ICON_PAINTBRUSH1X] = new FXXPMIcon(a, paintbrush1x_xpm);
    myIcons[ICON_PAINTBRUSH2X] = new FXXPMIcon(a, paintbrush2x_xpm);
    myIcons[ICON_PAINTBRUSH3X] = new FXXPMIcon(a, paintbrush3x_xpm);
    myIcons[ICON_PAINTBRUSH4X] = new FXXPMIcon(a, paintbrush4x_xpm);
    myIcons[ICON_PAINTBRUSH5X] = new FXXPMIcon(a, paintbrush5x_xpm);
    myIcons[ICON_RUBBER1X] = new FXXPMIcon(a, rubber1x_xpm);
    myIcons[ICON_RUBBER2X] = new FXXPMIcon(a, rubber2x_xpm);
    myIcons[ICON_RUBBER3X] = new FXXPMIcon(a, rubber3x_xpm);
    myIcons[ICON_RUBBER4X] = new FXXPMIcon(a, rubber4x_xpm);
    myIcons[ICON_RUBBER5X] = new FXXPMIcon(a, rubber5x_xpm);
    myIcons[ICON_EDITGRAPH] = new FXXPMIcon(a, edit_graph_xpm);

    myIcons[ICON_UNDO] = new FXXPMIcon(a, undo_xpm);
    myIcons[ICON_REDO] = new FXXPMIcon(a, redo_xpm);
    myIcons[ICON_NETEDIT] = new FXXPMIcon(a, netedit_xpm);
    myIcons[ICON_LOCK] = new FXXPMIcon(a, lock_xpm);
    myIcons[ICON_ADD] = new FXXPMIcon(a, add_xpm);
    myIcons[ICON_REMOVE] = new FXXPMIcon(a, remove_xpm);
    myIcons[ICON_NETEDITARROW] = new FXXPMIcon(a, netEditArrow_xmp);

    myIcons[ICON_LANEPEDESTRIAN] = new FXXPMIcon(a, lanepedestrian);
    myIcons[ICON_LANEBUS] = new FXXPMIcon(a, lanebus);
    myIcons[ICON_LANEBIKE] = new FXXPMIcon(a, lanebike);

    myIcons[ICON_EXT] = new FXXPMIcon(a, ext_xpm);
    myIcons[ICON_CUT_SWELL] = new FXXPMIcon(a, cut_xpm);

    myIcons[ICON_MODEADDITIONAL] = new FXXPMIcon(a, modeadditional_xpm);
    myIcons[ICON_MODECONNECTION] = new FXXPMIcon(a, modeconnection_xpm);
    myIcons[ICON_MODECREATEEDGE] = new FXXPMIcon(a, modecreateedge_xpm);
    myIcons[ICON_MODECROSSING] = new FXXPMIcon(a, modecrossing_xpm);
    myIcons[ICON_MODEDELETE] = new FXXPMIcon(a, modedelete_xpm);
    myIcons[ICON_MODEINSPECT] = new FXXPMIcon(a, modeinspect_xpm);
    myIcons[ICON_MODEMOVE] = new FXXPMIcon(a, modemove_xpm);
    myIcons[ICON_MODESELECT] = new FXXPMIcon(a, modeselect_xpm);
    myIcons[ICON_MODETLS] = new FXXPMIcon(a, modetrafficlight_xpm);

    myIcons[ICON_CLEANJUNCTIONS] = new FXXPMIcon(a, cleanjunctions_xpm);
    myIcons[ICON_COMPUTEJUNCTIONS] = new FXXPMIcon(a, computejunctions_xpm);
    myIcons[ICON_JOINJUNCTIONS] = new FXXPMIcon(a, joinjunctions_xpm);
    myIcons[ICON_OPTIONS] = new FXXPMIcon(a, options_xpm);

    myIcons[ICON_JUNCTION] = new FXXPMIcon(a, junction_xpm);
    myIcons[ICON_EDGE] = new FXXPMIcon(a, edge_xpm);
    myIcons[ICON_LANE] = new FXXPMIcon(a, lane_xpm);
    myIcons[ICON_CONNECTION] = new FXXPMIcon(a, connection_xpm);
    myIcons[ICON_PROHIBITION] = new FXXPMIcon(a, prohibition_xpm);
    myIcons[ICON_CROSSING] = new FXXPMIcon(a, crossing_xpm);

    myIcons[ICON_BUSSTOP] = new FXXPMIcon(a, busstop_xpm);
    myIcons[ICON_CONTAINERSTOP] = new FXXPMIcon(a, containerstop_xpm);
    myIcons[ICON_CHARGINGSTATION] = new FXXPMIcon(a, chargingstation_xpm);
    myIcons[ICON_E1] = new FXXPMIcon(a, e1_xpm);
    myIcons[ICON_E2] = new FXXPMIcon(a, e2_xpm);
    myIcons[ICON_E3] = new FXXPMIcon(a, e3_xpm);
    myIcons[ICON_E3ENTRY] = new FXXPMIcon(a, e3entry_xpm);
    myIcons[ICON_E3EXIT] = new FXXPMIcon(a, e3exit_xpm);
    myIcons[ICON_REROUTER] = new FXXPMIcon(a, rerouter_xpm);
    myIcons[ICON_ROUTEPROBE] = new FXXPMIcon(a, routeprobe_xpm);
    myIcons[ICON_VAPORIZER] = new FXXPMIcon(a, vaporizer_xpm);
    myIcons[ICON_VARIABLESPEEDSIGN] = new FXXPMIcon(a, variablespeedsign_xpm);
    myIcons[ICON_CALIBRATOR] = new FXXPMIcon(a, calibrator_xpm);

    myIcons[ICON_VCLASS_IGNORING] = new FXXPMIcon(a, vclass_ignoring_xpm);
    myIcons[ICON_VCLASS_PRIVATE] = new FXXPMIcon(a, vclass_private_xpm);
    myIcons[ICON_VCLASS_EMERGENCY] = new FXXPMIcon(a, vclass_emergency_xpm);
    myIcons[ICON_VCLASS_AUTHORITY] = new FXXPMIcon(a, vclass_authority_xpm);
    myIcons[ICON_VCLASS_ARMY] = new FXXPMIcon(a, vclass_army_xpm);
    myIcons[ICON_VCLASS_VIP] = new FXXPMIcon(a, vclass_vip_xpm);
    myIcons[ICON_VCLASS_PASSENGER] = new FXXPMIcon(a, vclass_passenger_xpm);
    myIcons[ICON_VCLASS_HOV] = new FXXPMIcon(a, vclass_hov_xpm);
    myIcons[ICON_VCLASS_TAXI] = new FXXPMIcon(a, vclass_taxi_xpm);
    myIcons[ICON_VCLASS_BUS] = new FXXPMIcon(a, vclass_bus_xpm);
    myIcons[ICON_VCLASS_COACH] = new FXXPMIcon(a, vclass_coach_xpm);
    myIcons[ICON_VCLASS_DELIVERY] = new FXXPMIcon(a, vclass_delivery_xpm);
    myIcons[ICON_VCLASS_TRUCK] = new FXXPMIcon(a, vclass_truck_xpm);
    myIcons[ICON_VCLASS_TRAILER] = new FXXPMIcon(a, vclass_trailer_xpm);
    myIcons[ICON_VCLASS_TRAM] = new FXXPMIcon(a, vclass_tram_xpm);
    myIcons[ICON_VCLASS_RAIL_URBAN] = new FXXPMIcon(a, vclass_rail_urban_xpm);
    myIcons[ICON_VCLASS_RAIL] = new FXXPMIcon(a, vclass_rail_xpm);
    myIcons[ICON_VCLASS_RAIL_ELECTRIC] = new FXXPMIcon(a, vclass_rail_electric_xpm);
    myIcons[ICON_VCLASS_MOTORCYCLE] = new FXXPMIcon(a, vclass_motorcycle_xpm);
    myIcons[ICON_VCLASS_MOPED] = new FXXPMIcon(a, vclass_moped_xpm);
    myIcons[ICON_VCLASS_BICYCLE] = new FXXPMIcon(a, vclass_bicycle_xpm);
    myIcons[ICON_VCLASS_PEDESTRIAN] = new FXXPMIcon(a, vclass_pedestrian_xpm);
    myIcons[ICON_VCLASS_EVEHICLE] = new FXXPMIcon(a, vclass_evehicle_xpm);
    myIcons[ICON_VCLASS_SHIP] = new FXXPMIcon(a, vclass_ship_xpm);
    myIcons[ICON_VCLASS_CUSTOM1] = new FXXPMIcon(a, vclass_custom1_xpm);
    myIcons[ICON_VCLASS_CUSTOM2] = new FXXPMIcon(a, vclass_custom2_xpm);

    myIcons[ICON_ACCEPT] = new FXXPMIcon(a, accept_xpm);
    myIcons[ICON_CANCEL] = new FXXPMIcon(a, cancel_xpm);
    myIcons[ICON_CORRECT] = new FXXPMIcon(a, correct_xpm);
    myIcons[ICON_ERROR] = new FXXPMIcon(a, error_xpm);
    myIcons[ICON_RESET] = new FXXPMIcon(a, reset_xpm);
    myIcons[ICON_WARNING] = new FXXPMIcon(a, warning_xpm);

    // ... and create them
    for (int i = 0; i < ICON_MAX; i++) {
        if (myIcons[i] != 0) {
            myIcons[i]->create();
        }
    }
}


GUIIconSubSys::~GUIIconSubSys() {
    for (int i = 0; i < ICON_MAX; i++) {
        delete myIcons[i];
    }
}


void
GUIIconSubSys::initIcons(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUIIconSubSys(a);
}


FXIcon*
GUIIconSubSys::getIcon(GUIIcon which) {
    return myInstance->myIcons[which];
}


void
GUIIconSubSys::close() {
    delete myInstance;
    myInstance = 0;
}


/****************************************************************************/

