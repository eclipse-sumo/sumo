/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/common/MsgHandler.h>
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

GNELane::DrawingConstants::DrawingConstants(const GNELane* lane) :
    myLane(lane) {
}


void
GNELane::DrawingConstants::update(const GUIVisualizationSettings& s) {
    // get lane struct
    const auto &laneStruct = myLane->myParentEdge->getNBEdge()->getLaneStruct(myLane->getIndex());
    // get selection scale
    const double selectionScale = myLane->isAttributeCarrierSelected() || myLane->myParentEdge->isAttributeCarrierSelected() ? s.selectorFrameScale : 1;
    // get lane width
    const double laneWidth = (laneStruct.width == -1? SUMO_const_laneWidth : laneStruct.width);
    // calculate exaggeration
    myExaggeration = selectionScale * s.laneWidthExaggeration;
    // get detail level
    myDetail = s.getDetailLevel(myExaggeration);
    // check if draw lane as railway
    myDrawAsRailway = isRailway(laneStruct.permissions) && ((laneStruct.permissions & SVC_BUS) == 0) && s.showRails;
    // adjust rest of parameters depending if draw as railway
    if (myDrawAsRailway) {
        // draw as railway: assume standard gauge of 1435mm when lane width is not set
        // draw foot width 150mm, assume that distance between rail feet inner sides is reduced on both sides by 39mm with regard to the gauge
        // assume crosstie length of 181% gauge (2600mm for standard gauge)
        myDrawingWidth = (laneWidth == SUMO_const_laneWidth ?  1.4350 : laneWidth) * myExaggeration;
        // set offset
        myOffset = myDrawingWidth * 0.8;
        // update half gauge
        myDrawingWidth *= 0.4;
    } else {
        // calculate exaggerated drawing width
        myDrawingWidth = laneWidth * myExaggeration * 0.5;
        // calculate internal drawing width
        myInternalDrawingWidth = myDrawingWidth - (2 * SUMO_const_laneMarkWidth);
    }

    // check if draw superposed
    myDrawSuperposed = (s.spreadSuperposed && myLane->getParentEdge()->getNBEdge()->isBidiRail());
    if (myDrawSuperposed) {
        // reduce width
        myDrawingWidth *= 0.4;
        myOffset = myDrawingWidth * 0.5;
    }
}


double
GNELane::DrawingConstants::getExaggeration() const {
    return myExaggeration;
}


double
GNELane::DrawingConstants::getDrawingWidth() const {
    return myDrawingWidth;
}


double
GNELane::DrawingConstants::getInternalDrawingWidth() const {
    return myInternalDrawingWidth;
}


double
GNELane::DrawingConstants::getOffset() const {
    return myOffset;
}


GUIVisualizationSettings::Detail
GNELane::DrawingConstants::getDetail() const {
    return myDetail;
}


bool
GNELane::DrawingConstants::drawAsRailway() const {
    return myDrawAsRailway;
}


bool
GNELane::DrawingConstants::drawSuperposed() const {
    return myDrawSuperposed;
}

// ---------------------------------------------------------------------------
// GNELane - methods
// ---------------------------------------------------------------------------

