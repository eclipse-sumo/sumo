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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
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

//#define GUILane_DEBUG_DRAW_WALKING_AREA_VERTICES
//#define GUILane_DEBUG_DRAW_VERTICES

// ===========================================================================
// method definitions
// ===========================================================================
GUILane::GUILane(const std::string& id, SUMOReal maxSpeed, SUMOReal length,
                 MSEdge* const edge, int numericalID,
                 const PositionVector& shape, SUMOReal width,
                 SVCPermissions permissions, int index) :
    MSLane(id, maxSpeed, length, edge, numericalID, shape, width, permissions, index),
    GUIGlObject(GLO_LANE, id),
    myAmClosed(false) {
    if (MSGlobals::gUseMesoSim) {
        myShape = splitAtSegments(shape);
        assert(fabs(myShape.length() - shape.length()) < POSITION_EPS);
        assert(myShapeSegments.size() == myShape.size());
    }
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    myShapeColors.reserve(myShape.size() - 1);
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
}


GUILane::~GUILane() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle insertion ------
void
GUILane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed, SUMOReal posLat,
                            const MSLane::VehCont::iterator& at,
                            MSMoveReminder::Notification notification) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::incorporateVehicle(veh, pos, speed, posLat, at, notification);
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
GUILane::removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification, bool notify) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::removeVehicle(remVehicle, notification, notify);
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


SUMOReal
GUILane::setPartialOccupation(MSVehicle* v) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::setPartialOccupation(v);
}


void
GUILane::resetPartialOccupation(MSVehicle* v) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::resetPartialOccupation(v);
}


// ------ Drawing methods ------
void
GUILane::drawLinkNo(const GUIVisualizationSettings& s) const {
    int noLinks = (int)myLinks.size();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    if (getEdge().isCrossing()) {
        // draw indices at the start and end of the crossing
        MSLink* link = MSLinkContHelper::getConnectingLink(*getLogicalPredecessorLane(), *this);
        PositionVector shape = getShape();
        shape.extrapolate(0.5); // draw on top of the walking area
        drawTextAtEnd(toString(link->getIndex()), shape, 0, s.drawLinkJunctionIndex);
        drawTextAtEnd(toString(link->getIndex()), shape.reverse(), 0, s.drawLinkJunctionIndex);
        return;
    }
    // draw all links
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = myHalfLaneWidth;
    const bool lefthand = MSNet::getInstance()->lefthand();
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        drawTextAtEnd(toString(myLinks[lefthand ? noLinks - 1 - i : i]->getIndex()), getShape(), x2, s.drawLinkJunctionIndex);
        x1 -= w;
    }
}


void
GUILane::drawTLSLinkNo(const GUIVisualizationSettings& s, const GUINet& net) const {
    int noLinks = (int)myLinks.size();
    if (noLinks == 0) {
        return;
    }
    if (getEdge().isCrossing()) {
        // draw indices at the start and end of the crossing
        MSLink* link = MSLinkContHelper::getConnectingLink(*getLogicalPredecessorLane(), *this);
        int linkNo = net.getLinkTLIndex(link);
        if (linkNo >= 0) {
            PositionVector shape = getShape();
            shape.extrapolate(0.5); // draw on top of the walking area
            drawTextAtEnd(toString(linkNo), shape, 0, s.drawLinkTLIndex);
            drawTextAtEnd(toString(linkNo), shape.reverse(), 0, s.drawLinkTLIndex);
        }
        return;
    }
    // draw all links
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = myHalfLaneWidth;
    const bool lefthand = MSNet::getInstance()->lefthand();
    for (int i = noLinks; --i >= 0;) {
        SUMOReal x2 = x1 - (SUMOReal)(w / 2.);
        int linkNo = net.getLinkTLIndex(myLinks[lefthand ? noLinks - 1 - i : i]);
        if (linkNo < 0) {
            continue;
        }
        drawTextAtEnd(toString(linkNo), getShape(), x2, s.drawLinkTLIndex);
        x1 -= w;
    }
}


