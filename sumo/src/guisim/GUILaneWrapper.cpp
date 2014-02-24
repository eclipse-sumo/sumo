/****************************************************************************/
/// @file    GUILaneWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#ifdef HAVE_OSG
#include <osg/Geometry>
#endif
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/geom/PositionVector.h>
#include <microsim/MSNet.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include "GUIEdge.h"
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
#include <utils/common/SUMOVehicleClass.h>
#include <utils/gui/div/GLHelper.h>
#include <gui/GUIViewTraffic.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <guisim/GUIVehicle.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/HelpersHarmonoise.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUILaneWrapper::GUILaneWrapper(MSLane& lane, const PositionVector& shape, unsigned int index) :
    GUIGlObject(GLO_LANE, lane.getID()),
    myLane(lane),
    myShape(shape),
    myIndex(index)
#ifdef HAVE_OSG
    , myGeom(0)
#endif
{
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo2D(s));
        myShapeRotations.push_back(RAD2DEG(atan2(s.x() - f.x(), f.y() - s.y())));
    }
    //
    myHalfLaneWidth = (SUMOReal)(myLane.getWidth() / 2.);
    myQuarterLaneWidth = (SUMOReal)(myLane.getWidth() / 4.);
}


GUILaneWrapper::~GUILaneWrapper() {}


bool
GUILaneWrapper::forLane(const MSLane& lane) const {
    return (&myLane) == (&lane);
}



void
GUILaneWrapper::drawLinkNo() const {
    unsigned int noLinks = getLinkNumber();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    SUMOReal w = myLane.getWidth() / (SUMOReal) noLinks;
    SUMOReal x1 = myLane.getWidth() / (SUMOReal) 2.;
    glPushMatrix();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        GLHelper::drawText(toString(getLane().getLinkCont()[i]->getRespondIndex()),
                           Position(x2, 0), 0, .6, RGBColor(128, 128, 255, 255), 180);
        x1 -= w;
    }
    glPopMatrix();
}


void
GUILaneWrapper::drawTLSLinkNo(const GUINet& net) const {
    unsigned int noLinks = getLinkNumber();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    SUMOReal w = myLane.getWidth() / (SUMOReal) noLinks;
    SUMOReal x1 = (SUMOReal)(myLane.getWidth() / 2.);
    glPushMatrix();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        int linkNo = net.getLinkTLIndex(getLane().getLinkCont()[i]);
        if (linkNo < 0) {
            continue;
        }
        GLHelper::drawText(toString(linkNo),
                           Position(x2, 0), 0, .6, RGBColor(128, 128, 255, 255), 180);
        x1 -= w;
    }
    glPopMatrix();
}


