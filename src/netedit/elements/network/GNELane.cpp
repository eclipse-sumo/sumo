/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNELane.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A class for visualizing Lane geometry (adapted from GNELaneWrapper)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNEDeleteFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <netedit/frames/demand/GNERouteFrame.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNELane.h"
#include "GNEInternalLane.h"
#include "GNEConnection.h"
#include "GNEEdgeTemplate.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(GNELane, FXDelegator, 0, 0)

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNELane::LaneDrawingConstants - methods
// ---------------------------------------------------------------------------

GNELane::LaneDrawingConstants::LaneDrawingConstants(const GUIVisualizationSettings& s, const GNELane* lane) :
    selectionScale(lane->isAttributeCarrierSelected() || lane->myParentEdge->isAttributeCarrierSelected() ? s.selectorFrameScale : 1),
    exaggeration(selectionScale * s.laneWidthExaggeration),
    halfWidth2(exaggeration * (lane->myParentEdge->getNBEdge()->getLaneWidth(lane->getIndex()) / 2 - SUMO_const_laneMarkWidth / 2)),
    halfWidth(lane->drawUsingSelectColor() ? halfWidth2 - exaggeration * 0.3 : halfWidth2) {
    // start drawing lane checking whether it is not too small
    //selectionScale = lane->isAttributeCarrierSelected() || lane->myParentEdge->isAttributeCarrierSelected() ? s.selectionScale : 1;
    //exaggeration = selectionScale * s.laneWidthExaggeration; // * s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));
    // compute lane-marking intersection points)
    //halfWidth2 = exaggeration * (lane->myParentEdge->getNBEdge()->getLaneWidth(lane->getIndex()) / 2 - SUMO_const_laneMarkWidth / 2);

    // Draw as a normal lane, and reduce width to make sure that a selected edge can still be seen
    //halfWidth =  lane->drawUsingSelectColor() ? halfWidth2 - exaggeration * 0.3 : halfWidth2;
}


GNELane::LaneDrawingConstants::LaneDrawingConstants() :
    selectionScale(0),
    exaggeration(0),
    halfWidth2(0),
    halfWidth(0) {
}

// ---------------------------------------------------------------------------
// GNELane - methods
// ---------------------------------------------------------------------------

GNELane::GNELane(GNEEdge* edge, const int index) :
    GNENetworkElement(edge->getNet(), edge->getNBEdge()->getLaneID(index), GLO_LANE, SUMO_TAG_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
                                myParentEdge(edge),
                                myIndex(index),
                                mySpecialColor(nullptr),
                                mySpecialColorValue(-1),
myLane2laneConnections(this) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNELane::GNELane() :
    GNENetworkElement(nullptr, "dummyConstructorGNELane", GLO_LANE, SUMO_TAG_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
myParentEdge(nullptr),
myIndex(-1),
mySpecialColor(nullptr),
mySpecialColorValue(-1),
myLane2laneConnections(this) {
}


GNELane::~GNELane() {}


GNEEdge*
GNELane::getParentEdge() const {
    return myParentEdge;
}


bool
GNELane::allowPedestrians() const {
    return (myParentEdge->getNBEdge()->getPermissions(myIndex) & SVC_PEDESTRIAN) > 0;
}


const GUIGeometry&
GNELane::getLaneGeometry() const {
    return myLaneGeometry;
}


const PositionVector&
GNELane::getLaneShape() const {
    if (myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.size() > 0) {
        return myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape;
    } else {
        return myParentEdge->getNBEdge()->getLaneShape(myIndex);
    }
}


const std::vector<double>&
GNELane::getShapeRotations() const {
    return myLaneGeometry.getShapeRotations();
}


const std::vector<double>&
GNELane::getShapeLengths() const {
    return myLaneGeometry.getShapeLengths();
}


void
GNELane::updateGeometry() {
    // Clear texture containers
    myLaneRestrictedTexturePositions.clear();
    myLaneRestrictedTextureRotations.clear();
    // get lane shape and extend if is too short
    auto laneShape = getLaneShape();
    if (laneShape.length2D() < 1) {
        laneShape.extrapolate2D(1 - laneShape.length2D());
    }
    // Obtain lane shape of NBEdge
    myLaneGeometry.updateGeometry(laneShape);
    // update connections
    myLane2laneConnections.updateLane2laneConnection();
    // update additionals children associated with this lane
    for (const auto& additional : getParentAdditionals()) {
        additional->updateGeometry();
    }
    // update additionals parents associated with this lane
    for (const auto& additional : getChildAdditionals()) {
        additional->updateGeometry();
    }
    // update partial demand elements parents associated with this lane
    for (const auto& demandElement : getParentDemandElements()) {
        demandElement->updateGeometry();
    }
    // update partial demand elements children associated with this lane
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->updateGeometry();
    }
    // Update geometry of parent generic datas that have this edge as parent
    for (const auto& additionalParent : getParentGenericDatas()) {
        additionalParent->updateGeometry();
    }
    // Update geometry of additionals generic datas vinculated to this edge
    for (const auto& childAdditionals : getChildGenericDatas()) {
        childAdditionals->updateGeometry();
    }
    // compute geometry of path elements elements vinculated with this lane (depending of showDemandElements)
    if (myNet->getViewNet() && myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
        for (const auto& childAdditional : getChildAdditionals()) {
            childAdditional->computePathElement();
        }
        for (const auto& childDemandElement : getChildDemandElements()) {
            childDemandElement->computePathElement();
        }
        for (const auto& childGenericData : getChildGenericDatas()) {
            childGenericData->computePathElement();
        }
    }
    // in Move mode, connections aren't updated
    if (myNet->getViewNet() && myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE) {
        // Update incoming connections of this lane
        const auto incomingConnections = getGNEIncomingConnections();
        for (const auto& connection : incomingConnections) {
            connection->updateGeometry();
        }
        // Update outgoings connections of this lane
        const auto outGoingConnections = getGNEOutcomingConnections();
        for (const auto& connection : outGoingConnections) {
            connection->updateGeometry();
        }
    }
    // if lane has enought length for show textures of restricted lanes
    if ((getLaneShapeLength() > 4)) {
        // if lane is restricted
        if (isRestricted(SVC_PEDESTRIAN) || isRestricted(SVC_BICYCLE) || isRestricted(SVC_BUS)) {
            // get values for position and rotation of icons
            for (int i = 2; i < getLaneShapeLength() - 1; i += 15) {
                myLaneRestrictedTexturePositions.push_back(myLaneGeometry.getShape().positionAtOffset(i));
                myLaneRestrictedTextureRotations.push_back(myLaneGeometry.getShape().rotationDegreeAtOffset(i));
            }
        }
    }
}


Position
GNELane::getPositionInView() const {
    return getLaneShape().positionAtOffset2D(getLaneShape().length2D() * 0.5);
}


GNEMoveOperation*
GNELane::getMoveOperation() {
    // edit depending if shape is being edited
    if (isShapeEdited()) {
        // calculate move shape operation
        return calculateMoveShapeOperation(getLaneShape(), myNet->getViewNet()->getPositionInformation(),
                                           myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.laneGeometryPointRadius, true);
    } else {
        return nullptr;
    }
}


void
GNELane::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (isShapeEdited()) {
        // get original shape
        PositionVector shape = getLaneShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.laneGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(GUIIcon::CROSSING, "remove geometry point of " + getTagStr());
                undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(shape)));
                undoList->end();
            }
        }
    }
}


void
GNELane::drawLinkNo(const GUIVisualizationSettings& s) const {
    // only draw links number depending of the scale and if isn't being drawn for selecting
    if ((s.scale >= 10) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
        // first check that drawLinkJunctionIndex must be drawn
        if (s.drawLinkJunctionIndex.show(myParentEdge->getToJunction())) {
            // get connections
            const std::vector<NBEdge::Connection>& cons = myParentEdge->getNBEdge()->getConnectionsFromLane(myIndex);
            // get number of links
            const int noLinks = (int)cons.size();
            // only continue if there is links
            if (noLinks > 0) {
                // push link matrix
                GLHelper::pushMatrix();
                // move front
                glTranslated(0, 0, GLO_TEXTNAME);
                // calculate width
                const double width = myParentEdge->getNBEdge()->getLaneWidth(myIndex) / (double) noLinks;
                // get X1
                double x1 = myParentEdge->getNBEdge()->getLaneWidth(myIndex) / 2;
                // iterate over links
                for (int i = noLinks; --i >= 0;) {
                    // calculate x2
                    const double x2 = x1 - (double)(width / 2.);
                    // get link index
                    const int linkIndex = myParentEdge->getNBEdge()->getToNode()->getConnectionIndex(myParentEdge->getNBEdge(),
                                          cons[s.lefthand ? noLinks - 1 - i : i]);
                    // draw link index
                    GLHelper::drawTextAtEnd(toString(linkIndex), myLaneGeometry.getShape(), x2, s.drawLinkJunctionIndex, s.scale);
                    // update x1
                    x1 -= width;
                }
                // pop link matrix
                GLHelper::popMatrix();
            }
        }
        // draw TLSLink No
        if (s.drawLinkTLIndex.show(myParentEdge->getToJunction()) && (myParentEdge->getToJunction()->getNBNode()->getControllingTLS().size() > 0)) {
            // get connections
            const std::vector<NBEdge::Connection>& cons = myParentEdge->getNBEdge()->getConnectionsFromLane(myIndex);
            // get numer of links
            const int noLinks = (int)cons.size();
            // only continue if there are links
            if (noLinks > 0) {
                // push link matrix
                GLHelper::pushMatrix();
                // move t front
                glTranslated(0, 0, GLO_TEXTNAME);
                // calculate width
                const double w = myParentEdge->getNBEdge()->getLaneWidth(myIndex) / (double) noLinks;
                // calculate x1
                double x1 = myParentEdge->getNBEdge()->getLaneWidth(myIndex) / 2;
                // iterate over links
                for (int i = noLinks; --i >= 0;) {
                    // calculate x2
                    const double x2 = x1 - (double)(w / 2.);
                    // get link number
                    const int linkNo = cons[s.lefthand ? noLinks - 1 - i : i].tlLinkIndex;
                    // draw link number
                    GLHelper::drawTextAtEnd(toString(linkNo), myLaneGeometry.getShape(), x2, s.drawLinkTLIndex, s.scale);
                    // update x1
                    x1 -= w;
                }
                // pop link matrix
                GLHelper::popMatrix();
            }
        }
    }
}


