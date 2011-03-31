/****************************************************************************/
/// @file    GUIIconSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Helper for icons loading and usage
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "locate_tls.xpm"
#include "locate_add.xpm"
#include "locate_shape.xpm"

#include "colorwheel.xpm"
#include "savedb.xpm"
#include "removedb.xpm"
#include "recenter_view.xpm"
#include "allow_rotation.xpm"
#include "show_tooltips.xpm"
#include "magnify.xpm"
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

#include "cut.xpm"
#include "ext.xpm"


static const char *empty_xpm[] = {
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
GUIIconSubSys *GUIIconSubSys::myInstance = 0;


// ===========================================================================
// member definitions
// ===========================================================================
GUIIconSubSys::GUIIconSubSys(FXApp *a) {
    // build icons
    myIcons[ICON_APP] = new FXXPMIcon(a, document_xpm);
    myIcons[ICON_EMPTY] = new FXXPMIcon(a, empty_xpm);
    myIcons[ICON_OPEN_CONFIG] = new FXXPMIcon(a, fileopen_xpm);
    myIcons[ICON_OPEN_NET] = new FXXPMIcon(a, netopen_xpm);
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
    myIcons[ICON_LOCATETLS] = new FXXPMIcon(a, locate_tls_xpm);
    myIcons[ICON_LOCATEADD] = new FXXPMIcon(a, locate_add_xpm);
    myIcons[ICON_LOCATESHAPE] = new FXXPMIcon(a, locate_shape_xpm);

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
    myIcons[ICON_EDITGRAPH]    = new FXXPMIcon(a, edit_graph_xpm);

    myIcons[ICON_UNDO]    = new FXXPMIcon(a, undo_xpm);
    myIcons[ICON_REDO]    = new FXXPMIcon(a, redo_xpm);
    myIcons[ICON_NETEDIT] = new FXXPMIcon(a, netedit_xpm);

    myIcons[ICON_EXT]    = new FXXPMIcon(a, ext_xpm);
    myIcons[ICON_CUT_SWELL]    = new FXXPMIcon(a, cut_xpm);

    // ... and create them
    for (int i=0; i<ICON_MAX; i++) {
        if (myIcons[i]!=0) {
            myIcons[i]->create();
        }
    }
}


GUIIconSubSys::~GUIIconSubSys() {
    for (int i=0; i<ICON_MAX; i++) {
        delete myIcons[i];
    }
}


void
GUIIconSubSys::init(FXApp *a) {
    assert(myInstance==0);
    myInstance = new GUIIconSubSys(a);
}


FXIcon *
GUIIconSubSys::getIcon(GUIIcon which) {
    return myInstance->myIcons[which];
}


void
GUIIconSubSys::close() {
    delete myInstance;
    myInstance = 0;
}



/****************************************************************************/

