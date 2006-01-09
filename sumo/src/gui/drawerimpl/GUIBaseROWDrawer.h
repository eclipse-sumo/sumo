#ifndef GUIBaseROWDrawer_h
#define GUIBaseROWDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseROWDrawer.h -
//  Base class for drawing right of way - rules
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
// Revision 1.10  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.9  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/11/23 10:05:21  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.4  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/12 13:45:25  dkrajzew
// visualisation of tl-logics added
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

#include <map>
#include <utils/gfx/RGBColor.h>
#include <microsim/MSLink.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws lanes as simple, one-colored straights
 */
class GUIBaseROWDrawer {
public:
    /// constructor
    GUIBaseROWDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseROWDrawer();

    void drawGLROWs(const GUINet &net,
        size_t *which, size_t maxEdges, SUMOReal width, bool showLane2Lane,
        bool withArrows);

protected:
    void drawGLROWs_Only(const GUINet &net,
        size_t *which, size_t maxEdges, SUMOReal width,
        bool withArrows);

    void drawGLROWs_WithConnections(const GUINet &net,
        size_t *which, size_t maxEdges, SUMOReal width,
        bool withArrows);

private:
    /// initialises the drawing
    virtual void initStep();

    /// Function that realises the drawing of lik rules
    virtual void drawLinkRules(const GUINet &net,
        const GUILaneWrapper &lane) = 0;

    void initTexture(size_t no);

    virtual void drawArrows(const GUILaneWrapper &lane) = 0;

protected:
    /// Definition of a storage for link colors
    typedef std::map<MSLink::LinkState, RGBColor> LinkColorMap;

    /// The colors to use for certain link types
    LinkColorMap myLinkColors;

protected:
    /// The list of edges to consider at drawing
    std::vector<GUIEdge*> &myEdges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

