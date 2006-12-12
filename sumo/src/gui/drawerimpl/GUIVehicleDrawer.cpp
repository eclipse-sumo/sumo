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
// Revision 1.1  2006/12/12 12:10:43  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.19  2006/11/16 12:30:53  dkrajzew
// warnings removed
//
// Revision 1.18  2006/10/12 10:14:26  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.17  2006/07/06 06:26:44  dkrajzew
// added blinker visualisation and vehicle tracking (unfinished)
//
// Revision 1.16  2006/01/31 10:51:18  dkrajzew
// unused methods documented out (kept for further use)
//
// Revision 1.15  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.14  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.13  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.12  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
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
// refactored vehicle and lane coloring scheme usage to allow optional coloring
//  schemes
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <guisim/GUIVehicle.h>
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include "GUIVehicleDrawer.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/lanechanging/MSLCM_DK2004.h>
#include <utils/glutils/GLHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_CPhone.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * static members definitions
 * ======================================================================= */
GUIColoringSchemesMap<GUIVehicle> GUIVehicleDrawer::myColoringSchemes;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIVehicleDrawer::GUIVehicleDrawer(const std::vector<GUIEdge*> &edges)
    : myEdges(edges)
{
}


std::map<int, FXColor> colorBla;


GUIVehicleDrawer::~GUIVehicleDrawer()
{
}


void
GUIVehicleDrawer::drawGLVehicles(size_t *onWhich, size_t maxEdges,
                                 GUISUMOAbstractView::VisualizationSettings &settings)
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
                    continue; // !!! optional
                }
                size_t noLanes = edge->nLanes();
                for(size_t i=0; i<noLanes; i++) {
                    // get the lane
                    GUILaneWrapper &laneGeom = edge->getLaneGeometry(i);
                    drawLanesVehicles(laneGeom, settings);
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
    if(length<8) {
        glScaled(1, length, 1);
        glBegin( GL_TRIANGLES );
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 1);
            glVertex2d(0+1.25, 1);
        glEnd();
    } else {
        glBegin( GL_TRIANGLES );
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
drawAction_drawVehicleBlinker(const GUIVehicle &veh)
{
    int state = veh.getLaneChangeModel().getState();
    glColor3f(1, .5, 0);
    if((state&LCA_URGENT)==0) {
        glColor3f(.8f, .4f, 0);
    }
    if((state&LCA_LEFT)!=0) {
        glTranslated(1, BLINKER_POS_FRONT, 0);
        GLHelper::drawFilledCircle(.5, 6);
        glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
        GLHelper::drawFilledCircle(.5, 6);
        glTranslated(-1, +BLINKER_POS_BACK-veh.getLength(), 0);
    } else if((state&LCA_RIGHT)!=0) {
        glTranslated(-1, .5, 0);
        GLHelper::drawFilledCircle(.5, 6);
        glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
        GLHelper::drawFilledCircle(.5, 6);
        glTranslated(1, +BLINKER_POS_BACK-veh.getLength(), 0);
    } else {
        MSLinkCont::const_iterator link = veh.getLane().succLinkSec( veh, 1, veh.getLane() );
        if(link!=veh.getLane().getLinkCont().end()) {
            switch((*link)->getDirection()) {
            case MSLink::LINKDIR_TURN:
            case MSLink::LINKDIR_LEFT:
            case MSLink::LINKDIR_PARTLEFT:
                glTranslated(1, BLINKER_POS_FRONT, 0);
                GLHelper::drawFilledCircle(.5, 6);
                glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
                GLHelper::drawFilledCircle(.5, 6);
                glTranslated(-1, +BLINKER_POS_BACK-veh.getLength(), 0);
                break;
            case MSLink::LINKDIR_RIGHT:
            case MSLink::LINKDIR_PARTRIGHT:
                glTranslated(-1, BLINKER_POS_FRONT, 0);
                GLHelper::drawFilledCircle(.5, 6);
                glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
                GLHelper::drawFilledCircle(.5, 6);
                glTranslated(1, +BLINKER_POS_BACK-veh.getLength(), 0);
                break;
            default:
                break;
            }
        }
    }
}


inline void
drawAction_C2CdrawVehicleRadius(const GUIVehicle &veh)
{
    if(veh.isEquipped()) {
        /*
        int cluster = veh->getClusterId();
        if(veh->getConnections().size()==0){
            cluster = -1;
        }
        if(colorBla.find(cluster)==colorBla.end()) {
            int r = (int) ((double) rand() / (double) RAND_MAX * 255.);
            int g = (int) ((double) rand() / (double) RAND_MAX * 255.);
            int b = (int) ((double) rand() / (double) RAND_MAX * 255.);
            colorBla[cluster] = FXRGB(r, g, b);
        }
        FXColor c = colorBla[cluster];
        glColor3f(
		    (float) ((float) FXREDVAL(c) /255.),
			(float) ((float) FXGREENVAL(c) /255.),
			(float) ((float) FXBLUEVAL(c) /255.));
            */
        GLHelper::drawOutlineCircle(MSGlobals::gLANRange, MSGlobals::gLANRange-2, 24);
    }
}




void
GUIVehicleDrawer::initStep()
{
    glMatrixMode( GL_MODELVIEW );
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void
GUIVehicleDrawer::drawLanesVehicles(GUILaneWrapper &lane,
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
    size_t shapePos = 0;
    SUMOReal positionOffset = 0;
    for(v=vehicles.begin(); v!=vehicles.end(); v++) {
        GUIVehicle *veh = static_cast<GUIVehicle*>(*v);
        SUMOReal vehiclePosition = veh->getPositionOnLane();
        while( shapePos<rots.size()-1 && vehiclePosition>positionOffset+lengths[shapePos]) {
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
        if(myShowToolTips) {
            glPushName(veh->getGlID());
        }
            // set color
        GUIVehicleDrawer::getSchemesMap().getColorer(settings.vehicleMode)->setGlColor(*veh);
            // draw the vehicle
        SUMOReal upscale = settings.vehicleExaggeration;
        drawAction_drawVehicleAsTrianglePlus(*veh, upscale);
            // draw the blinker if wished
        if(settings.showBlinker) {
            drawAction_drawVehicleBlinker(*veh);
        }
            // draw the c2c-circle
        if(true) { // !!!
            drawAction_C2CdrawVehicleRadius(*veh);
        }
            // draw the wish to change the lane
        if(true) {//!!!
            MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
            glColor3f(.5, .5, 1);
            glBegin(GL_LINES);
            glVertex2f(0, 0);
            glVertex2f(m.getChangeProbability(), .5);
            glEnd();
        }
            // draw best lanes
        if(true) {
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
            // removed the gl-id if wished
        if(myShowToolTips) {
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
*/

GUIColoringSchemesMap<GUIVehicle> &
GUIVehicleDrawer::getSchemesMap()
{
    return myColoringSchemes;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