void
GUILane::drawTextAtEnd(const std::string& text, const PositionVector& shape, SUMOReal x, const GUIVisualizationTextSettings& settings) const {
    glPushMatrix();
    const Position& end = shape.back();
    const Position& f = shape[-2];
    const SUMOReal rot = RAD2DEG(atan2((end.x() - f.x()), (f.y() - end.y())));
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    GLHelper::drawText(text, Position(x, 0.26), 0, .6 * settings.size / 50, settings.color, 180);
    glPopMatrix();
}


void
GUILane::drawLinkRules(const GUIVisualizationSettings& s, const GUINet& net) const {
    int noLinks = (int)myLinks.size();
    if (noLinks == 0) {
        drawLinkRule(s, net, 0, getShape(), 0, 0);
        return;
    }
    if (getEdge().isCrossing()) {
        // draw rules at the start and end of the crossing
        MSLink* link = MSLinkContHelper::getConnectingLink(*getLogicalPredecessorLane(), *this);
        PositionVector shape = getShape();
        shape.extrapolate(0.5); // draw on top of the walking area
        drawLinkRule(s, net, link, shape, 0, myWidth);
        drawLinkRule(s, net, link, shape.reverse(), 0, myWidth);
        return;
    }
    // draw all links
    SUMOReal w = myWidth / (SUMOReal) noLinks;
    SUMOReal x1 = 0;
    const bool lefthand = MSNet::getInstance()->lefthand();
    for (int i = 0; i < noLinks; ++i) {
        SUMOReal x2 = x1 + w;
        drawLinkRule(s, net, myLinks[lefthand ? noLinks - 1 - i : i], getShape(), x1, x2);
        x1 = x2;
    }
}


void
GUILane::drawLinkRule(const GUIVisualizationSettings& s, const GUINet& net, MSLink* link, const PositionVector& shape, SUMOReal x1, SUMOReal x2) const {
    const Position& end = shape.back();
    const Position& f = shape[-2];
    const SUMOReal rot = RAD2DEG(atan2((end.x() - f.x()), (f.y() - end.y())));
    if (link == 0) {
        GLHelper::setColor(GUIVisualizationSettings::getLinkColor(LINKSTATE_DEADEND));
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
    } else {
        glPushMatrix();
        glTranslated(end.x(), end.y(), 0);
        glRotated(rot, 0, 0, 1);
        // select glID
        switch (link->getState()) {
            case LINKSTATE_TL_GREEN_MAJOR:
            case LINKSTATE_TL_GREEN_MINOR:
            case LINKSTATE_TL_RED:
            case LINKSTATE_TL_REDYELLOW:
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
        GLHelper::setColor(GUIVisualizationSettings::getLinkColor(link->getState()));
        if (!(drawAsRailway(s) || drawAsWaterway(s)) || link->getState() != LINKSTATE_MAJOR) {
            // the white bar should be the default for most railway
            // links and looks ugly so we do not draw it
            glBegin(GL_QUADS);
            glVertex2d(x1 - myHalfLaneWidth, 0.0);
            glVertex2d(x1 - myHalfLaneWidth, 0.5);
            glVertex2d(x2 - myHalfLaneWidth, 0.5);
            glVertex2d(x2 - myHalfLaneWidth, 0.0);
            glEnd();
        }
        glPopName();
        glPopMatrix();
    }
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
    glColor3d(1, 1, 1);
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);
    if (myWidth < SUMO_const_laneWidth) {
        glScaled(myWidth / SUMO_const_laneWidth, 1, 1);
    }
    for (std::vector<MSLink*>::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        LinkDirection dir = (*i)->getDirection();
        LinkState state = (*i)->getState();
        if (state == LINKSTATE_TL_OFF_NOSIGNAL || dir == LINKDIR_NODIR) {
            continue;
        }
        switch (dir) {
            case LINKDIR_STRAIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
                GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_TURN:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 90, .5, .05);
                GLHelper::drawBoxLine(Position(0.5, 2.5), 180, 1, .05);
                GLHelper::drawTriangleAtEnd(Position(0.5, 2.5), Position(0.5, 4), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_TURN_LEFTHAND:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                GLHelper::drawBoxLine(Position(-0.5, 2.5), -180, 1, .05);
                GLHelper::drawTriangleAtEnd(Position(-0.5, 2.5), Position(-0.5, 4), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_LEFT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 90, 1, .05);
                GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.5, 2.5), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_RIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.5, 2.5), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_PARTLEFT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), 45, .7, .05);
                GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.2, 1.3), (SUMOReal) 1, (SUMOReal) .25);
                break;
            case LINKDIR_PARTRIGHT:
                GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                GLHelper::drawBoxLine(Position(0, 2.5), -45, .7, .05);
                GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.2, 1.3), (SUMOReal) 1, (SUMOReal) .25);
                break;
            default:
                break;
        }
    }
    glPopMatrix();
}