GNELane::GNELane(GNEEdge* edge, const int index) :
    GNENetworkElement(edge->getNet(), edge->getNBEdge()->getLaneID(index), GLO_LANE, SUMO_TAG_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
    myParentEdge(edge),
    myIndex(index),
    myDrawingConstants(new DrawingConstants(this)),
    mySpecialColor(nullptr),
    mySpecialColorValue(-1),
    myLane2laneConnections(this){
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNELane::GNELane() :
    GNENetworkElement(nullptr, "dummyConstructorGNELane", GLO_LANE, SUMO_TAG_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
    myParentEdge(nullptr),
    myIndex(-1),
    myDrawingConstants(nullptr),
    mySpecialColor(nullptr),
    mySpecialColorValue(-1),
    myLane2laneConnections(this) {
}


GNELane::~GNELane() {
    if (myDrawingConstants) {
        delete myDrawingConstants;
    }
}


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


const GNELane::DrawingConstants*
GNELane::getDrawingConstants() const {
    return myDrawingConstants;
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


bool
GNELane::checkDrawFromContour() const {
    // get inspected ACs
    const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
    // check if we're inspecting a connection
    if ((inspectedACs.size() == 1) && (inspectedACs.front()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) &&
            inspectedACs.front()->getAttribute(GNE_ATTR_FROM_LANEID) == getID()) {
        return true;
    } else {
        return false;
    }
}


bool
GNELane::checkDrawToContour() const {
    // get inspected ACs
    const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
    // check if we're inspecting a connection
    if ((inspectedACs.size() == 1) && (inspectedACs.front()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) &&
            inspectedACs.front()->getAttribute(GNE_ATTR_TO_LANEID) == getID()) {
        return true;
    } else {
        return false;
    }
}


bool
GNELane::checkDrawRelatedContour() const {
    return false;
}


bool
GNELane::checkDrawOverContour() const {
    // get modes
    const auto& modes = myNet->getViewNet()->getEditModes();
    // get vehicle frame
    const auto& vehicleFrame = myNet->getViewNet()->getViewParent()->getVehicleFrame();
    const auto& personFramePlanSelector = myNet->getViewNet()->getViewParent()->getPersonFrame()->getPlanSelector();
    const auto& personPlanFramePlanSelector = myNet->getViewNet()->getViewParent()->getPersonPlanFrame()->getPlanSelector();
    const auto& containerFramePlanSelector = myNet->getViewNet()->getViewParent()->getContainerFrame()->getPlanSelector();
    const auto& containerPlanFramePlanSelector = myNet->getViewNet()->getViewParent()->getContainerPlanFrame()->getPlanSelector();
    // check if we're in vehicle mode
    if (vehicleFrame->shown()) {
        // get current vehicle template
        const auto vehicleTemplate = vehicleFrame->getVehicleTagSelector()->getCurrentTemplateAC();
        // check if vehicle can be placed over from-to edges
        if (vehicleTemplate && vehicleTemplate->getTagProperty().vehicleEdges()) {
            // check if lane is under cursor
            return gPostDrawing.isElementUnderCursor(this);
        } else {
            return false;
        }
    } else if (modes.isCurrentSupermodeDemand()) {
        // check if we're in person or personPlan modes
        if (((modes.demandEditMode == DemandEditMode::DEMAND_PERSON) && personFramePlanSelector->markEdges()) ||
                ((modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) && personPlanFramePlanSelector->markEdges()) ||
                ((modes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) && containerFramePlanSelector->markEdges()) ||
                ((modes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) && containerPlanFramePlanSelector->markEdges())) {
            return myNet->getViewNet()->getObjectsUnderCursor().getGUIGlObjectFront() == this;
        }
    }
    return false;
}


bool
GNELane::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNELane::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


GNEMoveOperation*
GNELane::getMoveOperation() {
    // edit depending if shape is being edited
    if (isShapeEdited()) {
        // calculate move shape operation
        return calculateMoveShapeOperation(getLaneShape(), myNet->getViewNet()->getPositionInformation(),
                                           myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.laneGeometryPointRadius,
                                           true, false);
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
                undoList->begin(this, "remove geometry point of " + getTagStr());
                GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(shape), undoList);
                undoList->end();
            }
        }
    }
}


void
GNELane::drawLinkNo(const GUIVisualizationSettings& s) const {
    // check draw conditions
    if (s.drawLinkJunctionIndex.show(myParentEdge->getToJunction())) {
        // get connections
        const auto &connections = myParentEdge->getNBEdge()->getConnectionsFromLane(myIndex);
        // get number of links
        const int noLinks = (int)connections.size();
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
            for (int i = noLinks; i >= 0; i--) {
                // calculate x2
                const double x2 = x1 - (double)(width / 2.);
                // get link index
                const int linkIndex = myParentEdge->getNBEdge()->getToNode()->getConnectionIndex(myParentEdge->getNBEdge(),
                                        connections[s.lefthand ? noLinks - 1 - i : i]);
                // draw link index
                GLHelper::drawTextAtEnd(toString(linkIndex), myLaneGeometry.getShape(), x2, s.drawLinkJunctionIndex, s.scale);
                // update x1
                x1 -= width;
            }
            // pop link matrix
            GLHelper::popMatrix();
        }
    }
}


