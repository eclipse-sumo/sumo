//---------------------------------------------------------------------------//
//                        GUIIconSubSys.cpp -
//  Helper for icons loading and usage
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 08.03.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/01/27 14:33:26  dkrajzew
// code beautifying
//
// Revision 1.3  2004/12/12 17:23:59  agaubatz
// Editor Tool Widgets included
//
// Revision 1.2  2004/12/02 13:54:23  agaubatz
// Netedit update, A. Gaubatz
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:58:53  dksumo
// 'netedit' icons added
//
// Revision 1.1  2004/10/22 12:50:51  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/08/02 11:40:41  dkrajzew
// camera icon added; moved window icons to antother format
//
// Revision 1.4  2004/07/05 09:30:58  dkrajzew
// bug on unitialised icon usage patched
//
// Revision 1.3  2004/07/02 08:16:14  dkrajzew
// windows layouts added; reload button added
//
// Revision 1.2  2004/04/02 10:59:47  dkrajzew
// selection visualisation flags added
//
// Revision 1.1  2004/03/19 12:38:57  dkrajzew
// porting to FOX
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <fx.h>
#include <cassert>
#include "GUIIcons.h"
#include "GUIIconSubSys.h"

#include "filesave.xpm"
#include "fileopen.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "step.xpm"
#include "new_window.xpm"
#include "document.xpm"

#include "locate.xpm"
#include "locate_junction.xpm"
#include "locate_edge.xpm"
#include "locate_vehicle.xpm"
#include "locate_add.xpm"

#include "recenter_view.xpm"
#include "show_legend.xpm"
#include "allow_rotation.xpm"
#include "colour_lane.xpm"
#include "colour_vehicle.xpm"
#include "show_grid.xpm"
#include "show_tooltips.xpm"
#include "show_geometry.xpm"
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
#include "reload.cpp"
#include "manip.cpp"
#include "camera.xpm"
/*
#include "vlimit_20.cpp"
#include "vlimit_40.cpp"
#include "vlimit_60.cpp"
#include "vlimit_80.cpp"
#include "vlimit_100.cpp"
#include "vlimit_120.cpp"
#include "vlimit_140.cpp"
#include "vlimit_160.cpp"
#include "vlimit_180.cpp"
*/
//#include "vlimit_200.cpp"


/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
GUIIconSubSys *GUIIconSubSys::myInstance = 0;


/* =========================================================================
 * member definitions
 * ======================================================================= */
GUIIconSubSys::GUIIconSubSys(FXApp *a)
{
    // build icons
    myIcons[ICON_APP] = new FXXPMIcon(a, document_xpm);
    myIcons[ICON_OPEN] = new FXXPMIcon(a, fileopen_xpm);
    myIcons[ICON_RELOAD] = new FXGIFIcon(a, reload);
    myIcons[ICON_SAVE] = new FXXPMIcon(a, filesave_xpm);
    myIcons[ICON_CLOSE] = 0;//new FXXPMIcon(a, close);
    myIcons[ICON_START] = new FXXPMIcon(a, play_xpm);
    myIcons[ICON_STOP] = new FXXPMIcon(a, stop_xpm);
    myIcons[ICON_STEP] = new FXXPMIcon(a, step_xpm);
    myIcons[ICON_MICROVIEW] = new FXXPMIcon(a, new_window_xpm);
    myIcons[ICON_LAGGRVIEW] = new FXXPMIcon(a, new_window_xpm);
    myIcons[ICON_RECENTERVIEW] = new FXXPMIcon(a, recenter_view_xpm);
    myIcons[ICON_SHOWLEGEND] = new FXXPMIcon(a, show_legend_xpm);
    myIcons[ICON_ALLOWROTATION] = new FXXPMIcon(a, allow_rotation_xpm);

    myIcons[ICON_LOCATE] = new FXXPMIcon(a, locate_xpm);
    myIcons[ICON_LOCATEJUNCTION] = new FXXPMIcon(a, locate_junction_xpm);
    myIcons[ICON_LOCATEEDGE] = new FXXPMIcon(a, locate_edge_xpm);
    myIcons[ICON_LOCATEVEHICLE] = new FXXPMIcon(a, locate_vehicle_xpm);
    myIcons[ICON_LOCATEADD] = new FXXPMIcon(a, locate_add_xpm);

    myIcons[ICON_COLOURLANES] = new FXXPMIcon(a, colour_lane_xpm);
    myIcons[ICON_COLOURVEHICLES] = new FXXPMIcon(a, colour_vehicle_xpm);
    myIcons[ICON_SHOWGRID] = new FXXPMIcon(a, show_grid_xpm);
    myIcons[ICON_SHOWTOOLTIPS] = new FXXPMIcon(a, show_tooltips_xpm);
    myIcons[ICON_SHOWFULLGEOM] = new FXXPMIcon(a, show_geometry_xpm);
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
    myIcons[ICON_RUBBER1X]     = new FXXPMIcon(a, rubber1x_xpm);
    myIcons[ICON_RUBBER2X]     = new FXXPMIcon(a, rubber2x_xpm);
    myIcons[ICON_RUBBER3X]     = new FXXPMIcon(a, rubber3x_xpm);
    myIcons[ICON_RUBBER4X]     = new FXXPMIcon(a, rubber4x_xpm);
    myIcons[ICON_RUBBER5X]     = new FXXPMIcon(a, rubber5x_xpm);
    myIcons[ICON_EDITGRAPH]    = new FXXPMIcon(a, edit_graph_xpm);

    // vss
    myIcons[ICON_SPEEDLIMIT_020KMH] = 0;//new FXGIFIcon(a, vlimit_20);
    myIcons[ICON_SPEEDLIMIT_040KMH] = 0;//new FXGIFIcon(a, vlimit_40);
    myIcons[ICON_SPEEDLIMIT_060KMH] = 0;//new FXGIFIcon(a, vlimit_60);
    myIcons[ICON_SPEEDLIMIT_080KMH] = 0;//new FXGIFIcon(a, vlimit_80);
    myIcons[ICON_SPEEDLIMIT_100KMH] = 0;//new FXGIFIcon(a, vlimit_100);
    myIcons[ICON_SPEEDLIMIT_120KMH] = 0;//new FXGIFIcon(a, vlimit_120);
    myIcons[ICON_SPEEDLIMIT_140KMH] = 0;//new FXGIFIcon(a, vlimit_140);
    myIcons[ICON_SPEEDLIMIT_160KMH] = 0;//new FXGIFIcon(a, vlimit_160);
    myIcons[ICON_SPEEDLIMIT_180KMH] = 0;//new FXGIFIcon(a, vlimit_180);
//    myIcons[ICON_SPEEDLIMIT_200KMH] = new FXGIFIcon(a, vlimit_200);

    // ... and create them
    for(int i=0; i<ICON_MAX; i++) {
        if(myIcons[i]!=0) {
            myIcons[i]->create();
        }
    }
}


GUIIconSubSys::~GUIIconSubSys()
{
    for(int i=0; i<ICON_MAX; i++) {
        delete myIcons[i];
    }
}


void
GUIIconSubSys::init(FXApp *a)
{
    assert(myInstance==0);
    myInstance = new GUIIconSubSys(a);
}


FXIcon *
GUIIconSubSys::getIcon(GUIIcon which)
{
    return myInstance->myIcons[which];
}


void
GUIIconSubSys::close()
{
    delete myInstance;
    myInstance = 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