void
GUILane::drawLane2LaneConnections() const {
    for (std::vector<MSLink*>::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        const MSLane* connected = (*i)->getLane();
        if (connected == 0) {
            continue;
        }
        GLHelper::setColor(GUIVisualizationSettings::getLinkColor((*i)->getState()));
        glBegin(GL_LINES);
        const Position& p1 = getShape()[-1];
        const Position& p2 = connected->getShape()[0];
        glVertex2d(p1.x(), p1.y());
        glVertex2d(p2.x(), p2.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(p1, p2, (SUMOReal) .4, (SUMOReal) .2);
    }
}


void
GUILane::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(getGlID());
    const bool isCrossing = myEdge->getPurpose() == MSEdge::EDGEFUNCTION_CROSSING;
    const bool isWalkingArea = myEdge->getPurpose() == MSEdge::EDGEFUNCTION_WALKINGAREA;
    const bool isInternal = isCrossing || isWalkingArea || myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL;
    bool mustDrawMarkings = false;
    SUMOReal exaggeration = s.laneWidthExaggeration;
    if (MSGlobals::gUseMesoSim) {
        GUIEdge* myGUIEdge = dynamic_cast<GUIEdge*>(myEdge);
        exaggeration *= s.edgeScaler.getScheme().getColor(myGUIEdge->getScaleValue(s.edgeScaler.getActive()));
    } else {
        exaggeration *= s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));
    }
    const bool drawDetails =  s.scale * exaggeration > 5 && !s.drawForSelecting;
    if (isCrossing || isWalkingArea) {
        // draw internal lanes on top of junctions
        glTranslated(0, 0, GLO_JUNCTION + 0.1);
    } else if (isWaterway(myPermissions)) {
        // draw waterways below normal roads
        glTranslated(0, 0, getType() - 0.2);
    } else {
        glTranslated(0, 0, getType());
    }
    // set lane color
    setColor(s);
    if (MSGlobals::gUseMesoSim) {
        myShapeColors.clear();
        const std::vector<RGBColor>& segmentColors = static_cast<const GUIEdge*>(myEdge)->getSegmentColors();
        if (segmentColors.size() > 0) {
            // apply segment specific shape colors
            //std::cout << getID() << " shape=" << myShape << " shapeSegs=" << toString(myShapeSegments) << "\n";
            for (int ii = 0; ii < (int)myShape.size() - 1; ++ii) {
                myShapeColors.push_back(segmentColors[myShapeSegments[ii]]);
            }
        }
    }
    // recognize full transparency and simply don't draw
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    if (color[3] != 0 && s.scale * exaggeration > s.laneMinSize) {
        // draw lane
        // check whether it is not too small
        if (s.scale * exaggeration < 1.) {
            if (myShapeColors.size() > 0) {
                GLHelper::drawLine(myShape, myShapeColors);
            } else {
                GLHelper::drawLine(myShape);
            }
            glPopMatrix();
        } else {
            GUINet* net = (GUINet*) MSNet::getInstance();
            if (drawAsRailway(s)) {
                // draw as railway
                const SUMOReal halfRailWidth = 0.725 * exaggeration;
                if (myShapeColors.size() > 0) {
                    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, myShapeColors, halfRailWidth);
                } else {
                    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, halfRailWidth);
                }
                glColor3d(1, 1, 1);
                glTranslated(0, 0, .1);
                GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, halfRailWidth - 0.2);
                setColor(s);
                drawCrossties(0.3 * exaggeration, 1 * exaggeration, 1 * exaggeration);
            } else if (isCrossing) {
                if (s.drawCrossingsAndWalkingareas) {
                    // determine priority to decide color
                    MSLink* link = MSLinkContHelper::getConnectingLink(*getLogicalPredecessorLane(), *this);
                    if (link->havePriority() || net->getLinkTLIndex(link) > 0) {
                        glColor3d(0.9, 0.9, 0.9);
                    } else {
                        glColor3d(0.1, 0.1, 0.1);
                    }
                    glTranslated(0, 0, .2);
                    drawCrossties(0.5, 1.0, getWidth() * 0.5);
                    glTranslated(0, 0, -.2);
                }
            } else if (isWalkingArea) {
                if (s.drawCrossingsAndWalkingareas) {
                    glTranslated(0, 0, .2);
                    if (s.scale * exaggeration < 20.) {
                        GLHelper::drawFilledPoly(myShape, true);
                    } else {
                        GLHelper::drawFilledPolyTesselated(myShape, true);
                    }
                    glTranslated(0, 0, -.2);
#ifdef GUILane_DEBUG_DRAW_WALKING_AREA_VERTICES
                    GLHelper::debugVertices(myShape, 80 / s.scale);
#endif
                }
            } else {
                const SUMOReal halfWidth = isInternal ? myQuarterLaneWidth : myHalfLaneWidth;
                mustDrawMarkings = !isInternal && myPermissions != 0 && myPermissions != SVC_PEDESTRIAN && exaggeration == 1.0 && !isWaterway(myPermissions);
                const int cornerDetail = drawDetails && !isInternal ? (int)(s.scale * exaggeration) : 0;
                const SUMOReal offset = halfWidth * MAX2((SUMOReal)0, (exaggeration - 1));
                if (myShapeColors.size() > 0) {
                    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, myShapeColors, halfWidth * exaggeration, cornerDetail, offset);
                } else {
                    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, halfWidth * exaggeration, cornerDetail, offset);
                }
            }