void
GNELane::drawTLSLinkNo(const GUIVisualizationSettings& s) const {
    // check conditions
    if ((myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::LaneDetails) && s.drawLinkTLIndex.show(myParentEdge->getToJunction()) &&
            (myParentEdge->getToJunction()->getNBNode()->getControllingTLS().size() > 0)) {
        // get connections
        const auto &connections = myParentEdge->getNBEdge()->getConnectionsFromLane(myIndex);
        // get numer of links
        const int noLinks = (int)connections.size();
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
            for (int i = noLinks; i >= 0; --i) {
                // calculate x2
                const double x2 = x1 - (double)(w / 2.);
                // get link number
                const int linkNo = connections[s.lefthand ? noLinks - 1 - i : i].tlLinkIndex;
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


void
GNELane::drawLaneArrows(const GUIVisualizationSettings& s) const {
    if (s.showLinkDecals && myParentEdge->getToJunction()->isLogicValid()) {
        // calculate begin, end and rotation
        const Position& begin = myLaneGeometry.getShape()[-2];
        const Position& end = myLaneGeometry.getShape().back();
        const double rot = GUIGeometry::calculateRotation(begin, end);
        // push arrow matrix
        GLHelper::pushMatrix();
        // move front (note: must draw on top of junction shape?
        glTranslated(0, 0, 0.5);
        // change color depending of spreadSuperposed
        if (myDrawingConstants->drawSuperposed()) {
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
    // update lane drawing constan
    myDrawingConstants->update(s);
    // check drawing conditions
    if (!s.drawForObjectUnderCursor) {
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
        // draw lane
        drawLane(s);
        // Pop layer matrix
        GLHelper::popMatrix();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(s, myDrawingConstants->getDetail(), this, getType(), getPositionInView(), 1);
    }
    // draw children
    drawChildren(s);
    // draw dotted geometry
    myContour.drawDottedContourExtruded(s, myDrawingConstants->getDetail(), myLaneGeometry.getShape(),
                                        myDrawingConstants->getDrawingWidth(), 1, true, true, s.dottedContourSettings.segmentWidth);
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
    // check if draw children elements
    if (s.drawForObjectUnderCursor || (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::Additionals)) {
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
        // draw path additional elements
        myNet->getPathManager()->drawLanePathElements(s, this);
    }
}


void
GNELane::drawMarkingsAndBoundings(const GUIVisualizationSettings& s) const {
    // check conditions
    if (s.laneShowBorders && !myDrawingConstants->drawAsRailway()) {
        // declare separator width
        const auto separatorWidth = SUMO_const_laneMarkWidth * 0.5;
        // push matrix
        GLHelper::pushMatrix();
        // translate
        glTranslated(0, 0, 0.1);
        // set white color
        GLHelper::setColor(RGBColor::WHITE);
        // continue depending of index
        if (myIndex == 0) {
            // in the first lane, draw a separator
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry,
                                separatorWidth, myDrawingConstants->getDrawingWidth() - separatorWidth);
        } else {
            // get permissions between this and previous lane
            const auto permissionsA = myParentEdge->getNBEdge()->getPermissions(myIndex - 1);
            const auto permissionsB = myParentEdge->getNBEdge()->getPermissions(myIndex);
            // get change left and right for passengers
            const bool changeLeft = myParentEdge->getNBEdge()->allowsChangingLeft(myIndex - 1, SVC_PASSENGER);
            const bool changeRight = myParentEdge->getNBEdge()->allowsChangingRight(myIndex, SVC_PASSENGER);
            // if permissions are similar, draw markings. In other case, draw a separator
            if (permissionsA & permissionsB) {
                GLHelper::drawInverseMarkings(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(),
                                              3, 6, myDrawingConstants->getDrawingWidth(), changeLeft, changeRight, s.lefthand, 1);
            } else {
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry,
                                          separatorWidth, myDrawingConstants->getDrawingWidth() + separatorWidth);
            }
            // check if we have change prohibitions
            if ((changeLeft && changeRight) == false) {
                // push prohibitions matrix
                GLHelper::pushMatrix();
                // move top
                glTranslated(0, 0, 0.05);
                // set color
                GLHelper::setColor(RGBColor::ORANGE);
                // higlight prohibition
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry,
                                          myDrawingConstants->getDrawingWidth(), myDrawingConstants->getOffset());
                // pop prohibitionsmatrix
                GLHelper::popMatrix();
            }
        }
        // check if draw last separator
        if (myIndex == (myParentEdge->getNBEdge()->getNumLanes() - 1)) {
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myLaneGeometry,
                                separatorWidth, (myDrawingConstants->getDrawingWidth() * -1) + separatorWidth);
        }
        // pop matrix
        GLHelper::popMatrix();
/*
        // optionally draw inverse markings
        bool haveChangeProhibitions = false;
        if (myIndex > 0 && (myParentEdge->getNBEdge()->getPermissions(myIndex - 1) & myParentEdge->getNBEdge()->getPermissions(myIndex)) != 0) {
            GLHelper::pushMatrix();
            glTranslated(0, 0, 0.1);
            const bool cl = myParentEdge->getNBEdge()->allowsChangingLeft(myIndex - 1, SVC_PASSENGER);
            const bool cr = myParentEdge->getNBEdge()->allowsChangingRight(myIndex, SVC_PASSENGER);
            GLHelper::drawInverseMarkings(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(),
                                          3, 6, myDrawingConstants->getDrawingWidth(), cl, cr, s.lefthand, myDrawingConstants->getExaggeration());
            haveChangeProhibitions = !(cl && cr);
            GLHelper::popMatrix();
        }
        // draw markings
        GLHelper::pushMatrix();
        if (haveChangeProhibitions) {

        }
        GLHelper::popMatrix();
*/
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
        GUIDesigns::buildFXMenuCommand(ret, TL("Copy parent edge name to clipboard"), nullptr, ret, MID_COPY_EDGE_NAME);
        buildNameCopyPopupEntry(ret);
    }
    // stop if we're in data mode
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return ret;
    }
    // build lane selection
    if (isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Remove Lane From Selected"), GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), myNet->getViewNet(), MID_REMOVESELECT);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Add Lane To Selected"), GUIIconSubSys::getIcon(GUIIcon::FLAG_PLUS), myNet->getViewNet(), MID_ADDSELECT);
    }
    // build edge selection
    if (myParentEdge->isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Remove Edge From Selected"), GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), myNet->getViewNet(), MID_GNE_REMOVESELECT_EDGE);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Add Edge To Selected"), GUIIconSubSys::getIcon(GUIIcon::FLAG_PLUS), myNet->getViewNet(), MID_GNE_ADDSELECT_EDGE);
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
        FXMenuCommand* resetEndPoints = GUIDesigns::buildFXMenuCommand(ret, TL("Reset edge end points"), nullptr, &parent, MID_GNE_RESET_GEOMETRYPOINT);
        // enable or disable reset end points
        if (myParentEdge->hasCustomEndPoints()) {
            resetEndPoints->enable();
        } else {
            resetEndPoints->disable();
        }
        // check if we clicked over a geometry point
        if ((editMode == NetworkEditMode::NETWORK_MOVE) && myParentEdge->clickedOverGeometryPoint(mousePosition)) {
            GUIDesigns::buildFXMenuCommand(ret, TL("Set custom Geometry Point"), nullptr, &parent, MID_GNE_CUSTOM_GEOMETRYPOINT);
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
                GUIDesigns::buildFXMenuCommand(ret, TL("Select state for all links from this edge:"), nullptr, nullptr, 0);
                const std::vector<std::string> names = GNEInternalLane::LinkStateNames.getStrings();
                for (auto it : names) {
                    FXuint state = GNEInternalLane::LinkStateNames.get(it);
                    FXMenuRadio* mc = new FXMenuRadio(ret, it.c_str(), this, FXDataTarget::ID_OPTION + state);
                    mc->setSelBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                    mc->setBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                }
            }
        } else {
            FXMenuCommand* mc = GUIDesigns::buildFXMenuCommand(ret, TL("Additional options available in 'Inspect Mode'"), nullptr, nullptr, 0);
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
            GUIDesigns::buildFXMenuCommand(ret, TL("Shape pos: ") + toString(pos), nullptr, nullptr, 0);
            GUIDesigns::buildFXMenuCommand(ret, TL("Length pos: ") + toString(pos * getLaneParametricLength() / getLaneShapeLength()), nullptr, nullptr, 0);
            if (myParentEdge->getNBEdge()->getDistance() != 0) {
                GUIDesigns::buildFXMenuCommand(ret, TL("Distance: ") + toString(myParentEdge->getNBEdge()->getDistancAt(pos)), nullptr, nullptr, 0);
            }
            GUIDesigns::buildFXMenuCommand(ret, TL("Height: ") + toString(firstAnglePos.z()), nullptr, nullptr, 0);
            GUIDesigns::buildFXMenuCommand(ret, TL("Angle: ") + toString((GeomHelper::naviDegree(angle))), nullptr, nullptr, 0);
        }
    }
    return ret;
}


