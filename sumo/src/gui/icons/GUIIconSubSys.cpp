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
#include "flag.xpm"
#include "flag_plus.xpm"
#include "flag_minus.xpm"


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
    myIcons[ICON_FLAG] = new FXXPMIcon(a, flag_xpm);
    myIcons[ICON_FLAG_PLUS] = new FXXPMIcon(a, flag_plus_xpm);
    myIcons[ICON_FLAG_MINUS] = new FXXPMIcon(a, flag_minus_xpm);
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