#ifdef GUILane_DEBUG_DRAW_VERTICES
            GLHelper::debugVertices(myShape, 80 / s.scale);
#endif
            glPopMatrix();
            // draw ROWs (not for inner lanes)
            if ((!isInternal || isCrossing) && (drawDetails || s.drawForSelecting)) {
                glPushMatrix();
                glTranslated(0, 0, GLO_JUNCTION); // must draw on top of junction shape
                glTranslated(0, 0, .5);
                if (drawDetails) {
                    if (MSGlobals::gLateralResolution > 0 && s.showSublanes) {
                        // draw sublane-borders
                        GLHelper::setColor(GLHelper::getColor().changedBrightness(51));
                        for (SUMOReal offset = -myHalfLaneWidth; offset < myHalfLaneWidth; offset += MSGlobals::gLateralResolution) {
                            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.01, 0, offset);
                        }
                    }
                    if (s.showLinkDecals && !drawAsRailway(s) && !drawAsWaterway(s) && myPermissions != SVC_PEDESTRIAN) {
                        drawArrows();
                    }
                    if (s.showLane2Lane) {
                        // this should be independent to the geometry:
                        //  draw from end of first to the begin of second
                        drawLane2LaneConnections();
                    }
                    if (s.showLaneDirection) {
                        drawDirectionIndicators();
                    }
                    glTranslated(0, 0, .1);
                    if (s.drawLinkJunctionIndex.show) {
                        drawLinkNo(s);
                    }
                    if (s.drawLinkTLIndex.show) {
                        drawTLSLinkNo(s, *net);
                    }
                }
                // make sure link rules are drawn so tls can be selected via right-click
                if (s.showLinkRules) {
                    drawLinkRules(s, *net);
                }
                glPopMatrix();
            }
        }
        if (mustDrawMarkings && drawDetails && s.laneShowBorders) { // needs matrix reset
            drawMarkings(s, exaggeration);
        }
        if (drawDetails && isInternal && myPermissions == SVC_BICYCLE && exaggeration == 1.0 && s.showLinkDecals && s.laneShowBorders) {
            drawBikeMarkings();
        }
    } else {
        glPopMatrix();
    }
    // draw vehicles
    if (s.scale * s.vehicleSize.getExaggeration(s) > s.vehicleSize.minSize) {
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
    glPopName();
}