void
GNELane::drawArrows(const GUIVisualizationSettings& s, const bool spreadSuperposed) const {
    if (s.showLinkDecals && myParentEdge->getToJunction()->isLogicValid() && s.scale > 3) {
        // calculate begin, end and rotation
        const Position& begin = myLaneGeometry.getShape()[-2];
        const Position& end = myLaneGeometry.getShape().back();
        const double rot = GUIGeometry::calculateRotation(begin, end);
        // push arrow matrix
        GLHelper::pushMatrix();
        // move front (note: must draw on top of junction shape?
        glTranslated(0, 0, 0.5);
        // change color depending of spreadSuperposed
        if (spreadSuperposed) {
            GLHelper::setColor(RGBColor::CYAN);
        } else {
            GLHelper::setColor(RGBColor::WHITE);
        }
        // move to end
        glTranslated(end.x(), end.y(), 0);
        // rotate
        glRotated(rot, 0, 0, 1);
        const double width = myParentEdge->getNBEdge()->getLaneWidth(myIndex);
        if (width < SUMO_const_laneWidth) {
            glScaled(width / SUMO_const_laneWidth, 1, 1);
        }
        // get destiny node
        const NBNode* dest = myParentEdge->getNBEdge()->myTo;
        // draw all links iterating over connections
        for (const auto& connection : myParentEdge->getNBEdge()->myConnections) {
            if (connection.fromLane == myIndex) {
                // get link direction
                LinkDirection dir = dest->getDirection(myParentEdge->getNBEdge(), connection.toEdge, s.lefthand);
                // draw depending of link direction
                switch (dir) {
                    case LinkDirection::STRAIGHT:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
                        GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
                        break;
                    case LinkDirection::LEFT:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), 90, 1, .05);
                        GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.5, 2.5), (double) 1, (double) .25);
                        break;
                    case LinkDirection::RIGHT:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                        GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.5, 2.5), (double) 1, (double) .25);
                        break;
                    case LinkDirection::TURN:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), 90, .5, .05);
                        GLHelper::drawBoxLine(Position(0.5, 2.5), 180, 1, .05);
                        GLHelper::drawTriangleAtEnd(Position(0.5, 2.5), Position(0.5, 4), (double) 1, (double) .25);
                        break;
                    case LinkDirection::TURN_LEFTHAND:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                        GLHelper::drawBoxLine(Position(-0.5, 2.5), -180, 1, .05);
                        GLHelper::drawTriangleAtEnd(Position(-0.5, 2.5), Position(-0.5, 4), (double) 1, (double) .25);
                        break;
                    case LinkDirection::PARTLEFT:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), 45, .7, .05);
                        GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.2, 1.3), (double) 1, (double) .25);
                        break;
                    case LinkDirection::PARTRIGHT:
                        GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                        GLHelper::drawBoxLine(Position(0, 2.5), -45, .7, .05);
                        GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.2, 1.3), (double) 1, (double) .25);
                        break;
                    case LinkDirection::NODIR:
                        GLHelper::drawBoxLine(Position(1, 5.8), 245, 2, .05);
                        GLHelper::drawBoxLine(Position(-1, 5.8), 115, 2, .05);
                        glTranslated(0, 5, 0);
                        GLHelper::drawOutlineCircle(0.9, 0.8, 32);
                        glTranslated(0, -5, 0);
                        break;
                }
            }
        }
        // pop arrow matrix
        GLHelper::popMatrix();
    }
}


void
GNELane::drawLane2LaneConnections() const {
    GLHelper::pushMatrix();
    glTranslated(0, 0, 0.1); // must draw on top of junction shape
    std::vector<NBEdge::Connection> connections = myParentEdge->getNBEdge()->getConnectionsFromLane(myIndex);
    NBNode* node = myParentEdge->getNBEdge()->getToNode();
    const Position& startPos = myLaneGeometry.getShape()[-1];
    for (auto it : connections) {
        const LinkState state = node->getLinkState(myParentEdge->getNBEdge(), it.toEdge, it.fromLane, it.toLane, it.mayDefinitelyPass, it.tlID);
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
                throw ProcessError(TLF("Unexpected LinkState '%'", toString(state)));
        }
        const Position& endPos = it.toEdge->getLaneShape(it.toLane)[0];
        glBegin(GL_LINES);
        glVertex2d(startPos.x(), startPos.y());
        glVertex2d(endPos.x(), endPos.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(startPos, endPos, (double) 1.5, (double) .2);
    }
    GLHelper::popMatrix();
}


void
GNELane::drawGL(const GUIVisualizationSettings& s) const {
    // get lane drawing constants
    const LaneDrawingConstants laneDrawingConstants(s, this);
    // get lane color
    const RGBColor color = setLaneColor(s);
    // avoid draw invisible elements
    if (color.alpha() == 0) {
        return;
    }
    // get flag for draw lane as railway
    const bool drawRailway = drawAsRailway(s);
    // we draw the lanes with reduced width so that the lane markings below are visible (this avoids artifacts at geometry corners without having to)
    const bool spreadSuperposed = s.spreadSuperposed && drawRailway && myParentEdge->getNBEdge()->isBidiRail();
    // Push edge parent name
    GLHelper::pushName(myParentEdge->getGlID());
    // Push lane name
    GLHelper::pushName(getGlID());
    // Push layer matrix
    GLHelper::pushMatrix();
    // translate to front (note: Special case)
    if (myNet->getViewNet()->getFrontAttributeCarrier() == myParentEdge) {
        glTranslated(0, 0, GLO_FRONTELEMENT);
    } else if (myLaneGeometry.getShape().length2D() <= (s.neteditSizeSettings.junctionBubbleRadius * 2)) {
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_JUNCTION + 0.5);
    } else {
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_LANE);
    }
    // recognize full transparency and simply don't draw
    if ((color.alpha() == 0) || ((s.scale * laneDrawingConstants.exaggeration) < s.laneMinSize)) {
        // Pop draw matrix 1
        GLHelper::popMatrix();
        // Pop Lane Name
        GLHelper::popName();
    } else {
        if ((s.scale * laneDrawingConstants.exaggeration) < 1.) {
            // draw lane as line, depending of myShapeColors
            if (myShapeColors.size() > 0) {
                GLHelper::drawLine(myLaneGeometry.getShape(), myShapeColors);
            } else {
                GLHelper::drawLine(myLaneGeometry.getShape());
            }
        } else {
            // Check if lane has to be draw as railway and if isn't being drawn for selecting
            if (drawRailway && (!s.drawForRectangleSelection || spreadSuperposed)) {
                // draw as railway
                drawLaneAsRailway(s, laneDrawingConstants);
            } else {
                double offset = 0;
                double widthFactor = 1;
                if (s.spreadSuperposed && myParentEdge->getNBEdge()->getBidiEdge() != nullptr) {
                    widthFactor = 0.4; // create visible gap
                    offset = laneDrawingConstants.halfWidth * 0.5;
                }
                // draw as box lines
                GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), myShapeColors, laneDrawingConstants.halfWidth * widthFactor, 0, offset);
            }
            if (laneDrawingConstants.halfWidth != laneDrawingConstants.halfWidth2 && !spreadSuperposed) {
                // Push matrix
                GLHelper::pushMatrix();
                // move back
                glTranslated(0, 0, -0.1);
                // set selected edge color
                GLHelper::setColor(s.colorSettings.selectedEdgeColor);
                // draw again to show the selected edge
                GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), {}, laneDrawingConstants.halfWidth2);
                // Pop matrix
                GLHelper::popMatrix();
            }
            // only draw details depending of the scale and if isn't being drawn for selecting
            if ((s.scale >= 10) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
                // draw markings
                drawMarkings(s, laneDrawingConstants.exaggeration, drawRailway);
                // Draw direction indicators
                drawDirectionIndicators(s, laneDrawingConstants.exaggeration, drawRailway, spreadSuperposed);
            }
            // draw lane textures
            drawTextures(s, laneDrawingConstants);
            // draw start end shape points
            drawStartEndShapePoints(s);
        }
        // draw stopOffsets
        const auto& laneStopOffset = myParentEdge->getNBEdge()->getLaneStruct(myIndex).laneStopOffset;
        if (laneStopOffset.isDefined() && (laneStopOffset.getPermissions() & SVC_PASSENGER) != 0) {
            drawLaneStopOffset(s, laneStopOffset.getOffset());
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // draw lane arrows
        drawLaneArrows(s, laneDrawingConstants.exaggeration, spreadSuperposed);
        // draw shape edited
        drawShapeEdited(s);
        // Pop lane Name
        GLHelper::popName();
        // Pop edge Name
        GLHelper::popName();
        // draw link numbers
        drawLinkNo(s);
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), 1);
        // check if mouse is over element
        checkMouseOverLane(laneDrawingConstants.halfWidth);
        // draw dotted contours
        drawDottedContours(s, drawRailway, laneDrawingConstants.halfWidth);
        // draw children
        drawChildren(s);
        // draw path additional elements
        myNet->getPathManager()->drawLanePathElements(s, this);
    }
}


