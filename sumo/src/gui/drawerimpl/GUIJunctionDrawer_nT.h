#ifndef GUIJunctionDrawer_nT_h
#define GUIJunctionDrawer_nT_h
//---------------------------------------------------------------------------//
//                        GUIJunctionDrawer_nT.h -
//  Class for drawing junctions with no tooltip information
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
// Revision 1.4  2005/05/04 07:45:16  dkrajzew
// level 3 warnings removed
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
#include <vector>
#include "GUIBaseJunctionDrawer.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;


class GUIJunctionDrawer_nT :
    public GUIBaseJunctionDrawer
{
public:
    GUIJunctionDrawer_nT(std::vector<GUIJunctionWrapper*> &junctions);
    ~GUIJunctionDrawer_nT();
    void drawGLJunctions(size_t *which, size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme scheme);
};

#endif
