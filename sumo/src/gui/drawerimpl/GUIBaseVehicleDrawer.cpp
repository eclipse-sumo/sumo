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
// Revision 1.11  2005/07/12 12:06:11  dkrajzew
// first devices (mobile phones) added
//
// Revision 1.10  2005/04/29 10:59:42  dkrajzew
// debugging
//
// Revision 1.9  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.8  2004/11/23 10:05:21  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.7  2004/08/02 11:30:54  dkrajzew
// refactored vehicle and lane coloring scheme usage to allow optional coloring schemes
//
// Revision 1.6  2004/07/02 08:12:51  dkrajzew
// global object selection added
//
// Revision 1.5  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <guisim/GUIVehicle.h>
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include "GUIBaseVehicleDrawer.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/lanechanging/MSLCM_DK2004.h>

#include <microsim/devices/MSDevice_CPhone.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * static members definitions
 * ======================================================================= */
GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme, GUIVehicle>
    GUIBaseVehicleDrawer::myColoringSchemes;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseVehicleDrawer::GUIBaseVehicleDrawer(const std::vector<GUIEdge*> &edges)
    : myEdges(edges)
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
                    drawLanesVehicles(laneGeom, scheme);
                }
            }
        }
    }
}


void
GUIBaseVehicleDrawer::initStep()
{
    glMatrixMode( GL_MODELVIEW );
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


RGBColor
GUIBaseVehicleDrawer::getVehicleColor(const GUIVehicle &vehicle,
        GUISUMOAbstractView::VehicleColoringScheme scheme)
{
    switch(scheme) {
    case GUISUMOAbstractView::VCS_BY_SPEED:
        {
            double speed = vehicle.speed();
            double maxSpeed = 30; // !!!
            double fact = speed / maxSpeed / 2.0;
            return RGBColor(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    case GUISUMOAbstractView::VCS_SPECIFIED:
        {
            const RGBColor &col = vehicle.getDefinedColor();
            return RGBColor(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_TYPE:
        {
            const RGBColor &col =
                static_cast<const GUIVehicleType&>(vehicle.getVehicleType()).getColor();
            return RGBColor(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_ROUTE:
        {
            const RGBColor &col =
                static_cast<const GUIRoute&>(vehicle.getRoute()).getColor();
            return RGBColor(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM1:
        {
            const RGBColor &col = vehicle.getRandomColor1();
            return RGBColor(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM2:
        {
            const RGBColor &col = vehicle.getRandomColor2();
            return RGBColor(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE1:
        {
            float color = vehicle.getPassedColor() / (float) 255;
            return RGBColor(color, color, color);
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE2:
        {
            return vehicle.getLaneChangeColor2();
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE3:
        {
            return  RGBColor(1, 1, 0);
        }
        break;
    case GUISUMOAbstractView::VCS_WAITING1:
        {
            float color = float(vehicle.getWaitingTime())
                / 512.0f;
            if(color>1.0) {
                return RGBColor(0, 0, 0);
            } else {
                color = 1.0f - color;
                return RGBColor(color, color, color);
            }
        }
        break;
    case GUISUMOAbstractView::VCS_DEVICENO:
        {
            if(vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_DEV_NO_CPHONE)) {
                return RGBColor(0, 1, 0);
            }
            return RGBColor(1, 0, 0);
        }
        break;
    case GUISUMOAbstractView::VCS_DEVICE_STATE:
        {
            if(!vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_DEV_NO_CPHONE)) {
                return RGBColor(1, 0, 0);
            }
            const MSDevice_CPhone * const phone =
                (const MSDevice_CPhone * const)
                vehicle.getCORNPointerValue(MSCORN::CORN_P_VEH_DEV_CPHONE);
            switch(phone->GetState()) {
            case MSDevice_CPhone::STATE_OFF:
                return RGBColor(1, 0, 1);
            case MSDevice_CPhone::STATE_IDLE:
                return RGBColor(0, 0, 1);
            case MSDevice_CPhone::STATE_CONNECTED:
                return RGBColor(0, 1, 0);
            default:
                throw 1;
            }
        }
        break;
    case GUISUMOAbstractView::VCS_ROUTECHANGEOFFSET:
        {
            GUIVehicle &v = (GUIVehicle&) (vehicle);
            if( !v.hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET) ) {

                return RGBColor(220.0f/255.0f, 134.0f/255.0f, 228.0f/255.0f);
            } else {
                double val = v.getCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET);
                if(val>300) {
                    glColor3d(128.0f/255.0f, 128.0f/255.0f, 0/255.0f);
                } else {
                    val = 1.0f - (val / 600.0f);
                    return RGBColor(val, val, 0/255.0f);
                }
            }
            break;
        }
    case GUISUMOAbstractView::VCS_ROUTECHANGENUMBER:
        {
            GUIVehicle &v = (GUIVehicle&) (vehicle);
            if( !v.hasCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE) ) {

                return RGBColor(220.0/255.0, 134.0/255.0, 228.0/255.0);
            } else {
                double val = v.getCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE);
                if(val>10) {
                    val = 10;
                }
                val = val / 10.;
                return RGBColor(val, 1.0-val, 0);
            }
            break;
        }
    case GUISUMOAbstractView::VCS_LANECHANGE4:
        {
            const MSLCM_DK2004 &model =
                static_cast<const MSLCM_DK2004 &>(vehicle.getLaneChangeModel());
            double prob = model.getProb();
            if(prob>0) {
                if(prob>1) {
                    prob = 1;
                }
                return RGBColor(prob, 0.3, 1.0-prob);
            } else {
                if(prob<-1) {
                    prob = -1;
                }
                return RGBColor(0.3, -prob, 1.0+prob);
            }
            break;
        }
    default:
        break;
    }
    return RGBColor(1, 1, 1);
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
            glColor3d(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    case GUISUMOAbstractView::VCS_SPECIFIED:
        {
            const RGBColor &col = vehicle.getDefinedColor();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_TYPE:
        {
            const RGBColor &col =
                static_cast<const GUIVehicleType&>(vehicle.getVehicleType()).getColor();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_ROUTE:
        {
            const RGBColor &col =
                static_cast<const GUIRoute&>(vehicle.getRoute()).getColor();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM1:
        {
            const RGBColor &col = vehicle.getRandomColor1();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_RANDOM2:
        {
            const RGBColor &col = vehicle.getRandomColor2();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE1:
        {
            float color = vehicle.getPassedColor() / (float) 255;
            glColor3d(color, color, color);
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE2:
        {
            const RGBColor &col = vehicle.getLaneChangeColor2();
            glColor3d(col.red(), col.green(), col.blue());
        }
        break;
    case GUISUMOAbstractView::VCS_LANECHANGE3:
        {
            glColor3d(1, 1, 0);
        }
        break;
    case GUISUMOAbstractView::VCS_WAITING1:
        {
            double color = double(vehicle.getWaitingTime())
                / 512.0;
            if(color>1.0) {
                glColor3d(0, 0, 0);
            } else {
                color = 1.0 - color;
                glColor3d(color, color, color);
            }
        }
        break;
    case GUISUMOAbstractView::VCS_ROUTECHANGEOFFSET:
        {
            GUIVehicle &v = (GUIVehicle&) (vehicle);
            if( !v.hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET) ) {

                glColor3d(220.0/255.0, 134.0/255.0, 228.0/255.0);
            } else {
                double val = v.getCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET);
                if(val>300) {
                    glColor3d(128.0/255.0, 128.0/255.0, 0/255.0);
                } else {
                    val = 1.0 - (val / 600.0);
                    glColor3d(val, val, 0/255.0);
                }
            }
            break;
        }
    case GUISUMOAbstractView::VCS_ROUTECHANGENUMBER:
        {
            GUIVehicle &v = (GUIVehicle&) (vehicle);
            if( !v.hasCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE) ) {

                glColor3d(220.0/255.0, 134.0/255.0, 228.0/255.0);
            } else {
                double val = v.getCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE);
                if(val>10) {
                    val = 10;
                }
                val = val / 10.;
                glColor3d(val, 1.0-val, 0);
            }
            break;
        }
    case GUISUMOAbstractView::VCS_LANECHANGE4:
        {
            const MSLCM_DK2004 &model =
                static_cast<const MSLCM_DK2004 &>(vehicle.getLaneChangeModel());
            double prob = model.getProb();
            if(prob>0) {
                if(prob>1) {
                    prob = 1;
                }
                glColor3d(prob, 0.3, 1.0-prob);
            } else {
                if(prob<-1) {
                    prob = -1;
                }
                glColor3d(0.3, -prob, 1.0+prob);
            }
            break;
        }
    case GUISUMOAbstractView::VCS_DEVICENO:
        {
            if(vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_DEV_NO_CPHONE)) {
                glColor3d(0, 1, 0);
                return;
            }
            glColor3d(1, 0, 0);
        }
        break;
    case GUISUMOAbstractView::VCS_DEVICE_STATE:
        {
            if(!vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_DEV_NO_CPHONE)) {
                glColor3d(1, 0, 0);
                return;
            }
            const MSDevice_CPhone * const phone =
                (const MSDevice_CPhone * const)
                vehicle.getCORNPointerValue(MSCORN::CORN_P_VEH_DEV_CPHONE);
            switch(phone->GetState()) {
            case MSDevice_CPhone::STATE_OFF:
                glColor3d(1, 0, 1);
                return;
            case MSDevice_CPhone::STATE_IDLE:
                glColor3d(0, 0, 1);
                return;
            case MSDevice_CPhone::STATE_CONNECTED:
                glColor3d(0, 1, 0);
                return;
            default:
                throw 1;
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
    // vehicles are red if the lane change is urgent
    if((vehicle.getLaneChangeModel().getState()&LCA_URGENT)!=0) {
        glColor3d(1, 0.3f, 0.3f);
        return;
    }
    // vehicles are purpleif a lane change is wished (but not urgent)
    if((vehicle.getLaneChangeModel().getState()&LCA_SPEEDGAIN)!=0) {
        glColor3d(0.3f, 0.3f, 1);
        return;
    }
    // vehicles that want to stay at their lanes and are not interacting
    //  are blue
    glColor3d(0.3f, 1, 0.3f);
}


void
GUIBaseVehicleDrawer::setVehicleColor2Of3(const GUIVehicle &vehicle)
{
    // vehicle side will be yellow on their right side if changing to right,
    if((vehicle.getLaneChangeModel().getState()&(LCA_RIGHT|LCA_URGENT))!=0) {
        glColor3d(1, 1, 0);
    } else {
        // vehicles are purpleif a lane change is wished (but not urgent)
        if((vehicle.getLaneChangeModel().getState()&(LCA_AMBLOCKINGFOLLOWER))!=0) {
            glColor3d(1, 0.3f, 0.3f);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBACKBLOCKER)!=0) {
            glColor3d(1, 1, 1);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBACKBLOCKER_STANDING)!=0) {
            glColor3d(1, 1, 0);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBLOCKINGLEADER)!=0) {
            glColor3d(0.3f, 0.3f, 1);
            return;
        }
        // vehicles are purpleif a lane change is wished (but not urgent)
        if((vehicle.getLaneChangeModel().getState()&(LCA_AMBLOCKINGFOLLOWER_DONTBRAKE))!=0) {
            glColor3d(1, 0, 1);
            return;
        }
        // vehicles that want to stay at their lanes and are not interacting
        //  are blue
        glColor3d(0.3f, 1, 0.3f);
    }
}


void
GUIBaseVehicleDrawer::setVehicleColor3Of3(const GUIVehicle &vehicle)
{
    if((vehicle.getLaneChangeModel().getState()&(LCA_LEFT|LCA_URGENT))!=0) {
        glColor3d(1, 1, 0);
    } else {
        // vehicles are purpleif a lane change is wished (but not urgent)
        if((vehicle.getLaneChangeModel().getState()&(LCA_AMBLOCKINGFOLLOWER))!=0) {
            glColor3d(1, 0.3f, 0.3f);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBACKBLOCKER)!=0) {
            glColor3d(1, 1, 1);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBACKBLOCKER_STANDING)!=0) {
            glColor3d(1, 1, 0);
            return;
        }
        // vehicles are red if the lane change is urgent
        if((vehicle.getLaneChangeModel().getState()&LCA_AMBLOCKINGLEADER)!=0) {
            glColor3d(0.3f, 0.3f, 1);
            return;
        }
        // vehicles are purpleif a lane change is wished (but not urgent)
        if((vehicle.getLaneChangeModel().getState()&(LCA_AMBLOCKINGFOLLOWER_DONTBRAKE))!=0) {
            glColor3d(1, 0, 1);
            return;
        }
        // vehicles that want to stay at their lanes and are not interacting
        //  are blue
        glColor3d(0.3f, 1, 0.3f);
    }
}


GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme, GUIVehicle> &
GUIBaseVehicleDrawer::getSchemesMap()
{
    return myColoringSchemes;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


