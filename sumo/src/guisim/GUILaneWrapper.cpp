/****************************************************************************/
/// @file    GUILaneWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
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


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
        myLane(lane), myShape(shape)
{
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
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265);
    }
}


GUILaneWrapper::~GUILaneWrapper() throw()
{}


SUMOReal
GUILaneWrapper::getLength() const
{
    return myLane.myLength;
}


SUMOReal
GUILaneWrapper::visLength() const
{
    return myVisLength;
}


MSEdge::EdgeBasicFunction
GUILaneWrapper::getPurpose() const
{
    return myLane.myEdge->getPurpose();
}


SUMOReal
GUILaneWrapper::maxSpeed() const
{
    return myLane.maxSpeed();
}


SUMOReal
GUILaneWrapper::getOverallMaxSpeed()
{
    return myAllMaxSpeed;
}


bool
GUILaneWrapper::forLane(const MSLane &lane) const
{
    return (&myLane)==(&lane);
}



void
ROWdrawAction_drawLinkNo(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
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
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (size_t i=0; i<noLinks; ++i) {
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
ROWdrawAction_drawTLSLinkNo(const GUINet &net, const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
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
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (size_t i=0; i<noLinks; ++i) {
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
                            bool showToolTips)
{
    size_t noLinks = lane.getLinkNumber();
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
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
    for (size_t i=0; i<noLinks; ++i) {
        SUMOReal x2 = x1 + w;
        MSLink::LinkState state = lane.getLinkState(i);
        if (showToolTips) {
            switch (state) {
            case MSLink::LINKSTATE_TL_GREEN:
            case MSLink::LINKSTATE_TL_RED:
            case MSLink::LINKSTATE_TL_YELLOW:
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
        case MSLink::LINKSTATE_TL_GREEN:
            glColor3d(0, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_RED:
            glColor3d(1, 0, 0);
            break;
        case MSLink::LINKSTATE_TL_YELLOW:
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
ROWdrawAction_drawArrows(const GUILaneWrapper &lane, bool showToolTips)
{
    size_t noLinks = lane.getLinkNumber();
    if (noLinks==0) {
        return;
    }
    // draw all links
    const Position2D &end = lane.getShape().getEnd();
    const Position2D &f = lane.getShape()[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
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
    for (size_t i=0; i<noLinks; ++i) {
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
ROWdrawAction_drawLane2LaneConnections(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    for (size_t i=0; i<noLinks; ++i) {
        MSLink::LinkState state = lane.getLinkState(i);
        const MSLane *connected = lane.getLinkLane(i);
        if(connected==0) {
            continue;
        }
        switch (state) {
        case MSLink::LINKSTATE_TL_GREEN:
            glColor3d(0, 1, 0);
            break;
        case MSLink::LINKSTATE_TL_RED:
            glColor3d(1, 0, 0);
            break;
        case MSLink::LINKSTATE_TL_YELLOW:
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
GUILaneWrapper::drawGL(const GUIVisualizationSettings &s) const throw()
{
    // set lane color
    GUIColoringSchemesMap<GUILaneWrapper> &sm = GUIViewTraffic::getLaneSchemesMap(); //!!!
    sm.getColorer(s.laneEdgeMode)->setGlColor(*this);
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
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, SUMO_const_halfLaneWidth*1.);
        } else {
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, SUMO_const_quarterLaneWidth*1.);
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

        MSLane::VehCont::const_iterator v;
        glPushMatrix();
        glTranslated(laneBeg.x(), laneBeg.y(), 0);
        glRotated(myShapeRotations[0], 0, 0, 1);
        // go through the vehicles
        int shapePos = 0;
        SUMOReal positionOffset = 0;
        for (v=vehicles.begin(); v!=vehicles.end(); v++) {
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
GUILaneWrapper::drawBordersGL(const GUIVisualizationSettings &s) const throw()
{
    GUIColoringSchemesMap<GUILaneWrapper> &sm = GUIViewTraffic::getLaneSchemesMap(); //!!!
    sm.getColorer(s.laneEdgeMode)->setGlColor(*this);
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
                             GUISUMOAbstractView &parent) throw()
{
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
    return ret;
}


GUIParameterTableWindow *
GUILaneWrapper::getParameterWindow(GUIMainWindow &app,
                                   GUISUMOAbstractView &) throw()
{
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
GUILaneWrapper::getMicrosimID() const throw()
{
    return myLane.getID();
}


Boundary
GUILaneWrapper::getCenteringBoundary() const throw()
{
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(20);
    return b;
}




const Position2DVector &
GUILaneWrapper::getShape() const
{
    return myShape;
}


size_t
GUILaneWrapper::getLinkNumber() const
{
    return myLane.getLinkCont().size();
}


MSLink::LinkState
GUILaneWrapper::getLinkState(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getState();
}


MSLink::LinkDirection
GUILaneWrapper::getLinkDirection(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getDirection();
}


MSLane *
GUILaneWrapper::getLinkLane(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getLane();
}


int
GUILaneWrapper::getLinkRespondIndex(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getRespondIndex();
}


const DoubleVector &
GUILaneWrapper::getShapeRotations() const
{
    return myShapeRotations;
}


const DoubleVector &
GUILaneWrapper::getShapeLengths() const
{
    return myShapeLengths;
}


const MSLane::VehCont &
GUILaneWrapper::getVehiclesSecure()
{
    return myLane.getVehiclesSecure();
}


void
GUILaneWrapper::releaseVehicles()
{
    myLane.releaseVehicles();
}


unsigned int
GUILaneWrapper::getLinkTLID(const GUINet &net, size_t pos) const
{
    return net.getLinkTLID(myLane.getLinkCont()[pos]);
}


int
GUILaneWrapper::getLinkTLIndex(const GUINet &net, size_t pos) const
{
    return net.getLinkTLIndex(myLane.getLinkCont()[pos]);
}


const MSEdge * const
GUILaneWrapper::getMSEdge() const
{
    return myLane.getEdge();
}



#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>

void
GUILaneWrapper::selectSucessors()
{
    SUMOReal maxDist = 2000;
    SUMOReal minDist = 1000;
    SUMOReal maxSpeed = 55.0;

    std::vector<GUILaneWrapper*> selected;
    selected.push_back(this);
    std::vector<std::pair<GUILaneWrapper*, SUMOReal> > toProc;
    toProc.push_back(std::pair<GUILaneWrapper*, SUMOReal>(this, 0));

    while (!toProc.empty()) {
        std::pair<GUILaneWrapper*, SUMOReal> laneAndDist =
            toProc.back();
        toProc.pop_back();
        if (laneAndDist.second<minDist||
                (laneAndDist.second<maxDist&&laneAndDist.first->maxSpeed()<maxSpeed)) {
            selected.push_back(laneAndDist.first);

            const GUIEdge * const e = static_cast<const GUIEdge * const>(laneAndDist.first->getMSEdge());
            std::vector<MSEdge*> followingEdges = e->getFollowingEdges();
            std::vector<MSEdge*> incomingEdges = e->getIncomingEdges();
            copy(incomingEdges.begin(), incomingEdges.end(), back_inserter(followingEdges));
            for (std::vector<MSEdge*>::iterator i=followingEdges.begin(); i!=followingEdges.end(); ++i) {
                const std::vector<MSLane*> * const lanes = (*i)->getLanes();
                for (std::vector<MSLane*>::const_iterator j=lanes->begin(); j!=lanes->end(); ++j) {
                    if (find(selected.begin(), selected.end(), &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j))==selected.end()) {
                        toProc.push_back(std::pair<GUILaneWrapper*, SUMOReal>(
                                             &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j),
                                             laneAndDist.second+laneAndDist.first->getLength()));
                    }
                }
            }
        }
    }

    for (std::vector<GUILaneWrapper*>::iterator k=selected.begin(); k!=selected.end(); ++k) {
        gSelected.select((*k)->getType(), (*k)->getGlID());
    }

    const Position2DVector &shape = getShape();
    Position2D initPos = shape.positionAtLengthPosition(getLength()/(SUMOReal) 2.0);
    Position2DVector poly;
    for (SUMOReal i=0; i<360; i += 40) {
        SUMOReal random1 = RandHelper::rand();
        Position2D p = initPos;
        p.add(sin(i)*30+random1*20, cos(i)*30+random1*20);
        poly.push_back(p);
    }
}


SUMOReal
GUILaneWrapper::firstWaitingTime() const
{
    return myLane.myVehicles.size()==0
           ? 0
           : (SUMOReal)(*(myLane.myVehicles.end()-1))->getWaitingTime();
}


SUMOReal
GUILaneWrapper::getDensity() const
{
    return myLane.getDensity();
}


SUMOReal
GUILaneWrapper::getEdgeLaneNumber() const
{
    return (SUMOReal) myLane.getEdge()->getLanes()->size();
}


/****************************************************************************/