void
GUILaneWrapper::drawLinkRules(const GUINet& net) const {
    unsigned int noLinks = getLinkNumber();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
    if (noLinks == 0) {
        glPushName(getGlID());
        // draw a grey bar if no links are on the street
        glColor3d(0.5, 0.5, 0.5);
        glPushMatrix();
        glTranslated(end.x(), end.y(), 0);
        glRotated(rot, 0, 0, 1);
        glBegin(GL_QUADS);
        glVertex2d(-myHalfLaneWidth, 0.0);
        glVertex2d(-myHalfLaneWidth, 0.5);
        glVertex2d(myHalfLaneWidth, 0.5);
        glVertex2d(myHalfLaneWidth, 0.0);
        glEnd();
        glPopMatrix();
        glPopName();
        return;
    }
    // draw all links
    SUMOReal w = myLane.getWidth() / (SUMOReal) noLinks;
    SUMOReal x1 = 0;
    glPushMatrix();
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i = 0; i < noLinks; ++i) {
        SUMOReal x2 = x1 + w;
        MSLink* link = getLane().getLinkCont()[i];
        // select glID
        switch (link->getState()) {
            case LINKSTATE_TL_GREEN_MAJOR:
            case LINKSTATE_TL_GREEN_MINOR:
            case LINKSTATE_TL_RED:
            case LINKSTATE_TL_YELLOW_MAJOR:
            case LINKSTATE_TL_YELLOW_MINOR:
            case LINKSTATE_TL_OFF_BLINKING:
                glPushName(net.getLinkTLID(link));
                break;
            case LINKSTATE_MAJOR:
            case LINKSTATE_MINOR:
            case LINKSTATE_EQUAL:
            case LINKSTATE_TL_OFF_NOSIGNAL:
            default:
                glPushName(getGlID());
                break;
        }
        // select color
        switch (link->getState()) {
            case LINKSTATE_TL_GREEN_MAJOR:
            case LINKSTATE_TL_GREEN_MINOR:
                glColor3d(0, 1, 0);
                break;
            case LINKSTATE_TL_RED:
                glColor3d(1, 0, 0);
                break;
            case LINKSTATE_TL_YELLOW_MAJOR:
            case LINKSTATE_TL_YELLOW_MINOR:
                glColor3d(1, 1, 0);
                break;
            case LINKSTATE_TL_OFF_BLINKING:
                glColor3d(.7, .7, 0);
                break;
            case LINKSTATE_TL_OFF_NOSIGNAL:
                glColor3d(0, 1, 1);
                break;
            case LINKSTATE_MAJOR:
                glColor3d(1, 1, 1);
                break;
            case LINKSTATE_MINOR:
                glColor3d(.2, .2, .2);
                break;
            case LINKSTATE_EQUAL:
                glColor3d(.5, .5, .5);
                break;
            case LINKSTATE_DEADEND:
                glColor3d(0, 0, 0);
                break;
        }
        glBegin(GL_QUADS);
        glVertex2d(x1 - myHalfLaneWidth, 0.0);
        glVertex2d(x1 - myHalfLaneWidth, 0.5);
        glVertex2d(x2 - myHalfLaneWidth, 0.5);
        glVertex2d(x2 - myHalfLaneWidth, 0.0);
        glEnd();
        glPopName();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
GUILaneWrapper::drawArrows() const {
    unsigned int noLinks = getLinkNumber();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    const Position& end = getShape().back();
    const Position& f = getShape()[-2];
    SUMOReal rot = (SUMOReal) atan2((end.x() - f.x()), (f.y() - end.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
    glPushMatrix();
    glPushName(0);
    glColor3d(1, 1, 1);
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i = 0; i < noLinks; ++i) {
        LinkDirection dir = getLane().getLinkCont()[i]->getDirection();
        LinkState state = getLane().getLinkCont()[i]->getState();
        if (state == LINKSTATE_TL_OFF_NOSIGNAL || dir == LINKDIR_NODIR) {
            continue;
        }
        switch (dir) {
            case LINKDIR_STRAIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0, 4), Position(0, 1)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_TURN:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 90, .5, .05);
                GLHelper::drawBoxLine(Position(0.5, 2.5), 180, 1, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0.5, 2.5), Position(0.5, 4)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_LEFT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 90, 1, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0, 2.5), Position(1.5, 2.5)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_RIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0, 2.5), Position(-1.5, 2.5)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_PARTLEFT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 45, .7, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0, 2.5), Position(1.2, 1.3)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_PARTRIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), -45, .7, .05);
                GLHelper::drawTriangleAtEnd(Line(Position(0, 2.5), Position(-1.2, 1.3)), (SUMOReal) 1, (SUMOReal) .25);
                break;
            default:
                break;
        }
    }
    glPopMatrix();
    glPopName();
}


