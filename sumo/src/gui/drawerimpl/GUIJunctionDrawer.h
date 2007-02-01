/****************************************************************************/
/// @file    GUIJunctionDrawer.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id: $
///
// Base class for drawing junctions
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
#ifndef GUIJunctionDrawer_h
#define GUIJunctionDrawer_h
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

#include <vector>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIJunctionWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
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
                                 GUISUMOAbstractView::JunctionColoringScheme scheme,
                                 GUISUMOAbstractView::VisualizationSettings &settings);

    /// Draws the junctions
    virtual void drawGLJunctionNames(size_t *which, size_t maxJunctions,
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


#endif

/****************************************************************************/

