#ifndef GUIJunctionDrawer_h
#define GUIJunctionDrawer_h
//---------------------------------------------------------------------------//
//                        GUIJunctionDrawer.h -
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
// Revision 1.2  2007/01/09 11:12:01  dkrajzew
// the names of nodes, additional structures, vehicles, edges, pois may now be shown
//
// Revision 1.1  2006/12/12 12:10:40  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.7  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <utils/gui/windows/GUISUMOAbstractView.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIJunctionDrawer
 * @brief Base class for drawing junctions
 */
class GUIJunctionDrawer
{
public:
    /// Constructor
    GUIJunctionDrawer(std::vector<GUIJunctionWrapper*> &junctions);

    /// Destructor
    virtual ~GUIJunctionDrawer();

    /// Draws the junctions
    virtual void drawGLJunctions(size_t *which, size_t maxJunctions,
        SUMOReal scale,
        GUISUMOAbstractView::JunctionColoringScheme scheme,
        GUISUMOAbstractView::VisualizationSettings &settings);

    /// sets the information whether the gl-id shall be set
    void setGLID(bool val);

protected:
    /// The list of junctions to consider at drawing
    std::vector<GUIJunctionWrapper*> &myJunctions;

    /// Information whether the gl-id shall be set
    bool myShowToolTips;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