void
GNELane::deleteGLObject() {
    // Check if edge can be deleted
    if (GNEDeleteFrame::SubordinatedElements(this).checkElements(myNet->getViewNet()->getViewParent()->getDeleteFrame()->getProtectElements())) {
        myNet->deleteLane(this, myNet->getViewNet()->getUndoList(), false);
    }
}


void
GNELane::updateGLObject() {
    updateGeometry();
}


void
GNELane::drawChildren(const GUIVisualizationSettings& s) const {
    // draw additional children
    for (const auto& additional : getChildAdditionals()) {
        // check that ParkingAreas aren't draw two times
        additional->drawGL(s);
    }
    // draw demand element children
    for (const auto& demandElement : getChildDemandElements()) {
        if (!demandElement->getTagProperty().isPlacedInRTree()) {
            demandElement->drawGL(s);
        }
    }
}


void
GNELane::drawMarkings(const GUIVisualizationSettings& s, const double exaggeration, const bool drawRailway) const {
    if (s.laneShowBorders && (exaggeration == 1) && !drawRailway) {
        const double myHalfLaneWidth = myParentEdge->getNBEdge()->getLaneWidth(myIndex) / 2;
        GLHelper::pushMatrix();
        glTranslated(0, 0, 0.1);
        // optionally draw inverse markings
        bool haveChangeProhibitions = false;
        if (myIndex > 0 && (myParentEdge->getNBEdge()->getPermissions(myIndex - 1) & myParentEdge->getNBEdge()->getPermissions(myIndex)) != 0) {
            const bool cl = myParentEdge->getNBEdge()->allowsChangingLeft(myIndex - 1, SVC_PASSENGER);
            const bool cr = myParentEdge->getNBEdge()->allowsChangingRight(myIndex, SVC_PASSENGER);
            GLHelper::drawInverseMarkings(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(),
                                          3, 6, myHalfLaneWidth, cl, cr, s.lefthand, exaggeration);
            haveChangeProhibitions = !(cl && cr);
        }
        GLHelper::popMatrix();
        GLHelper::pushMatrix();
        if (haveChangeProhibitions) {
            // highlightchange prohibitions
            glTranslated(0, 0, -0.05);
            GLHelper::setColor(RGBColor::ORANGE);
            const double offset = myHalfLaneWidth * exaggeration * (s.lefthand ? -1 : 1);
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry, (myHalfLaneWidth * 0.5) * exaggeration, offset);
            glTranslated(0, 0, +0.05);
        }
        // draw white boundings and white markings
        glTranslated(0, 0, -0.1);
        GLHelper::setColor(RGBColor::WHITE);
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry, (myHalfLaneWidth + SUMO_const_laneMarkWidth) * exaggeration);
        GLHelper::popMatrix();
    }
}


GUIGLObjectPopupMenu*
GNELane::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // first obtain edit mode (needed because certain Commands depend of current edit mode)
    const NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
    // get mouse position
    const auto mousePosition = myNet->getViewNet()->getPositionInformation();
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    // build copy names entry
    if (editMode != NetworkEditMode::NETWORK_TLS) {
        GUIDesigns::buildFXMenuCommand(ret, "Copy parent edge name to clipboard", nullptr, ret, MID_COPY_EDGE_NAME);
        buildNameCopyPopupEntry(ret);
    }
    // stop if we're in data mode
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return ret;
    }
    // build lane selection
    if (isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(ret, "Remove Lane From Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), myNet->getViewNet(), MID_REMOVESELECT);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Add Lane To Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_PLUS), myNet->getViewNet(), MID_ADDSELECT);
    }
    // build edge selection
    if (myParentEdge->isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(ret, "Remove Edge From Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), myNet->getViewNet(), MID_GNE_REMOVESELECT_EDGE);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Add Edge To Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_PLUS), myNet->getViewNet(), MID_GNE_ADDSELECT_EDGE);
    }
    // stop if we're in data mode
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return ret;
    }
    // add separator
    new FXMenuSeparator(ret);
    if (editMode != NetworkEditMode::NETWORK_TLS) {
        // build show parameters menu
        buildShowParamsPopupEntry(ret);
        // build position copy entry
        buildPositionCopyEntry(ret, app);
    }
    // check if we're in supermode network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        // create end point
        FXMenuCommand* resetEndPoints = GUIDesigns::buildFXMenuCommand(ret, "Reset edge end points", nullptr, &parent, MID_GNE_RESET_GEOMETRYPOINT);
        // enable or disable reset end points
        if (myParentEdge->hasCustomEndPoints()) {
            resetEndPoints->enable();
        } else {
            resetEndPoints->disable();
        }
        // check if we clicked over a geometry point
        if ((editMode == NetworkEditMode::NETWORK_MOVE) && myParentEdge->clickedOverGeometryPoint(mousePosition)) {
            GUIDesigns::buildFXMenuCommand(ret, "Set custom Geometry Point", nullptr, &parent, MID_GNE_CUSTOM_GEOMETRYPOINT);
        }
        // add separator
        new FXMenuSeparator(ret);
        //build operations
        if ((editMode != NetworkEditMode::NETWORK_CONNECT) && (editMode != NetworkEditMode::NETWORK_TLS)) {
            // build edge operations
            buildEdgeOperations(parent, ret);
            // build lane operations
            buildLaneOperations(parent, ret);
            // build template operations
            buildTemplateOperations(parent, ret);
            // add separator
            new FXMenuSeparator(ret);
            // build rechable operations
            buildRechableOperations(parent, ret);
        } else if (editMode == NetworkEditMode::NETWORK_TLS) {
            if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->controlsEdge(myParentEdge)) {
                GUIDesigns::buildFXMenuCommand(ret, "Select state for all links from this edge:", nullptr, nullptr, 0);
                const std::vector<std::string> names = GNEInternalLane::LinkStateNames.getStrings();
                for (auto it : names) {
                    FXuint state = GNEInternalLane::LinkStateNames.get(it);
                    FXMenuRadio* mc = new FXMenuRadio(ret, it.c_str(), this, FXDataTarget::ID_OPTION + state);
                    mc->setSelBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                    mc->setBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                }
            }
        } else {
            FXMenuCommand* mc = GUIDesigns::buildFXMenuCommand(ret, "Additional options available in 'Inspect Mode'", nullptr, nullptr, 0);
            mc->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
        }
        // build shape positions menu
        if (editMode != NetworkEditMode::NETWORK_TLS) {
            new FXMenuSeparator(ret);
            // get lane shape
            const auto& laneShape = myLaneGeometry.getShape();
            // get variables
            const double pos = laneShape.nearest_offset_to_point2D(mousePosition);
            const Position firstAnglePos = laneShape.positionAtOffset2D(pos - 0.001);
            const Position secondAnglePos = laneShape.positionAtOffset2D(pos);
            const double angle = firstAnglePos.angleTo2D(secondAnglePos);

            // build menu commands
            GUIDesigns::buildFXMenuCommand(ret, "Shape pos: " + toString(pos), nullptr, nullptr, 0);
            GUIDesigns::buildFXMenuCommand(ret, "Length pos: " + toString(pos * getLaneParametricLength() / getLaneShapeLength()), nullptr, nullptr, 0);
            if (myParentEdge->getNBEdge()->getDistance() != 0) {
                GUIDesigns::buildFXMenuCommand(ret, "Distance: " + toString(myParentEdge->getNBEdge()->getDistancAt(pos)), nullptr, nullptr, 0);
            }
            GUIDesigns::buildFXMenuCommand(ret, "Height: " + toString(firstAnglePos.z()), nullptr, nullptr, 0);
            GUIDesigns::buildFXMenuCommand(ret, "Angle: " + toString((GeomHelper::naviDegree(angle))), nullptr, nullptr, 0);
        }
    }
    return ret;
}


double
GNELane::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


void
GNELane::updateCenteringBoundary(const bool /*updateGrid*/) {
    if (myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.size() == 0) {
        myBoundary = myParentEdge->getNBEdge()->getLaneStruct(myIndex).shape.getBoxBoundary();
    } else {
        myBoundary = myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.getBoxBoundary();
    }
}


int
GNELane::getIndex() const {
    return myIndex;
}

void
GNELane::setIndex(int index) {
    myIndex = index;
    setMicrosimID(myParentEdge->getNBEdge()->getLaneID(index));
}


double
GNELane::getSpeed() const {
    return myParentEdge->getNBEdge()->getLaneSpeed(myIndex);
}


double
GNELane::getLaneParametricLength() const  {
    double laneParametricLength = myParentEdge->getNBEdge()->getLoadedLength();
    if (laneParametricLength > 0) {
        return laneParametricLength;
    } else {
        throw ProcessError(TL("Lane Parametric Length cannot be never 0"));
    }
}