void
GUILane::drawMarkings(const GUIVisualizationSettings& s, SUMOReal scale) const {
    glPushMatrix();
    glTranslated(0, 0, GLO_EDGE);
    setColor(s);
    // optionally draw inverse markings
    if (myIndex > 0 && (myEdge->getLanes()[myIndex - 1]->getPermissions() & myPermissions) != 0) {
        SUMOReal mw = (myHalfLaneWidth + SUMO_const_laneOffset + .01) * scale * (MSNet::getInstance()->lefthand() ? -1 : 1);
        int e = (int) getShape().size() - 1;
        for (int i = 0; i < e; ++i) {
            glPushMatrix();
            glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
            glRotated(myShapeRotations[i], 0, 0, 1);
            for (SUMOReal t = 0; t < myShapeLengths[i]; t += 6) {
                const SUMOReal length = MIN2((SUMOReal)3, myShapeLengths[i] - t);
                glBegin(GL_QUADS);
                glVertex2d(-mw, -t);
                glVertex2d(-mw, -t - length);
                glVertex2d(myQuarterLaneWidth * scale, -t - length);
                glVertex2d(myQuarterLaneWidth * scale, -t);
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
        (myHalfLaneWidth + SUMO_const_laneOffset) * scale);
    glPopMatrix();
}


void
GUILane::drawBikeMarkings() const {
    // draw bike lane markings onto the intersection
    glColor3d(1, 1, 1);
    int e = (int) getShape().size() - 1;
    SUMOReal mw = (myHalfLaneWidth + SUMO_const_laneOffset);
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), GLO_JUNCTION + 0.1);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += 0.5) {
            // left and right marking
            for (int side = -1; side <= 1; side += 2) {
                glBegin(GL_QUADS);
                glVertex2d(side * mw, -t);
                glVertex2d(side * mw, -t - 0.35);
                glVertex2d(side * (mw + SUMO_const_laneOffset), -t - 0.35);
                glVertex2d(side * (mw + SUMO_const_laneOffset), -t);
                glEnd();
            }
        }
        glPopMatrix();
    }
}

