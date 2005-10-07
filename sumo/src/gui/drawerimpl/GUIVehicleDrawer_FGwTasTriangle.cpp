//---------------------------------------------------------------------------//
//                        GUIVehicleDrawer_FGwTasTriangle.h -
//  Class for drawing vehicles on full geom with tooltip info as triangles
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:29  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/07/12 11:58:17  dkrajzew
// visualisation of large vehicles added
//
// Revision 1.5  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:05:22  dkrajzew
// removed some warnings and adapted the new class hierarchy
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

#include <guisim/GUIVehicle.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include <utils/geom/Position2DVector.h>
#include "GUIVehicleDrawer_FGwTasTriangle.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIVehicleDrawer_FGwTasTriangle::GUIVehicleDrawer_FGwTasTriangle(
        std::vector<GUIEdge*> &edges)
    : GUIBaseVehicleDrawer(edges)
{
}


GUIVehicleDrawer_FGwTasTriangle::~GUIVehicleDrawer_FGwTasTriangle()
{
}


void
GUIVehicleDrawer_FGwTasTriangle::drawLanesVehicles(GUILaneWrapper &lane,
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
    SUMOReal positionOffset = 0;
    for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
        MSVehicle *veh = *v;
        SUMOReal vehiclePosition = veh->pos();
        while(  shapePos<rots.size()-1
                &&
                vehiclePosition>positionOffset+lengths[shapePos]) {

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
GUIVehicleDrawer_FGwTasTriangle::drawVehicle(const GUIVehicle &vehicle,
            GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    glPushName(vehicle.getGlID());
    glBegin( GL_TRIANGLES );
    SUMOReal length = vehicle.length();
    if(length<8) {
        if(scheme!=GUISUMOAbstractView::VCS_LANECHANGE3) {
            setVehicleColor(vehicle, scheme);
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 0+length);
            glVertex2d(0+1.25, 0+length);
        } else {
            setVehicleColor1Of3(vehicle);
            glVertex2d(0, 0);
            setVehicleColor2Of3(vehicle);
            glVertex2d(0-1.25, 0+length);
            setVehicleColor3Of3(vehicle);
            glVertex2d(0+1.25, 0+length);
        }
    } else {
        if(scheme!=GUISUMOAbstractView::VCS_LANECHANGE3) {
            setVehicleColor(vehicle, scheme);
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 0+2);
            glVertex2d(0+1.25, 0+2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0-1.25, length-2);
            glVertex2d(0+1.25, length-2);
            glVertex2d(0+1.25, 2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0+1.25, length-2);
        } else {
            setVehicleColor1Of3(vehicle);
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 0+2);
            glVertex2d(0+1.25, 0+2);
            setVehicleColor2Of3(vehicle);
            glVertex2d(0-1.25, 2);
            glVertex2d(0-1.25, length-2);
            glVertex2d(0+1.25, length-2);
            setVehicleColor3Of3(vehicle);
            glVertex2d(0+1.25, 2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0+1.25, length-2);
        }
    }
    glEnd();
    glPopName();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