double
GNELane::getLaneShapeLength() const {
    return myLaneGeometry.getShape().length();
}


bool
GNELane::isRestricted(SUMOVehicleClass vclass) const {
    return myParentEdge->getNBEdge()->getPermissions(myIndex) == vclass;
}


const GNELane2laneConnection&
GNELane::getLane2laneConnections() const {
    return myLane2laneConnections;
}


std::string
GNELane::getAttribute(SumoXMLAttr key) const {
    const NBEdge* edge = myParentEdge->getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_SPEED:
            return toString(edge->getLaneSpeed(myIndex));
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(edge->getPermissions(myIndex));
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(edge->getPermissions(myIndex)));
        case SUMO_ATTR_CHANGE_LEFT:
            return getVehicleClassNames(edge->getLaneStruct(myIndex).changeLeft);
        case SUMO_ATTR_CHANGE_RIGHT:
            return getVehicleClassNames(edge->getLaneStruct(myIndex).changeRight);
        case SUMO_ATTR_WIDTH:
            if (edge->getLaneStruct(myIndex).width == NBEdge::UNSPECIFIED_WIDTH) {
                return "default";
            } else {
                return toString(edge->getLaneStruct(myIndex).width);
            }
        case SUMO_ATTR_FRICTION:
            return toString(edge->getLaneStruct(myIndex).friction);
        case SUMO_ATTR_ENDOFFSET:
            return toString(edge->getLaneStruct(myIndex).endOffset);
        case SUMO_ATTR_ACCELERATION:
            return toString(edge->getLaneStruct(myIndex).accelRamp);
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(edge->getLaneStruct(myIndex).customShape);
        case GNE_ATTR_OPPOSITE:
            return toString(edge->getLaneStruct(myIndex).oppositeID);
        case SUMO_ATTR_TYPE:
            return edge->getLaneStruct(myIndex).type;
        case SUMO_ATTR_INDEX:
            return toString(myIndex);
        case GNE_ATTR_STOPOFFSET:
            return toString(edge->getLaneStruct(myIndex).laneStopOffset.getOffset());
        case GNE_ATTR_STOPOEXCEPTION:
            if (edge->getLaneStruct(myIndex).laneStopOffset.isDefined()) {
                return toString(edge->getLaneStruct(myIndex).laneStopOffset.getExceptions());
            } else {
                return "";
            }
        case GNE_ATTR_PARENT:
            return myParentEdge->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return myParentEdge->getNBEdge()->getLaneStruct(myIndex).getParametersStr();
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
        case SUMO_ATTR_CHANGE_LEFT:
        case SUMO_ATTR_CHANGE_RIGHT:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_FRICTION:
        case SUMO_ATTR_ENDOFFSET:
        case SUMO_ATTR_ACCELERATION:
        case SUMO_ATTR_CUSTOMSHAPE:
        case GNE_ATTR_OPPOSITE:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_INDEX:
        case GNE_ATTR_STOPOFFSET:
        case GNE_ATTR_STOPOEXCEPTION:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            // no special handling
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELane::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_INDEX:
            return false;
        case SUMO_ATTR_SPEED:
            return canParse<double>(value);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_CHANGE_LEFT:
        case SUMO_ATTR_CHANGE_RIGHT:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            if (value.empty() || (value == "default")) {
                return true;
            } else {
                return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
            }
        case SUMO_ATTR_FRICTION:
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
        case GNE_ATTR_OPPOSITE: {
            if (value.empty()) {
                return true;
            } else {
                NBEdge* oppEdge = myNet->getEdgeCont().retrieve(value.substr(0, value.rfind("_")));
                if (oppEdge == nullptr || oppEdge->getLaneID(oppEdge->getNumLanes() - 1) != value) {
                    return false;
                }
                NBEdge* edge = myParentEdge->getNBEdge();
                if (oppEdge->getFromNode() != edge->getToNode() || oppEdge->getToNode() != edge->getFromNode()) {
                    WRITE_WARNINGF(TL("Opposite lane '%' does not connect the same nodes as edge '%'!"), value, edge->getID());
                    return false;
                }
                return true;
            }
        }
        case SUMO_ATTR_TYPE:
            return true;
        case GNE_ATTR_STOPOFFSET:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_STOPOEXCEPTION:
            return canParseVehicleClasses(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELane::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_INDEX:
            return false;
        default:
            return true;
    }
}


bool
GNELane::isAttributeComputed(SumoXMLAttr key) const {
    const NBEdge* edge = myParentEdge->getNBEdge();
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return (edge->getLaneStruct(myIndex).width == NBEdge::UNSPECIFIED_WIDTH);
        default:
            return false;
    }
}


const Parameterised::Map&
GNELane::getACParametersMap() const {
    return myParentEdge->getNBEdge()->getLaneStruct(myIndex).getParametersMap();
}


void
GNELane::setSpecialColor(const RGBColor* color, double colorValue) {
    mySpecialColor = color;
    mySpecialColorValue = colorValue;
}

// ===========================================================================
// private
// ===========================================================================

void
GNELane::setAttribute(SumoXMLAttr key, const std::string& value) {
    // get parent edge
    NBEdge* edge = myParentEdge->getNBEdge();
    // get template editor
    GNEInspectorFrame::TemplateEditor* templateEditor = myNet->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // check if we have to update template
    const bool updateTemplate = templateEditor->getEdgeTemplate() ? (templateEditor->getEdgeTemplate()->getID() == myParentEdge->getID()) : false;
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_INDEX:
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
        case SUMO_ATTR_CHANGE_LEFT:
            edge->setPermittedChanging(myIndex, parseVehicleClasses(value), edge->getLaneStruct(myIndex).changeRight);
            break;
        case SUMO_ATTR_CHANGE_RIGHT:
            edge->setPermittedChanging(myIndex, edge->getLaneStruct(myIndex).changeLeft, parseVehicleClasses(value));
            break;
        case SUMO_ATTR_WIDTH:
            if (value.empty() || (value == "default")) {
                edge->setLaneWidth(myIndex, NBEdge::UNSPECIFIED_WIDTH);
            } else {
                edge->setLaneWidth(myIndex, parse<double>(value));
            }
            // update edge parent boundary
            myParentEdge->updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_FRICTION:
            edge->setFriction(myIndex, parse<double>(value));
            break;
        case SUMO_ATTR_ENDOFFSET:
            edge->setEndOffset(myIndex, parse<double>(value));
            break;
        case SUMO_ATTR_ACCELERATION:
            edge->setAcceleration(myIndex, parse<bool>(value));
            break;
        case SUMO_ATTR_CUSTOMSHAPE: {
            // set new shape
            edge->setLaneShape(myIndex, parse<PositionVector>(value));
            // update edge parent boundary
            myParentEdge->updateCenteringBoundary(true);
            break;
        }
        case GNE_ATTR_OPPOSITE: {
            if (value != "") {
                NBEdge* oppEdge = myNet->getEdgeCont().retrieve(value.substr(0, value.rfind("_")));
                oppEdge->getLaneStruct(oppEdge->getNumLanes() - 1).oppositeID = getID();
            } else {
                // reset prior oppEdge if existing
                const std::string oldValue = myParentEdge->getNBEdge()->getLaneStruct(myIndex).oppositeID;
                NBEdge* oppEdge = myNet->getEdgeCont().retrieve(oldValue.substr(0, oldValue.rfind("_")));
                if (oppEdge != nullptr) {
                    oppEdge->getLaneStruct(oppEdge->getNumLanes() - 1).oppositeID = "";
                }
            }
            myParentEdge->getNBEdge()->getLaneStruct(myIndex).oppositeID = value;
            break;
        }
        case SUMO_ATTR_TYPE:
            edge->getLaneStruct(myIndex).type = value;
            break;
        case GNE_ATTR_STOPOFFSET:
            edge->getLaneStruct(myIndex).laneStopOffset.setOffset(parse<double>(value));
            break;
        case GNE_ATTR_STOPOEXCEPTION:
            if (value.empty()) {
                edge->getLaneStruct(myIndex).laneStopOffset.reset();
            } else {
                edge->getLaneStruct(myIndex).laneStopOffset.setExceptions(value);
            }
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            myParentEdge->getNBEdge()->getLaneStruct(myIndex).setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // update template
    if (updateTemplate) {
        templateEditor->setEdgeTemplate(myParentEdge);
    }
    // invalidate path calculator
    myNet->getPathManager()->getPathCalculator()->invalidatePathCalculator();
}


void
GNELane::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
    myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape = moveResult.shapeToUpdate;
    // update geometry
    updateGeometry();
}


void
GNELane::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(GUIIcon::LANE, "moving " + toString(SUMO_ATTR_CUSTOMSHAPE) + " of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate)));
    undoList->end();
}


void
GNELane::drawLaneArrows(const GUIVisualizationSettings& s, const double exaggeration, const bool spreadSuperposed) const {
    // only draw details depending of the scale and if isn't being drawn for selecting
    if (((s.scale * exaggeration) >= 1.) && (s.scale >= 10) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
        // Push layer matrix
        GLHelper::pushMatrix();
        // translate to front (note: Special case)
        if (myNet->getViewNet()->getFrontAttributeCarrier() == myParentEdge) {
            glTranslated(0, 0, GLO_FRONTELEMENT);
        } else {
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_LANEARROWS);
        }
        // draw arrows
        drawArrows(s, spreadSuperposed);
        // Pop layer matrix
        GLHelper::popMatrix();
    }
}