void
GUILane::drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const {
    glPushMatrix();
    // draw on top of of the white area between the rails
    glTranslated(0, 0, 0.1);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.0);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += spacing) {
            glBegin(GL_QUADS);
            glVertex2d(-halfWidth, -t);
            glVertex2d(-halfWidth, -t - length);
            glVertex2d(halfWidth, -t - length);
            glVertex2d(halfWidth, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
}


void
GUILane::drawDirectionIndicators() const {
    glColor3d(0.3, 0.3, 0.3);
    glPushMatrix();
    glTranslated(0, 0, GLO_EDGE);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += myWidth) {
            const SUMOReal length = MIN2((SUMOReal)myHalfLaneWidth, myShapeLengths[i] - t);
            glBegin(GL_TRIANGLES);
            glVertex2d(0, -t - length);
            glVertex2d(-myQuarterLaneWidth, -t);
            glVertex2d(+myQuarterLaneWidth, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
}



// ------ inherited from GUIGlObject
GUIGLObjectPopupMenu*
GUILane::getPopUpMenu(GUIMainWindow& app,
                      GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    new FXMenuCommand(ret, "Copy edge name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    const SUMOReal pos = interpolateGeometryPosToLanePos(myShape.nearest_offset_to_point2D(parent.getPositionInformation()));
    const SUMOReal height = myShape.positionAtOffset2D(myShape.nearest_offset_to_point2D(parent.getPositionInformation())).z();
    new FXMenuCommand(ret, ("pos: " + toString(pos) + " height: " + toString(height)).c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, false);
    new FXMenuSeparator(ret);
    if (myAmClosed) {
        if (myPermissionChanges.empty()) {
            new FXMenuCommand(ret, "Reopen lane", 0, &parent, MID_CLOSE_LANE);
            new FXMenuCommand(ret, "Reopen edge", 0, &parent, MID_CLOSE_EDGE);
        } else {
            new FXMenuCommand(ret, "Reopen lane (override rerouter)", 0, &parent, MID_CLOSE_LANE);
            new FXMenuCommand(ret, "Reopen edge (override rerouter)", 0, &parent, MID_CLOSE_EDGE);
        }
    } else {
        new FXMenuCommand(ret, "Close lane", 0, &parent, MID_CLOSE_LANE);
        new FXMenuCommand(ret, "Close edge", 0, &parent, MID_CLOSE_EDGE);
    }
    new FXMenuCommand(ret, "Add rerouter", 0, &parent, MID_ADD_REROUTER);
    return ret;
}


GUIParameterTableWindow*
GUILane::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 14);
    // add items
    ret->mkItem("maxspeed [m/s]", false, getSpeedLimit());
    ret->mkItem("length [m]", false, myLength);
    ret->mkItem("width [m]", false, myWidth);
    ret->mkItem("street name", false, myEdge->getStreetName());
    ret->mkItem("stored traveltime [s]", true, new FunctionBinding<GUILane, SUMOReal>(this, &GUILane::getStoredEdgeTravelTime));
    ret->mkItem("loaded weight", true, new FunctionBinding<GUILane, SUMOReal>(this, &GUILane::getLoadedEdgeWeight));
    ret->mkItem("routing speed [m/s]", true, new FunctionBinding<MSEdge, SUMOReal>(myEdge, &MSEdge::getRoutingSpeed));
    ret->mkItem("brutto occupancy [%]", true, new FunctionBinding<GUILane, SUMOReal>(this, &GUILane::getBruttoOccupancy, 100.));
    ret->mkItem("netto occupancy [%]", true, new FunctionBinding<GUILane, SUMOReal>(this, &GUILane::getNettoOccupancy, 100.));
    ret->mkItem("edge type", false, myEdge->getEdgeType());
    ret->mkItem("priority", false, myEdge->getPriority());
    ret->mkItem("allowed vehicle class", false, getVehicleClassNames(myPermissions));
    ret->mkItem("disallowed vehicle class", false, getVehicleClassNames(~myPermissions));
    ret->mkItem("permission code", false, myPermissions);
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUILane::getCenteringBoundary() const {
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(10);
    // ensure that vehicles and persons on the side are drawn even if the edge
    // is outside the view
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


SUMOReal
GUILane::getStoredEdgeTravelTime() const {
    MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
    if (!ews.knowsTravelTime(myEdge)) {
        return -1;
    } else {
        SUMOReal value(0);
        ews.retrieveExistingTravelTime(myEdge, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), value);
        return value;
    }
}


SUMOReal
GUILane::getLoadedEdgeWeight() const {
    MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
    if (!ews.knowsEffort(myEdge)) {
        return -1;
    } else {
        SUMOReal value(-1);
        ews.retrieveExistingEffort(myEdge, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), value);
        return value;
    }
}


void
GUILane::setColor(const GUIVisualizationSettings& s) const {
    if (MSGlobals::gUseMesoSim) {
        GLHelper::setColor(static_cast<const GUIEdge*>(myEdge)->getMesoColor());
    } else {
        const GUIColorer& c = s.laneColorer;
        if (!setFunctionalColor(c.getActive()) && !setMultiColor(c)) {
            GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
        }
    }
}


bool
GUILane::setFunctionalColor(int activeScheme) const {
    switch (activeScheme) {
        case 18: {
            SUMOReal hue = GeomHelper::naviDegree(myShape.beginEndAngle()); // [0-360]
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        default:
            return false;
    }
}


bool
GUILane::setMultiColor(const GUIColorer& c) const {
    const int activeScheme = c.getActive();
    myShapeColors.clear();
    switch (activeScheme) {
        case 22: // color by height at segment start
            for (PositionVector::const_iterator ii = myShape.begin(); ii != myShape.end() - 1; ++ii) {
                myShapeColors.push_back(c.getScheme().getColor(ii->z()));
            }
            return true;
        case 24: // color by inclination  at segment start
            for (int ii = 1; ii < (int)myShape.size(); ++ii) {
                const SUMOReal inc = (myShape[ii].z() - myShape[ii - 1].z()) / MAX2(POSITION_EPS, myShape[ii].distanceTo2D(myShape[ii - 1]));
                myShapeColors.push_back(c.getScheme().getColor(inc));
            }
            return true;
        default:
            return false;
    }
}


SUMOReal
GUILane::getColorValue(int activeScheme) const {
    switch (activeScheme) {
        case 0:
            switch (myPermissions) {
                case SVC_PEDESTRIAN:
                    return 1;
                case SVC_BICYCLE:
                    return 2;
                case 0:
                    return 3;
                case SVC_SHIP:
                    return 4;
                case SVC_AUTHORITY:
                    return 6;
                default:
                    break;
            }
            if ((myPermissions & SVC_PASSENGER) != 0 || isRailway(myPermissions)) {
                return 0;
            } else {
                return 5;
            }
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
            return getCO2Emissions() / myLength;
        case 9:
            return getCOEmissions() / myLength;
        case 10:
            return getPMxEmissions() / myLength;
        case 11:
            return getNOxEmissions() / myLength;
        case 12:
            return getHCEmissions() / myLength;
        case 13:
            return getFuelConsumption() / myLength;
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
                ews.retrieveExistingTravelTime(myEdge, 0, value);
                return 100 * myLength / value / getSpeedLimit();
            }
        }
        case 17: {
            return 1 / myLengthGeometryFactor;
        }
        case 19: {
            return getLoadedEdgeWeight();
        }
        case 20: {
            return myEdge->getPriority();
        }
        case 21: {
            // color by z of first shape point
            return getShape()[0].z();
        }
        case 23: {
            // color by incline
            return (getShape()[-1].z() - getShape()[0].z()) / getLength();
        }
        case 25: {
            // color by average speed
            return getMeanSpeed();
        }
        case 26: {
            // color by average relative speed
            return getMeanSpeed() / myMaxSpeed;
        }
        case 27: {
            // color by routing device assumed speed
            return myEdge->getRoutingSpeed();
        }
        case 28:
            return getElectricityConsumption() / myLength;
        case 29:
            return MSNet::getInstance()->getInsertionControl().getPendingEmits(this);
    }
    return 0;
}


