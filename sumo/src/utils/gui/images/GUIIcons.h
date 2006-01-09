#ifndef GUIIcons_h
#define GUIIcons_h
//---------------------------------------------------------------------------//
//                        GUIIcons.h -
//  Base class for additional objects (emitter, detectors etc.)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
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
// $Log$
// Revision 1.6  2006/01/09 13:37:38  dkrajzew
// further visualization options added
//
// Revision 1.5  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * enumerations
 * ======================================================================= */
enum GUIIcon {
    ICON_APP = 0,
    ICON_OPEN,
    ICON_RELOAD,
    ICON_SAVE,
    ICON_CLOSE,

    ICON_START,
    ICON_STOP,
    ICON_STEP,

    ICON_MICROVIEW,
    ICON_LAGGRVIEW,

    ICON_RECENTERVIEW,
    ICON_SHOWLEGEND,
    ICON_ALLOWROTATION,

    ICON_LOCATE,
    ICON_LOCATEJUNCTION,
    ICON_LOCATEEDGE,
    ICON_LOCATEVEHICLE,
    ICON_LOCATEADD,

    ICON_SHOWGRID,
    ICON_SHOWTOOLTIPS,
    ICON_SHOWFULLGEOM,
    ICON_EDITVIEWPORT,

    ICON_APP_TRACKER,
    ICON_APP_FINDER,
    ICON_APP_BREAKPOINTS,
    ICON_APP_TLSTRACKER,
    ICON_APP_TABLE,
    ICON_APP_SELECTOR,


    ICON_YES,
    ICON_NO,

    ICON_FLAG,
    ICON_FLAG_PLUS,
    ICON_FLAG_MINUS,

    ICON_WINDOWS_CASCADE,
    ICON_WINDOWS_TILE_VERT,
    ICON_WINDOWS_TILE_HORI,

    ICON_MANIP,
    ICON_CAMERA,

    ICON_SPEEDLIMIT_020KMH,
    ICON_SPEEDLIMIT_040KMH,
    ICON_SPEEDLIMIT_060KMH,
    ICON_SPEEDLIMIT_080KMH,
    ICON_SPEEDLIMIT_100KMH,
    ICON_SPEEDLIMIT_120KMH,
    ICON_SPEEDLIMIT_140KMH,
    ICON_SPEEDLIMIT_160KMH,
    ICON_SPEEDLIMIT_180KMH,

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

    ICON_EXT,
    ICON_CUT_SWELL,

    ICON_MAX

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
