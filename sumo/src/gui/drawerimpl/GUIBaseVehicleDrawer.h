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
#include <gui/GUISUMOAbstractView.h>
#include "GUIColoringSchemesMap.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;
class GUIVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws vehicles as coloured triangles
 */
class GUIBaseVehicleDrawer {
public:
    /// constructor
    GUIBaseVehicleDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseVehicleDrawer();

    void drawGLVehicles(size_t *onWhich, size_t maxEdges,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

    static GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme> &
        getSchemesMap();

protected:
    /// initialises the drawing
    void initStep();

    virtual void drawLanesVehicles(GUILaneWrapper &lane,
        GUISUMOAbstractView::VehicleColoringScheme scheme) = 0;

    /// draws a single vehicle; no tool-tip informations (faster)
/*    virtual void drawVehicle(const GUILaneWrapper &lane,
        const GUIVehicle &veh,
        GUISUMOAbstractView::VehicleColoringScheme scheme) = 0;
*/
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
    /// The list of edges to consider at drawing
    std::vector<GUIEdge*> &myEdges;

    static GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme>
        myColoringSchemes;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

