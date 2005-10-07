#ifndef GUIJunctionDrawer_wT_h
#define GUIJunctionDrawer_wT_h
//---------------------------------------------------------------------------//
//                        GUIJunctionDrawer_wT.cpp -
//  Class for drawing junctions with tooltip information
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.6  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 11:37:41  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.3  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include "GUIBaseJunctionDrawer.h"

class GUIJunctionWrapper;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIJunctionDrawer_wT :
    public GUIBaseJunctionDrawer
{
public:
    GUIJunctionDrawer_wT(std::vector<GUIJunctionWrapper*> &junctions);
    ~GUIJunctionDrawer_wT();
    void drawGLJunctions(size_t *which, size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme scheme);
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
