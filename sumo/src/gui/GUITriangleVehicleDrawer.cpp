//---------------------------------------------------------------------------//
//                        GUITriangleVehicleDrawer.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.10  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.9  2003/06/06 10:32:20  dkrajzew
// got rid of glut
//
// Revision 1.8  2003/04/07 10:15:14  dkrajzew
// glut reinserted
//
// Revision 1.7  2003/04/04 15:12:05  roessel
// Commented out #include <glut.h>
//
// Revision 1.6  2003/04/02 11:50:27  dkrajzew
// a working tool tip implemented
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/02/07 10:34:14  dkrajzew
// files updated
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
#include "GUIViewTraffic.h"
#include "GUITriangleVehicleDrawer.h"

#include <qgl.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUITriangleVehicleDrawer::GUITriangleVehicleDrawer(std::vector<GUIEdge*> &edges)
    : GUIVehicleDrawer(edges)
{
}


GUITriangleVehicleDrawer::~GUITriangleVehicleDrawer()
{
}


void
GUITriangleVehicleDrawer::drawGLVehicles(size_t *onWhich, size_t maxEdges,
                                         bool showToolTips,
                                         GUIViewTraffic::VehicleColoringScheme scheme)
{
    initStep();
    // draw the vehicles
//    if(showToolTips) {
        // go through edges
        for(size_t i=0; i<maxEdges; i++ ) {
            if(onWhich[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((onWhich[i]&pos)!=0) {
                    GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                    size_t noLanes = edge->nLanes();
                    for(size_t i=0; i<noLanes; i++) {
                        // get the lane
                        GUILaneWrapper &laneGeom = edge->getLaneGeometry(i);
                        MSLane &lane = edge->getLane(i);
                        // retrieve vehicles from lane; disallow simulation
                        const MSLane::VehCont &vehicles = lane.getVehiclesSecure();
                        /// check whether tool-tip informations shall be generated
                        if(showToolTips) {
                            // go through the vehicles
                            for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
                                MSVehicle *veh = *v;
                                drawVehicleWithTooltips(laneGeom,
                                    static_cast<GUIVehicle&>(*veh), scheme);
                            }
                        } else {
                            // go through the vehicles
                            for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
                                MSVehicle *veh = *v;
                                drawVehicleNoTooltips(laneGeom,
                                    static_cast<GUIVehicle&>(*veh), scheme);
                            }
                        }
                        // allow lane simulation
                        lane.releaseVehicles();
                    }
                }
            }
        }
//    }
}


void
GUITriangleVehicleDrawer::initStep()
{
    glLineWidth (0.1);
    glMatrixMode( GL_MODELVIEW );
}


void
GUITriangleVehicleDrawer::drawVehicleNoTooltips(const GUILaneWrapper &lane,
            const GUIVehicle &vehicle,
            GUIViewTraffic::VehicleColoringScheme scheme)
{
    const Position2D &laneEnd = lane.getBegin();
    const Position2D &laneDir = lane.getDirection();
    double posX = laneEnd.x() - laneDir.x() * vehicle.pos();
    double posY = laneEnd.y() - laneDir.y() * vehicle.pos();
    glTranslated(posX, posY, 0);
    glRotated(lane.getRotation(), 0, 0, 1);
    glBegin( GL_TRIANGLES );
    if(scheme!=GUIViewTraffic::VCS_LANECHANGE3) {
        setVehicleColor(vehicle, scheme);
        glVertex2f(0, 0);
        glVertex2f(0-1.25, 0+vehicle.length());
        glVertex2f(0+1.25, 0+vehicle.length());
    } else {
        setVehicleColor1Of3(vehicle);
        glVertex2f(0, 0);
        setVehicleColor2Of3(vehicle);
        glVertex2f(0-1.25, 0+vehicle.length());
        setVehicleColor3Of3(vehicle);
        glVertex2f(0+1.25, 0+vehicle.length());
    }
    glEnd();
    glRotated(-lane.getRotation(), 0, 0, 1);
    glTranslated(-posX, -posY, 0);
}