void
GUILaneWrapper::drawLane2LaneConnections() const {
    unsigned int noLinks = getLinkNumber();
    for (unsigned int i = 0; i < noLinks; ++i) {
        LinkState state = getLane().getLinkCont()[i]->getState();
        const MSLane* connected = getLane().getLinkCont()[i]->getLane();
        if (connected == 0) {
            continue;
        }
        switch (state) {
            case LINKSTATE_TL_GREEN_MAJOR:
            case LINKSTATE_TL_GREEN_MINOR:
                glColor3d(0, 1, 0);
                break;
            case LINKSTATE_TL_RED:
                glColor3d(1, 0, 0);
                break;
            case LINKSTATE_TL_YELLOW_MAJOR:
            case LINKSTATE_TL_YELLOW_MINOR:
                glColor3d(1, 1, 0);
                break;
            case LINKSTATE_TL_OFF_BLINKING:
                glColor3d(1, 1, 0);
                break;
            case LINKSTATE_TL_OFF_NOSIGNAL:
                glColor3d(0, 1, 1);
                break;
            case LINKSTATE_MAJOR:
                glColor3d(1, 1, 1);
                break;
            case LINKSTATE_MINOR:
                glColor3d(.2, .2, .2);
                break;
            case LINKSTATE_EQUAL:
                glColor3d(.5, .5, .5);
                break;
            case LINKSTATE_DEADEND:
                glColor3d(0, 0, 0);
                break;
        }

        glBegin(GL_LINES);
        const Position& p1 = getShape()[-1];
        const Position& p2 = connected->getShape()[0];
        glVertex2f(p1.x(), p1.y());
        glVertex2f(p2.x(), p2.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(Line(p1, p2), (SUMOReal) .4, (SUMOReal) .2);
    }
}


void
GUILaneWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    const bool isInternal = getLane().getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL;
    bool mustDrawMarkings = false;
    const bool drawDetails =  s.scale * s.laneWidthExaggeration > 5;
    if (isInternal) {
        // draw internal lanes on top of junctions
        glTranslated(0, 0, GLO_JUNCTION + 0.1);
    } else {
        glTranslated(0, 0, getType());
    }
    // set lane color
    if (!MSGlobals::gUseMesoSim) {
        setColor(s);
        glPushName(getGlID()); // do not register for clicks in MESOSIM
    }
    // draw lane
    // check whether it is not too small
    if (s.scale * s.laneWidthExaggeration < 1.) {
        GLHelper::drawLine(myShape);
        if (!MSGlobals::gUseMesoSim) {
            glPopName();
        }
        glPopMatrix();
    } else if (isRailway(getLane().getPermissions())) {
        // draw as railway
        const SUMOReal halfRailWidth = 0.725;
        GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, halfRailWidth * s.laneWidthExaggeration);
        glColor3d(1, 1, 1);
        glTranslated(0, 0, .1);
        GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, (halfRailWidth - 0.2) * s.laneWidthExaggeration);
        drawCrossties(s);
        if (!MSGlobals::gUseMesoSim) {
            glPopName();
        }
        glPopMatrix();
    } else {
        const SUMOReal laneWidth = isInternal ? myQuarterLaneWidth : myHalfLaneWidth;
        mustDrawMarkings = !isInternal;
        GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, laneWidth * s.laneWidthExaggeration);
        if (!MSGlobals::gUseMesoSim) {
            glPopName();
        }
        glPopMatrix();
        // draw ROWs (not for inner lanes)
        if (!isInternal && drawDetails) {
            glPushMatrix();
            glTranslated(0, 0, GLO_JUNCTION); // must draw on top of junction shape
            GUINet* net = (GUINet*) MSNet::getInstance();
            glTranslated(0, 0, .2);
            drawLinkRules(*net);
            if (s.showLinkDecals) {
                drawArrows();
            }
            if (s.showLane2Lane) {
                // this should be independent to the geometry:
                //  draw from end of first to the begin of second
                drawLane2LaneConnections();
            }
            glTranslated(0, 0, .1);
            if (s.drawLinkJunctionIndex) {
                drawLinkNo();
            }
            if (s.drawLinkTLIndex) {
                drawTLSLinkNo(*net);
            }
            glPopMatrix();
        }
    }
    if (mustDrawMarkings && drawDetails) { // needs matrix reset
        drawMarkings(s);
    }
    // draw vehicles
    if (s.scale > s.minVehicleSize) {
        // retrieve vehicles from lane; disallow simulation
        const MSLane::VehCont& vehicles = myLane.getVehiclesSecure();
        for (MSLane::VehCont::const_iterator v = vehicles.begin(); v != vehicles.end(); ++v) {
            if ((*v)->getLane() == &myLane) {
                static_cast<const GUIVehicle* const>(*v)->drawGL(s);
            } // else: this is the shadow during a continuous lane change
        }
        // draw parking vehicles
        const std::set<const MSVehicle*> parking = MSVehicleTransfer::getInstance()->getParkingVehicles(&myLane);
        for (std::set<const MSVehicle*>::const_iterator v = parking.begin(); v != parking.end(); ++v) {
            static_cast<const GUIVehicle* const>(*v)->drawGL(s);
        }
        // allow lane simulation
        myLane.releaseVehicles();
    }
}


