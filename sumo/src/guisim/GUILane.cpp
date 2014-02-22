/****************************************************************************/
/// @file    GUILane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Representation of a lane in the micro simulation (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utility>
#include <utils/foxtools/MFXMutex.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Position.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include "GUILane.h"
#include "GUIEdge.h"
#include "GUIVehicle.h"
#include "GUINet.h"

#ifdef HAVE_OSG
#include <osg/Geometry>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUILane::GUILane(const std::string& id, SUMOReal maxSpeed, SUMOReal length,
                 MSEdge* const edge, unsigned int numericalID,
                 const PositionVector& shape, SUMOReal width,
                 SVCPermissions permissions, unsigned int index)
    : MSLane(id, maxSpeed, length, edge, numericalID, shape, width, permissions),
      GUIGlObject(GLO_LANE, id) {
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
    myHalfLaneWidth = (SUMOReal)(myWidth / 2.);
    myQuarterLaneWidth = (SUMOReal)(myWidth / 4.);
    myIndex = index;
}


GUILane::~GUILane() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle insertion ------
void
GUILane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed,
                            const MSLane::VehCont::iterator& at,
                            MSMoveReminder::Notification notification) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::incorporateVehicle(veh, pos, speed, at, notification);
}


// ------ Access to vehicles ------
const MSLane::VehCont&
GUILane::getVehiclesSecure() const {
    myLock.lock();
    return myVehicles;
}


void
GUILane::releaseVehicles() const {
    myLock.unlock();
}


void
GUILane::planMovements(const SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::planMovements(t);
}


bool
GUILane::executeMovements(SUMOTime t, std::vector<MSLane*>& into) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::executeMovements(t, into);
}


MSVehicle*
GUILane::removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::removeVehicle(remVehicle, notification);
}


void
GUILane::swapAfterLaneChange(SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::swapAfterLaneChange(t);
}


bool
GUILane::integrateNewVehicle(SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::integrateNewVehicle(t);
}


void
GUILane::detectCollisions(SUMOTime timestep, const std::string& stage) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::detectCollisions(timestep, stage);
}


// ------ Drawing methods ------
void
GUILane::drawLinkNo() const {
    unsigned int noLinks = (unsigned int)myLinks.size();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = myHalfLaneWidth;
    glPushMatrix();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    const SUMOReal rot = RAD2DEG(atan2((s.x() - f.x()), (f.y() - s.y())));
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        GLHelper::drawText(toString(myLinks[i]->getIndex()),
                           Position(x2, 0), 0, .6, RGBColor(128, 128, 255, 255), 180);
        x1 -= w;
    }
    glPopMatrix();
}