void
GNELane::drawShapeEdited(const GUIVisualizationSettings& s) const {
    // if shape is being edited, draw point and green line
    if (myShapeEdited) {
        // push shape edited matrix
        GLHelper::pushMatrix();
        // translate
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_JUNCTION + 1);
        // set selected edge color
        GLHelper::setColor(s.colorSettings.editShapeColor);
        // draw again to show the selected edge
        GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), {}, 0.25);
        // move front
        glTranslated(0, 0, 1);
        // color
        const RGBColor darkerColor = s.colorSettings.editShapeColor.changedBrightness(-32);
        // draw geometry points
        GUIGeometry::drawGeometryPoints(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry.getShape(), darkerColor, RGBColor::BLACK,
                                        s.neteditSizeSettings.laneGeometryPointRadius, 1, myNet->getViewNet()->getNetworkViewOptions().editingElevation(), true);
        // Pop shape edited matrix
        GLHelper::popMatrix();
    }
}


void
GNELane::checkMouseOverLane(const double laneWidth) const {
    bool withinGeometry = false;
    if (myParentEdge->getLanes().size() == 1) {
        withinGeometry = mouseWithinGeometry(getLaneShape(), laneWidth);
    } else if (myNet->getViewNet()->getNetworkViewOptions().selectEdges() && !myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        withinGeometry = mouseWithinGeometry(getLaneShape(), laneWidth, myParentEdge->getGUIGlObject());
    } else if (!myNet->getViewNet()->getNetworkViewOptions().selectEdges() && myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        withinGeometry = mouseWithinGeometry(getLaneShape(), laneWidth, myParentEdge->getGUIGlObject());
    } else {
        withinGeometry = mouseWithinGeometry(getLaneShape(), laneWidth);
    }
    // check if mark lane (and their parent edge)
    if (withinGeometry && (gPostDrawing.markedLane == nullptr)) {
        // mark lane and their parent edge
        gPostDrawing.markedLane = this;
        gPostDrawing.markedEdge = myParentEdge;
    }
}


void
GNELane::drawDottedContours(const GUIVisualizationSettings& s, const bool drawRailway, const double laneWidth) const {
    // check if dotted contours has to be drawn
    if (!drawRailway) {
        // inspect contour
        if (myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                ((myNet->getViewNet()->isAttributeCarrierInspected(myParentEdge) && (myParentEdge->getLanes().size() == 1)))) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, getLaneShape(), laneWidth, 1, true, true);
        }
        // front contour
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this) ||
                ((myNet->getViewNet()->getFrontAttributeCarrier() == myParentEdge) && (myParentEdge->getLanes().size() == 1))) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, getLaneShape(), laneWidth, 1, true, true);
        }
        // orange contour
        if (myNet->getViewNet()->getViewParent()->getAdditionalFrame()->getLanesSelector()->isNetworkElementSelected(this) ||
                (myNet->getViewNet()->getViewParent()->getAdditionalFrame()->getEdgesSelector()->isNetworkElementSelected(myParentEdge) && (myParentEdge->getLanes().size() == 1))) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::ORANGE, getLaneShape(), laneWidth, 1, true, true);
        }
        // delete contour
        if (myNet->getViewNet()->drawDeleteContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, getLaneShape(), laneWidth, 1, true, true);
        }
        // select contour
        if (myNet->getViewNet()->drawSelectContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, getLaneShape(), laneWidth, 1, true, true);
        }
    }
}


RGBColor
GNELane::setLaneColor(const GUIVisualizationSettings& s) const {
    // get inspected attribute carriers
    const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
    // declare a RGBColor variable
    RGBColor color;
    // get inspected AC
    const GNEAttributeCarrier* inspectedAC = inspectedACs.size() > 0 ? inspectedACs.front() : nullptr;
    // we need to draw lanes with a special color if we're inspecting a Trip or Flow and this lane belongs to a via's edge.
    if (inspectedAC && (inspectedAC->isAttributeCarrierSelected() == false) &&
            ((inspectedAC->getTagProperty().getTag() == SUMO_TAG_TRIP) || (inspectedAC->getTagProperty().getTag() == SUMO_TAG_FLOW))) {
        // obtain attribute "via"
        std::vector<std::string> viaEdges = parse<std::vector<std::string> >(inspectedAC->getAttribute(SUMO_ATTR_VIA));
        // iterate over viaEdges
        for (const auto& edge : viaEdges) {
            // check if parent edge is in the via edges
            if (myParentEdge->getID() == edge) {
                // set green color in GLHelper and return it
                color = RGBColor::GREEN;
            }
        }
    }
    if (mySpecialColor != nullptr) {
        // If special color is enabled, set it
        color = *mySpecialColor;
    } else if (drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        color = s.colorSettings.selectedLaneColor;
    } else if (myParentEdge->drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        color = s.colorSettings.selectedEdgeColor;
    } else {
        // Get normal lane color
        const GUIColorer& c = s.laneColorer;
        if (!setFunctionalColor(c.getActive(), color) && !setMultiColor(s, c, color)) {
            color = c.getScheme().getColor(getColorValue(s, c.getActive()));
        }
    }
    // special color for conflicted candidate edges
    if (myParentEdge->isConflictedCandidate()) {
        // extra check for route frame
        if (myNet->getViewNet()->getViewParent()->getRouteFrame()->getPathCreator()->drawCandidateEdgesWithSpecialColor()) {
            color = s.candidateColorSettings.conflict;
        }
    }
    // special color for special candidate edges
    if (myParentEdge->isSpecialCandidate()) {
        // extra check for route frame
        if (myNet->getViewNet()->getViewParent()->getRouteFrame()->getPathCreator()->drawCandidateEdgesWithSpecialColor()) {
            color = s.candidateColorSettings.special;
        }
    }
    // special color for candidate edges
    if (myParentEdge->isPossibleCandidate()) {
        // extra check for route frame
        if (myNet->getViewNet()->getViewParent()->getRouteFrame()->getPathCreator()->drawCandidateEdgesWithSpecialColor()) {
            color = s.candidateColorSettings.possible;
        }
    }
    // special color for source candidate edges
    if (myParentEdge->isSourceCandidate()) {
        color = s.candidateColorSettings.source;
    }
    // special color for target candidate edges
    if (myParentEdge->isTargetCandidate()) {
        color = s.candidateColorSettings.target;
    }
    // special color for invalid candidate edges
    if (myParentEdge->isInvalidCandidate()) {
        color = s.candidateColorSettings.invalid;
    }
    // special color for source candidate lanes
    if (mySourceCandidate) {
        color = s.candidateColorSettings.source;
    }
    // special color for target candidate lanes
    if (myTargetCandidate) {
        color = s.candidateColorSettings.target;
    }
    // special color for special candidate lanes
    if (mySpecialCandidate) {
        color = s.candidateColorSettings.special;
    }
    // special color for possible candidate lanes
    if (myPossibleCandidate) {
        color = s.candidateColorSettings.possible;
    }
    // special color for conflicted candidate lanes
    if (myConflictedCandidate) {
        color = s.candidateColorSettings.conflict;
    }
    // special color for invalid candidate lanes
    if (myInvalidCandidate) {
        color = s.candidateColorSettings.invalid;
    }
    // set color in GLHelper
    GLHelper::setColor(color);
    return color;
}


bool
GNELane::setFunctionalColor(int activeScheme, RGBColor& col) const {
    switch (activeScheme) {
        case 6: {
            double hue = GeomHelper::naviDegree(myLaneGeometry.getShape().beginEndAngle()); // [0-360]
            col = RGBColor::fromHSV(hue, 1., 1.);
            return true;
        }
        default:
            return false;
    }
}


bool
GNELane::setMultiColor(const GUIVisualizationSettings& s, const GUIColorer& c, RGBColor& col) const {
    const int activeScheme = c.getActive();
    myShapeColors.clear();
    switch (activeScheme) {
        case 9: // color by height at segment start
            for (PositionVector::const_iterator ii = myLaneGeometry.getShape().begin(); ii != myLaneGeometry.getShape().end() - 1; ++ii) {
                myShapeColors.push_back(c.getScheme().getColor(ii->z()));
            }
            col = c.getScheme().getColor(getColorValue(s, 8));
            return true;
        case 11: // color by inclination  at segment start
            for (int ii = 1; ii < (int)myLaneGeometry.getShape().size(); ++ii) {
                const double inc = (myLaneGeometry.getShape()[ii].z() - myLaneGeometry.getShape()[ii - 1].z()) / MAX2(POSITION_EPS, myLaneGeometry.getShape()[ii].distanceTo2D(myLaneGeometry.getShape()[ii - 1]));
                myShapeColors.push_back(c.getScheme().getColor(inc));
            }
            col = c.getScheme().getColor(getColorValue(s, 10));
            return true;
        default:
            return false;
    }
}


