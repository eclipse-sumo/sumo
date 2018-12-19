/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNELane.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing Lane geometry (adapted from GNELaneWrapper)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEInternalLane.h"
#include "GNEConnection.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(GNELane, FXDelegator, 0, 0)

// ===========================================================================
// method definitions
// ===========================================================================

GNELane::GNELane(GNEEdge& edge, const int index) :
    GNENetElement(edge.getNet(), edge.getNBEdge()->getLaneID(index), GLO_LANE, SUMO_TAG_LANE),
    myParentEdge(edge),
    myIndex(index),
    mySpecialColor(nullptr) {
}

GNELane::GNELane() :
    GNENetElement(nullptr, "dummyConstructorGNELane", GLO_LANE, SUMO_TAG_LANE),
    myParentEdge(GNEEdge::DummyEdge),
    myIndex(-1),
    mySpecialColor(nullptr) {
}


GNELane::~GNELane() {
}


void
GNELane::drawLinkNo(const GUIVisualizationSettings& s) const {
    const std::vector<NBEdge::Connection>& cons = myParentEdge.getNBEdge()->getConnectionsFromLane(myIndex);
    int noLinks = (int)cons.size();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    glPushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.5);
    double w = myParentEdge.getNBEdge()->getLaneWidth(myIndex) / (double) noLinks;
    double x1 = myParentEdge.getNBEdge()->getLaneWidth(myIndex) / 2;
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    for (int i = noLinks; --i >= 0;) {
        double x2 = x1 - (double)(w / 2.);
        const int linkIndex = myParentEdge.getNBEdge()->getToNode()->getConnectionIndex(myParentEdge.getNBEdge(),
                              cons[lefthand ? noLinks - 1 - i : i]);
        GLHelper::drawTextAtEnd(toString(linkIndex), getShape(), x2, s.drawLinkJunctionIndex.size, s.drawLinkJunctionIndex.color);
        x1 -= w;
    }
    glPopMatrix();
}


void
GNELane::drawTLSLinkNo(const GUIVisualizationSettings& s) const {
    const std::vector<NBEdge::Connection>& cons = myParentEdge.getNBEdge()->getConnectionsFromLane(myIndex);
    int noLinks = (int)cons.size();
    if (noLinks == 0) {
        return;
    }
    // draw all links
    glPushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.5);
    double w = myParentEdge.getNBEdge()->getLaneWidth(myIndex) / (double) noLinks;
    double x1 = myParentEdge.getNBEdge()->getLaneWidth(myIndex) / 2;
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    for (int i = noLinks; --i >= 0;) {
        double x2 = x1 - (double)(w / 2.);
        int linkNo = cons[lefthand ? noLinks - 1 - i : i].tlLinkIndex;
        GLHelper::drawTextAtEnd(toString(linkNo), getShape(), x2, s.drawLinkTLIndex.size, s.drawLinkTLIndex.color);
        x1 -= w;
    }
    glPopMatrix();
}


void
GNELane::drawLinkRules() const {
}


void
GNELane::drawArrows() const {
    const Position& end = getShape().back();
    const Position& f = getShape()[-2];
    double rot = (double) atan2((end.x() - f.x()), (f.y() - end.y())) * (double) 180.0 / (double)M_PI;
    glPushMatrix();
    glPushName(0);
    glTranslated(0, 0, GLO_JUNCTION + .1); // must draw on top of junction shape
    glColor3d(1, 1, 1);
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);

    // draw all links
    const std::vector<NBEdge::Connection>& edgeCons = myParentEdge.getNBEdge()->myConnections;
    NBNode* dest = myParentEdge.getNBEdge()->myTo;
    for (auto i : edgeCons) {
        if (i.fromLane == myIndex) {
            LinkDirection dir = dest->getDirection(myParentEdge.getNBEdge(), i.toEdge, OptionsCont::getOptions().getBool("lefthand"));
            switch (dir) {
                case LINKDIR_STRAIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
                    break;
                case LINKDIR_LEFT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 90, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.5, 2.5), (double) 1, (double) .25);
                    break;
                case LINKDIR_RIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.5, 2.5), (double) 1, (double) .25);
                    break;
                case LINKDIR_TURN:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 90, .5, .05);
                    GLHelper::drawBoxLine(Position(0.5, 2.5), 180, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0.5, 2.5), Position(0.5, 4), (double) 1, (double) .25);
                    break;
                case LINKDIR_TURN_LEFTHAND:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                    GLHelper::drawBoxLine(Position(-0.5, 2.5), -180, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(-0.5, 2.5), Position(-0.5, 4), (double) 1, (double) .25);
                    break;
                case LINKDIR_PARTLEFT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 45, .7, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.2, 1.3), (double) 1, (double) .25);
                    break;
                case LINKDIR_PARTRIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -45, .7, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.2, 1.3), (double) 1, (double) .25);
                    break;
                case LINKDIR_NODIR:
                    GLHelper::drawBoxLine(Position(1, 5.8), 245, 2, .05);
                    GLHelper::drawBoxLine(Position(-1, 5.8), 115, 2, .05);
                    glTranslated(0, 5, 0);
                    GLHelper::drawOutlineCircle(0.9, 0.8, 32);
                    glTranslated(0, -5, 0);
                    break;
            }
        }
    }
    glPopName();
    glPopMatrix();
}