void
GUILaneWrapper::drawMarkings(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(0);
    glTranslated(0, 0, GLO_EDGE);
#ifdef HAVE_INTERNAL
    if (!MSGlobals::gUseMesoSim)
#endif
        setColor(s);
    // optionally draw inverse markings
    if (myIndex > 0) {
        SUMOReal mw = myHalfLaneWidth + SUMO_const_laneOffset + .01;
        int e = (int) getShape().size() - 1;
        for (int i = 0; i < e; ++i) {
            glPushMatrix();
            glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
            glRotated(myShapeRotations[i], 0, 0, 1);
            for (SUMOReal t = 0; t < myShapeLengths[i]; t += 6) {
                glBegin(GL_QUADS);
                glVertex2d(-mw, -t);
                glVertex2d(-mw, -t - 3.);
                glVertex2d(myQuarterLaneWidth, -t - 3.);
                glVertex2d(myQuarterLaneWidth, -t);
                glEnd();
            }
            glPopMatrix();
        }
    }
    // draw white boundings and white markings
    glColor3d(1, 1, 1);
    GLHelper::drawBoxLines(
        getShape(),
        getShapeRotations(),
        getShapeLengths(),
        getHalfWidth() + SUMO_const_laneOffset);
    glPopMatrix();
    glPopName();
}