double
GNELane::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNELane::getCenteringBoundary() const {
    return myContour.getContourBoundary();
}


void
GNELane::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


int
GNELane::getIndex() const {
    return myIndex;
}


void
GNELane::setIndex(int index) {
    myIndex = index;
    setNetworkElementID(myParentEdge->getNBEdge()->getLaneID(index));
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
        case SUMO_ATTR_FROM_JUNCTION:
            return myParentEdge->getFromJunction()->getID();
        case SUMO_ATTR_TO_JUNCTION:
            return myParentEdge->getToJunction()->getID();
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
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
        case GNE_ATTR_STOPOEXCEPTION:
            return myParentEdge->getNBEdge()->getLaneStruct(myIndex).laneStopOffset.getOffset() > 0;
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
    undoList->begin(this, "moving " + toString(SUMO_ATTR_CUSTOMSHAPE) + " of " + getTagStr());
    GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate), undoList);
    undoList->end();
}


void
GNELane::drawLane(const GUIVisualizationSettings& s) const {
    // set lane colors
    setLaneColor(s);
    // continue depending of detail level
    if (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::Lane) {
        // Check if lane has to be draw as railway and if isn't being drawn for selecting
        if (myDrawingConstants->drawAsRailway()) {
            // draw as railway
            drawLaneAsRailway(s);
        } else if (myShapeColors.size() > 0) {
            // draw box lines with own colors
            GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                                   myLaneGeometry.getShapeLengths(), myShapeColors, myDrawingConstants->getDrawingWidth(), 0,
                                   myDrawingConstants->getOffset());
        } else {
            // draw box lines with current color
            GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                                   myLaneGeometry.getShapeLengths(), myDrawingConstants->getDrawingWidth(), 0,
                                   myDrawingConstants->getOffset());
        }
        // draw back edge
        drawSelectedLane(s);
        // draw start end shape points
        drawStartEndGeometryPoints(s);
        // check if draw details
        if (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::LaneDetails) {
            // draw markings
            drawMarkingsAndBoundings(s);
            // Draw direction indicators
            drawDirectionIndicators(s);
            // draw lane textures
            drawTextures(s);
            // draw lane arrows
            drawLaneArrows(s);
            // draw link numbers
            drawLinkNo(s);
            // draw TLS link numbers
            drawTLSLinkNo(s);
            // draw stopOffsets
            drawLaneStopOffset(s);
        }
        // draw shape edited
        drawShapeEdited(s);
    } else if ((myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::LaneSimple) ||
               ((myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::LaneSimpleOnlyFirst) && (myIndex == 0))) {
        // draw lane as line, depending of myShapeColors
        if (myShapeColors.size() > 0) {
            GLHelper::drawLine(myLaneGeometry.getShape(), myShapeColors);
        } else {
            GLHelper::drawLine(myLaneGeometry.getShape());
        }
    }
}


