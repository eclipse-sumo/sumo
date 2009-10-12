/****************************************************************************/
/// @file    GUILaneWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <iostream>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNet.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUINet.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/RandHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <gui/GUIViewTraffic.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <guisim/GUIVehicle.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/HelpersHarmonoise.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
SUMOReal GUILaneWrapper::myAllMaxSpeed = 0;


// ===========================================================================
// method definitions
// ===========================================================================
GUILaneWrapper::GUILaneWrapper(GUIGlObjectStorage &idStorage,
                               MSLane &lane, const Position2DVector &shape) throw()
        : GUIGlObject(idStorage, "lane:"+lane.getID()),
        myLane(lane), myShape(shape) {
    SUMOReal x1 = shape[0].x();
    SUMOReal y1 = shape[0].y();
    SUMOReal x2 = shape[-1].x();
    SUMOReal y2 = shape[-1].y();
    SUMOReal length = getLength();
    // also the virtual length is set in here
    myVisLength = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    // check maximum speed
    if (myAllMaxSpeed<lane.maxSpeed()) {
        myAllMaxSpeed = lane.maxSpeed();
    }
    //
    myShapeRotations.reserve(myShape.size()-1);
    myShapeLengths.reserve(myShape.size()-1);
    int e = (int) myShape.size() - 1;
    for (int i=0; i<e; ++i) {
        const Position2D &f = myShape[i];
        const Position2D &s = myShape[i+1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI);
    }
}


GUILaneWrapper::~GUILaneWrapper() throw() {}


SUMOReal
GUILaneWrapper::getLength() const {
    return myLane.myLength;
}


SUMOReal
GUILaneWrapper::visLength() const {
    return myVisLength;
}


MSEdge::EdgeBasicFunction
GUILaneWrapper::getPurpose() const {
    return myLane.myEdge->getPurpose();
}


SUMOReal
GUILaneWrapper::maxSpeed() const {
    return myLane.maxSpeed();
}


SUMOReal
GUILaneWrapper::getOverallMaxSpeed() {
    return myAllMaxSpeed;
}


bool
GUILaneWrapper::forLane(const MSLane &lane) const {
    return (&myLane)==(&lane);
}



