/****************************************************************************/
/// @file    GUIROWDrawer.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id$
///
// Draws links (mainly their right-of-way)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIROWDrawer_h
#define GUIROWDrawer_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <utils/gfx/RGBColor.h>
#include <microsim/MSLink.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/glutils/GLHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUILaneWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
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
class GUIROWDrawer
{
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


#endif

/****************************************************************************/

