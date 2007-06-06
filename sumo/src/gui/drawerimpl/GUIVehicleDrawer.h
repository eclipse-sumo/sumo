/****************************************************************************/
/// @file    GUIVehicleDrawer.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id$
///
// Base class for vehicle drawing
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
#ifndef GUIVehicleDrawer_h
#define GUIVehicleDrawer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/glutils/GLHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUILaneWrapper;
class GUIVehicle;
class GUIEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Draws vehicles as coloured triangles
 */
class GUIVehicleDrawer
{
public:
    /// constructor
    GUIVehicleDrawer(const std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIVehicleDrawer();

    /// Draws the vehicles that are on the marked edges
    void drawGLVehicles(size_t *onWhich, size_t maxEdges,
                        SUMOReal scale,
                        const GUIColoringSchemesMap<GUIVehicle> &schemes,
                        GUISUMOAbstractView::VisualizationSettings &settings/*,
                                                                                GUIBaseColorer<GUIVehicle> &colorer, float upscale*/);

    void setGLID(bool val);

protected:
    /// initialises the drawing
    void initStep();

    /// Draws all vehicles that are on the given lane
    virtual void drawLanesVehicles(GUILaneWrapper &lane,
                                   SUMOReal scale,
                                   const GUIColoringSchemesMap<GUIVehicle> &schemes,
                                   const GUISUMOAbstractView::VisualizationSettings &settings);

protected:
    /// The list of edges to consider at drawing
    const std::vector<GUIEdge*> &myEdges;

    /// Information whether the gl-id shall be set
    bool myShowToolTips;

};


#endif

/****************************************************************************/

