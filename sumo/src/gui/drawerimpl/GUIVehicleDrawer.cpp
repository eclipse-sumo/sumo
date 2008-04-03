/****************************************************************************/
/// @file    GUIVehicleDrawer.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id$
///
// Base class for vehicle drawing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <guisim/GUIVehicle.h>
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include "GUIVehicleDrawer.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/MSLCM_DK2004.h>
#include <utils/gui/div/GLHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_CPhone.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/RandHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleDrawer::GUIVehicleDrawer(const std::vector<GUIEdge*> &edges)
        : myEdges(edges)
{}


std::map<int, FXColor> colorBla;


GUIVehicleDrawer::~GUIVehicleDrawer()
{}


void
GUIVehicleDrawer::drawGLVehicles(size_t *onWhich, size_t maxEdges,
                                 SUMOReal scale,
                                 const GUIColoringSchemesMap<GUIVehicle> &schemes,
                                 GUISUMOAbstractView::VisualizationSettings &settings)
{
    initStep();
    // go through edges
    for (size_t i=0; i<maxEdges; ++i) {
        if (onWhich[i]==0) {
            continue;
        }
        size_t pos = 1;
        for (size_t j=0; j<32; j++, pos<<=1) {
            if ((onWhich[i]&pos)!=0) {
                GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                if (edge->getPurpose()==MSEdge::EDGEFUNCTION_SOURCE) {
                    continue; // !!! optional
                }
                size_t noLanes = edge->nLanes();
                for (size_t i=0; i<noLanes; ++i) {
                    // get the lane
                    GUILaneWrapper &laneGeom = edge->getLaneGeometry(i);
                    drawLanesVehicles(laneGeom, scale, schemes, settings);
                }
            }
        }
    }
}


inline void
drawAction_drawVehicleAsTrianglePlus(const GUIVehicle &veh, SUMOReal upscale)
{
    SUMOReal length = veh.getLength();
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
    if (length<8) {
        glScaled(1, length, 1);
        glBegin(GL_TRIANGLES);
        glVertex2d(0, 0);
        glVertex2d(0-1.25, 1);
        glVertex2d(0+1.25, 1);
        glEnd();
    } else {
        glBegin(GL_TRIANGLES);
        glVertex2d(0, 0);
        glVertex2d(0-1.25, 0+2);
        glVertex2d(0+1.25, 0+2);
        glVertex2d(0-1.25, 2);
        glVertex2d(0-1.25, length);
        glVertex2d(0+1.25, length);
        glVertex2d(0+1.25, 2);
        glVertex2d(0-1.25, 2);
        glVertex2d(0+1.25, length);
        glEnd();
    }
    glPopMatrix();
}

#define BLINKER_POS_FRONT 1.
#define BLINKER_POS_BACK 1.

inline void
drawAction_drawVehicleBlinker(GUIVehicle &veh)
{
    int dir = veh.getCORNIntValue(MSCORN::CORN_VEH_BLINKER);
    if (dir==0) {
        return;
    }
    glColor3f(1., .8, 0);
    glTranslated(dir, BLINKER_POS_FRONT, 0);
    GLHelper::drawFilledCircle(.5, 6);
    glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
    GLHelper::drawFilledCircle(.5, 6);
    glTranslated(-dir, +BLINKER_POS_BACK-veh.getLength(), 0);
}