void
GNELane::drawLane2LaneConnections() const {
    glPushMatrix();
    glPushName(0);
    glTranslated(0, 0, GLO_JUNCTION + .1); // must draw on top of junction shape
    std::vector<NBEdge::Connection> connections = myParentEdge.getNBEdge()->getConnectionsFromLane(myIndex);
    NBNode* node = myParentEdge.getNBEdge()->getToNode();
    const Position& startPos = getShape()[-1];
    for (auto it : connections) {
        const LinkState state = node->getLinkState(myParentEdge.getNBEdge(), it.toEdge, it.fromLane, it.toLane, it.mayDefinitelyPass, it.tlID);
        switch (state) {
            case LINKSTATE_TL_OFF_NOSIGNAL:
                glColor3d(1, 1, 0);
                break;
            case LINKSTATE_TL_OFF_BLINKING:
                glColor3d(0, 1, 1);
                break;
            case LINKSTATE_MAJOR:
                glColor3d(1, 1, 1);
                break;
            case LINKSTATE_MINOR:
                glColor3d(.4, .4, .4);
                break;
            case LINKSTATE_STOP:
                glColor3d(.7, .4, .4);
                break;
            case LINKSTATE_EQUAL:
                glColor3d(.7, .7, .7);
                break;
            case LINKSTATE_ALLWAY_STOP:
                glColor3d(.7, .7, 1);
                break;
            case LINKSTATE_ZIPPER:
                glColor3d(.75, .5, 0.25);
                break;
            default:
                throw ProcessError("Unexpected LinkState '" + toString(state) + "'");
        }
        const Position& endPos = it.toEdge->getLaneShape(it.toLane)[0];
        glBegin(GL_LINES);
        glVertex2d(startPos.x(), startPos.y());
        glVertex2d(endPos.x(), endPos.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(startPos, endPos, (double) 1.5, (double) .2);
    }
    glPopName();
    glPopMatrix();
}


void
GNELane::drawGL(const GUIVisualizationSettings& s) const {
    // Push draw matrix 1
    glPushMatrix();
    // Push name
    glPushName(getGlID());
    // Traslate to fromt
    glTranslated(0, 0, getType());
    setLaneColor(s);
    // start drawing lane checking whether it is not too small
    const double selectionScale = isAttributeCarrierSelected() || myParentEdge.isAttributeCarrierSelected() ? s.selectionScale : 1;
    double exaggeration = selectionScale * s.laneWidthExaggeration; // * s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));
    // XXX apply usefull scale values
    //exaggeration *= s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));

    // recognize full transparency and simply don't draw
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    if ((color[3] == 0) || (s.scale * exaggeration < s.laneMinSize)) {
        // Pop draw matrix 1
        glPopMatrix();
    } else if (s.scale * exaggeration < 1.) {
        // draw as lines, depending of myShapeColors
        if (myShapeColors.size() > 0) {
            GLHelper::drawLine(getShape(), myShapeColors);
        } else {
            GLHelper::drawLine(getShape());
        }
        // Pop draw matrix 1
        glPopMatrix();
    } else {
        // we draw the lanes with reduced width so that the lane markings below are visible
        // (this avoids artifacts at geometry corners without having to
        // compute lane-marking intersection points)
        const double halfWidth2 = exaggeration * (myParentEdge.getNBEdge()->getLaneWidth(myIndex) / 2 - SUMO_const_laneMarkWidth / 2);
        // Draw as a normal lane, and reduce width to make sure that a selected edge can still be seen
        const double halfWidth =  myParentEdge.isAttributeCarrierSelected() ? halfWidth2 - exaggeration * 0.3 : halfWidth2;
        const bool spreadSuperposed = s.spreadSuperposed && drawAsRailway(s) && myParentEdge.getNBEdge()->isBidiRail();
        // Check if lane has to be draw as railway and if isn't being drawn for selecting
        if (drawAsRailway(s) && (!s.drawForSelecting || spreadSuperposed)) {
            PositionVector shape = getShape();
            const double width = myParentEdge.getNBEdge()->getLaneWidth(myIndex);
            // draw as railway: assume standard gauge of 1435mm when lane width is not set
            // draw foot width 150mm, assume that distance between rail feet inner sides is reduced on both sides by 39mm with regard to the gauge
            // assume crosstie length of 181% gauge (2600mm for standard gauge)
            double halfGauge = 0.5 * (width == SUMO_const_laneWidth ?  1.4350 : width) * exaggeration;
            if (spreadSuperposed) {
                shape.move2side(halfGauge * 0.8);
                halfGauge *= 0.4;
                //std::cout << "spreadSuperposed " << getID() << " old=" << getShape() << " new=" << shape << "\n";
            }
            const double halfInnerFeetWidth = halfGauge - 0.039 * exaggeration;
            const double halfRailWidth = halfInnerFeetWidth + 0.15 * exaggeration;
            const double halfCrossTieWidth = halfGauge * 1.81;
            // Draw box depending of myShapeColors
            if (myShapeColors.size() > 0) {
                GLHelper::drawBoxLines(shape, myShapeRotations, myShapeLengths, myShapeColors, halfRailWidth);
            } else {
                GLHelper::drawBoxLines(shape, myShapeRotations, myShapeLengths, halfRailWidth);
            }
            // Save current color
            RGBColor current = GLHelper::getColor();
            // Draw gray on top with reduced width (the area between the two tracks)
            glColor3d(0.8, 0.8, 0.8);
            glTranslated(0, 0, .1);
            GLHelper::drawBoxLines(shape, myShapeRotations, myShapeLengths, halfInnerFeetWidth);
            // Set current color back
            GLHelper::setColor(current);
            // Draw crossties
            GLHelper::drawCrossTies(shape, myShapeRotations, myShapeLengths, 0.26 * exaggeration, 0.6 * exaggeration, halfCrossTieWidth, s.drawForSelecting);
        } else {
            if (myShapeColors.size() > 0) {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, myShapeColors, halfWidth);
            } else {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfWidth);
            }
        }
        if (halfWidth != halfWidth2 && !spreadSuperposed) {
            // draw again to show the selected edge
            GLHelper::setColor(s.selectedEdgeColor);
            glTranslated(0, 0, -.1);
            GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfWidth2);
        }
        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myNet->getViewNet()->getDottedAC() == this)) {
            GLHelper::drawShapeDottedContour(getType(), myParentEdge.getNBEdge()->getLaneStruct(myIndex).shape, halfWidth);
        }
        // Pop draw matrix 1
        glPopMatrix();
        // only draw details depending of the scale and if isn't being drawn for selecting
        if ((s.scale >= 10) && !s.drawForSelecting) {
            // if exaggeration is 1, draw drawMarkings
            if (s.laneShowBorders && exaggeration == 1 && !drawAsRailway(s)) {
                drawMarkings(s, exaggeration);
            }
            // draw ROWs only if target junction has a valid logic)
            if (s.showLinkDecals && myParentEdge.getGNEJunctionDestiny()->isLogicValid() && s.scale > 3) {
                drawArrows();
            }
            // Draw direction indicators if the correspondient option is enabled
            if (s.showLaneDirection) {
                if (drawAsRailway(s)) {
                    // improve visibility of superposed rail edges
                    setLaneColor(s);
                } else {
                    glColor3d(0.3, 0.3, 0.3);
                }
                drawDirectionIndicators(exaggeration, spreadSuperposed);
            }
            if (s.drawLinkJunctionIndex.show) {
                drawLinkNo(s);
            }
            if (s.drawLinkTLIndex.show) {
                drawTLSLinkNo(s);
            }
        }
        // If there are texture of restricted lanes to draw, check if icons can be drawn
        if (!s.drawForSelecting && !OptionsCont::getOptions().getBool("disable-laneIcons") && (myLaneRestrictedTexturePositions.size() > 0) && (s.scale >= 10)) {
            // Declare default width of icon (3)
            double iconWidth = 1;
            // Obtain width of icon, if width of lane is different
            if (myParentEdge.getNBEdge()->getLaneStruct(myIndex).width != -1) {
                iconWidth = myParentEdge.getNBEdge()->getLaneStruct(myIndex).width / 3;
            }
            // Draw list of icons
            for (int i = 0; i < (int)myLaneRestrictedTexturePositions.size(); i++) {
                // Push draw matrix 2
                glPushMatrix();
                // Set white color
                glColor3d(1, 1, 1);
                // Traslate matrix 2
                glTranslated(myLaneRestrictedTexturePositions.at(i).x(), myLaneRestrictedTexturePositions.at(i).y(), getType() + 0.1);
                // Rotate matrix 2
                glRotated(myLaneRestrictedTextureRotations.at(i), 0, 0, -1);
                glRotated(-90, 0, 0, 1);
                // draw texture box depending of type of restriction
                if (isRestricted(SVC_PEDESTRIAN)) {
                    GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LANEPEDESTRIAN), iconWidth);
                } else if (isRestricted(SVC_BICYCLE)) {
                    GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LANEBIKE), iconWidth);
                } else if (isRestricted(SVC_BUS)) {
                    GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LANEBUS), iconWidth);
                }
                // Pop draw matrix 2
                glPopMatrix();
            }
        }
        // draw a Start/endPoints if lane has a custom shape
        if (!s.drawForSelecting && (myParentEdge.getNBEdge()->getLaneStruct(myIndex).customShape.size() > 1)) {
            GLHelper::setColor(s.junctionColorer.getSchemes()[0].getColor(2));
            if (isAttributeCarrierSelected() && s.laneColorer.getActive() != 1) {
                // override with special colors (unless the color scheme is based on selection)
                GLHelper::setColor(s.selectedEdgeColor.changedBrightness(-20));
            }
            // obtain circle width and resolution
            double circleWidth = GNEEdge::SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration) / 2;
            int circleResolution = GNEAttributeCarrier::getCircleResolution(s);
            // obtain custom shape
            const PositionVector& customShape = myParentEdge.getNBEdge()->getLaneStruct(myIndex).customShape;
            // draw s
            glPushMatrix();
            glTranslated(customShape.front().x(), customShape.front().y(), GLO_JUNCTION + 0.01);
            GLHelper::drawFilledCircle(circleWidth, circleResolution);
            glTranslated(0, 0, 0.01);
            GLHelper::drawText("S", Position(), 0, circleWidth, RGBColor::WHITE);
            glPopMatrix();
            // draw line between Junction and point
            glPushMatrix();
            glTranslated(0, 0, GLO_JUNCTION - 0.01);
            glLineWidth(4);
            GLHelper::drawLine(customShape.front(), myParentEdge.getGNEJunctionSource()->getPositionInView());
            glPopMatrix();
            // draw "e"
            glPushMatrix();
            glTranslated(customShape.back().x(), customShape.back().y(), GLO_JUNCTION + 0.01);
            GLHelper::drawFilledCircle(circleWidth, circleResolution);
            glTranslated(0, 0, 0.01);
            GLHelper::drawText("E", Position(), 0, circleWidth, RGBColor::WHITE);
            glPopMatrix();
            // draw line between Junction and point
            glPushMatrix();
            glTranslated(0, 0, GLO_JUNCTION - 0.01);
            glLineWidth(4);
            GLHelper::drawLine(customShape.back(), myParentEdge.getGNEJunctionDestiny()->getPositionInView());
            glPopMatrix();
        }
    }
    // Pop Name
    glPopName();
}