void
GNELane::drawSelectedLane(const GUIVisualizationSettings& s) const {
    // only draw if lane is selected
    if (drawUsingSelectColor()) {
        // Push matrix
        GLHelper::pushMatrix();
        // move back
        glTranslated(0, 0, 0.1);
        // set selected edge color
        GLHelper::setColor(s.colorSettings.selectedLaneColor);
        // draw internal box line
        GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                               myLaneGeometry.getShapeLengths(), myDrawingConstants->getInternalDrawingWidth(),
                               myDrawingConstants->getOffset());
        // Pop matrix
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
        // draw shape around
        GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                                myLaneGeometry.getShapeLengths(), 0.25);
        // move front
        glTranslated(0, 0, 1);
        // color
        const RGBColor darkerColor = s.colorSettings.editShapeColor.changedBrightness(-32);
        // draw geometry points
        GUIGeometry::drawGeometryPoints(s, this, myNet->getViewNet()->getPositionInformation(), myLaneGeometry.getShape(), darkerColor, RGBColor::BLACK,
                                        s.neteditSizeSettings.laneGeometryPointRadius, 1, myNet->getViewNet()->getNetworkViewOptions().editingElevation(), true);
        // Pop shape edited matrix
        GLHelper::popMatrix();
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
GNELane::drawLaneStopOffset(const GUIVisualizationSettings& s) const {
    const auto& laneStopOffset = myParentEdge->getNBEdge()->getLaneStruct(myIndex).laneStopOffset;
    // check conditions
    if (laneStopOffset.isDefined() && (laneStopOffset.getPermissions() & SVC_PASSENGER) != 0) {
        const Position& end = getLaneShape().back();
        const Position& f = getLaneShape()[-2];
        const double rot = RAD2DEG(atan2((end.x() - f.x()), (f.y() - end.y())));
        GLHelper::setColor(s.getLinkColor(LINKSTATE_MAJOR));
        GLHelper::pushMatrix();
        glTranslated(end.x(), end.y(), 1);
        glRotated(rot, 0, 0, 1);
        glTranslated(0, laneStopOffset.getOffset(), 0);
        glBegin(GL_QUADS);
            glVertex2d(-myDrawingConstants->getDrawingWidth(), 0.0);
            glVertex2d(-myDrawingConstants->getDrawingWidth(), 0.2);
            glVertex2d(myDrawingConstants->getDrawingWidth(), 0.2);
            glVertex2d(myDrawingConstants->getDrawingWidth(), 0.0);
        glEnd();
        GLHelper::popMatrix();
    }
}


bool
GNELane::drawAsWaterway(const GUIVisualizationSettings& s) const {
    return isWaterway(myParentEdge->getNBEdge()->getPermissions(myIndex)) && s.showRails && !s.drawForRectangleSelection; // reusing the showRails setting
}