double
GNELane::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    const SVCPermissions myPermissions = myParentEdge->getNBEdge()->getPermissions(myIndex);
    if (mySpecialColor != nullptr && mySpecialColorValue != std::numeric_limits<double>::max()) {
        return mySpecialColorValue;
    }
    switch (activeScheme) {
        case 0:
            switch (myPermissions) {
                case SVC_PEDESTRIAN:
                    return 1;
                case SVC_BICYCLE:
                    return 2;
                case 0:
                    // forbidden road or green verge
                    return myParentEdge->getNBEdge()->getPermissions() == 0 ? 10 : 3;
                case SVC_SHIP:
                    return 4;
                case SVC_AUTHORITY:
                    return 8;
                default:
                    break;
            }
            if (myParentEdge->getNBEdge()->isMacroscopicConnector()) {
                return 9;
            } else if (isRailway(myPermissions)) {
                if ((myPermissions & SVC_BUS) != 0) {
                    return 6;
                } else {
                    return 5;
                }
            } else if ((myPermissions & SVC_PASSENGER) != 0) {
                if ((myPermissions & (SVC_RAIL_CLASSES & ~SVC_RAIL_FAST)) != 0 && (myPermissions & SVC_SHIP) == 0) {
                    return 6;
                } else {
                    return 0;
                }
            } else {
                return 7;
            }
        case 1:
            return isAttributeCarrierSelected() || myParentEdge->isAttributeCarrierSelected();
        case 2:
            return (double)myPermissions;
        case 3:
            return myParentEdge->getNBEdge()->getLaneSpeed(myIndex);
        case 4:
            return myParentEdge->getNBEdge()->getNumLanes();
        case 5: {
            return myParentEdge->getNBEdge()->getLoadedLength() / myParentEdge->getNBEdge()->getLaneStruct(myIndex).shape.length();
        }
        // case 6: by angle (functional)
        case 7: {
            return myParentEdge->getNBEdge()->getPriority();
        }
        case 8: {
            // color by z of first shape point
            return myLaneGeometry.getShape()[0].z();
        }
        // case 9: by segment height
        case 10: {
            // color by incline
            return (myLaneGeometry.getShape()[-1].z() - myLaneGeometry.getShape()[0].z()) /  myParentEdge->getNBEdge()->getLength();
        }
        // case 11: by segment incline

        case 12: {
            // by numerical edge param value
            if (myParentEdge->getNBEdge()->knowsParameter(s.edgeParam)) {
                try {
                    return StringUtils::toDouble(myParentEdge->getNBEdge()->getParameter(s.edgeParam, "0"));
                } catch (NumberFormatException&) {
                    try {
                        return StringUtils::toBool(myParentEdge->getNBEdge()->getParameter(s.edgeParam, "0"));
                    } catch (BoolFormatException&) {
                        return -1;
                    }
                }
            } else {
                return GUIVisualizationSettings::MISSING_DATA;
            }
        }
        case 13: {
            // by numerical lane param value
            if (myParentEdge->getNBEdge()->getLaneStruct(myIndex).knowsParameter(s.laneParam)) {
                try {
                    return StringUtils::toDouble(myParentEdge->getNBEdge()->getLaneStruct(myIndex).getParameter(s.laneParam, "0"));
                } catch (NumberFormatException&) {
                    try {
                        return StringUtils::toBool(myParentEdge->getNBEdge()->getLaneStruct(myIndex).getParameter(s.laneParam, "0"));
                    } catch (BoolFormatException&) {
                        return -1;
                    }
                }
            } else {
                return GUIVisualizationSettings::MISSING_DATA;
            }
        }
        case 14: {
            return myParentEdge->getNBEdge()->getDistance();
        }
        case 15: {
            return fabs(myParentEdge->getNBEdge()->getDistance());
        }
    }
    return 0;
}


bool
GNELane::drawAsRailway(const GUIVisualizationSettings& s) const {
    return isRailway(myParentEdge->getNBEdge()->getPermissions(myIndex))
           && (myParentEdge->getNBEdge()->getPermissions(myIndex) & SVC_BUS) == 0
           && s.showRails && (!s.drawForRectangleSelection || s.spreadSuperposed);
}


void
GNELane::drawOverlappedRoutes(const int numRoutes) const {
    // get middle point and angle
    const Position center = myLaneGeometry.getShape().positionAtOffset2D(myLaneGeometry.getShape().length2D() * 0.5);
    const double angle = myLaneGeometry.getShape().rotationDegreeAtOffset(myLaneGeometry.getShape().length2D() * 0.5);
    // Push route matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_ROUTE + 1);
    // get middle
    GLHelper::drawText(toString(numRoutes) + " routes", center, 0, 1.8, RGBColor::BLACK, angle + 90);
    // pop route matrix
    GLHelper::popMatrix();

}


void
GNELane::drawLaneStopOffset(const GUIVisualizationSettings& s, const double offset) const {
    GNELane::LaneDrawingConstants laneDrawingConstants(s, this);
    const Position& end = getLaneShape().back();
    const Position& f = getLaneShape()[-2];
    const double rot = RAD2DEG(atan2((end.x() - f.x()), (f.y() - end.y())));
    GLHelper::setColor(s.getLinkColor(LINKSTATE_MAJOR));
    GLHelper::pushMatrix();
    glTranslated(end.x(), end.y(), 1);
    glRotated(rot, 0, 0, 1);
    glTranslated(0, offset, 0);
    glBegin(GL_QUADS);
    glVertex2d(-laneDrawingConstants.halfWidth, 0.0);
    glVertex2d(-laneDrawingConstants.halfWidth, 0.2);
    glVertex2d(laneDrawingConstants.halfWidth, 0.2);
    glVertex2d(laneDrawingConstants.halfWidth, 0.0);
    glEnd();
    GLHelper::popMatrix();
}


bool
GNELane::drawAsWaterway(const GUIVisualizationSettings& s) const {
    return isWaterway(myParentEdge->getNBEdge()->getPermissions(myIndex)) && s.showRails && !s.drawForRectangleSelection; // reusing the showRails setting
}


void
GNELane::drawDirectionIndicators(const GUIVisualizationSettings& s, double exaggeration, const bool drawAsRailway, const bool spreadSuperposed) const {
    // Draw direction indicators if the correspondient option is enabled
    if (s.showLaneDirection) {
        // improve visibility of superposed rail edges
        if (drawAsRailway) {
            setLaneColor(s);
        } else {
            glColor3d(0.3, 0.3, 0.3);
        }
        // get width and sideOffset
        const double width = MAX2(NUMERICAL_EPS, (myParentEdge->getNBEdge()->getLaneWidth(myIndex) * exaggeration * (spreadSuperposed ? 0.4 : 1)));
        const double sideOffset = spreadSuperposed ? width * -0.5 : 0;
        // push direction indicator matrix
        GLHelper::pushMatrix();
        // move to front
        glTranslated(0, 0, 0.1);
        // iterate over shape
        for (int i = 0; i < (int) myLaneGeometry.getShape().size() - 1; ++i) {
            // push triangle matrix
            GLHelper::pushMatrix();
            // move front
            glTranslated(myLaneGeometry.getShape()[i].x(), myLaneGeometry.getShape()[i].y(), 0.1);
            // rotate
            glRotated(myLaneGeometry.getShapeRotations()[i], 0, 0, 1);
            // calculate subwidth
            for (double subWidth = 0; subWidth < myLaneGeometry.getShapeLengths()[i]; subWidth += width) {
                // calculate length
                const double length = MIN2(width * 0.5, myLaneGeometry.getShapeLengths()[i] - subWidth);
                // draw triangle
                glBegin(GL_TRIANGLES);
                glVertex2d(sideOffset, -subWidth - length);
                glVertex2d(sideOffset - width * 0.25, -subWidth);
                glVertex2d(sideOffset + width * 0.25, -subWidth);
                glEnd();
            }
            // pop triangle matrix
            GLHelper::popMatrix();
        }
        // pop direction indicator matrix
        GLHelper::popMatrix();
    }
}


void
GNELane::drawLaneAsRailway(const GUIVisualizationSettings& s, const LaneDrawingConstants& laneDrawingConstants) const {
    // we draw the lanes with reduced width so that the lane markings below are visible
    // (this avoids artifacts at geometry corners without having to
    const bool spreadSuperposed = s.spreadSuperposed && myParentEdge->getNBEdge()->getBidiEdge() != nullptr;
    // get lane shape
    PositionVector shape = myLaneGeometry.getShape();
    // get width
    const double width = myParentEdge->getNBEdge()->getLaneWidth(myIndex);
    // draw as railway: assume standard gauge of 1435mm when lane width is not set
    // draw foot width 150mm, assume that distance between rail feet inner sides is reduced on both sides by 39mm with regard to the gauge
    // assume crosstie length of 181% gauge (2600mm for standard gauge)
    double halfGauge = 0.5 * (width == SUMO_const_laneWidth ?  1.4350 : width) * laneDrawingConstants.exaggeration;
    // check if we have to modify shape
    if (spreadSuperposed) {
        shape.move2side(halfGauge * 0.8);
        halfGauge *= 0.4;
        //std::cout << "spreadSuperposed " << getID() << " old=" << myLaneGeometry.getShape() << " new=" << shape << "\n";
    }
    // calculate constant
    const double halfInnerFeetWidth = halfGauge - 0.039 * laneDrawingConstants.exaggeration;
    const double halfRailWidth = halfInnerFeetWidth + 0.15 * laneDrawingConstants.exaggeration;
    const double halfCrossTieWidth = halfGauge * 1.81;
    // Draw lane geometry
    GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(), shape, myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), myShapeColors, halfRailWidth);
    // Save current color
    RGBColor current = GLHelper::getColor();
    // Draw gray on top with reduced width (the area between the two tracks)
    glColor3d(0.8, 0.8, 0.8);
    // move
    glTranslated(0, 0, 0.1);
    // draw lane geometry again
    GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(), shape, myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), {}, halfInnerFeetWidth);
    // Set current color back
    GLHelper::setColor(current);
    // Draw crossties
    GLHelper::drawCrossTies(shape, myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), 0.26 * laneDrawingConstants.exaggeration, 0.6 * laneDrawingConstants.exaggeration, halfCrossTieWidth, s.drawForRectangleSelection);
    // check if dotted contours has to be drawn
    if (myNet->getViewNet()->isAttributeCarrierInspected(this) ||
            (myNet->getViewNet()->isAttributeCarrierInspected(myParentEdge) && (myParentEdge->getLanes().size() == 1))) {
        GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, halfGauge, 1, true, true);
    }
    if ((myNet->getViewNet()->getFrontAttributeCarrier() == this) ||
            ((myNet->getViewNet()->getFrontAttributeCarrier() == myParentEdge) && (myParentEdge->getLanes().size() == 1))) {
        GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, halfGauge, 1, true, true);
    }
}