SUMOReal
GUILane::getScaleValue(int activeScheme) const {
    switch (activeScheme) {
        case 0:
            return 0;
        case 1:
            return gSelected.isSelected(getType(), getGlID()) ||
                   gSelected.isSelected(GLO_EDGE, dynamic_cast<GUIEdge*>(myEdge)->getGlID());
        case 2:
            return getSpeedLimit();
        case 3:
            return getBruttoOccupancy();
        case 4:
            return getNettoOccupancy();
        case 5:
            return firstWaitingTime();
        case 6:
            return getEdgeLaneNumber();
        case 7:
            return getCO2Emissions() / myLength;
        case 8:
            return getCOEmissions() / myLength;
        case 9:
            return getPMxEmissions() / myLength;
        case 10:
            return getNOxEmissions() / myLength;
        case 11:
            return getHCEmissions() / myLength;
        case 12:
            return getFuelConsumption() / myLength;
        case 13:
            return getHarmonoise_NoiseEmissions();
        case 14: {
            return getStoredEdgeTravelTime();
        }
        case 15: {
            MSEdgeWeightsStorage& ews = MSNet::getInstance()->getWeightsStorage();
            if (!ews.knowsTravelTime(myEdge)) {
                return -1;
            } else {
                SUMOReal value(0);
                ews.retrieveExistingTravelTime(myEdge, 0, value);
                return 100 * myLength / value / getSpeedLimit();
            }
        }
        case 16: {
            return 1 / myLengthGeometryFactor;
        }
        case 17: {
            return getLoadedEdgeWeight();
        }
        case 18: {
            return myEdge->getPriority();
        }
        case 19: {
            // scale by average speed
            return getMeanSpeed();
        }
        case 20: {
            // scale by average relative speed
            return getMeanSpeed() / myMaxSpeed;
        }
        case 21:
            return getElectricityConsumption() / myLength;
        case 22:
            return MSNet::getInstance()->getInsertionControl().getPendingEmits(this);
    }
    return 0;
}


