#ifndef GUIBaseVehicleDrawer_h
#define GUIBaseVehicleDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseVehicleDrawer.h -
//  Base class for vehicle drawing
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
// Revision 1.5  2004/11/23 10:05:21  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.4  2004/08/02 11:30:54  dkrajzew
// refactored vehicle and lane coloring scheme usage to allow optional coloring schemes
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
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;
class GUIVehicle;
class GUIEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws vehicles as coloured triangles
 */
class GUIBaseVehicleDrawer {
public:
    /// constructor
    GUIBaseVehicleDrawer(const std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseVehicleDrawer();

    /// Draws the vehicles that are on the marked edges
    void drawGLVehicles(size_t *onWhich, size_t maxEdges,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

    /// Returns the list of available coloring schemes
    static GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme, GUIVehicle> &
        getSchemesMap();

    /// sets the colour of the vehicle to draw
    RGBColor getVehicleColor(const GUIVehicle &vehicle,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

    /// sets the colour of the vehicle to draw
    void setVehicleColor(const GUIVehicle &vehicle,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

    /// Sets the front color of the vehicle if the vehicle shall be draw in more than a single color
    void setVehicleColor1Of3(const GUIVehicle &vehicle);

    /// Sets the right color of the vehicle if the vehicle shall be draw in more than a single color
    void setVehicleColor2Of3(const GUIVehicle &vehicle);

    /// Sets the left color of the vehicle if the vehicle shall be draw in more than a single color
    void setVehicleColor3Of3(const GUIVehicle &vehicle);

protected:
    /// initialises the drawing
    void initStep();

    /// Draws all vehicles that are on the given lane
    virtual void drawLanesVehicles(GUILaneWrapper &lane,
        GUISUMOAbstractView::VehicleColoringScheme scheme) = 0;

protected:
    /// The list of edges to consider at drawing
    const std::vector<GUIEdge*> &myEdges;

    /** @brief The list of coloring schemes that may be used
        They are not fixed as they may change in dependence to the available parameter */
    static GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme, GUIVehicle>
        myColoringSchemes;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