void
GNELane::drawTextures(const GUIVisualizationSettings& s, const LaneDrawingConstants& laneDrawingConstants) const {
    // check all conditions for drawing textures
    if (!s.drawForRectangleSelection && !s.drawForPositionSelection && !s.disableLaneIcons &&
            (myLaneRestrictedTexturePositions.size() > 0) &&
            s.drawDetail(s.detailSettings.laneTextures, laneDrawingConstants.exaggeration)) {
        // Declare default width of icon (3)
        double iconWidth = 1;
        // Obtain width of icon, if width of lane is different
        if (myParentEdge->getNBEdge()->getLaneStruct(myIndex).width != -1) {
            iconWidth = myParentEdge->getNBEdge()->getLaneStruct(myIndex).width / 3;
        }
        // Draw list of icons
        for (int i = 0; i < (int)myLaneRestrictedTexturePositions.size(); i++) {
            // Push draw matrix 2
            GLHelper::pushMatrix();
            // Set white color
            glColor3d(1, 1, 1);
            // Translate matrix 2
            glTranslated(myLaneRestrictedTexturePositions.at(i).x(), myLaneRestrictedTexturePositions.at(i).y(), 0.1);
            // Rotate matrix 2
            glRotated(myLaneRestrictedTextureRotations.at(i), 0, 0, -1);
            glRotated(90, 0, 0, 1);
            // draw texture box depending of type of restriction
            if (isRestricted(SVC_PEDESTRIAN)) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::LANE_PEDESTRIAN), iconWidth);
            } else if (isRestricted(SVC_BICYCLE)) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::LANE_BIKE), iconWidth);
            } else if (isRestricted(SVC_BUS)) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::LANE_BUS), iconWidth);
            }
            // Pop draw matrix 2
            GLHelper::popMatrix();
        }
    }
}


void
GNELane::drawStartEndShapePoints(const GUIVisualizationSettings& s) const {
    // draw a Start/endPoints if lane has a custom shape
    if (!s.drawForRectangleSelection && (myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.size() > 1)) {
        GLHelper::setColor(s.junctionColorer.getSchemes()[0].getColor(2));
        if (drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(s.colorSettings.selectedEdgeColor.changedBrightness(-20));
        }
        // obtain circle width and resolution
        double circleWidth = GNEEdge::SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration) / 2;
        // Obtain exaggeration of the draw
        const double exaggeration = getExaggeration(s);
        // obtain custom shape
        const PositionVector& customShape = myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape;
        // draw s depending of detail
        if (s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
            // push start matrix
            GLHelper::pushMatrix();
            // move to shape start position
            glTranslated(customShape.front().x(), customShape.front().y(), 0.1);
            // draw circle
            GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
            // check if we can draw "S"
            if (!s.drawForPositionSelection) {
                // move top
                glTranslated(0, 0, 0.1);
                // draw "S"
                GLHelper::drawText("S", Position(), 0.1, circleWidth, RGBColor::WHITE);
            }
            // pop start matrix
            GLHelper::popMatrix();
        }
        // draw line between junction and start position
        GLHelper::pushMatrix();
        // move top
        glTranslated(0, 0, 0.1);
        // set line width
        glLineWidth(4);
        // draw line
        GLHelper::drawLine(customShape.front(), myParentEdge->getFromJunction()->getPositionInView());
        // pop line matrix
        GLHelper::popMatrix();
        // draw "e" depending of detail
        if (s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
            // push start matrix
            GLHelper::pushMatrix();
            // move to end position
            glTranslated(customShape.back().x(), customShape.back().y(), 0.1);
            // draw filled circle
            GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
            // check if we can draw "E"
            if (!s.drawForPositionSelection) {
                // move top
                glTranslated(0, 0, 0.1);
                // draw "E"
                GLHelper::drawText("E", Position(), 0, circleWidth, RGBColor::WHITE);
            }
            // pop start matrix
            GLHelper::popMatrix();
        }
        // draw line between Junction and end position
        GLHelper::pushMatrix();
        // move top
        glTranslated(0, 0, 0.1);
        // set line width
        glLineWidth(4);
        // draw line
        GLHelper::drawLine(customShape.back(), myParentEdge->getToJunction()->getPositionInView());
        // pop line matrix
        GLHelper::popMatrix();
    }
}


std::string
GNELane::getParentName() const {
    return myParentEdge->getID();
}


long
GNELane::onDefault(FXObject* obj, FXSelector sel, void* data) {
    myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->handleMultiChange(this, obj, sel, data);
    return 1;
}


std::vector<GNEConnection*>
GNELane::getGNEIncomingConnections() {
    // Declare a vector to save incoming connections
    std::vector<GNEConnection*> incomingConnections;
    // Obtain incoming edges if junction source was already created
    GNEJunction* junctionSource =  myParentEdge->getFromJunction();
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
    // Obtain GNEConnection of parent edge
    const std::vector<GNEConnection*>& edgeConnections = myParentEdge->getGNEConnections();
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
    // update outcoming connections of lane
    std::vector<GNEConnection*> outcomingConnections = getGNEOutcomingConnections();
    for (auto i : outcomingConnections) {
        i->updateID();
    }
}


double
GNELane::getLengthGeometryFactor() const {
    // factor should not be 0
    if (myParentEdge->getNBEdge()->getFinalLength() > 0) {
        return MAX2(POSITION_EPS, (getLaneShape().length() / myParentEdge->getNBEdge()->getFinalLength()));
    } else {
        return POSITION_EPS;
    };
}


