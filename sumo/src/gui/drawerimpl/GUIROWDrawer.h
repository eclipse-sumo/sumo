#ifndef GUIROWDrawer_h
#define GUIROWDrawer_h
//---------------------------------------------------------------------------//
//                        GUIROWDrawer.h -
//  Draws links (mainly their right-of-way)
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
// Revision 1.2  2006/12/21 13:23:54  dkrajzew
// added visualization of tls/junction link indices
//
// Revision 1.1  2006/12/12 12:10:42  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.11  2006/05/16 07:48:29  dkrajzew
// code beautifying
//
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
#include <utils/glutils/GLHelper.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIROWDrawer
 * @brief Draws links (mainly their right-of-way)
 *
 * This class goes through all lanes of all visible edges and draws their
 *  links. Right-Of-Way for each link is always drawn, additionally the
 *  visualisation settings are asked whether
 * a) the link decal (the arrows that display the driving direction)
 * b) the link's junction index
 * c) the link's tls index
 * d) connections between lanes
 * shall be drawn.
 *
 * All methods that realise the drawing of those features are externalised
 *  from the method that calls them and are only defined in the cpp-file
 *  (are not exported).
 */
class GUIROWDrawer {
public:
    /// constructor
    GUIROWDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIROWDrawer();

    /// draws the right-of-way rules
    void drawGLROWs(const GUINet &net, size_t *which, size_t maxEdges,
        SUMOReal width, GUISUMOAbstractView::VisualizationSettings &settings);

    /// Informs the drawer whether glids shall be set
    void setGLID(bool val);

    /// Definition of a storage for link colors
    typedef std::map<MSLink::LinkState, RGBColor> LinkColorMap;

private:
    /// Information whether the gl-id shall be set
    bool myShowToolTips;

protected:
    /// The colors to use for certain link types
    LinkColorMap myLinkColors;

    /// The list of edges to consider at drawing
    std::vector<GUIEdge*> &myEdges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

