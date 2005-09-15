#ifndef GUIVehicleDrawer_FGnTasTriangle_h
#define GUIVehicleDrawer_FGnTasTriangle_h
//---------------------------------------------------------------------------//
//                        GUIVehicleDrawer_FGnTasTriangle.h -
//  Class for drawing vehicles on full geometry with no tooltips as triangles
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
// Revision 1.5  2005/09/15 11:05:29  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/29 11:10:58  dkrajzew
// codestyle adapted
//
// Revision 1.3  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include "GUIBaseVehicleDrawer.h"


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
class GUIVehicleDrawer_FGnTasTriangle :
    public GUIBaseVehicleDrawer {
public:
    /// constructor
    GUIVehicleDrawer_FGnTasTriangle(std::vector<GUIEdge*> &edges);

    /// destructor
    ~GUIVehicleDrawer_FGnTasTriangle();

private:
    void drawLanesVehicles(GUILaneWrapper &lane,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

    void drawVehicle(const GUIVehicle &vehicle,
        GUISUMOAbstractView::VehicleColoringScheme scheme);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