void
GNELane::buildEdgeOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret) {
    // Create basic commands
    std::string edgeDescPossibleMulti = toString(SUMO_TAG_EDGE);
    const int edgeSelSize = myParentEdge->isAttributeCarrierSelected() ? myNet->getAttributeCarriers()->getNumberOfSelectedEdges() : 0;
    if (edgeSelSize && myParentEdge->isAttributeCarrierSelected() && (edgeSelSize > 1)) {
        edgeDescPossibleMulti = toString(edgeSelSize) + " " + toString(SUMO_TAG_EDGE) + "s";
    }
    // create menu pane for edge operations
    FXMenuPane* edgeOperations = new FXMenuPane(ret);
    ret->insertMenuPaneChild(edgeOperations);
    if (edgeSelSize > 0) {
        new FXMenuCascade(ret, ("edge operations (" + toString(edgeSelSize) + " selected)").c_str(), nullptr, edgeOperations);
    } else {
        new FXMenuCascade(ret, "edge operations", nullptr, edgeOperations);
    }
    // create menu commands for all edge operations
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Split edge here", nullptr, &parent, MID_GNE_EDGE_SPLIT);
    auto splitBothDirections = GUIDesigns::buildFXMenuCommand(edgeOperations, "Split edge in both directions here (no simmetric opposite edge)", nullptr, &parent, MID_GNE_EDGE_SPLIT_BIDI);
    // check if allow split edge in both directions
    splitBothDirections->disable();
    const auto oppositeEdges = myParentEdge->getOppositeEdges();
    if (oppositeEdges.size() == 0) {
        splitBothDirections->setText("Split edge in both directions here (no opposite edge)");
    } else {
        for (const auto& oppositeEdge : oppositeEdges) {
            // get reverse inner geometry
            const auto reverseGeometry = oppositeEdge->getNBEdge()->getInnerGeometry().reverse();
            if (reverseGeometry == myParentEdge->getNBEdge()->getInnerGeometry()) {
                splitBothDirections->enable();
                splitBothDirections->setText("Split edge in both directions here");
            }
        }
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Set geometry endpoint here (shift-click)", nullptr, &parent, MID_GNE_EDGE_EDIT_ENDPOINT);
    // restore geometry points depending of selection status
    if (myParentEdge->isAttributeCarrierSelected()) {
        if (edgeSelSize == 1) {
            GUIDesigns::buildFXMenuCommand(edgeOperations, "Restore both geometry endpoints", nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
        } else {
            GUIDesigns::buildFXMenuCommand(edgeOperations, "Restore geometry endpoints of all selected edges", nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
        }
    } else {
        GUIDesigns::buildFXMenuCommand(edgeOperations, "Restore geometry endpoint (shift-click)", nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Reverse " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_REVERSE);
    auto reverse = GUIDesigns::buildFXMenuCommand(edgeOperations, "Add reverse direction for " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_ADD_REVERSE);
    if (myParentEdge->getReverseEdge() != nullptr) {
        reverse->disable();
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Add reverse disconnected direction for " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_ADD_REVERSE_DISCONNECTED);
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Reset lengths for " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_RESET_LENGTH);
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Straighten " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN);
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Smooth " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_SMOOTH);
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Straighten elevation of " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN_ELEVATION);
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Smooth elevation of " + edgeDescPossibleMulti, nullptr, &parent, MID_GNE_EDGE_SMOOTH_ELEVATION);
}


void
GNELane::buildLaneOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret) {
    // Get icons
    FXIcon* pedestrianIcon = GUIIconSubSys::getIcon(GUIIcon::LANE_PEDESTRIAN);
    FXIcon* bikeIcon = GUIIconSubSys::getIcon(GUIIcon::LANE_BIKE);
    FXIcon* busIcon = GUIIconSubSys::getIcon(GUIIcon::LANE_BUS);
    FXIcon* greenVergeIcon = GUIIconSubSys::getIcon(GUIIcon::LANEGREENVERGE);
    // declare number of selected lanes
    int numSelectedLanes = 0;
    // if lane is selected, calculate number of restricted lanes
    bool edgeHasSidewalk = false;
    bool edgeHasBikelane = false;
    bool edgeHasBuslane = false;
    bool differentLaneShapes = false;
    if (isAttributeCarrierSelected()) {
        const auto selectedLanes = myNet->getAttributeCarriers()->getSelectedLanes();
        // update numSelectedLanes
        numSelectedLanes = (int)selectedLanes.size();
        // iterate over selected lanes
        for (const auto& selectedLane : selectedLanes) {
            if (selectedLane->myParentEdge->hasRestrictedLane(SVC_PEDESTRIAN)) {
                edgeHasSidewalk = true;
            }
            if (selectedLane->myParentEdge->hasRestrictedLane(SVC_BICYCLE)) {
                edgeHasBikelane = true;
            }
            if (selectedLane->myParentEdge->hasRestrictedLane(SVC_BUS)) {
                edgeHasBuslane = true;
            }
            if (selectedLane->myParentEdge->getNBEdge()->getLaneStruct(selectedLane->getIndex()).customShape.size() != 0) {
                differentLaneShapes = true;
            }
        }
    } else {
        edgeHasSidewalk = myParentEdge->hasRestrictedLane(SVC_PEDESTRIAN);
        edgeHasBikelane = myParentEdge->hasRestrictedLane(SVC_BICYCLE);
        edgeHasBuslane = myParentEdge->hasRestrictedLane(SVC_BUS);
        differentLaneShapes = myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.size() != 0;
    }
    // create menu pane for lane operations
    FXMenuPane* laneOperations = new FXMenuPane(ret);
    ret->insertMenuPaneChild(laneOperations);
    if (numSelectedLanes > 0) {
        new FXMenuCascade(ret, ("lane operations (" + toString(numSelectedLanes) + " selected)").c_str(), nullptr, laneOperations);
    } else {
        new FXMenuCascade(ret, "lane operations", nullptr, laneOperations);
    }
    GUIDesigns::buildFXMenuCommand(laneOperations, "Duplicate lane", nullptr, &parent, MID_GNE_LANE_DUPLICATE);
    GUIDesigns::buildFXMenuCommand(laneOperations, "Set custom lane shape", nullptr, &parent, MID_GNE_LANE_EDIT_SHAPE);
    FXMenuCommand* resetCustomShape = GUIDesigns::buildFXMenuCommand(laneOperations, "reset custom shape", nullptr, &parent, MID_GNE_LANE_RESET_CUSTOMSHAPE);
    if (!differentLaneShapes) {
        resetCustomShape->disable();
    }
    FXMenuCommand* resetOppositeLane = GUIDesigns::buildFXMenuCommand(laneOperations, "reset opposite lane", nullptr, &parent, MID_GNE_LANE_RESET_OPPOSITELANE);
    if (getAttribute(GNE_ATTR_OPPOSITE).empty()) {
        resetOppositeLane->disable();
    }
    // Create panel for lane operations and insert it in ret
    FXMenuPane* addSpecialLanes = new FXMenuPane(laneOperations);
    ret->insertMenuPaneChild(addSpecialLanes);
    FXMenuPane* removeSpecialLanes = new FXMenuPane(laneOperations);
    ret->insertMenuPaneChild(removeSpecialLanes);
    FXMenuPane* transformSlanes = new FXMenuPane(laneOperations);
    ret->insertMenuPaneChild(transformSlanes);
    // Create menu comands for all add special lanes
    FXMenuCommand* addSidewalk = GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_ADD_SIDEWALK);
    FXMenuCommand* addBikelane = GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_ADD_BIKE);
    FXMenuCommand* addBuslane = GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_LANE_ADD_BUS);
    // if parent edge is selected, always add greenverge in front
    if (myParentEdge->isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_FRONT);
    } else {
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Greenverge (front)", greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_FRONT);
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, "Greenverge (back)", greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_BACK);
    }
    // Create menu comands for all remove special lanes and disable it
    FXMenuCommand* removeSidewalk = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_REMOVE_SIDEWALK);
    removeSidewalk->disable();
    FXMenuCommand* removeBikelane = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_REMOVE_BIKE);
    removeBikelane->disable();
    FXMenuCommand* removeBuslane = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_LANE_REMOVE_BUS);
    removeBuslane->disable();
    FXMenuCommand* removeGreenVerge = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_REMOVE_GREENVERGE);
    removeGreenVerge->disable();
    // Create menu comands for all transform special lanes and disable it
    FXMenuCommand* transformLaneToSidewalk = GUIDesigns::buildFXMenuCommand(transformSlanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_LANE_TRANSFORM_SIDEWALK);
    FXMenuCommand* transformLaneToBikelane = GUIDesigns::buildFXMenuCommand(transformSlanes, "Bikelane", bikeIcon, &parent, MID_GNE_LANE_TRANSFORM_BIKE);
    FXMenuCommand* transformLaneToBuslane = GUIDesigns::buildFXMenuCommand(transformSlanes, "Buslane", busIcon, &parent, MID_GNE_LANE_TRANSFORM_BUS);
    FXMenuCommand* transformLaneToGreenVerge = GUIDesigns::buildFXMenuCommand(transformSlanes, "Greenverge", greenVergeIcon, &parent, MID_GNE_LANE_TRANSFORM_GREENVERGE);
    // add menuCascade for lane operations
    new FXMenuCascade(laneOperations, ("add restricted " + toString(SUMO_TAG_LANE)).c_str(), nullptr, addSpecialLanes);
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
    if (isRestricted(SVC_IGNORING)) {
        transformLaneToGreenVerge->disable();
        removeGreenVerge->enable();
    }
    // Check if cascade menu must be disabled
    if (!edgeHasSidewalk && !edgeHasBikelane && !edgeHasBuslane && !isRestricted(SVC_IGNORING)) {
        cascadeRemoveSpecialLane->disable();
    }
    // for whatever reason, sonar complains in the next line that cascadeRemoveSpecialLane may leak, but fox does the cleanup
}  // NOSONAR


void
GNELane::buildTemplateOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret) {
    // Create basic commands
    std::string edgeDescPossibleMulti = toString(SUMO_TAG_EDGE);
    const int edgeSelSize = myParentEdge->isAttributeCarrierSelected() ? myNet->getAttributeCarriers()->getNumberOfSelectedEdges() : 0;
    if (edgeSelSize && myParentEdge->isAttributeCarrierSelected() && (edgeSelSize > 1)) {
        edgeDescPossibleMulti = toString(edgeSelSize) + " " + toString(SUMO_TAG_EDGE) + "s";
    }
    // create menu pane for edge operations
    FXMenuPane* edgeOperations = new FXMenuPane(ret);
    ret->insertMenuPaneChild(edgeOperations);
    if (edgeSelSize > 0) {
        new FXMenuCascade(ret, ("template operations (" + toString(edgeSelSize) + " selected)").c_str(), nullptr, edgeOperations);
    } else {
        new FXMenuCascade(ret, "template operations", nullptr, edgeOperations);
    }
    // create menu commands for all edge operations
    GUIDesigns::buildFXMenuCommand(edgeOperations, "Use edge as template", nullptr, &parent, MID_GNE_EDGE_USEASTEMPLATE);
    auto applyTemplate = GUIDesigns::buildFXMenuCommand(edgeOperations, "Apply template", nullptr, &parent, MID_GNE_EDGE_APPLYTEMPLATE);
    // check if disable apply template
    if (myNet->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate() == nullptr) {
        applyTemplate->disable();
    }

}


void
GNELane::buildRechableOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret) {
    // addreachability menu
    FXMenuPane* reachableByClass = new FXMenuPane(ret);
    ret->insertMenuPaneChild(reachableByClass);
    if (myNet->isNetRecomputed()) {
        new FXMenuCascade(ret, "Select reachable", GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), reachableByClass);
        for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
            GUIDesigns::buildFXMenuCommand(reachableByClass, vClass.c_str(), VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(vClass)), &parent, MID_REACHABILITY);
        }
    } else {
        FXMenuCommand* menuCommand = GUIDesigns::buildFXMenuCommand(ret, "Select reachable (compute junctions)", nullptr, nullptr, 0);
        menuCommand->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    }
}

/****************************************************************************/

