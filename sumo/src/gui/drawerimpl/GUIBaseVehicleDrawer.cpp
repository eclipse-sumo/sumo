//---------------------------------------------------------------------------//
//                        GUIBaseLaneDrawer.h -
//  Base class for vehicle drawing;
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
// Revision 1.4  2003/10/20 08:01:29  dkrajzew
// vehicles are not drawn on source lanes
//
// Revision 1.3  2003/10/15 11:31:24  dkrajzew
// display of needed lanechanging patched
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
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
#include "GUIBaseVehicleDrawer.h"

#include <qgl.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseVehicleDrawer::GUIBaseVehicleDrawer(std::vector<GUIEdge*> &edges)
    : GUIVehicleDrawer(edges)
{
}


GUIBaseVehicleDrawer::~GUIBaseVehicleDrawer()
{
}


void
GUIBaseVehicleDrawer::drawGLVehicles(size_t *onWhich, size_t maxEdges,
        GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    initStep();
    // go through edges
    for(size_t i=0; i<maxEdges; i++ ) {
        if(onWhich[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((onWhich[i]&pos)!=0) {
                GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                if(edge->isSource()) {
                    continue;
                }
                size_t noLanes = edge->nLanes();
                for(size_t i=0; i<noLanes; i++) {
                    // get the lane
                    GUILaneWrapper &laneGeom = edge->getLaneGeometry(i);
//                    MSLane &lane = edge->getLane(i);
                    drawLanesVehicles(laneGeom, scheme);
                }
            }
        }
    }
}


void
GUIBaseVehicleDrawer::initStep()
{
//    glLineWidth (0.1);
    glMatrixMode( GL_MODELVIEW );
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void
GUIBaseVehicleDrawer::setVehicleColor(const GUIVehicle &vehicle,
                                          GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    switch(scheme) {
    case GUISUMOAbstractView::VCS_BY_SPEED:
        {
            double speed = vehicle.speed();
            double maxSpeed = 30; // !!!
            double fact = speed / maxSpeed / 2.0;
            glColor3f(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    case GUISUMOAbstractView::VCS_SPECIFIED:
        {
            const RGBColor &col = vehicle.getDefinedColor();
            glColor3f(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM1:
        {
            const RGBColor &col = vehicle.getRandomColor1();
            glColor3f(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM2:
        {
            const RGBColor &col = vehicle.getRandomColor2();
            glColor3f(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE1:
        {
            float color = vehicle.getPassedColor() / (float) 255;
            glColor3f(color, color, color);
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE2:
        {
            const RGBColor &col = vehicle.getLaneChangeColor2();
            glColor3f(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_WAITING1:
        {
            float color = double(vehicle.getWaitingTime())
                / 512.0;
            if(color>1.0) {
                glColor3f(0, 0, 0);
            } else {
                color = 1.0 - color;
                glColor3f(color, color, color);
            }
        }
        break;
    default:
        throw 1;
    }
}


void
GUIBaseVehicleDrawer::setVehicleColor1Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NONE) {
        glColor3f(0, 0, 1);
        return;
    }
    // vehicles on false lanes will be red (others green)
    if((vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NEEDS_DIRECTION_CHANGE)) {
        glColor3f(1, 0, 0);
    }
}


void
GUIBaseVehicleDrawer::setVehicleColor2Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NONE) {
        glColor3f(0, 0, 1);
        return;
    }
    // left side of the vehicles; wants to go/is gone into this direction:
    //  vehicle side will be yellow, otherwise green/red in dependence
    // whether a lanechange is needed
    // vehicles on false lanes will be red (others green)
    if(vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NEEDS_DIRECTION_CHANGE) {
        if(vehicle.getLaneChangeDirection()==MSVehicle::LaneChangeState::LCDIR_RIGHT) {
            glColor3f(1, 1, 0);
        } else {
            glColor3f(1, 0, 0);
        }
    }
}


void
GUIBaseVehicleDrawer::setVehicleColor3Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NONE) {
        glColor3f(0, 0, 1);
        return;
    }
    // right side of the vehicles; wants to go/is gone into this direction:
    //  vehicle side will be yellow, otherwise green/red in dependence
    // whether a lanechange is needed
    // vehicles on false lanes will be red (others green)
    if(vehicle.getLaneChangeAction()==MSVehicle::LaneChangeState::LCACT_NEEDS_DIRECTION_CHANGE) {
        if(vehicle.getLaneChangeDirection()==MSVehicle::LaneChangeState::LCDIR_LEFT) {
            glColor3f(1, 1, 0);
        } else {
            glColor3f(1, 0, 0);
        }
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIBaseVehicleDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