void
GNELane::drawDirectionIndicators(const GUIVisualizationSettings& s) const {
    // Draw direction indicators if the correspondient option is enabled
    if (s.showLaneDirection) {
        // improve visibility of superposed rail edges
        if (!myDrawingConstants->drawAsRailway()) {
            glColor3d(0.3, 0.3, 0.3);
        }
        // get width and sideOffset
        const double width = MAX2(NUMERICAL_EPS, (myDrawingConstants->getDrawingWidth() * 2 * myDrawingConstants->getExaggeration() * (myDrawingConstants->drawSuperposed() ? 0.4 : 1)));
        const double sideOffset = myDrawingConstants->drawSuperposed() ? width * -0.5 : 0;
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
GNELane::drawLaneAsRailway(const GUIVisualizationSettings& s) const {
    // calculate constant
    const double halfInnerFeetWidth = myDrawingConstants->getDrawingWidth() - 0.039 * myDrawingConstants->getExaggeration();
    const double halfRailWidth = halfInnerFeetWidth + 0.15 * myDrawingConstants->getExaggeration();
    const double halfCrossTieWidth = myDrawingConstants->getDrawingWidth() * 1.81;
    // draw as box lines
    if (myShapeColors.size() > 0) {
        GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                               myLaneGeometry.getShapeLengths(), myShapeColors, halfRailWidth,
                               myDrawingConstants->getOffset());
    } else {
        GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                               myLaneGeometry.getShapeLengths(), halfRailWidth, myDrawingConstants->getOffset());
    }
    // continue depending of detail
    if (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::LaneDetails) {
        // Save current color
        RGBColor current = GLHelper::getColor();
        // Draw gray on top with reduced width (the area between the two tracks)
        glColor3d(0.8, 0.8, 0.8);
        // move
        glTranslated(0, 0, 0.1);
        // draw as box lines
        GLHelper::drawBoxLines(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(),
                               myLaneGeometry.getShapeLengths(), halfInnerFeetWidth,
                               myDrawingConstants->getOffset());
        // Set current color back
        GLHelper::setColor(current);
        // Draw crossties
        GLHelper::drawCrossTies(myLaneGeometry.getShape(), myLaneGeometry.getShapeRotations(), myLaneGeometry.getShapeLengths(), 0.26 * myDrawingConstants->getExaggeration(), 0.6 * myDrawingConstants->getExaggeration(), halfCrossTieWidth, s.drawForRectangleSelection);
    }
}