inline void
drawAction_drawVehicleName(const GUIVehicle &veh, SUMOReal size)
{
    glPushMatrix();
    glTranslated(0, veh.getLength() / 2., 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    pfSetPosition(0, 0);
    pfSetScale(size);
    SUMOReal w = pfdkGetStringWidth(veh.microsimID().c_str());
    glRotated(180, 0, 1, 0);
    glTranslated(-w/2., 0.4, 0);
    pfDrawString(veh.microsimID().c_str());
    glPopMatrix();
}


#ifdef HAVE_BOYOM_C2C
inline void
drawAction_C2CdrawVehicleRadius(const GUIVehicle &veh)
{
    if (veh.isEquipped()) {
        int cluster = veh.getClusterId();
        if (veh.getConnections().size()==0) {
            cluster = -1;
        }
        if (colorBla.find(cluster)==colorBla.end()) {
            int r = RandHelper::rand(256);
            int g = RandHelper::rand(256);
            int b = RandHelper::rand(256);
            colorBla[cluster] = FXRGB(r, g, b);
        }
        FXColor c = colorBla[cluster];
        glColor3f(
            (float)((float) FXREDVAL(c) /255.),
            (float)((float) FXGREENVAL(c) /255.),
            (float)((float) FXBLUEVAL(c) /255.));
        GLHelper::drawOutlineCircle(MSGlobals::gLANRange, MSGlobals::gLANRange-2, 24);
    }
}
#endif



void
GUIVehicleDrawer::initStep()
{
    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void
GUIVehicleDrawer::drawLanesVehicles(GUILaneWrapper &lane,
                                    SUMOReal scale,
                                    const GUIColoringSchemesMap<GUIVehicle> &schemes,
                                    const GUISUMOAbstractView::VisualizationSettings &settings)
{
    // retrieve vehicles from lane; disallow simulation
    const MSLane::VehCont &vehicles = lane.getVehiclesSecure();
    const DoubleVector &lengths = lane.getShapeLengths();
    const DoubleVector &rots = lane.getShapeRotations();
    const Position2DVector &geom = lane.getShape();
    const Position2D &laneBeg = geom[0];

    MSLane::VehCont::const_iterator v;
    glPushMatrix();
    glTranslated(laneBeg.x(), laneBeg.y(), 0);
    glRotated(rots[0], 0, 0, 1);
    // go through the vehicles
    int shapePos = 0;
    SUMOReal positionOffset = 0;
    for (v=vehicles.begin(); v!=vehicles.end(); v++) {
        GUIVehicle *veh = static_cast<GUIVehicle*>(*v);
        SUMOReal vehiclePosition = veh->getPositionOnLane();
        while (shapePos<(int)rots.size()-1 && vehiclePosition>positionOffset+lengths[shapePos]) {
            glPopMatrix();
            positionOffset += lengths[shapePos];
            shapePos++;
            glPushMatrix();
            glTranslated(geom[shapePos].x(), geom[shapePos].y(), 0);
            glRotated(rots[shapePos], 0, 0, 1);
        }
        glPushMatrix();
        glTranslated(0, -(vehiclePosition-positionOffset), 0);

        // begin drawing
        // set the gl-id if wished
        if (myShowToolTips) {
            glPushName(veh->getGlID());
        }
        // set color
        schemes.getColorer(settings.vehicleMode)->setGlColor(*veh);
        // draw the vehicle
        SUMOReal upscale = settings.vehicleExaggeration;
        drawAction_drawVehicleAsTrianglePlus(*veh, upscale);
        // draw the blinker if wished
        if (settings.showBlinker) {
            drawAction_drawVehicleBlinker(*veh);
        }
        // draw the c2c-circle
#ifdef HAVE_BOYOM_C2C
        if (settings.drawcC2CRadius) {
            drawAction_C2CdrawVehicleRadius(*veh);
        }
#endif
        // draw the wish to change the lane
        if (settings.drawLaneChangePreference) {
            MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
            glColor3f(.5, .5, 1);
            glBegin(GL_LINES);
            glVertex2f(0, 0);
            glVertex2f(m.getChangeProbability(), .5);
            glEnd();
        }
        // draw best lanes
        if (true) {
            /*
            const MSLane &l = veh->getLane();
            SUMOReal r1 = veh->allowedContinuationsLength(&l, 0);
            SUMOReal r2 = l.getLeftLane()!=0 ? veh->allowedContinuationsLength(l.getLeftLane(), 0) : 0;
            SUMOReal r3 = l.getRightLane()!=0 ? veh->allowedContinuationsLength(l.getRightLane(), 0) : 0;
            SUMOReal mmax = MAX3(r1, r2, r3);
            glBegin(GL_LINES);
            glVertex2f(0, 0);
            glVertex2f(0, r1/mmax/2.);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(.4, 0);
            glVertex2f(.4, r2/mmax/2.);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(-.4, 0);
            glVertex2f(-.4, r3/mmax/2.);
            glEnd();
            */
        }
        if (settings.drawVehicleName) {
            // compute name colors
            glColor3f(settings.vehicleNameColor.red(), settings.vehicleNameColor.green(), settings.vehicleNameColor.blue());
            drawAction_drawVehicleName(*veh, settings.vehicleNameSize / scale);
        }
        // removed the gl-id if wished
        if (myShowToolTips) {
            glPopName();
        }
        glPopMatrix();
    }
    // allow lane simulation
    lane.releaseVehicles();
    glPopMatrix();
}


void
GUIVehicleDrawer::setGLID(bool val)
{
    myShowToolTips = val;
}


/*
RGBColor
GUIVehicleDrawer::getVehicleColor(const GUIVehicle &vehicle,
    GUIBaseColorer<GUIVehicle> &colorer)
{
return colorer.getMinColor();
switch(scheme) {
case GUISUMOAbstractView::VCS_BY_SPEED:
    {
        SUMOReal speed = vehicle.speed();
        SUMOReal maxSpeed = 30; // !!!
        SUMOReal fact = speed / maxSpeed / (SUMOReal) 2.0;
        return RGBColor((SUMOReal) (1.0-fact), (SUMOReal) 0.5, (SUMOReal) (0.5+fact));
    }
    break;
case GUISUMOAbstractView::VCS_SPECIFIED:
    {
        if(vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED)) {
            return RGBColor(
                vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED),
                vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_GREEN),
                vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_BLUE));
        }
        return RGBColor(1, 1, 0);
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
        SUMOReal color = vehicle.getPassedColor() / (SUMOReal) 255;
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
        SUMOReal color = SUMOReal(vehicle.getWaitingTime())
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
        }
    }
    break;
case GUISUMOAbstractView::VCS_ROUTECHANGEOFFSET:
    {
        GUIVehicle &v = (GUIVehicle&) (vehicle);
        if( !v.hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET) ) {

            return RGBColor(220.0f/255.0f, 134.0f/255.0f, 228.0f/255.0f);
        } else {
            SUMOReal val = v.getCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET);
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

            return RGBColor(
                (SUMOReal) (220.0/255.0),
                (SUMOReal) (134.0/255.0),
                (SUMOReal) (228.0/255.0));
        } else {
            SUMOReal val = v.getCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE);
            if(val>10) {
                val = 10;
            }
            val = val / (SUMOReal) 10.;
            return RGBColor((SUMOReal) val, (SUMOReal) (1.0-val), (SUMOReal) 0);
        }
        break;
    }
case GUISUMOAbstractView::VCS_LANECHANGE4:
    {
        const MSLCM_DK2004 &model =
            static_cast<const MSLCM_DK2004 &>(vehicle.getLaneChangeModel());
        SUMOReal prob = model.getProb();
        if(prob>0) {
            if(prob>1) {
                prob = 1;
            }
            return RGBColor((SUMOReal) prob, (SUMOReal) 0.3, (SUMOReal) (1.0-prob));
        } else {
            if(prob<-1) {
                prob = -1;
            }
            return RGBColor((SUMOReal) 0.3, (SUMOReal) -prob, (SUMOReal) (1.0+prob));
        }
        break;
    }
default:
    break;
}
return RGBColor(1, 1, 1);
}
*/



/*
void
GUIVehicleDrawer::setVehicleColor(const GUIVehicle &vehicle,
        GUIBaseColorer<GUIVehicle> &colorer)
{
    /
    switch(scheme) {
    case GUISUMOAbstractView::VCS_BY_SPEED:
        {
            SUMOReal speed = vehicle.speed();
            SUMOReal maxSpeed = 30; // !!!
            SUMOReal fact = speed / maxSpeed / (SUMOReal) 2.0;
            glColor3d((SUMOReal) (1.0-fact), (SUMOReal) 0.5, (SUMOReal) (0.5+fact));
        }
        break;
    case GUISUMOAbstractView::VCS_SPECIFIED:
        {
            if(vehicle.hasCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED)) {
                 glColor3d(
                    vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED),
                    vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_GREEN),
                    vehicle.getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_BLUE));
            } else {
                glColor3d(1, 1, 0);
            }
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
            SUMOReal color = vehicle.getPassedColor() / (SUMOReal) 255;
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
            SUMOReal color = SUMOReal(vehicle.getWaitingTime())
                / (SUMOReal) 512.0;
            if(color>1.0) {
                glColor3d(0, 0, 0);
            } else {
                color = (SUMOReal) 1.0 - color;
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
                SUMOReal val = v.getCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET);
                if(val>300) {
                    glColor3d(128.0/255.0, 128.0/255.0, 0/255.0);
                } else {
                    val = (SUMOReal) 1.0 - (val / (SUMOReal) 600.0);
                    glColor3d(val, val, 0/(SUMOReal) 255.0);
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
                SUMOReal val = v.getCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE);
                if(val>10) {
                    val = 10;
                }
                val = val / (SUMOReal) 10.;
                glColor3d(val, (SUMOReal) 1.0-val, 0);
            }
            break;
        }
    case GUISUMOAbstractView::VCS_LANECHANGE4:
        {
            const MSLCM_DK2004 &model =
                static_cast<const MSLCM_DK2004 &>(vehicle.getLaneChangeModel());
            SUMOReal prob = model.getProb();
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
            }
        }
        break;
    default:
    }
}


void
GUIVehicleDrawer::setVehicleColor1Of3(const GUIVehicle &vehicle)
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
GUIVehicleDrawer::setVehicleColor2Of3(const GUIVehicle &vehicle)
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
GUIVehicleDrawer::setVehicleColor3Of3(const GUIVehicle &vehicle)
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



/****************************************************************************/