void
GUILaneWrapper::drawCrossties(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(0);
    if (!MSGlobals::gUseMesoSim) {
        setColor(s);
    }
    // draw on top of of the white area between the rails
    glTranslated(0, 0, 0.1);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += 1) {
            glBegin(GL_QUADS);
            glVertex2d(-1, -t);
            glVertex2d(-1, -t - 0.3);
            glVertex2d(1.0, -t - 0.3);
            glVertex2d(1.0, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
    glPopName();
}

GUIGLObjectPopupMenu*
GUILaneWrapper::getPopUpMenu(GUIMainWindow& app,
                             GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    const SUMOReal pos = myLane.interpolateGeometryPosToLanePos(
                             myShape.nearest_offset_to_point2D(parent.getPositionInformation()));
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUILaneWrapper::getParameterWindow(GUIMainWindow& app,
                                   GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 4);
    // add items
    ret->mkItem("maxspeed [m/s]", false, myLane.getSpeedLimit());
    ret->mkItem("length [m]", false, myLane.getLength());
    ret->mkItem("permissions", false, getAllowedVehicleClassNames(myLane.getPermissions()));
    ret->mkItem("street name", false, myLane.getEdge().getStreetName());
    ret->mkItem("stored traveltime [s]", true, new FunctionBinding<GUILaneWrapper, SUMOReal>(this, &GUILaneWrapper::getStoredEdgeTravelTime));
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUILaneWrapper::getCenteringBoundary() const {
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(20);
    return b;
}




const PositionVector&
GUILaneWrapper::getShape() const {
    return myShape;
}


unsigned int
GUILaneWrapper::getLinkNumber() const {
    return (unsigned int) myLane.getLinkCont().size();
}


const std::vector<SUMOReal>&
GUILaneWrapper::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<SUMOReal>&
GUILaneWrapper::getShapeLengths() const {
    return myShapeLengths;
}


SUMOReal
GUILaneWrapper::firstWaitingTime() const {
    return myLane.myVehicles.size() == 0
           ? 0
           : (*(myLane.myVehicles.end() - 1))->getWaitingSeconds();
}


SUMOReal
GUILaneWrapper::getEdgeLaneNumber() const {
    return (SUMOReal) myLane.getEdge().getLanes().size();
}


// ------------ Current state retrieval
SUMOReal
GUILaneWrapper::getNormedHBEFA_CO2Emissions() const {
    return myLane.getHBEFA_CO2Emissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedHBEFA_COEmissions() const {
    return myLane.getHBEFA_COEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedHBEFA_PMxEmissions() const {
    return myLane.getHBEFA_PMxEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedHBEFA_NOxEmissions() const {
    return myLane.getHBEFA_NOxEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedHBEFA_HCEmissions() const {
    return myLane.getHBEFA_HCEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedHBEFA_FuelConsumption() const {
    return myLane.getHBEFA_FuelConsumption() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_CO2Emissions() const {
    return myLane.getPHEMlight_CO2Emissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_COEmissions() const {
    return myLane.getPHEMlight_COEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_PMxEmissions() const {
    return myLane.getPHEMlight_PMxEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_NOxEmissions() const {
    return myLane.getPHEMlight_NOxEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_HCEmissions() const {
    return myLane.getPHEMlight_HCEmissions() / myLane.getLength();
}


SUMOReal
GUILaneWrapper::getNormedPHEMlight_FuelConsumption() const {
    return myLane.getPHEMlight_FuelConsumption() / myLane.getLength();
}


// ------------ 
void
GUILaneWrapper::setColor(const GUIVisualizationSettings& s) const {
    GLHelper::setColor(s.laneColorer.getScheme().getColor(getColorValue(s.laneColorer.getActive())));
}


SUMOReal
GUILaneWrapper::getStoredEdgeTravelTime() const {
    MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
    MSEdge& e = getLane().getEdge();
    if (!ews.knowsTravelTime(&e)) {
        return -1;
    } else {
        SUMOReal value(0);
        ews.retrieveExistingTravelTime(&e, 0, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), value);
        return value;
    }
}

SUMOReal
GUILaneWrapper::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 1:
            return (gSelected.isSelected(getType(), getGlID()) ||
                    gSelected.isSelected(GLO_EDGE, dynamic_cast<GUIEdge*>(&(getLane().getEdge()))->getGlID()));
        case 2: {
            if (getLane().allowsVehicleClass(SVC_PASSENGER)) {
                return 0;
            } else {
                return 1;
            }
        }
        case 3:
            return getLane().getSpeedLimit();
        case 4:
            return getLane().getOccupancy();
        case 5:
            return firstWaitingTime();
        case 6:
            return getEdgeLaneNumber();
        case 7:
            return getNormedHBEFA_CO2Emissions();
        case 8:
            return getNormedHBEFA_COEmissions();
        case 9:
            return getNormedHBEFA_PMxEmissions();
        case 10:
            return getNormedHBEFA_NOxEmissions();
        case 11:
            return getNormedHBEFA_HCEmissions();
        case 12:
            return getNormedHBEFA_FuelConsumption();
        case 13:
            return getLane().getHarmonoise_NoiseEmissions();
        case 14:
            return getNormedPHEMlight_CO2Emissions();
        case 15:
            return getNormedPHEMlight_COEmissions();
        case 16:
            return getNormedPHEMlight_PMxEmissions();
        case 17:
            return getNormedPHEMlight_NOxEmissions();
        case 18:
            return getNormedPHEMlight_HCEmissions();
        case 19:
            return getNormedPHEMlight_FuelConsumption();
        case 20: {
            return getStoredEdgeTravelTime();
        }
        case 21: {
            MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
            MSEdge& e = getLane().getEdge();
            if (!ews.knowsTravelTime(&e)) {
                return -1;
            } else {
                SUMOReal value(0);
                ews.retrieveExistingTravelTime(&e, 0, 0, value);
                return 100 * getLane().getLength() / value / getLane().getSpeedLimit();
            }
        }
    }
    return 0;
}

#ifdef HAVE_OSG
void
GUILaneWrapper::updateColor(const GUIVisualizationSettings& s) {
    const RGBColor& col = s.laneColorer.getScheme().getColor(getColorValue(s.laneColorer.getActive()));
    osg::Vec4ubArray* colors = dynamic_cast<osg::Vec4ubArray*>(myGeom->getColorArray());
    (*colors)[0].set(col.red(), col.green(), col.blue(), col.alpha());
    myGeom->setColorArray(colors);
}
#endif


/****************************************************************************/