void
ROWdrawAction_drawLinkNo(const GUILaneWrapper &lane) {
    unsigned int noLinks = lane.getLinkNumber();
    if (noLinks==0) {
        return;
    }

    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = SUMO_const_laneWidth / (SUMOReal) 2.;
    glPushMatrix();
    glColor3d(.5, .5, 1);
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i=0; i<noLinks; ++i) {
        SUMOReal x2 = x1 - (SUMOReal)(w/2.);
        int linkNo = lane.getLinkRespondIndex(i);
        glPushMatrix();
        //glTranslated(0, veh.getLength() / 2., 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1);
        SUMOReal tw = pfdkGetStringWidth(toString(linkNo).c_str());
        glRotated(180, 0, 1, 0);
        glTranslated(x2-tw/2., 0.5, 0);
        pfDrawString(toString(linkNo).c_str());
        glPopMatrix();
        x1 -= w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawTLSLinkNo(const GUINet &net, const GUILaneWrapper &lane) {
    unsigned int noLinks = lane.getLinkNumber();
    if (noLinks==0) {
        return;
    }

    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = (SUMOReal)(SUMO_const_laneWidth / 2.);
    glPushMatrix();
    glColor3d(.5, .5, 1);
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i=0; i<noLinks; ++i) {
        SUMOReal x2 = x1 - (SUMOReal)(w/2.);
        int linkNo = lane.getLinkTLIndex(net, i);
        if (linkNo<0) {
            continue;
        }
        glPushMatrix();
        //glTranslated(0, veh.getLength() / 2., 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1);
        SUMOReal tw = pfdkGetStringWidth(toString(linkNo).c_str());
        glRotated(180, 0, 1, 0);
        glTranslated(x2-tw/2., 0.5, 0);
        pfDrawString(toString(linkNo).c_str());
        glPopMatrix();
        x1 -= w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawLinkRules(const GUINet &net, const GUILaneWrapper &lane,
                            bool showToolTips) {
    unsigned int noLinks = lane.getLinkNumber();
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI;
    if (noLinks==0) {
        if (showToolTips) {
            glPushName(lane.getGlID());
        }
        // draw a grey bar if no links are on the street
        glColor3f(0.5, 0.5, 0.5);
        glPushMatrix();
        glTranslated(end.x(), end.y(), 0);
        glRotated(rot, 0, 0, 1);
        glBegin(GL_QUADS);
        glVertex2d(-SUMO_const_halfLaneWidth, 0.0);
        glVertex2d(-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(SUMO_const_halfLaneWidth, 0.0);
        glEnd();
        glPopMatrix();
        if (showToolTips) {
            glPopName();
        }
        return;
    }
    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = 0;
    glPushMatrix();
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i=0; i<noLinks; ++i) {
        SUMOReal x2 = x1 + w;
        MSLink::LinkState state = lane.getLinkState(i);
        if (showToolTips) {
            switch (state) {
            case MSLink::LINKSTATE_TL_GREEN_MAJOR:
            case MSLink::LINKSTATE_TL_GREEN_MINOR:
            case MSLink::LINKSTATE_TL_RED:
            case MSLink::LINKSTATE_TL_YELLOW_MAJOR:
            case MSLink::LINKSTATE_TL_YELLOW_MINOR:
            case MSLink::LINKSTATE_TL_OFF_BLINKING:
                glPushName(lane.getLinkTLID(net, i));
                break;
            case MSLink::LINKSTATE_MAJOR:
            case MSLink::LINKSTATE_MINOR:
            case MSLink::LINKSTATE_EQUAL:
            case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
            default:
                glPushName(lane.getGlID());
                break;
            }
        }
        switch (state) {
        case MSLink::LINKSTATE_TL_GREEN_MAJOR:
        case MSLink::LINKSTATE_TL_GREEN_MINOR:
            glColor3d(0, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_RED:
            glColor3d(1, 0, 0);
            break;
        case MSLink::LINKSTATE_TL_YELLOW_MAJOR:
        case MSLink::LINKSTATE_TL_YELLOW_MINOR:
            glColor3d(1, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_OFF_BLINKING:
            glColor3d(1, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
            glColor3d(0, 1, 1);
            break;
        case MSLink::LINKSTATE_MAJOR:
            glColor3d(1, 1, 1);
            break;
        case MSLink::LINKSTATE_MINOR:
            glColor3d(.2, .2, .2);
            break;
        case MSLink::LINKSTATE_EQUAL:
            glColor3d(.5, .5, .5);
            break;
        case MSLink::LINKSTATE_DEADEND:
            glColor3d(0, 0, 0);
            break;
        }
        glBegin(GL_QUADS);
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.0);
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth,0.0);
        glEnd();
        if (showToolTips) {
            glPopName();
        }
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawArrows(const GUILaneWrapper &lane, bool showToolTips) {
    unsigned int noLinks = lane.getLinkNumber();
    if (noLinks==0) {
        return;
    }
    // draw all links
    const Position2D &end = lane.getShape().getEnd();
    const Position2D &f = lane.getShape()[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI;
    glPushMatrix();
    if (showToolTips) {
        glPushName(0);
    }
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i=0; i<noLinks; ++i) {
        MSLink::LinkDirection dir = lane.getLinkDirection(i);
        MSLink::LinkState state = lane.getLinkState(i);
        if (state==MSLink::LINKSTATE_TL_OFF_NOSIGNAL||dir==MSLink::LINKDIR_NODIR) {
            continue;
        }
        GUITexturesHelper::drawDirectionArrow((GUITexture) dir,
                                              1.5, 4.0, -1.5, 1);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    if (showToolTips) {
        glPopName();
    }
    glPopMatrix();
}


void
ROWdrawAction_drawLane2LaneConnections(const GUILaneWrapper &lane) {
    unsigned int noLinks = lane.getLinkNumber();
    for (unsigned int i=0; i<noLinks; ++i) {
        MSLink::LinkState state = lane.getLinkState(i);
        const MSLane *connected = lane.getLinkLane(i);
        if (connected==0) {
            continue;
        }
        switch (state) {
        case MSLink::LINKSTATE_TL_GREEN_MAJOR:
        case MSLink::LINKSTATE_TL_GREEN_MINOR:
            glColor3d(0, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_RED:
            glColor3d(1, 0, 0);
            break;
        case MSLink::LINKSTATE_TL_YELLOW_MAJOR:
        case MSLink::LINKSTATE_TL_YELLOW_MINOR:
            glColor3d(1, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_OFF_BLINKING:
            glColor3d(1, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
            glColor3d(0, 1, 1);
            break;
        case MSLink::LINKSTATE_MAJOR:
            glColor3d(1, 1, 1);
            break;
        case MSLink::LINKSTATE_MINOR:
            glColor3d(.2, .2, .2);
            break;
        case MSLink::LINKSTATE_EQUAL:
            glColor3d(.5, .5, .5);
            break;
        case MSLink::LINKSTATE_DEADEND:
            glColor3d(0, 0, 0);
            break;
        }

        glBegin(GL_LINES);
        const Position2D &p1 = lane.getShape()[-1];
        const Position2D &p2 = connected->getShape()[0];
        glVertex2f(p1.x(), p1.y());
        glVertex2f(p2.x(), p2.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(Line2D(p1, p2), (SUMOReal) .4, (SUMOReal) .2);
    }
}


void
GUILaneWrapper::drawGL(const GUIVisualizationSettings &s) const throw() {
    // set lane color
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim)
#endif
        s.laneColorer.setGlColor(*this);
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    // draw lane
    glPolygonOffset(0, 0);
    // check whether it is not too small
    if (s.scale<1.) {
        GLHelper::drawLine(myShape);
        // (optional) clear id
        if (s.needsGlID) {
            glPopName();
        }
    } else {
        if (getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
            glPolygonOffset(0, 0.5);
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, SUMO_const_halfLaneWidth);
        } else {
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, SUMO_const_quarterLaneWidth);
        }
        // (optional) clear id
        if (s.needsGlID) {
            glPopName();
        }
        // draw ROWs (not for inner lanes)
        if (getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {// !!! getPurpose()
            glPolygonOffset(0, -2);
            GUINet *net = (GUINet*) MSNet::getInstance();
            ROWdrawAction_drawLinkRules(*net, *this, s.needsGlID);
            if (s.showLinkDecals) {
                ROWdrawAction_drawArrows(*this, s.needsGlID);
            }
            if (s.showLane2Lane) {
                // this should be independent to the geometry:
                //  draw from end of first to the begin of second
                ROWdrawAction_drawLane2LaneConnections(*this);
            }
            if (s.drawLinkJunctionIndex) {
                glPolygonOffset(0, -3);
                ROWdrawAction_drawLinkNo(*this);
            }
            if (s.drawLinkTLIndex) {
                glPolygonOffset(0, -3);
                ROWdrawAction_drawTLSLinkNo(*net, *this);
            }
        }
    }
    // draw vehicles
    if (s.scale>s.minVehicleSize) {
        // retrieve vehicles from lane; disallow simulation
        const MSLane::VehCont &vehicles = myLane.getVehiclesSecure();
        const Position2D &laneBeg = myShape[0];

        glPushMatrix();
        glTranslated(laneBeg.x(), laneBeg.y(), 0);
        glRotated(myShapeRotations[0], 0, 0, 1);
        // go through the vehicles
        int shapePos = 0;
        SUMOReal positionOffset = 0;
        for (MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); ++v) {
            const GUIVehicle * const veh = static_cast<const GUIVehicle*const>(*v);
            SUMOReal vehiclePosition = veh->getPositionOnLane();
            while (shapePos<(int)myShapeRotations.size()-1 && vehiclePosition>positionOffset+myShapeLengths[shapePos]) {
                glPopMatrix();
                positionOffset += myShapeLengths[shapePos];
                shapePos++;
                glPushMatrix();
                glTranslated(myShape[shapePos].x(), myShape[shapePos].y(), 0);
                glRotated(myShapeRotations[shapePos], 0, 0, 1);
            }
            glPushMatrix();
            glTranslated(0, -(vehiclePosition-positionOffset), 0);
            veh->drawGL(s);
            glPopMatrix();
        }
        // allow lane simulation
        myLane.releaseVehicles();
    }
    glPopMatrix();
}


void
GUILaneWrapper::drawBordersGL(const GUIVisualizationSettings &s) const throw() {
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim)
#endif
        s.laneColorer.setGlColor(*this);
    // check whether lane boundaries shall be drawn
    int e = (int) myShape.size() - 1;
    for (int i=0; i<e; i++) {
        glPushMatrix();
        glTranslated(myShape[i].x(), myShape[i].y(), 0);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t=0; t<myShapeLengths[i]; t+=6) {
            glBegin(GL_QUADS);
            glVertex2d(-1.8, -t);
            glVertex2d(-1.8, -t-3.);
            glVertex2d(1.0, -t-3.);
            glVertex2d(1.0, -t);
            glEnd();
        }
        glPopMatrix();
    }
}


GUIGLObjectPopupMenu *
GUILaneWrapper::getPopUpMenu(GUIMainWindow &app,
                             GUISUMOAbstractView &parent) throw() {
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    SUMOReal pos = myShape.nearest_position_on_line_to_point(parent.getPositionInformation());
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUILaneWrapper::getParameterWindow(GUIMainWindow &app,
                                   GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("maxspeed [m/s]", false, myLane.maxSpeed());
    ret->mkItem("length [m]", false, myLane.length());
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUILaneWrapper::getMicrosimID() const throw() {
    return myLane.getID();
}


Boundary
GUILaneWrapper::getCenteringBoundary() const throw() {
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(20);
    return b;
}




const Position2DVector &
GUILaneWrapper::getShape() const {
    return myShape;
}


unsigned int
GUILaneWrapper::getLinkNumber() const {
    return (unsigned int) myLane.getLinkCont().size();
}


MSLink::LinkState
GUILaneWrapper::getLinkState(unsigned int pos) const throw() {
    return myLane.getLinkCont()[pos]->getState();
}


MSLink::LinkDirection
GUILaneWrapper::getLinkDirection(unsigned int pos) const {
    return myLane.getLinkCont()[pos]->getDirection();
}


MSLane *
GUILaneWrapper::getLinkLane(unsigned int pos) const {
    return myLane.getLinkCont()[pos]->getLane();
}


int
GUILaneWrapper::getLinkRespondIndex(unsigned int pos) const {
    return myLane.getLinkCont()[pos]->getRespondIndex();
}


const DoubleVector &
GUILaneWrapper::getShapeRotations() const {
    return myShapeRotations;
}


const DoubleVector &
GUILaneWrapper::getShapeLengths() const {
    return myShapeLengths;
}


unsigned int
GUILaneWrapper::getLinkTLID(const GUINet &net, unsigned int pos) const {
    return net.getLinkTLID(myLane.getLinkCont()[pos]);
}


int
GUILaneWrapper::getLinkTLIndex(const GUINet &net, unsigned int pos) const {
    return net.getLinkTLIndex(myLane.getLinkCont()[pos]);
}


const MSEdge * const
    GUILaneWrapper::getMSEdge() const {
    return myLane.getEdge();
}



SUMOReal
GUILaneWrapper::firstWaitingTime() const {
    return myLane.myVehicles.size()==0
           ? 0
           : (SUMOReal)(*(myLane.myVehicles.end()-1))->getWaitingTime();
}


SUMOReal
GUILaneWrapper::getDensity() const {
    return myLane.getDensity();
}


SUMOReal
GUILaneWrapper::getEdgeLaneNumber() const {
    return (SUMOReal) myLane.getEdge()->getLanes()->size();
}


SUMOReal
GUILaneWrapper::getHBEFA_CO2Emissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_CO2Emissions();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHBEFA_COEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_COEmissions();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHBEFA_PMxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_PMxEmissions();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHBEFA_NOxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_NOxEmissions();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHBEFA_HCEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_HCEmissions();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHBEFA_FuelConsumption() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += static_cast<GUIVehicle*>(*i)->getHBEFA_FuelConsumption();
    }
    myLane.releaseVehicles();
    return ret / myLane.length();
}


SUMOReal
GUILaneWrapper::getHarmonoise_NoiseEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = myLane.getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        SUMOReal sv = static_cast<GUIVehicle*>(*i)->getHarmonoise_NoiseEmissions();
        ret += (SUMOReal) pow(10., (sv/10.));
    }
    myLane.releaseVehicles();
    return HelpersHarmonoise::sum(ret);
}


GUILaneWrapper::Colorer::Colorer() {
    mySchemes.push_back(GUIColorScheme("uniform", RGBColor(0,0,0), "", true));
    mySchemes.push_back(GUIColorScheme("by selection (lane-/streetwise)", RGBColor(0.7f, 0.7f, 0.7f), "unselected", true));
    mySchemes.back().addColor(RGBColor(0, .4f, .8f), 1, "selected");
    mySchemes.push_back(GUIColorScheme("by allowed speed (lanewise)", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0/3.6));
    mySchemes.push_back(GUIColorScheme("by current density (lanewise)", RGBColor(0,0,1)));
    mySchemes.back().addColor(RGBColor(1, 0, 0), (SUMOReal)0.95);
    mySchemes.push_back(GUIColorScheme("by first vehicle waiting time (lanewise)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)200);
    mySchemes.push_back(GUIColorScheme("by lane number (streetwise)", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0,0,1), (SUMOReal)5);
    // ... emissions ...
    mySchemes.push_back(GUIColorScheme("by CO2 emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(10./7.5/5.));
    mySchemes.push_back(GUIColorScheme("by CO emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(0.05/7.5/2.));
    mySchemes.push_back(GUIColorScheme("by PMx emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(.005/7.5/5.));
    mySchemes.push_back(GUIColorScheme("by NOx emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(.125/7.5/5.));
    mySchemes.push_back(GUIColorScheme("by HC emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(.02/7.5/4.));
    mySchemes.push_back(GUIColorScheme("by fuel consumption (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(.005/7.5*100.));
    mySchemes.push_back(GUIColorScheme("by noise emissions (Harmonoise)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)100);
}


SUMOReal
GUILaneWrapper::Colorer::getColorValue(const GUILaneWrapper& lane) const {
    switch (myActiveScheme) {
    case 1:
        return gSelected.isSelected(lane.getType(), lane.getGlID());
    case 2:
        return lane.maxSpeed();
    case 3:
        return lane.getDensity();
    case 4:
        return lane.firstWaitingTime();
    case 5:
        return lane.getEdgeLaneNumber();
    case 6:
        return lane.getHBEFA_CO2Emissions();
    case 7:
        return lane.getHBEFA_COEmissions();
    case 8:
        return lane.getHBEFA_PMxEmissions();
    case 9:
        return lane.getHBEFA_NOxEmissions();
    case 10:
        return lane.getHBEFA_HCEmissions();
    case 11:
        return lane.getHBEFA_FuelConsumption();
    case 12:
        return lane.getHarmonoise_NoiseEmissions();
    }
    return 0;
}

/****************************************************************************/

