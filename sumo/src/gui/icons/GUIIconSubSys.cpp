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
// Revision 1.3  2004/07/02 08:16:14  dkrajzew
// windows layouts added; reload button added
//
// Revision 1.2  2004/04/02 10:59:47  dkrajzew
// selection visualisation flags added
//
// Revision 1.1  2004/03/19 12:38:57  dkrajzew
// porting to FOX
//
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
#include "locate_junction.xpm"
#include "locate_edge.xpm"
#include "locate_vehicle.xpm"
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
#include "flag.cpp"
#include "flag_plus.cpp"
#include "flag_minus.cpp"
#include "windows_cascade.cpp"
#include "windows_tile_vertically.cpp"
#include "windows_tile_horizontally.cpp"
#include "reload.cpp"
#include "manip.cpp"
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
    myIcons[ICON_LOCATEJUNCTION] = new FXXPMIcon(a, locate_junction_xpm);
    myIcons[ICON_LOCATEEDGE] = new FXXPMIcon(a, locate_edge_xpm);
    myIcons[ICON_LOCATEVEHICLE] = new FXXPMIcon(a, locate_vehicle_xpm);
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
    myIcons[ICON_WINDOWS_CASCADE] = new FXGIFIcon(a, windows_cascade);
    myIcons[ICON_WINDOWS_TILE_VERT] = new FXGIFIcon(a, windows_tile_vertically);
    myIcons[ICON_WINDOWS_TILE_HORI] = new FXGIFIcon(a, windows_tile_horizontally);
    myIcons[ICON_MANIP] = new FXGIFIcon(a, manip);
/*
    myIcons[ICON_SPEEDLIMIT_020KMH] = new FXGIFIcon(a, vlimit_20);
    myIcons[ICON_SPEEDLIMIT_040KMH] = new FXGIFIcon(a, vlimit_40);
    myIcons[ICON_SPEEDLIMIT_060KMH] = new FXGIFIcon(a, vlimit_60);
    myIcons[ICON_SPEEDLIMIT_080KMH] = new FXGIFIcon(a, vlimit_80);
    myIcons[ICON_SPEEDLIMIT_100KMH] = new FXGIFIcon(a, vlimit_100);
    myIcons[ICON_SPEEDLIMIT_120KMH] = new FXGIFIcon(a, vlimit_120);
    myIcons[ICON_SPEEDLIMIT_140KMH] = new FXGIFIcon(a, vlimit_140);
    myIcons[ICON_SPEEDLIMIT_160KMH] = new FXGIFIcon(a, vlimit_160);
    myIcons[ICON_SPEEDLIMIT_180KMH] = new FXGIFIcon(a, vlimit_180);*/
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
