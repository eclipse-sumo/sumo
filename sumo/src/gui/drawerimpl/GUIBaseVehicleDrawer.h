#ifndef GUIBaseVehicleDrawer_h
#define GUIBaseVehicleDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseVehicleDrawer.h -
//  A class for vehicle drawing; Vehicles are drawn as triangles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <gui/GUISUMOAbstractView.h>


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
class GUIBaseVehicleDrawer :
    public GUISUMOAbstractView::GUIVehicleDrawer {
public:
    /// constructor
    GUIBaseVehicleDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseVehicleDrawer();

    void drawGLVehicles(size_t *onWhich, size_t maxEdges,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

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

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIBaseVehicleDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