void
GUILane::drawTLSLinkNo(const GUINet& net) const {
    unsigned int noLinks = (unsigned int)myLinks.size();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = myHalfLaneWidth;
    glPushMatrix();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    const SUMOReal rot = RAD2DEG(atan2((s.x() - f.x()), (f.y() - s.y())));
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        int linkNo = net.getLinkTLIndex(myLinks[i]);
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
GUILane::drawLinkRules(const GUINet& net) const {
    unsigned int noLinks = (unsigned int)myLinks.size();
    const PositionVector& g = getShape();
    const Position& end = g.back();
    const Position& f = g[-2];
    const Position& s = end;
    const SUMOReal rot = RAD2DEG(atan2((s.x() - f.x()), (f.y() - s.y())));
    if (noLinks == 0) {
        glPushName(getGlID());
        GLHelper::setColor(getLinkColor(LINKSTATE_DEADEND));
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
    const bool railway = isRailway(myPermissions);
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = 0;
    glPushMatrix();
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (unsigned int i = 0; i < noLinks; ++i) {
        SUMOReal x2 = x1 + w;
        MSLink* link = myLinks[i];
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
        GLHelper::setColor(getLinkColor(link->getState()));
        if (!railway || link->getState() != LINKSTATE_MAJOR) {
            // THE WHITE BAR SHOULD BE THE DEFAULT FOR MOST RAILWAY
            // LINKS AND LOOKS UGLY SO WE DO NOT DRAW IT
            glBegin(GL_QUADS);
            glVertex2d(x1 - myHalfLaneWidth, 0.0);
            glVertex2d(x1 - myHalfLaneWidth, 0.5);
            glVertex2d(x2 - myHalfLaneWidth, 0.5);
            glVertex2d(x2 - myHalfLaneWidth, 0.0);
            glEnd();
        }
        glPopName();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
GUILane::drawArrows() const {
    if (myLinks.size() == 0) {
        return;
    }
    // draw all links
    const Position& end = getShape().back();
    const Position& f = getShape()[-2];
    const SUMOReal rot = RAD2DEG(atan2((end.x() - f.x()), (f.y() - end.y())));
    glPushMatrix();
    glPushName(0);
    glColor3d(1, 1, 1);
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    for (std::vector<MSLink*>::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        LinkDirection dir = (*i)->getDirection();
        LinkState state = (*i)->getState();
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
GUILane::drawLane2LaneConnections() const {
    for (std::vector<MSLink*>::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        LinkState state = (*i)->getState();
        const MSLane* connected = (*i)->getLane();
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
            case LINKSTATE_STOP:
                glColor3d(.4, .2, .2);
                break;
            case LINKSTATE_EQUAL:
                glColor3d(.5, .5, .5);
                break;
            case LINKSTATE_ALLWAY_STOP:
                glColor3d(.2, .2, .4);
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
GUILane::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    const bool isInternal = myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL;
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
    } else {
        if (isRailway(myPermissions)) {
            // draw as railway
            const SUMOReal halfRailWidth = 0.725;
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, halfRailWidth * s.laneWidthExaggeration);
            glColor3d(1, 1, 1);
            glTranslated(0, 0, .1);
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, (halfRailWidth - 0.2) * s.laneWidthExaggeration);
            drawCrossties(s);
        } else {
            const SUMOReal laneWidth = isInternal ? myQuarterLaneWidth : myHalfLaneWidth;
            mustDrawMarkings = !isInternal;
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, laneWidth * s.laneWidthExaggeration);
        }
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
            if (s.showLinkDecals && !isRailway(myPermissions)) {
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
        const MSLane::VehCont& vehicles = getVehiclesSecure();
        for (MSLane::VehCont::const_iterator v = vehicles.begin(); v != vehicles.end(); ++v) {
            if ((*v)->getLane() == this) {
                static_cast<const GUIVehicle* const>(*v)->drawGL(s);
            } // else: this is the shadow during a continuous lane change
        }
        // draw parking vehicles
        const std::set<const MSVehicle*> parking = MSVehicleTransfer::getInstance()->getParkingVehicles(this);
        for (std::set<const MSVehicle*>::const_iterator v = parking.begin(); v != parking.end(); ++v) {
            static_cast<const GUIVehicle* const>(*v)->drawGL(s);
        }
        // allow lane simulation
        releaseVehicles();
    }
}


void
GUILane::drawMarkings(const GUIVisualizationSettings& s) const {
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
GUILane::drawCrossties(const GUIVisualizationSettings& s) const {
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

// ------ inherited from GUIGlObject
GUIGLObjectPopupMenu*
GUILane::getPopUpMenu(GUIMainWindow& app,
                      GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    const SUMOReal pos = interpolateGeometryPosToLanePos(myShape.nearest_offset_to_point2D(parent.getPositionInformation()));
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUILane::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 5);
    // add items
    ret->mkItem("maxspeed [m/s]", false, getSpeedLimit());
    ret->mkItem("length [m]", false, myLength);
    ret->mkItem("street name", false, myEdge->getStreetName());
    ret->mkItem("stored traveltime [s]", true, new FunctionBinding<GUILane, SUMOReal>(this, &GUILane::getStoredEdgeTravelTime));
    ret->mkItem("allowed vehicle class", false, getAllowedVehicleClassNames(myPermissions));
    ret->mkItem("disallowed vehicle class", false, getAllowedVehicleClassNames(~myPermissions));
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUILane::getCenteringBoundary() const {
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(20);
    return b;
}







const PositionVector&
GUILane::getShape() const {
    return myShape;
}


const std::vector<SUMOReal>&
GUILane::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<SUMOReal>&
GUILane::getShapeLengths() const {
    return myShapeLengths;
}


SUMOReal
GUILane::firstWaitingTime() const {
    return myVehicles.size() == 0 ? 0 : myVehicles.back()->getWaitingSeconds();
}


SUMOReal
GUILane::getEdgeLaneNumber() const {
    return (SUMOReal) myEdge->getLanes().size();
}


// ------------ Current state retrieval
SUMOReal
GUILane::getNormedHBEFA_CO2Emissions() const {
    return getHBEFA_CO2Emissions() / getLength();
}


SUMOReal
GUILane::getNormedHBEFA_COEmissions() const {
    return getHBEFA_COEmissions() / getLength();
}


SUMOReal
GUILane::getNormedHBEFA_PMxEmissions() const {
    return getHBEFA_PMxEmissions() / getLength();
}


SUMOReal
GUILane::getNormedHBEFA_NOxEmissions() const {
    return getHBEFA_NOxEmissions() / getLength();
}


SUMOReal
GUILane::getNormedHBEFA_HCEmissions() const {
    return getHBEFA_HCEmissions() / getLength();
}


SUMOReal
GUILane::getNormedHBEFA_FuelConsumption() const {
    return getHBEFA_FuelConsumption() / getLength();
}


void
GUILane::setColor(const GUIVisualizationSettings& s) const {
    GLHelper::setColor(s.laneColorer.getScheme().getColor(getColorValue(s.laneColorer.getActive())));
}


SUMOReal
GUILane::getStoredEdgeTravelTime() const {
    MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
    if (!ews.knowsTravelTime(myEdge)) {
        return -1;
    } else {
        SUMOReal value(0);
        ews.retrieveExistingTravelTime(myEdge, 0, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), value);
        return value;
    }
}

SUMOReal
GUILane::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 1:
            return gSelected.isSelected(getType(), getGlID()) ||
                   gSelected.isSelected(GLO_EDGE, dynamic_cast<GUIEdge*>(myEdge)->getGlID());
        case 2:
            return (SUMOReal)myPermissions;
        case 3:
            return getSpeedLimit();
        case 4:
            return getBruttoOccupancy();
        case 5:
            return getNettoOccupancy();
        case 6:
            return firstWaitingTime();
        case 7:
            return getEdgeLaneNumber();
        case 8:
            return getNormedHBEFA_CO2Emissions();
        case 9:
            return getNormedHBEFA_COEmissions();
        case 10:
            return getNormedHBEFA_PMxEmissions();
        case 11:
            return getNormedHBEFA_NOxEmissions();
        case 12:
            return getNormedHBEFA_HCEmissions();
        case 13:
            return getNormedHBEFA_FuelConsumption();
        case 14:
            return getHarmonoise_NoiseEmissions();
        case 15: {
            return getStoredEdgeTravelTime();
        }
        case 16: {
            MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
            if (!ews.knowsTravelTime(myEdge)) {
                return -1;
            } else {
                SUMOReal value(0);
                ews.retrieveExistingTravelTime(myEdge, 0, 0, value);
                return 100 * myLength / value / getSpeedLimit();
            }
        }
        case 17: {
            return 1 / myLengthGeometryFactor;
        }
    }
    return 0;
}

#ifdef HAVE_OSG
void
GUILane::updateColor(const GUIVisualizationSettings& s) {
    const RGBColor& col = s.laneColorer.getScheme().getColor(getColorValue(s.laneColorer.getActive()));
    osg::Vec4ubArray* colors = dynamic_cast<osg::Vec4ubArray*>(myGeom->getColorArray());
    (*colors)[0].set(col.red(), col.green(), col.blue(), col.alpha());
    myGeom->setColorArray(colors);
}
#endif



/****************************************************************************/

