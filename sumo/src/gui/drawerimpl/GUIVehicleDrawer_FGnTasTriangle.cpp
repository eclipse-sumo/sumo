//---------------------------------------------------------------------------//
//                        GUIVehicleDrawer_FGnTasTriangle.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
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

#include <guisim/GUIVehicle.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include <utils/geom/Position2DVector.h>
#include "GUIVehicleDrawer_FGnTasTriangle.h"

#include <qgl.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIVehicleDrawer_FGnTasTriangle::GUIVehicleDrawer_FGnTasTriangle(std::vector<GUIEdge*> &edges)
    : GUIBaseVehicleDrawer(edges)
{
}


GUIVehicleDrawer_FGnTasTriangle::~GUIVehicleDrawer_FGnTasTriangle()
{
}

void 
GUIVehicleDrawer_FGnTasTriangle::drawLanesVehicles(GUILaneWrapper &lane,
        GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    // retrieve vehicles from lane; disallow simulation
    const MSLane::VehCont &vehicles = lane.getVehiclesSecure();
    const DoubleVector &lengths = lane.getShapeLengths();
    const DoubleVector &rots = lane.getShapeRotations();
    const Position2DVector &geom = lane.getShape();
    const Position2D &laneBeg = geom.at(0);

    glPushMatrix();
    glTranslated(laneBeg.x(), laneBeg.y(), 0);
    glRotated(rots[0], 0, 0, 1);
    // go through the vehicles
    size_t shapePos = 0;
    double positionOffset = 0;
    for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
        MSVehicle *veh = *v;
        double vehiclePosition = veh->pos();
        while(shapePos<rots.size()-1 && vehiclePosition>positionOffset+lengths[shapePos]) {
            glPopMatrix();
            positionOffset += lengths[shapePos];
            shapePos++;
            glPushMatrix();
            glTranslated(geom.at(shapePos).x(), geom.at(shapePos).y(), 0);
            glRotated(rots[shapePos], 0, 0, 1);
        }
        glPushMatrix();
        glTranslated(0, -(vehiclePosition-positionOffset), 0);
        drawVehicle(static_cast<GUIVehicle&>(*veh), scheme);
        glPopMatrix();
    }
    // allow lane simulation
    lane.releaseVehicles();
    glPopMatrix();
}


void
GUIVehicleDrawer_FGnTasTriangle::drawVehicle(const GUIVehicle &vehicle,
            GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    glBegin( GL_TRIANGLES );
    if(scheme!=GUISUMOAbstractView::VCS_LANECHANGE3) {
        setVehicleColor(vehicle, scheme);
        glVertex2f(0, 0);
        glVertex2f(0-1.25, vehicle.length());
        glVertex2f(0+1.25, vehicle.length());
    } else {
        setVehicleColor1Of3(vehicle);
        glVertex2f(0, 0);
        setVehicleColor2Of3(vehicle);
        glVertex2f(0-1.25, vehicle.length());
        setVehicleColor3Of3(vehicle);
        glVertex2f(0+1.25, vehicle.length());
    }
    glEnd();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIVehicleDrawer_FGnTasTriangle.icc"
//#endif

// Local Variables:
// mode:C++
// End:


