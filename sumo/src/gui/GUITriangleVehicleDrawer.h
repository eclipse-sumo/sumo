#ifndef GUITriangleVehicleDrawer_h
#define GUITriangleVehicleDrawer_h
//---------------------------------------------------------------------------//
//                        GUITriangleVehicleDrawer.h -
//  A class for vehicle drawing; Vehicles are drawn as triangles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.2  2003/02/07 10:34:15  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUIViewTraffic.h"


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
class GUITriangleVehicleDrawer : public GUIViewTraffic::GUIVehicleDrawer {
public:
    /// constructor
    GUITriangleVehicleDrawer();

    /// destructor
    ~GUITriangleVehicleDrawer();

    /// initialises the drawing
    void initStep();

    /// draws a single vehicle; no tool-tip informations (faster)
    void drawVehicleNoTooltips(const GUILaneWrapper &lane,
        const GUIVehicle &veh, GUIViewTraffic::VehicleColoringScheme scheme);

    /// draws a single vehicle; tool-tip informations shall be generated
    void drawVehicleWithTooltips(const GUILaneWrapper &lane,
        const GUIVehicle &veh, GUIViewTraffic::VehicleColoringScheme scheme);

    /// ends the drawing
    void closeStep();

private:
    /// sets the colour of the vehicle to draw
    void setVehicleColor(const GUIVehicle &vehicle,
        GUIViewTraffic::VehicleColoringScheme scheme);

    void setVehicleColor1Of3(const GUIVehicle &vehicle);
    void setVehicleColor2Of3(const GUIVehicle &vehicle);
    void setVehicleColor3Of3(const GUIVehicle &vehicle);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUITriangleVehicleDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