bool
GUILane::drawAsRailway(const GUIVisualizationSettings& s) const {
    return isRailway(myPermissions) && s.showRails;
}


bool
GUILane::drawAsWaterway(const GUIVisualizationSettings& s) const {
    return isWaterway(myPermissions) && s.showRails; // reusing the showRails setting
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


void
GUILane::closeTraffic(bool rebuildAllowed) {
    MSGlobals::gCheckRoutes = false;
    if (myAmClosed) {
        myPermissionChanges.clear(); // reset rerouters
        resetPermissions(CHANGE_PERMISSIONS_GUI);
    } else {
        setPermissions(SVC_AUTHORITY, CHANGE_PERMISSIONS_GUI);
    }
    myAmClosed = !myAmClosed;
    if (rebuildAllowed) {
        getEdge().rebuildAllowedLanes();
    }
}


PositionVector
GUILane::splitAtSegments(const PositionVector& shape) {
    assert(MSGlobals::gUseMesoSim);
    int no = MELoop::numSegmentsFor(myLength, OptionsCont::getOptions().getFloat("meso-edgelength"));
    const SUMOReal slength = myLength / no;
    PositionVector result = shape;
    SUMOReal offset = 0;
    for (int i = 0; i < no; ++i) {
        offset += slength;
        Position pos = shape.positionAtOffset(offset);
        int index = result.indexOfClosest(pos);
        if (pos.distanceTo(result[index]) > POSITION_EPS) {
            index = result.insertAtClosest(pos);
        }
        while ((int)myShapeSegments.size() < index) {
            myShapeSegments.push_back(i);
        }
        //std::cout << "splitAtSegments " << getID() << " no=" << no << " i=" << i << " offset=" << offset << " index=" << index << " segs=" << toString(myShapeSegments) << " resultSize=" << result.size() << " result=" << toString(result) << "\n";
    }
    while (myShapeSegments.size() < result.size()) {
        myShapeSegments.push_back(no - 1);
    }
    return result;
}

/****************************************************************************/