void
GUITriangleVehicleDrawer::drawVehicleWithTooltips(const GUILaneWrapper &lane,
            const GUIVehicle &vehicle,
            GUIViewTraffic::VehicleColoringScheme scheme)
{
    const Position2D &laneEnd = lane.getBegin();
    const Position2D &laneDir = lane.getDirection();
    double posX = laneEnd.x() - laneDir.x() * vehicle.pos();
    double posY = laneEnd.y() - laneDir.y() * vehicle.pos();
    glTranslated(posX, posY, 0);
    glRotated(lane.getRotation(), 0, 0, 1);
    glPushName(vehicle.getGlID());
    glBegin( GL_TRIANGLES );
    if(scheme!=GUIViewTraffic::VCS_LANECHANGE3) {
        setVehicleColor(vehicle, scheme);
        glVertex2f(0, 0);
        glVertex2f(0-1.25, 0+vehicle.length());
        glVertex2f(0+1.25, 0+vehicle.length());
    } else {
        setVehicleColor1Of3(vehicle);
        glVertex2f(0, 0);
        setVehicleColor2Of3(vehicle);
        glVertex2f(0-1.25, 0+vehicle.length());
        setVehicleColor3Of3(vehicle);
        glVertex2f(0+1.25, 0+vehicle.length());
    }
    glEnd();
    glPopName();
    glRotated(-lane.getRotation(), 0, 0, 1);
    glTranslated(-posX, -posY, 0);
}


void
GUITriangleVehicleDrawer::setVehicleColor(const GUIVehicle &vehicle,
                                          GUIViewTraffic::VehicleColoringScheme scheme)
{
    switch(scheme) {
    case GUIViewTraffic::VCS_BY_SPEED:
        {
            double speed = vehicle.speed();
            double maxSpeed = 30; // !!!
            double fact = speed / maxSpeed / 2.0;
            glColor3f(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    case GUIViewTraffic::VCS_SPECIFIED:
        glColor3fv(vehicle.getDefinedColor());
        break;
    case GUIViewTraffic::VCS_RANDOM1:
        glColor3fv(vehicle.getRandomColor1());
        break;
    case GUIViewTraffic::VCS_RANDOM2:
        glColor3fv(vehicle.getRandomColor2());
        break;
    case GUIViewTraffic::VCS_LANECHANGE1:
        {
            float color = vehicle.getPassedColor() / (float) 255;
            glColor3f(color, color, color);
        }
        break;
    case GUIViewTraffic::VCS_LANECHANGE2:
        glColor3fv(vehicle.getLaneChangeColor2());
        break;
    case GUIViewTraffic::VCS_WAITING1:
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
GUITriangleVehicleDrawer::setVehicleColor1Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle._lcAction==MSVehicle::LCA_STRAIGHT) {
        glColor3f(0, 0, 1);
        return;
    }
    // vehicles on false lanes will be red (others green)
    if((vehicle._lcAction&MSVehicle::LCA_URGENT)!=0) {
        if((vehicle._lcAction&MSVehicle::LCA_LANEBEGIN)==0) {
            glColor3f(1, 0, 0);
        } else {
            glColor3f(0, 0, 1);
        }
    } else {
        glColor3f(0, 1, 0);
    }
}


void
GUITriangleVehicleDrawer::setVehicleColor2Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle._lcAction==MSVehicle::LCA_STRAIGHT) {
        glColor3f(0, 0, 1);
        return;
    }
    // left side of the vehicles; wants to go/is gone into this direction:
    //  vehicle side will be yellow, otherwise green/red in dependence
    // whether a lanechange is needed
    // vehicles on false lanes will be red (others green)
    if((vehicle._lcAction&MSVehicle::LCA_RIGHT)!=0) {
        glColor3f(1, 1, 0);
    } else {
        if((vehicle._lcAction&MSVehicle::LCA_URGENT)!=0) {
            glColor3f(1, 0, 0);
        } else {
            glColor3f(0, 1, 0);
        }
    }
}


void
GUITriangleVehicleDrawer::setVehicleColor3Of3(const GUIVehicle &vehicle)
{
    // the vehicle will be completely blue if no lane change action is needed
    //  and nothing was performed
    if(vehicle._lcAction==MSVehicle::LCA_STRAIGHT) {
        glColor3f(0, 0, 1);
        return;
    }
    // right side of the vehicles; wants to go/is gone into this direction:
    //  vehicle side will be yellow, otherwise green/red in dependence
    // whether a lanechange is needed
    // vehicles on false lanes will be red (others green)
    if((vehicle._lcAction&MSVehicle::LCA_LEFT)!=0) {
        glColor3f(1, 1, 0);
    } else {
        if((vehicle._lcAction&MSVehicle::LCA_URGENT)!=0) {
            glColor3f(1, 0, 0);
        } else {
            glColor3f(0, 1, 0);
        }
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUITriangleVehicleDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