void
GNELane::drawMarkings(const GUIVisualizationSettings& /* s */, double scale) const {
    glPushMatrix();
    glTranslated(0, 0, GLO_EDGE);
    const double myHalfLaneWidth = myParentEdge.getNBEdge()->getLaneWidth(myIndex) / 2;
    // optionally draw inverse markings
    if (myIndex > 0 && (myParentEdge.getNBEdge()->getPermissions(myIndex - 1) & myParentEdge.getNBEdge()->getPermissions(myIndex)) != 0) {
        double mw = (myHalfLaneWidth + SUMO_const_laneMarkWidth) * scale;
        double mw2 = (myHalfLaneWidth - SUMO_const_laneMarkWidth) * scale;
        if (OptionsCont::getOptions().getBool("lefthand")) {
            mw *= -1;
            mw2 *= -1;
        }
        int e = (int) getShape().size() - 1;
        for (int i = 0; i < e; ++i) {
            glPushMatrix();
            glTranslated(getShape()[i].x(), getShape()[i].y(), 2.1);
            glRotated(myShapeRotations[i], 0, 0, 1);
            for (double t = 0; t < myShapeLengths[i]; t += 6) {
                const double length = MIN2((double)3, myShapeLengths[i] - t);
                glBegin(GL_QUADS);
                glVertex2d(-mw, -t);
                glVertex2d(-mw, -t - length);
                glVertex2d(-mw2, -t - length);
                glVertex2d(-mw2, -t);
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
        (myHalfLaneWidth + SUMO_const_laneMarkWidth) * scale);
    glPopMatrix();
}


GUIGLObjectPopupMenu*
GNELane::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // first obtain edit mode (needed because certain Commands depend of current edit mode)
    const int editMode = parent.getVisualisationSettings()->editMode;
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    // build copy names entry
    if (editMode != GNE_NMODE_TLS) {
        new FXMenuCommand(ret, "Copy edge parent name to clipboard", nullptr, ret, MID_COPY_EDGE_NAME);
        buildNameCopyPopupEntry(ret);
    }
    // build selection
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    if (editMode != GNE_NMODE_TLS) {
        // build show parameters menu
        buildShowParamsPopupEntry(ret);
        // build position copy entry
        buildPositionCopyEntry(ret, false);
    }
    if (editMode != GNE_NMODE_CONNECT && editMode != GNE_NMODE_TLS && editMode != GNE_NMODE_CREATE_EDGE) {
        // Get icons
        FXIcon* pedestrianIcon = GUIIconSubSys::getIcon(ICON_LANEPEDESTRIAN);
        FXIcon* bikeIcon = GUIIconSubSys::getIcon(ICON_LANEBIKE);
        FXIcon* busIcon = GUIIconSubSys::getIcon(ICON_LANEBUS);
        FXIcon* greenVergeIcon = GUIIconSubSys::getIcon(ICON_LANEGREENVERGE);
        // Create basic commands
        std::string edgeDescPossibleMulti = toString(SUMO_TAG_EDGE);
        const int edgeSelSize = (int)myNet->retrieveEdges(true).size();
        if (edgeSelSize && myParentEdge.isAttributeCarrierSelected() && (edgeSelSize > 1)) {
            edgeDescPossibleMulti = toString(edgeSelSize) + " " + toString(SUMO_TAG_EDGE) + "s";
        }
        // if lane is selected, calculate number of restricted lanes
        bool edgeHasSidewalk = false;
        bool edgeHasBikelane = false;
        bool edgeHasBuslane = false;
        bool edgeHasGreenVerge = false;
        bool differentLaneShapes = false;
        if (isAttributeCarrierSelected()) {
            auto selectedLanes = myNet->retrieveLanes(true);
            for (auto i : selectedLanes) {
                if (i->getParentEdge().hasRestrictedLane(SVC_PEDESTRIAN)) {
                    edgeHasSidewalk = true;
                }
                if (i->getParentEdge().hasRestrictedLane(SVC_BICYCLE)) {
                    edgeHasBikelane = true;
                }
                if (i->getParentEdge().hasRestrictedLane(SVC_BUS)) {
                    edgeHasBuslane = true;
                }
                if (i->getParentEdge().hasRestrictedLane(SVC_IGNORING)) {
                    edgeHasGreenVerge = true;
                }
                if (i->getParentEdge().getNBEdge()->getLaneStruct(i->getIndex()).customShape.size() != 0) {
                    differentLaneShapes = true;
                }
            }
        } else {
            edgeHasSidewalk = myParentEdge.hasRestrictedLane(SVC_PEDESTRIAN);
            edgeHasBikelane = myParentEdge.hasRestrictedLane(SVC_BICYCLE);
            edgeHasBuslane = myParentEdge.hasRestrictedLane(SVC_BUS);
            edgeHasGreenVerge = myParentEdge.hasRestrictedLane(SVC_IGNORING);
            differentLaneShapes = myParentEdge.getNBEdge()->getLaneStruct(myIndex).customShape.size() != 0;
        }
        // create menu pane for edge operations
        FXMenuPane* edgeOperations = new FXMenuPane(ret);
        ret->insertMenuPaneChild(edgeOperations);
        new FXMenuCascade(ret, "edge operations", nullptr, edgeOperations);
        // create menu commands for all edge oeprations
        new FXMenuCommand(edgeOperations, "Split edge here", nullptr, &parent, MID_GNE_EDGE_SPLIT);
        new FXMenuCommand(edgeOperations, "Split edge in both direction here", nullptr, &parent, MID_GNE_EDGE_SPLIT_BIDI);
        new FXMenuCommand(edgeOperations, "Set geometry endpoint here (shift-click)", nullptr, &parent, MID_GNE_EDGE_EDIT_ENDPOINT);
        new FXMenuCommand(edgeOperations, "Restore geometry endpoint (shift-click)", nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
        new FXMenuCommand(edgeOperations, ("Reverse " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_REVERSE);
        new FXMenuCommand(edgeOperations, ("Add reverse direction for " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_ADD_REVERSE);
        new FXMenuCommand(edgeOperations, ("Straighten " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN);
        new FXMenuCommand(edgeOperations, ("Smooth " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_SMOOTH);
        new FXMenuCommand(edgeOperations, ("Straighten elevation of " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN_ELEVATION);
        new FXMenuCommand(edgeOperations, ("Smooth elevation of " + edgeDescPossibleMulti).c_str(), nullptr, &parent, MID_GNE_EDGE_SMOOTH_ELEVATION);
        // create menu pane for lane operations
        FXMenuPane* laneOperations = new FXMenuPane(ret);
        ret->insertMenuPaneChild(laneOperations);
        new FXMenuCascade(ret, "lane operations", nullptr, laneOperations);
        new FXMenuCommand(laneOperations, "Duplicate lane", nullptr, &parent, MID_GNE_LANE_DUPLICATE);
        if (differentLaneShapes) {
            new FXMenuCommand(laneOperations, "reset custom shape", nullptr, &parent, MID_GNE_LANE_RESET_CUSTOMSHAPE);
        }
        // Create panel for lane operations and insert it in ret
        FXMenuPane* addSpecialLanes = new FXMenuPane(laneOperations);
        ret->insertMenuPaneChild(addSpecialLanes);
        FXMenuPane* removeSpecialLanes = new FXMenuPane(laneOperations);
        ret->insertMenuPaneChild(removeSpecialLanes);
        FXMenuPane* transformSlanes = new FXMenuPane(laneOperations);
        ret->insertMenuPaneChild(transformSlanes);
        // Create menu comands for all add special lanes
        FXMenuCommand* addSidewalk = new FXMenuCommand(addSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_ADD_SIDEWALK);
        FXMenuCommand* addBikelane = new FXMenuCommand(addSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_ADD_BIKE);
        FXMenuCommand* addBuslane = new FXMenuCommand(addSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_LANE_ADD_BUS);
        FXMenuCommand* addGreenVerge = new FXMenuCommand(addSpecialLanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE);
        // Create menu comands for all remove special lanes and disable it
        FXMenuCommand* removeSidewalk = new FXMenuCommand(removeSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_REMOVE_SIDEWALK);
        removeSidewalk->disable();
        FXMenuCommand* removeBikelane = new FXMenuCommand(removeSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_REMOVE_BIKE);
        removeBikelane->disable();
        FXMenuCommand* removeBuslane = new FXMenuCommand(removeSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_LANE_REMOVE_BUS);
        removeBuslane->disable();
        FXMenuCommand* removeGreenVerge = new FXMenuCommand(removeSpecialLanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_REMOVE_GREENVERGE);
        removeGreenVerge->disable();
        // Create menu comands for all trasform special lanes and disable it
        FXMenuCommand* transformLaneToSidewalk = new FXMenuCommand(transformSlanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_TRANSFORM_SIDEWALK);
        FXMenuCommand* transformLaneToBikelane = new FXMenuCommand(transformSlanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_TRANSFORM_BIKE);
        FXMenuCommand* transformLaneToBuslane = new FXMenuCommand(transformSlanes, "Buslane", busIcon, &parent, MID_GNE_LANE_TRANSFORM_BUS);
        FXMenuCommand* transformLaneToGreenVerge = new FXMenuCommand(transformSlanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_TRANSFORM_GREENVERGE);
        // add menuCascade for lane operations
        FXMenuCascade* cascadeAddSpecialLane = new FXMenuCascade(laneOperations, ("add restricted " + toString(SUMO_TAG_LANE)).c_str(), nullptr, addSpecialLanes);
        FXMenuCascade* cascadeRemoveSpecialLane = new FXMenuCascade(laneOperations, ("remove restricted " + toString(SUMO_TAG_LANE)).c_str(), nullptr, removeSpecialLanes);
        new FXMenuCascade(laneOperations, ("transform to restricted " + toString(SUMO_TAG_LANE)).c_str(), nullptr, transformSlanes);
        // Enable and disable options depending of current transform of the lane
        if (edgeHasSidewalk) {
            transformLaneToSidewalk->disable();
            addSidewalk->disable();
            removeSidewalk->enable();
        }
        if (edgeHasBikelane) {
            transformLaneToBikelane->disable();
            addBikelane->disable();
            removeBikelane->enable();
        }
        if (edgeHasBuslane) {
            transformLaneToBuslane->disable();
            addBuslane->disable();
            removeBuslane->enable();
        }
        if (edgeHasGreenVerge) {
            transformLaneToGreenVerge->disable();
            addGreenVerge->disable();
            removeGreenVerge->enable();
        }
        // Check if cascade menus must be disabled
        if (edgeHasSidewalk && edgeHasBikelane && edgeHasBuslane && edgeHasGreenVerge) {
            cascadeAddSpecialLane->disable();
        }
        if (!edgeHasSidewalk && !edgeHasBikelane && !edgeHasBuslane && !edgeHasGreenVerge) {
            cascadeRemoveSpecialLane->disable();
        }
    } else if (editMode == GNE_NMODE_TLS) {
        if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->controlsEdge(myParentEdge)) {
            new FXMenuCommand(ret, "Select state for all links from this edge:", nullptr, nullptr, 0);
            const std::vector<std::string> names = GNEInternalLane::LinkStateNames.getStrings();
            for (auto it : names) {
                FXuint state = GNEInternalLane::LinkStateNames.get(it);
                FXMenuRadio* mc = new FXMenuRadio(ret, it.c_str(), this, FXDataTarget::ID_OPTION + state);
                mc->setSelBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                mc->setBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
            }
        }
    } else {
        FXMenuCommand* mc = new FXMenuCommand(ret, "Additional options available in 'Inspect Mode'", nullptr, nullptr, 0);
        mc->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    }
    // buildShowParamsPopupEntry(ret, false);
    // build shape positions menu
    if (editMode != GNE_NMODE_TLS) {
        new FXMenuSeparator(ret);
        const double pos = getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        const double height = getShape().positionAtOffset2D(getShape().nearest_offset_to_point2D(parent.getPositionInformation())).z();
        new FXMenuCommand(ret, ("Shape pos: " + toString(pos)).c_str(), nullptr, nullptr, 0);
        new FXMenuCommand(ret, ("Length pos: " + toString(pos * getLaneParametricLength() / getLaneShapeLength())).c_str(), nullptr, nullptr, 0);
        new FXMenuCommand(ret, ("Height: " + toString(height)).c_str(), nullptr, nullptr, 0);
    }
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);
    return ret;
}


Boundary
GNELane::getCenteringBoundary() const {
    // Lanes don't use myMovingGeometryBoundary
    Boundary b = getShape().getBoxBoundary();
    b.grow(10);
    return b;
}


const PositionVector&
GNELane::getShape() const {
    return myParentEdge.getNBEdge()->getLaneShape(myIndex);
}


const std::vector<double>&
GNELane::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<double>&
GNELane::getShapeLengths() const {
    return myShapeLengths;
}


Boundary
GNELane::getBoundary() const {
    if (myParentEdge.getNBEdge()->getLaneStruct(myIndex).customShape.size() == 0) {
        return myParentEdge.getNBEdge()->getLaneStruct(myIndex).shape.getBoxBoundary();
    } else {
        return myParentEdge.getNBEdge()->getLaneStruct(myIndex).customShape.getBoxBoundary();
    }
}


void
GNELane::updateGeometry(bool updateGrid) {
    // Clear containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    myLaneRestrictedTexturePositions.clear();
    myLaneRestrictedTextureRotations.clear();
    //double length = myParentEdge.getLength(); // @todo see ticket #448
    // may be different from length

    // Obtain lane and shape rotations
    int segments = (int) getShape().size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = getShape()[i];
            const Position& s = getShape()[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
    // update additional childs
    for (auto i : myAdditionalChilds) {
        i->updateGeometry(updateGrid);
    }
    // update additionals with this lane as chid
    for (auto i : myFirstAdditionalParents) {
        i->updateGeometry(updateGrid);
    }
    // update POIs associated to this lane
    for (auto i : myShapes) {
        i->updateGeometry(updateGrid);
    }
    // In Move mode, connections aren't updated
    if (myNet->getViewNet() && myNet->getViewNet()->getCurrentNetworkEditMode() != GNE_NMODE_MOVE) {
        // Update incoming connections of this lane
        auto incomingConnections = getGNEIncomingConnections();
        for (auto i : incomingConnections) {
            i->updateGeometry(updateGrid);
        }
        // Update outgoings connections of this lane
        auto outGoingConnections = getGNEOutcomingConnections();
        for (auto i : outGoingConnections) {
            i->updateGeometry(updateGrid);
        }
    }
    // If lane has enought length for show textures of restricted lanes
    if ((getLaneShapeLength() > 4)) {
        // if lane is restricted
        if (isRestricted(SVC_PEDESTRIAN) || isRestricted(SVC_BICYCLE) || isRestricted(SVC_BUS)) {
            // get values for position and rotation of icons
            for (int i = 2; i < getLaneShapeLength() - 1; i += 15) {
                myLaneRestrictedTexturePositions.push_back(getShape().positionAtOffset(i));
                myLaneRestrictedTextureRotations.push_back(getShape().rotationDegreeAtOffset(i));
            }
        }
    }
}

int
GNELane::getIndex() const {
    return myIndex;
}

void
GNELane::setIndex(int index) {
    myIndex = index;
    setMicrosimID(myParentEdge.getNBEdge()->getLaneID(index));
}


double
GNELane::getSpeed() const {
    return myParentEdge.getNBEdge()->getLaneSpeed(myIndex);
}


double
GNELane::getLaneParametricLength() const  {
    double laneParametricLenght = myParentEdge.getNBEdge()->getLoadedLength();
    if (laneParametricLenght > 0) {
        return laneParametricLenght;
    } else {
        throw ProcessError("Lane Parametric Lenght cannot be never 0");
    }
}


double
GNELane::getLaneShapeLength() const {
    return getShape().length();
}


void
GNELane::addShapeChild(GNEShape* shape) {
    // Check if Shape exist before remove
    if (std::find(myShapes.begin(), myShapes.end(), shape) == myShapes.end()) {
        myShapes.push_back(shape);
        // update Geometry of shape after add
        shape->updateGeometry(true);
    } else {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in lane with ID='" + getID() + "'");
    }
}


void
GNELane::removeShapeChild(GNEShape* shape) {
    auto it = std::find(myShapes.begin(), myShapes.end(), shape);
    // Check if Shape exist before remove
    if (it != myShapes.end()) {
        myShapes.erase(it);
    } else {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in lane with ID='" + getID() + "'");
    }
}


const std::vector<GNEShape*>&
GNELane::getShapeChilds() const {
    return myShapes;
}


bool
GNELane::isRestricted(SUMOVehicleClass vclass) const {
    return myParentEdge.getNBEdge()->getPermissions(myIndex) == vclass;
}


std::string
GNELane::getAttribute(SumoXMLAttr key) const {
    const NBEdge* edge = myParentEdge.getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_SPEED:
            return toString(edge->getLaneSpeed(myIndex));
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(edge->getPermissions(myIndex));
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(edge->getPermissions(myIndex)));
        case SUMO_ATTR_WIDTH:
            return toString(edge->getLaneStruct(myIndex).width);
        case SUMO_ATTR_ENDOFFSET:
            return toString(edge->getLaneStruct(myIndex).endOffset);
        case SUMO_ATTR_ACCELERATION:
            return toString(edge->getLaneStruct(myIndex).accelRamp);
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(edge->getLaneStruct(myIndex).customShape);
        case SUMO_ATTR_INDEX:
            return toString(myIndex);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

std::string
GNELane::getAttributeForSelection(SumoXMLAttr key) const {
    std::string result = getAttribute(key);
    if ((key == SUMO_ATTR_ALLOW || key == SUMO_ATTR_DISALLOW) && result.find("all") != std::string::npos) {
        result += " " + getVehicleClassNames(SVCAll, true);
    }
    return result;
}


void
GNELane::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_ENDOFFSET:
        case SUMO_ATTR_ACCELERATION:
        case SUMO_ATTR_CUSTOMSHAPE:
        case SUMO_ATTR_INDEX:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELane::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_SPEED:
            return canParse<double>(value);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
        case SUMO_ATTR_ENDOFFSET:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_ACCELERATION:
            return canParse<bool>(value);
        case SUMO_ATTR_CUSTOMSHAPE: {
            // A lane shape can either be empty or have more than 1 element
            if (value.empty()) {
                return true;
            } else if (canParse<PositionVector>(value)) {
                return parse<PositionVector>(value).size() > 1;
            }
            return false;
        }
        case SUMO_ATTR_INDEX:
            return canParse<int>(value) && (parse<int>(value) == myIndex);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNELane::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : myParentEdge.getNBEdge()->getLaneStruct(myIndex).getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNELane::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : myParentEdge.getNBEdge()->getLaneStruct(myIndex).getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNELane::setGenericParametersStr(const std::string& value) {
    // clear parameters
    myParentEdge.getNBEdge()->getLaneStruct(myIndex).clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            myParentEdge.getNBEdge()->getLaneStruct(myIndex).setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}


void
GNELane::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}

// ===========================================================================
// private
// ===========================================================================

void
GNELane::setAttribute(SumoXMLAttr key, const std::string& value) {
    NBEdge* edge = myParentEdge.getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
            edge->setSpeed(myIndex, parse<double>(value));
            break;
        case SUMO_ATTR_ALLOW:
            edge->setPermissions(parseVehicleClasses(value), myIndex);
            break;
        case SUMO_ATTR_DISALLOW:
            edge->setPermissions(invertPermissions(parseVehicleClasses(value)), myIndex);
            break;
        case SUMO_ATTR_WIDTH:
            edge->setLaneWidth(myIndex, parse<double>(value));
            break;
        case SUMO_ATTR_ENDOFFSET:
            edge->setEndOffset(myIndex, parse<double>(value));
            break;
        case SUMO_ATTR_ACCELERATION:
            edge->setAcceleration(myIndex, parse<bool>(value));
            break;
        case SUMO_ATTR_CUSTOMSHAPE: {
            // first remove edge parent from net
            myNet->removeGLObjectFromGrid(&myParentEdge);
            // set new shape
            edge->setLaneShape(myIndex, parse<PositionVector>(value));
            // add edge parent into net again
            myNet->addGLObjectIntoGrid(&myParentEdge);
            break;
        }
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        updateGeometry(true);
    }
}


void
GNELane::mouseOverObject(const GUIVisualizationSettings&) const {
}


void
GNELane::setLaneColor(const GUIVisualizationSettings& s) const {
    if (mySpecialColor != nullptr) {
        // If special color is enabled, set it
        GLHelper::setColor(*mySpecialColor);
    } else if (isAttributeCarrierSelected() && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        GLHelper::setColor(s.selectedLaneColor);
    } else if (myParentEdge.isAttributeCarrierSelected() && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        GLHelper::setColor(s.selectedEdgeColor);
    } else {
        // Get normal lane color
        const GUIColorer& c = s.laneColorer;
        if (!setFunctionalColor(c.getActive()) && !setMultiColor(c)) {
            GLHelper::setColor(c.getScheme().getColor(getColorValue(s, c.getActive())));
        }
    }
}

bool
GNELane::setFunctionalColor(int activeScheme) const {
    switch (activeScheme) {
        case 6: {
            double hue = GeomHelper::naviDegree(getShape().beginEndAngle()); // [0-360]
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        default:
            return false;
    }
}


bool
GNELane::setMultiColor(const GUIColorer& c) const {
    const int activeScheme = c.getActive();
    myShapeColors.clear();
    switch (activeScheme) {
        case 9: // color by height at segment start
            for (PositionVector::const_iterator ii = getShape().begin(); ii != getShape().end() - 1; ++ii) {
                myShapeColors.push_back(c.getScheme().getColor(ii->z()));
            }
            return true;
        case 11: // color by inclination  at segment start
            for (int ii = 1; ii < (int)getShape().size(); ++ii) {
                const double inc = (getShape()[ii].z() - getShape()[ii - 1].z()) / MAX2(POSITION_EPS, getShape()[ii].distanceTo2D(getShape()[ii - 1]));
                myShapeColors.push_back(c.getScheme().getColor(inc));
            }
            return true;
        default:
            return false;
    }
}


double
GNELane::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    const SVCPermissions myPermissions = myParentEdge.getNBEdge()->getPermissions(myIndex);
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
                default:
                    break;
            }
            if (isRailway(myPermissions)) {
                return 5;
            } else if ((myPermissions & SVC_PASSENGER) != 0) {
                return 0;
            } else {
                return 6;
            }
        case 1:
            return isAttributeCarrierSelected() || myParentEdge.isAttributeCarrierSelected();
        case 2:
            return (double)myPermissions;
        case 3:
            return myParentEdge.getNBEdge()->getLaneSpeed(myIndex);
        case 4:
            return myParentEdge.getNBEdge()->getNumLanes();
        case 5: {
            return myParentEdge.getNBEdge()->getLoadedLength() / myParentEdge.getNBEdge()->getLength();
        }
        // case 6: by angle (functional)
        case 7: {
            return myParentEdge.getNBEdge()->getPriority();
        }
        case 8: {
            // color by z of first shape point
            return getShape()[0].z();
        }
        // case 9: by segment height
        case 10: {
            // color by incline
            return (getShape()[-1].z() - getShape()[0].z()) /  myParentEdge.getNBEdge()->getLength();
        }
        // case 11: by segment incline

        case 12: {
            // by numerical edge param value
            try {
                return StringUtils::toDouble(myParentEdge.getNBEdge()->getParameter(s.edgeParam, "0"));
            } catch (NumberFormatException&) {
                WRITE_WARNING("Edge parameter '" + myParentEdge.getNBEdge()->getParameter(s.edgeParam, "0") + "' key '" + s.edgeParam + "' is not a number for edge '" + myParentEdge.getID() + "'");
                return 0;
            }
        }
        case 13: {
            // by numerical lane param value
            try {
                return StringUtils::toDouble(myParentEdge.getNBEdge()->getLaneStruct(myIndex).getParameter(s.laneParam, "0"));
            } catch (NumberFormatException&) {
                WRITE_WARNING("Lane parameter '" + myParentEdge.getNBEdge()->getLaneStruct(myIndex).getParameter(s.laneParam, "0") + "' key '" + s.laneParam + "' is not a number for lane '" + getID() + "'");
                return 0;
            }
        }
    }
    return 0;
}


void
GNELane::removeLaneOfAdditionalParents(GNEUndoList* undoList, bool allowEmpty) {
    // iterate over all additional parents of lane
    for (auto i : myFirstAdditionalParents) {
        // Obtain attribute LANES of additional
        std::vector<std::string>  laneIDs = parse<std::vector<std::string> >(i->getAttribute(SUMO_ATTR_LANES));
        // check that at least there is an lane
        if (laneIDs.empty()) {
            throw ProcessError("Additional lane childs is empty");
        } else if ((laneIDs.size() == 1) && (allowEmpty == false)) {
            // remove entire Additional if SUMO_ATTR_LANES cannot be empty
            if (laneIDs.front() == getID()) {
                undoList->add(new GNEChange_Additional(i, false), true);
            } else {
                throw ProcessError("lane ID wasnt' found in Additional");
            }
        } else {
            auto it = std::find(laneIDs.begin(), laneIDs.end(), getID());
            if (it != laneIDs.end()) {
                // set new attribute in Additional
                laneIDs.erase(it);
                i->setAttribute(SUMO_ATTR_LANES, toString(laneIDs), undoList);
            } else {
                throw ProcessError("lane ID wasnt' found in Additional");
            }
        }
    }
}


bool
GNELane::drawAsRailway(const GUIVisualizationSettings& s) const {
    return isRailway(myParentEdge.getNBEdge()->getPermissions(myIndex)) && s.showRails && (!s.drawForSelecting || s.spreadSuperposed);
}


bool
GNELane::drawAsWaterway(const GUIVisualizationSettings& s) const {
    return isWaterway(myParentEdge.getNBEdge()->getPermissions(myIndex)) && s.showRails && !s.drawForSelecting; // reusing the showRails setting
}


void
GNELane::drawDirectionIndicators(double exaggeration, bool spreadSuperposed) const {
    const double width = MAX2(NUMERICAL_EPS, (myParentEdge.getNBEdge()->getLaneWidth(myIndex) * exaggeration
                              * (spreadSuperposed ? 0.4 : 1)));
    const double sideOffset = spreadSuperposed ? width * -0.5 : 0;
    glPushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.1);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (double t = 0; t < myShapeLengths[i]; t += width) {
            const double length = MIN2(width * 0.5, myShapeLengths[i] - t);
            glBegin(GL_TRIANGLES);
            glVertex2d(sideOffset, -t - length);
            glVertex2d(sideOffset - width * 0.25, -t);
            glVertex2d(sideOffset + width * 0.25, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
}



std::string
GNELane::getParentName() const {
    return myParentEdge.getMicrosimID();
}


long
GNELane::onDefault(FXObject* obj, FXSelector sel, void* data) {
    myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->handleMultiChange(this, obj, sel, data);
    return 1;
}


GNEEdge&
GNELane::getParentEdge() {
    return myParentEdge;
}


std::vector<GNEConnection*>
GNELane::getGNEIncomingConnections() {
    // Declare a vector to save incoming connections
    std::vector<GNEConnection*> incomingConnections;
    // Obtain incoming edges if junction source was already created
    GNEJunction* junctionSource =  myParentEdge.getGNEJunctionSource();
    if (junctionSource) {
        // Iterate over incoming GNEEdges of junction
        for (auto i : junctionSource->getGNEIncomingEdges()) {
            // Iterate over connection of incoming edges
            for (auto j : i->getGNEConnections()) {
                if (j->getNBEdgeConnection().fromLane == getIndex()) {
                    incomingConnections.push_back(j);
                }
            }
        }
    }
    return incomingConnections;
}


std::vector<GNEConnection*>
GNELane::getGNEOutcomingConnections() {
    // Obtain GNEConnection of edge parent
    const std::vector<GNEConnection*>& edgeConnections = myParentEdge.getGNEConnections();
    std::vector<GNEConnection*> outcomingConnections;
    // Obtain outgoing connections
    for (auto i : edgeConnections) {
        if (i->getNBEdgeConnection().fromLane == getIndex()) {
            outcomingConnections.push_back(i);
        }
    }
    return outcomingConnections;
}


void
GNELane::updateConnectionIDs() {
    // update incoming connections of lane
    std::vector<GNEConnection*> incomingConnections = getGNEIncomingConnections();
    for (auto i : incomingConnections) {
        i->updateID();
    }
    // update outocming connections of lane
    std::vector<GNEConnection*> outcomingConnections = getGNEOutcomingConnections();
    for (auto i : outcomingConnections) {
        i->updateID();
    }
}


double
GNELane::getLengthGeometryFactor() const {
    // factor should not be 0
    if (myParentEdge.getNBEdge()->getFinalLength() > 0) {
        return MAX2(POSITION_EPS, (myParentEdge.getNBEdge()->getLaneShape(myIndex).length() / myParentEdge.getNBEdge()->getFinalLength()));
    } else {
        return POSITION_EPS;
    };
}


void
GNELane::startGeometryMoving() {
    // Lanes don't need to save the current Centering Boundary, due they are parts of an Edge
    // Save current centering boundary of additional childs
    for (auto i : myAdditionalChilds) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of additionals with this lane as chid
    for (auto i : myFirstAdditionalParents) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of POIs associated to this lane
    for (auto i : myShapes) {
        i->startGeometryMoving();
    }
}


void
GNELane::endGeometryMoving() {
    // Lanes don't need to save the current Centering Boundary, due they are parts of an Edge
    // Restore centering boundary of additionals with this lane as chid
    for (auto i : myAdditionalChilds) {
        i->endGeometryMoving();
    }
    // Restore centering boundary of additionals with this lane as chid
    for (auto i : myFirstAdditionalParents) {
        i->endGeometryMoving();
    }
    // Restore centering boundary of POIs associated to this lane
    for (auto i : myShapes) {
        i->endGeometryMoving();
    }
}

/****************************************************************************/