void
GNELane::drawTextures(const GUIVisualizationSettings& s) const {
    // check all conditions for drawing textures
    if (!s.disableLaneIcons && (myLaneRestrictedTexturePositions.size() > 0)) {
        // Declare default width of icon (3)
        const double iconWidth = myDrawingConstants->getDrawingWidth() * 0.6;
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
GNELane::drawStartEndGeometryPoints(const GUIVisualizationSettings& s) const {
    // draw a Start/endPoints if lane has a custom shape
    if ((myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::GeometryPoint) && (myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape.size() > 1)) {
        // obtain circle width and resolution
        const double circleWidth = GNEEdge::SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration) / 2;
        // obtain custom shape
        const PositionVector& customShape = myParentEdge->getNBEdge()->getLaneStruct(myIndex).customShape;
        // set color (override with special colors unless the color scheme is based on selection)
        if (drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
            GLHelper::setColor(s.colorSettings.selectedEdgeColor.changedBrightness(-20));
        } else {
            GLHelper::setColor(s.junctionColorer.getSchemes()[0].getColor(2));
        }
        // push start matrix
        GLHelper::pushMatrix();
        // move to shape start position
        glTranslated(customShape.front().x(), customShape.front().y(), 0.1);
        // draw circle
        GLHelper::drawFilledCircleDetailled(myDrawingConstants->getDetail(), circleWidth);
        // draw s depending of detail
        if (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::Text) {
            // move top
            glTranslated(0, 0, 0.1);
            // draw "S"
            GLHelper::drawText("S", Position(), 0.1, circleWidth, RGBColor::WHITE);
        }
        // pop start matrix
        GLHelper::popMatrix();
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
        // push start matrix
        GLHelper::pushMatrix();
        // move to end position
        glTranslated(customShape.back().x(), customShape.back().y(), 0.1);
        // draw filled circle
        GLHelper::drawFilledCircleDetailled(myDrawingConstants->getDetail(), circleWidth);
        // draw "e" depending of detail
        if (myDrawingConstants->getDetail() <= GUIVisualizationSettings::Detail::Text) {
            // move top
            glTranslated(0, 0, 0.1);
            // draw "E"
            GLHelper::drawText("E", Position(), 0, circleWidth, RGBColor::WHITE);
        }
        // pop start matrix
        GLHelper::popMatrix();
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
        for (const auto& incomingEdge : junctionSource->getGNEIncomingEdges()) {
            // Iterate over connection of incoming edges
            for (const auto& connection : incomingEdge->getGNEConnections()) {
                if (connection->getLaneFrom()->getIndex() == getIndex()) {
                    incomingConnections.push_back(connection);
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
    for (const auto& connection : edgeConnections) {
        if (connection->getLaneTo()->getIndex() == getIndex()) {
            outcomingConnections.push_back(connection);
        }
    }
    return outcomingConnections;
}


void
GNELane::updateConnectionIDs() {
    // update incoming connections of lane
    std::vector<GNEConnection*> incomingConnections = getGNEIncomingConnections();
    for (const auto& incomingConnection : incomingConnections) {
        incomingConnection->updateConnectionID();
    }
    // update outcoming connections of lane
    std::vector<GNEConnection*> outcomingConnections = getGNEOutcomingConnections();
    for (const auto& outcomingConnection : outcomingConnections) {
        outcomingConnection->updateConnectionID();
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
        new FXMenuCascade(ret, TLF("Edge operations (% selected)", toString(edgeSelSize)).c_str(), nullptr, edgeOperations);
    } else {
        new FXMenuCascade(ret, TL("Edge operations"), nullptr, edgeOperations);
    }
    // create menu commands for all edge operations
    GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Split edge here"), nullptr, &parent, MID_GNE_EDGE_SPLIT);
    auto splitBothDirections = GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Split edge in both directions here (no symmetric opposite edge)"), nullptr, &parent, MID_GNE_EDGE_SPLIT_BIDI);
    // check if allow split edge in both directions
    splitBothDirections->disable();
    const auto oppositeEdges = myParentEdge->getOppositeEdges();
    if (oppositeEdges.size() == 0) {
        splitBothDirections->setText(TL("Split edge in both directions here (no opposite edge)"));
    } else {
        for (const auto& oppositeEdge : oppositeEdges) {
            // get reverse inner geometry
            const auto reverseGeometry = oppositeEdge->getNBEdge()->getInnerGeometry().reverse();
            if (reverseGeometry == myParentEdge->getNBEdge()->getInnerGeometry()) {
                splitBothDirections->enable();
                splitBothDirections->setText(TL("Split edge in both directions here"));
            }
        }
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Set geometry endpoint here (shift-click)"), nullptr, &parent, MID_GNE_EDGE_EDIT_ENDPOINT);
    // restore geometry points depending of selection status
    if (myParentEdge->isAttributeCarrierSelected()) {
        if (edgeSelSize == 1) {
            GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Restore both geometry endpoints"), nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
        } else {
            GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Restore geometry endpoints of all selected edges"), nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
        }
    } else {
        GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Restore geometry endpoint (shift-click)"), nullptr, &parent, MID_GNE_EDGE_RESET_ENDPOINT);
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Reverse %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_REVERSE);
    auto reverse = GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Add reverse direction for %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_ADD_REVERSE);
    if (myParentEdge->getReverseEdge() != nullptr) {
        reverse->disable();
    }
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Add reverse disconnected direction for %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_ADD_REVERSE_DISCONNECTED);
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Reset lengths for %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_RESET_LENGTH);
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Straighten %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN);
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Smooth %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_SMOOTH);
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Straighten elevation of %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_STRAIGHTEN_ELEVATION);
    GUIDesigns::buildFXMenuCommand(edgeOperations, TLF("Smooth elevation of %", edgeDescPossibleMulti), nullptr, &parent, MID_GNE_EDGE_SMOOTH_ELEVATION);
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
        new FXMenuCascade(ret, TLF("Lane operations (% selected)", toString(numSelectedLanes)).c_str(), nullptr, laneOperations);
    } else {
        new FXMenuCascade(ret, TL("Lane operations"), nullptr, laneOperations);
    }
    GUIDesigns::buildFXMenuCommand(laneOperations, TL("Duplicate lane"), nullptr, &parent, MID_GNE_LANE_DUPLICATE);
    GUIDesigns::buildFXMenuCommand(laneOperations, TL("Set custom lane shape"), nullptr, &parent, MID_GNE_LANE_EDIT_SHAPE);
    FXMenuCommand* resetCustomShape = GUIDesigns::buildFXMenuCommand(laneOperations, TL("Reset custom shape"), nullptr, &parent, MID_GNE_LANE_RESET_CUSTOMSHAPE);
    if (!differentLaneShapes) {
        resetCustomShape->disable();
    }
    FXMenuCommand* resetOppositeLane = GUIDesigns::buildFXMenuCommand(laneOperations, TL("Reset opposite lane"), nullptr, &parent, MID_GNE_LANE_RESET_OPPOSITELANE);
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
    FXMenuCommand* addSidewalk = GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Sidewalk"), pedestrianIcon, &parent, MID_GNE_LANE_ADD_SIDEWALK);
    FXMenuCommand* addBikelane = GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Bike lane"), bikeIcon, &parent, MID_GNE_LANE_ADD_BIKE);
    FXMenuCommand* addBuslane = GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Bus lane"), busIcon, &parent, MID_GNE_LANE_ADD_BUS);
    // if parent edge is selected, always add greenverge in front
    if (myParentEdge->isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Green verge"), greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_FRONT);
    } else {
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Green verge (front)"), greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_FRONT);
        GUIDesigns::buildFXMenuCommand(addSpecialLanes, TL("Green verge (back)"), greenVergeIcon, &parent, MID_GNE_LANE_ADD_GREENVERGE_BACK);
    }
    // Create menu comands for all remove special lanes and disable it
    FXMenuCommand* removeSidewalk = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, TL("Sidewalk"), pedestrianIcon, &parent, MID_GNE_LANE_REMOVE_SIDEWALK);
    removeSidewalk->disable();
    FXMenuCommand* removeBikelane = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, TL("Bike lane"), bikeIcon, &parent, MID_GNE_LANE_REMOVE_BIKE);
    removeBikelane->disable();
    FXMenuCommand* removeBuslane = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, TL("Bus lane"), busIcon, &parent, MID_GNE_LANE_REMOVE_BUS);
    removeBuslane->disable();
    FXMenuCommand* removeGreenVerge = GUIDesigns::buildFXMenuCommand(removeSpecialLanes, TL("Green verge"), greenVergeIcon, &parent, MID_GNE_LANE_REMOVE_GREENVERGE);
    removeGreenVerge->disable();
    // Create menu comands for all transform special lanes and disable it
    FXMenuCommand* transformLaneToSidewalk = GUIDesigns::buildFXMenuCommand(transformSlanes, TL("Sidewalk"), pedestrianIcon, &parent, MID_GNE_LANE_TRANSFORM_SIDEWALK);
    FXMenuCommand* transformLaneToBikelane = GUIDesigns::buildFXMenuCommand(transformSlanes, TL("Bike lane"), bikeIcon, &parent, MID_GNE_LANE_TRANSFORM_BIKE);
    FXMenuCommand* transformLaneToBuslane = GUIDesigns::buildFXMenuCommand(transformSlanes, TL("Bus lane"), busIcon, &parent, MID_GNE_LANE_TRANSFORM_BUS);
    FXMenuCommand* transformLaneToGreenVerge = GUIDesigns::buildFXMenuCommand(transformSlanes, TL("Green verge"), greenVergeIcon, &parent, MID_GNE_LANE_TRANSFORM_GREENVERGE);
    // add menuCascade for lane operations
    new FXMenuCascade(laneOperations, TLF("Add restricted %", toString(SUMO_TAG_LANE)).c_str(), nullptr, addSpecialLanes);
    FXMenuCascade* cascadeRemoveSpecialLane = new FXMenuCascade(laneOperations, TLF("Remove restricted %", toString(SUMO_TAG_LANE)).c_str(), nullptr, removeSpecialLanes);
    new FXMenuCascade(laneOperations, TLF("Transform to restricted %", toString(SUMO_TAG_LANE)).c_str(), nullptr, transformSlanes);
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
        new FXMenuCascade(ret, TLF("Template operations (% selected)", toString(edgeSelSize)).c_str(), nullptr, edgeOperations);
    } else {
        new FXMenuCascade(ret, TL("Template operations"), nullptr, edgeOperations);
    }
    // create menu commands for all edge operations
    GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Use edge as template"), nullptr, &parent, MID_GNE_EDGE_USEASTEMPLATE);
    auto applyTemplate = GUIDesigns::buildFXMenuCommand(edgeOperations, TL("Apply template"), nullptr, &parent, MID_GNE_EDGE_APPLYTEMPLATE);
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
        new FXMenuCascade(ret, TL("Select reachable"), GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), reachableByClass);
        for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
            GUIDesigns::buildFXMenuCommand(reachableByClass, vClass.c_str(), VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(vClass)), &parent, MID_REACHABILITY);
        }
    } else {
        FXMenuCommand* menuCommand = GUIDesigns::buildFXMenuCommand(ret, TL("Select reachable (compute junctions)"), nullptr, nullptr, 0);
        menuCommand->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    }
}

/****************************************************************************/
