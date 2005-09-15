#ifndef GUIBaseJunctionDrawer_h
#define GUIBaseJunctionDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseJunctionDrawer.h -
//  Base class for drawing junctions
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
// Revision 1.6  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:05:21  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.3  2004/03/19 12:34:30  dkrajzew
// porting to FOX
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <utils/gui/windows/GUISUMOAbstractView.h>

class GUIJunctionWrapper;

class GUIBaseJunctionDrawer
{
public:
    GUIBaseJunctionDrawer(std::vector<GUIJunctionWrapper*> &junctions);
    virtual ~GUIBaseJunctionDrawer();
    virtual void drawGLJunctions(size_t *which, size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme scheme) = 0;

protected:
    /// The list of junctions to consider at drawing
    std::vector<GUIJunctionWrapper*> &myJunctions;

};

#endif
