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
#include "variablespeedsignal.xpm"
#include "calibrator.xpm"


static const char* empty_xpm[] = {
    "    1    1        1            1",
    ". c None",
    "."
};


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
    myIcons[ICON_VARIABLESPEEDSIGNAL] = new FXXPMIcon(a, variablespeedsignal_xpm);
    myIcons[ICON_CALIBRATOR] = new FXXPMIcon(a, calibrator_xpm);

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

