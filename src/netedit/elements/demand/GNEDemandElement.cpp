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
/// @file    GNEDemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// A abstract class for demand elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNEDemandElement.h"


// ===========================================================================
// static definitions
// ===========================================================================
const double GNEDemandElement::myPersonPlanArrivalPositionDiameter = SUMO_const_halfLaneWidth;

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElement::GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, id, icon, options),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myStackedLabelNumber(0) {
    // check if is template
    myIsTemplate = (id == "");
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, demandElementParent->getID(), icon, options),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myStackedLabelNumber(0) {
}


GNEDemandElement::~GNEDemandElement() {}


void
GNEDemandElement::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // currently there isn't demand elements with removable geometry points
}


GUIGlObject*
GNEDemandElement::getGUIGlObject() {
    return this;
}


const GUIGeometry&
GNEDemandElement::getDemandElementGeometry() {
    return myDemandElementGeometry;
}


GNEDemandElement*
GNEDemandElement::getPreviousChildDemandElement(const GNEDemandElement* demandElement) const {
    // first check if there are demand elements
    if (getChildDemandElements().empty()) {
        return nullptr;
    } else {
        // find child demand element
        auto it = std::find(getChildDemandElements().begin(), getChildDemandElements().end(), demandElement);
        // return element or null depending of iterator
        if (it == getChildDemandElements().end()) {
            // in this case, we assume that the last child is the previos child
            return getChildDemandElements().back();
        } else if (it == getChildDemandElements().begin()) {
            return nullptr;
        } else {
            return *(it - 1);
        }
    }
}


GNEDemandElement*
GNEDemandElement::getNextChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(getChildDemandElements().begin(), getChildDemandElements().end(), demandElement);
    // return element or null depending of iterator
    if (it == getChildDemandElements().end()) {
        return nullptr;
    } else if (it == (getChildDemandElements().end() - 1)) {
        return nullptr;
    } else {
        return *(it + 1);
    }
}


void
GNEDemandElement::updateDemandElementGeometry(const GNELane* lane, const double posOverLane) {
    myDemandElementGeometry.updateGeometry(lane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
}


void
GNEDemandElement::updateDemandElementStackLabel(const int stack) {
    myStackedLabelNumber = stack;
}


void
GNEDemandElement::updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane) {
    mySpreadGeometry.updateGeometry(lane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
}


void
GNEDemandElement::openDemandElementDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have a demand element dialog");
}


std::string
GNEDemandElement::getBegin() const {
    throw InvalidArgument(getTagStr() + " doesn't have a begin time");
}


GUIGLObjectPopupMenu*
GNEDemandElement::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    return ret;
}


GUIParameterTableWindow*
GNEDemandElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


bool
GNEDemandElement::isGLObjectLocked() {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return myNet->getViewNet()->getLockManager().isObjectLocked(getType(), isAttributeCarrierSelected());
    } else {
        return true;
    }
}


void
GNEDemandElement::markAsFrontElement() {
    myNet->getViewNet()->setFrontAttributeCarrier(this);
}


void
GNEDemandElement::deleteGLObject() {
    // we need an special checks due hierarchies
    if (myTagProperty.isPersonPlan() || myTagProperty.isContainerPlan()) {
        // get person/container plarent
        GNEDemandElement* parent = getParentDemandElements().front();
        // if this is the last person/container plan element, remove parent instead plan
        if (parent->getChildDemandElements().size() == 1) {
            parent->deleteGLObject();
        } else {
            myNet->deleteDemandElement(this, myNet->getViewNet()->getUndoList());
        }
    } else if (getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
        // remove parent demand element
        getParentDemandElements().front()->deleteGLObject();
    } else {
        myNet->deleteDemandElement(this, myNet->getViewNet()->getUndoList());
    }
}


void
GNEDemandElement::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


void
GNEDemandElement::updateGLObject() {
    updateGeometry();
}


bool
GNEDemandElement::isPathElementSelected() const {
    return mySelected;
}


double
GNEDemandElement::getPathElementDepartValue() const {
    // get previous person Plan
    const GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    // check if this is the first person plan
    if (previousPersonPlan) {
        if (previousPersonPlan->getParentAdditionals().size() > 0) {
            if (previousPersonPlan->getTagProperty().isStopPerson()) {
                // calculate busStop end
                const double endPos = previousPersonPlan->getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
                // check endPos
                if (endPos < 0.3) {
                    return endPos;
                } else {
                    return (endPos - 0.3);
                }
            } else {
                // use busStop center
                return previousPersonPlan->getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_CENTER);
            }
        } else {
            // use arrival pos
            return previousPersonPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        }
    } else {
        // use pedestrian departPos
        return getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
    }
}


Position
GNEDemandElement::getPathElementDepartPos() const {
    // get previous person Plan
    const GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    // check if this is the first person plan
    if (previousPersonPlan) {
        if (previousPersonPlan->getParentAdditionals().size() > 0) {
            if (previousPersonPlan->getTagProperty().isStopPerson()) {
                // get busStop
                const GNEAdditional* busStop = previousPersonPlan->getParentAdditionals().front();
                // get length
                const double length = busStop->getAdditionalGeometry().getShape().length2D();
                // check length
                if (length < 0.3) {
                    return busStop->getAdditionalGeometry().getShape().back();
                } else {
                    return busStop->getAdditionalGeometry().getShape().positionAtOffset2D(length - 0.3);
                }
            } else {
                // use busStop center
                return previousPersonPlan->getParentAdditionals().front()->getAdditionalGeometry().getShape().getLineCenter();
            }
        } else {
            // use arrival pos
            return previousPersonPlan->getAttributePosition(SUMO_ATTR_ARRIVALPOS);
        }
    } else if (getParentJunctions().size() > 0) {
        return getParentJunctions().front()->getNBNode()->getPosition();
    } else {
        // use pedestrian departPos
        return getParentDemandElements().at(0)->getAttributePosition(SUMO_ATTR_DEPARTPOS);
    }
}


double
GNEDemandElement::getPathElementArrivalValue() const {
    // check if this person plan ends in a busStop
    if (getParentAdditionals().size() > 0) {
        // get next person Plan
        const GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
        // continue depending if is an stop or a person plan
        if (nextPersonPlan && (nextPersonPlan->getTagProperty().getTag() == GNE_TAG_STOPPERSON_BUSSTOP)) {
            // calculate busStop end
            const double endPos = getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            // check endPos
            if (endPos < 0.3) {
                return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            } else {
                return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS) - 0.3;
            }
        } else {
            return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_CENTER);
        }
    } else {
        return getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    }
}


Position
GNEDemandElement::getPathElementArrivalPos() const {
    // check if this person plan ends in a busStop
    if (getParentAdditionals().size() > 0) {
        // get next person Plan
        const GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
        // continue depending if is an stop or a person plan
        if (nextPersonPlan && (nextPersonPlan->getTagProperty().getTag() == GNE_TAG_STOPPERSON_BUSSTOP)) {
            // get busStop
            const GNEAdditional* busStop = nextPersonPlan->getParentAdditionals().front();
            // get length
            const double length = busStop->getAdditionalGeometry().getShape().length2D();
            // check length
            if (length < 0.3) {
                return busStop->getAdditionalGeometry().getShape().back();
            } else {
                return busStop->getAdditionalGeometry().getShape().positionAtOffset2D(length - 0.3);
            }
        } else {
            return getParentAdditionals().front()->getAdditionalGeometry().getShape().getLineCenter();
        }
    } else if (getParentJunctions().size() > 0) {
        return getParentJunctions().back()->getNBNode()->getPosition();
    } else {
        return getAttributePosition(SUMO_ATTR_ARRIVALPOS);
    }
}

// ---------------------------------------------------------------------------
// GNEDemandElement - protected methods
// ---------------------------------------------------------------------------

bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myNet->getAttributeCarriers()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


const Position
GNEDemandElement::getBeginPosition(const double pedestrianDepartPos) const {
    if (myTagProperty.isStopPerson()) {
        return getPositionInView();
    } else {
        // get first lane
        const GNELane* firstLane = myNet->getPathManager()->getFirstLane(this);
        if (firstLane) {
            return firstLane->getLaneShape().positionAtOffset2D(pedestrianDepartPos);
        } else {
            return Position(0, 0);
        }
    }
}


std::vector<GNEDemandElement*>
GNEDemandElement::getInvalidStops() const {
    if (myTagProperty.isStop() || myTagProperty.isWaypoint()) {
        // get stops
        std::vector<GNEDemandElement*> invalidStops;
        // get edge stop index
        const auto edgeStopIndex = getEdgeStopIndex();
        // take all stops/waypoints with index = -1
        for (const auto& edgeStop : edgeStopIndex) {
            if (edgeStop.stopIndex == -1) {
                for (const auto& stop : edgeStop.stops) {
                    invalidStops.push_back(stop);
                }
            }
        }
        return invalidStops;
    } else {
        return {};
    }
}


bool
GNEDemandElement::drawPersonPlan() const {
    // check conditions
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        // show all person plans in network mode
        return true;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
               myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        // show all person plans
        return true;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() && isAttributeCarrierSelected()) {
        // show selected
        return true;
    } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentDemandElements().front())) {
        // person parent is inspected
        return true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedPerson() == getParentDemandElements().front()) {
        // person parent is locked
        return true;
    } else if (myNet->getViewNet()->getInspectedAttributeCarriers().empty()) {
        // nothing is inspected
        return false;
    } else {
        // get inspected AC
        const GNEAttributeCarrier* AC = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // check condition
        if (AC->getTagProperty().isPersonPlan() && AC->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT)) {
            // common person parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


bool
GNEDemandElement::drawContainerPlan() const {
    // check conditions
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showAllContainerPlans()) {
        // show all container plans in network mode
        return true;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
               myNet->getViewNet()->getDemandViewOptions().showAllContainerPlans()) {
        // show all container plans
        return true;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() && isAttributeCarrierSelected()) {
        // show selected
        return true;
    } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentDemandElements().front())) {
        // container parent is inspected
        return true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedContainer() == getParentDemandElements().front()) {
        // container parent is locked
        return true;
    } else if (myNet->getViewNet()->getInspectedAttributeCarriers().empty()) {
        // nothing is inspected
        return false;
    } else {
        // get inspected AC
        const GNEAttributeCarrier* AC = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // check condition
        if (AC->getTagProperty().isContainerPlan() && AC->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT)) {
            // common container parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


void
GNEDemandElement::drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const {
    // get inspected and front flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    // get person parent
    const GNEDemandElement* personParent = getParentDemandElements().front();
    // check if draw person plan element can be drawn
    if ((personPlanColor.alpha() != 0) && drawPlan && myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myTagProperty.getTag())) {
        // get inspected attribute carriers
        const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
        // get inspected person plan
        const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (personPlanInspected == this) || (personPlanInspected == personParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, lane) * personPlanWidth * (duplicateWidth ? 2 : 1);
        // declare path geometry
        GUIGeometry personPlanGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              getPathElementDepartValue(), getPathElementArrivalValue(),    // extrem positions
                                              getPathElementDepartPos(), getPathElementArrivalPos());       // extra positions
        } else if (segment->isFirstSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              getPathElementDepartValue(), -1,                 // extrem positions
                                              getPathElementDepartPos(), Position::INVALID);   // extra positions
        } else if (segment->isLastSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              -1, getPathElementArrivalValue(),                // extrem positions
                                              Position::INVALID, getPathElementArrivalPos());  // extra positions
        } else {
            personPlanGeometry = lane->getLaneGeometry();
        }
        // get color
        const RGBColor& pathColor = drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : personPlanColor;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront);
        // Set color
        GLHelper::setColor(pathColor);
        // draw geometry
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), personPlanGeometry, pathWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // check if this is the last segment
        if (segment->isLastSegment()) {
            // calculate circle width
            const double circleRadius = (duplicateWidth ? myPersonPlanArrivalPositionDiameter : (myPersonPlanArrivalPositionDiameter / 2.0));
            const double circleWidth = circleRadius * MIN2((double)0.5, s.laneWidthExaggeration);
            const double circleWidthSquared = circleWidth * circleWidth;
            // get geometryEndPos
            const Position geometryEndPos = getPathElementArrivalPos();
            // check if endPos can be drawn
            if (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= (circleWidthSquared + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area traslating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to pos and move to upper using GLO_PERSONTRIP (to avoid overlapping)
                glTranslated(geometryEndPos.x(), geometryEndPos.y(), 0);
                // Set person plan color
                GLHelper::setColor(pathColor);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                // pop draw matrix
                GLHelper::popMatrix();
            }
        }
        // check if we have to draw an red arrow or line
        if (segment->getNextSegment() && segment->getNextSegment()->getLane()) {
            // get firstPosition (last position of current lane shape)
            const Position from = lane->getLaneShape().back();
            // get lastPosition (first position of next lane shape)
            const Position to = segment->getNextSegment()->getLane()->getLaneShape().front();
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // draw child line
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || isAttributeCarrierSelected(), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // check if we have to draw an red arrow or line
        if (segment->getPreviousSegment() && segment->getPreviousSegment()->getLane()) {
            // get firstPosition (last position of current lane shape)
            const Position from = lane->getLaneShape().front();
            // get lastPosition (first position of next lane shape)
            const Position to = segment->getPreviousSegment()->getLane()->getLaneShape().back();
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // draw child line
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || isAttributeCarrierSelected(), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // Pop name
        GLHelper::popName();
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? personPlanGeometry.getShape() : lane->getLaneShape();
        // check if mouse is over element
        mouseWithinGeometry(shape, pathWidth);
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // front element contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
    // draw person parent if this is the edge first edge and this is the first plan
    if (getParentJunctions().empty() && (getFirstPathLane()->getParentEdge() == lane->getParentEdge()) &&
            (personParent->getChildDemandElements().front() == this)) {
        personParent->drawGL(s);
    }
}


void
GNEDemandElement::drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const {
    // get inspected and front flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    // check if draw person plan elements can be drawn
    if ((personPlanColor.alpha() != 0) && drawPlan && myNet->getPathManager()->getPathDraw()->drawPathGeometry(false, fromLane, toLane, myTagProperty.getTag())) {
        // get inspected attribute carriers
        const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
        // get person parent
        const GNEDemandElement* personParent = getParentDemandElements().front();
        // get inspected person plan
        const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (personPlanInspected == this) || (personPlanInspected == personParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, fromLane) * personPlanWidth * (duplicateWidth ? 2 : 1);
        // get color
        const RGBColor& color = drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : personPlanColor;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // push a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront);
        // check if draw lane2lane connection or a red line
        if (fromLane && fromLane->getLane2laneConnections().exist(toLane)) {
            // obtain lane2lane geometry
            const GUIGeometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
            // Set person plan color
            GLHelper::setColor(color);
            // draw lane2lane
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), lane2laneGeometry, pathWidth);
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // draw line between end of first shape and first position of second shape
            GLHelper::drawBoxLines({fromLane->getLaneShape().back(), toLane->getLaneShape().front()}, (0.5 * pathWidth));
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), 0.5);
        // check if shape dotted contour has to be drawn
        if (fromLane->getLane2laneConnections().exist(toLane) && dottedElement) {
            // check if mouse is over element
            mouseWithinGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(), pathWidth);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // front contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
        }
    }
}


GNEDemandElement::Problem
GNEDemandElement::isPersonPlanValid() const {
    // get previous child
    const auto previousChild = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    if (previousChild) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousChild->getParentLanes().size() == 1) {
            previousEdge = previousChild->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentAdditionals().size() == 1) {
            previousEdge = previousChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentEdges().size() > 0) {
            previousEdge = previousChild->getParentEdges().back();
        } else if (previousChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousChild->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // get first edge
        GNEEdge* firstEdge = nullptr;
        // check edge
        if (getParentLanes().size() == 1) {
            firstEdge = getParentLanes().front()->getParentEdge();
        } else if (getParentEdges().size() > 0) {
            firstEdge = getParentEdges().front();
        } else if (getParentAdditionals().size() == 1) {
            firstEdge = getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            firstEdge = getParentDemandElements().at(1)->getParentEdges().front();
        }
        // check junctions
        if ((previousChild->getParentJunctions().size() > 0) && (getParentJunctions().size() > 0)) {
            if (previousChild->getParentJunctions().back() != getParentJunctions().front()) {
                return Problem::DISCONNECTED_PLAN;
            }
        } else if (previousEdge && (getParentJunctions().size() > 0)) {
            if (previousEdge->getToJunction() != getParentJunctions().front()) {
                return Problem::DISCONNECTED_PLAN;
            }
        } else if (previousEdge != firstEdge) {
            return Problem::DISCONNECTED_PLAN;
        }
    }
    // get next child
    const auto nextChild = getParentDemandElements().at(0)->getNextChildDemandElement(this);
    if (nextChild) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextChild->getParentLanes().size() == 1) {
            nextEdge = nextChild->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentEdges().size() > 0) {
            nextEdge = nextChild->getParentEdges().front();
        } else if (nextChild->getParentAdditionals().size() == 1) {
            nextEdge = nextChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextChild->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // get last edge
        GNEEdge* lastEdge = nullptr;
        // check edge
        if (getParentLanes().size() == 1) {
            lastEdge = getParentLanes().front()->getParentEdge();
        } else if (getParentAdditionals().size() == 1) {
            lastEdge = getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (getParentEdges().size() > 0) {
            lastEdge = getParentEdges().back();
        } else if (getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            lastEdge = getParentDemandElements().at(1)->getParentEdges().back();
        }
        // compare both edges
        if ((nextChild->getParentJunctions().size() > 0) && (getParentJunctions().size() > 0)) {
            if (nextChild->getParentJunctions().front() != getParentJunctions().back()) {
                return Problem::DISCONNECTED_PLAN;
            }
        } else if (nextEdge && (getParentJunctions().size() > 0)) {
            if (nextEdge->getFromJunction() != getParentJunctions().back()) {
                return Problem::DISCONNECTED_PLAN;
            }
        } else if (lastEdge && (nextChild->getParentJunctions().size() > 0)) {
            if (lastEdge->getToJunction() != nextChild->getParentJunctions().front()) {
                return Problem::DISCONNECTED_PLAN;
            }
        } else if (nextEdge != lastEdge) {
            return Problem::DISCONNECTED_PLAN;
        }
    }
    // all ok, then return true
    return Problem::OK;
}


std::string
GNEDemandElement::getPersonPlanProblem() const {
    // get previous child
    const auto previousChild = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    if (previousChild) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousChild->getParentLanes().size() == 1) {
            previousEdge = previousChild->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentAdditionals().size() == 1) {
            previousEdge = previousChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentEdges().size() > 0) {
            previousEdge = previousChild->getParentEdges().back();
        } else if (previousChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousChild->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // get first edge
        GNEEdge* firstEdge = nullptr;
        // check edge
        if (getParentLanes().size() == 1) {
            firstEdge = getParentLanes().front()->getParentEdge();
        } else if (getParentEdges().size() > 0) {
            firstEdge = getParentEdges().front();
        } else if (getParentAdditionals().size() == 1) {
            firstEdge = getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            firstEdge = getParentDemandElements().at(1)->getParentEdges().front();
        }
        // compare elements
        if ((previousChild->getParentJunctions().size() > 0) && (getParentJunctions().size() > 0)) {
            return ("Junction '" + previousChild->getParentJunctions().back()->getID() +
                    "' is not consecutive with junction '" + getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && (getParentJunctions().size() > 0)) {
            return ("edge '" + previousEdge->getID() + "' is not consecutive with junction '" + getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && firstEdge && (previousEdge != firstEdge)) {
            return "Edge '" + previousEdge->getID() + "' is not consecutive with edge '" + firstEdge->getID() + "'";
        }
    }
    // get next child
    const auto nextChild = getParentDemandElements().at(0)->getNextChildDemandElement(this);
    if (nextChild) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextChild->getParentLanes().size() == 1) {
            nextEdge = nextChild->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentAdditionals().size() == 1) {
            nextEdge = nextChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentEdges().size() > 0) {
            nextEdge = nextChild->getParentEdges().front();
        } else if (nextChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextChild->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // get last edge
        GNEEdge* lastEdge = nullptr;
        // check edge
        if (getParentLanes().size() == 1) {
            lastEdge = getParentLanes().front()->getParentEdge();
        } else if (getParentAdditionals().size() == 1) {
            lastEdge = getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (getParentEdges().size() > 0) {
            lastEdge = getParentEdges().back();
        } else if (getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            lastEdge = getParentDemandElements().at(1)->getParentEdges().back();
        }
        // compare elements
        if ((nextChild->getParentJunctions().size() > 0) && (getParentJunctions().size() > 0)) {
            return ("Junction '" + nextChild->getParentJunctions().front()->getID() +
                    "' is not consecutive with junction '" + getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && (getParentJunctions().size() > 0)) {
            return ("edge '" + nextEdge->getID() + "' is not consecutive with junction '" + getParentJunctions().back()->getID() + "'");
        } else if (lastEdge && (nextChild->getParentJunctions().size() > 0)) {
            return ("edge '" + lastEdge->getID() + "' is not consecutive with junction '" + nextChild->getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && lastEdge && (nextEdge != lastEdge)) {
            return "Edge '" + lastEdge->getID() + "' is not consecutive with edge '" + nextEdge->getID() + "'";
        }
    }
    // undefined problem
    return "undefined problem";
}


void
GNEDemandElement::drawJunctionLine(const GNEDemandElement* element) const {
    // get two points
    const Position posA = element->getParentJunctions().front()->getPositionInView();
    const Position posB = element->getParentJunctions().back()->getPositionInView();
    const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
    const double len = posA.distanceTo2D(posB);
    // push draw matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, element->getType() + 0.1);
    // set trip color
    GLHelper::setColor(RGBColor::RED);
    // draw line
    GLHelper::drawBoxLine(posA, rot, len, 0.25);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEDemandElement::drawStackLabel(const std::string& element, const Position& position, const double rotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to  top
    glTranslated(position.x(), position.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW);
    glRotated(rotation, 0, 0, -1);
    glTranslated((width * exaggeration * 0.5) + (0.35 * exaggeration), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor(0, 128, 0));
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText(element + "s stacked: " + toString(myStackedLabelNumber), Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::WHITE, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEDemandElement::drawFlowLabel(const Position& position, const double rotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to  bot
    glTranslated(position.x(), position.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW);
    glRotated(rotation, 0, 0, -1);
    glTranslated(-1 * ((width * 0.5 * exaggeration) + (0.35 * exaggeration)), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor::CYAN);
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText("Flow", Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::BLACK, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEDemandElement::replaceDemandParentEdges(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEDemandElement::replaceDemandParentLanes(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEDemandElement::replaceFirstParentJunction(const std::string& value) {
    std::vector<GNEJunction*> parentJunctions = getParentJunctions();
    parentJunctions[0] = myNet->getAttributeCarriers()->retrieveJunction(value);
    // replace parent junctions
    replaceParentElements(this, parentJunctions);
}


void
GNEDemandElement::replaceLastParentJunction(const std::string& value) {
    std::vector<GNEJunction*> parentJunctions = getParentJunctions();
    parentJunctions[(int)parentJunctions.size() - 1] = myNet->getAttributeCarriers()->retrieveJunction(value);
    // replace parent junctions
    replaceParentElements(this, parentJunctions);
}


void
GNEDemandElement::replaceFirstParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[0] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEDemandElement::replaceLastParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[(int)parentEdges.size() - 1] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEDemandElement::replaceAdditionalParent(SumoXMLTag tag, const std::string& value) {
    std::vector<GNEAdditional*> parentAdditionals = getParentAdditionals();
    parentAdditionals[0] = myNet->getAttributeCarriers()->retrieveAdditional(tag, value);
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEDemandElement::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->getAttributeCarriers()->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


void
GNEDemandElement::setVTypeDistributionParent(const std::string& value) {
    std::vector<GNEDemandElement*> parents;
    if (value.size() > 0) {
        parents.push_back(myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, value));
    }
    replaceParentElements(this, parents);
}


bool
GNEDemandElement::checkChildDemandElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError(StringUtils::format("Calling non-implemented function checkChildDemandElementRestriction during saving of %. It muss be reimplemented in child class", getTagStr()));
}


std::vector<GNEDemandElement::EdgeStopIndex>
GNEDemandElement::getEdgeStopIndex() const {
    std::vector<GNEDemandElement::EdgeStopIndex> edgeStopIndex;
    // first check that this stop has parent
    if (getParentDemandElements().size() > 0) {
        // get path edges depending of parent
        std::vector<GNEEdge*> pathEdges;
        // get parent demand element
        const auto parent = getParentDemandElements().front();
        // continue depending of parent
        if (parent->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
            pathEdges = parent->getParentEdges();
        } else if (parent->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
            // get route edges
            if (parent->getParentDemandElements().size() > 1) {
                pathEdges = parent->getParentDemandElements().at(1)->getParentEdges();
            }
        } else if (parent->getTagProperty().hasEmbeddedRoute()) {
            // get embedded route edges
            pathEdges = parent->getChildDemandElements().front()->getParentEdges();
        } else {
            // get last parent edge
            const auto lastEdge = parent->getParentEdges().back();
            bool stop = false;
            const auto& pathElementSegments = myNet->getPathManager()->getPathElementSegments(parent);
            // extract all edges from pathElement parent
            for (auto it = pathElementSegments.begin(); (it != pathElementSegments.end()) && !stop; it++) {
                if ((*it)->getLane()) {
                    pathEdges.push_back((*it)->getLane()->getParentEdge());
                    // stop if path correspond to last edge
                    if (pathEdges.back() == lastEdge) {
                        stop = true;
                    }
                }
            }
        }
        // get all parent's stops and waypoints sorted by position
        for (const auto& demandElement : parent->getChildDemandElements()) {
            if (demandElement->getTagProperty().isStop() || demandElement->getTagProperty().isWaypoint()) {
                // get stop/waypoint edge
                GNEEdge* edge = nullptr;
                if (demandElement->getParentAdditionals().size() > 0) {
                    edge = demandElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
                } else {
                    edge = demandElement->getParentLanes().front()->getParentEdge();
                }
                // check if add a new edgeStopIndex or update last
                if ((edgeStopIndex.size() > 0) && (edgeStopIndex.back().edge == edge)) {
                    edgeStopIndex.back().stops.push_back(demandElement);
                } else {
                    edgeStopIndex.push_back(EdgeStopIndex(edge, demandElement));
                }
            }
        }
        // declare index for current stop
        int currentEdgeStopIndex = 0;
        for (int i = 0; (i < (int)pathEdges.size()) && (currentEdgeStopIndex < (int)edgeStopIndex.size()); i++) {
            // check if current edge stop index is in the path
            if (edgeStopIndex[currentEdgeStopIndex].edge == pathEdges.at(i)) {
                edgeStopIndex[currentEdgeStopIndex].stopIndex = i;
                currentEdgeStopIndex++;
            } else {
                // check if edge exist in the rest of the path
                bool next = false;
                for (int j = (i + 1); j < (int)pathEdges.size(); j++) {
                    if (edgeStopIndex[currentEdgeStopIndex].edge == pathEdges.at(j)) {
                        next = true;
                    }
                }
                if (!next) {
                    // ignore current stops (because is out of path)
                    currentEdgeStopIndex++;
                }
            }
        }
    }
    // sort stops by position
    for (auto& edgeStop : edgeStopIndex) {
        if (edgeStop.stops.size() > 1) {
            // copy all stops to a map to sort it by endPos
            std::map<double, std::vector<GNEDemandElement*> > sortedStops;
            for (const auto& stop : edgeStop.stops) {
                if (sortedStops.count(stop->getAttributeDouble(SUMO_ATTR_ENDPOS)) == 0) {
                    sortedStops[stop->getAttributeDouble(SUMO_ATTR_ENDPOS)] = {stop};
                } else {
                    sortedStops[stop->getAttributeDouble(SUMO_ATTR_ENDPOS)].push_back(stop);
                }
            }
            // update stops with sorted stops
            edgeStop.stops.clear();
            for (const auto& sortedStop : sortedStops) {
                edgeStop.stops.insert(edgeStop.stops.end(), sortedStop.second.begin(), sortedStop.second.end());
            }
        }
    }
    return edgeStopIndex;
}


void
GNEDemandElement::setFlowParameters(SUMOVehicleParameter* vehicleParameters, const SumoXMLAttr attribute, const bool value) {
    // modify parameters depending of given Flow attribute
    if (value) {
        switch (attribute) {
            case SUMO_ATTR_END:
                vehicleParameters->parametersSet |= VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                vehicleParameters->parametersSet |= VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                vehicleParameters->parametersSet |= VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                vehicleParameters->parametersSet |= VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                vehicleParameters->parametersSet |= VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                vehicleParameters->parametersSet |= VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    } else {
        switch (attribute) {
            case SUMO_ATTR_END:
                vehicleParameters->parametersSet &= ~VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                vehicleParameters->parametersSet &= ~VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                vehicleParameters->parametersSet &= ~VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                vehicleParameters->parametersSet &= ~VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                vehicleParameters->parametersSet &= ~VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                vehicleParameters->parametersSet &= ~VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    }
}


void
GNEDemandElement::adjustDefaultFlowAttributes(SUMOVehicleParameter* vehicleParameters) {
    // first check that this demand element is a flow
    if (myTagProperty.isFlow()) {
        // end
        if ((vehicleParameters->parametersSet & VEHPARS_END_SET) == 0) {
            setAttribute(SUMO_ATTR_END, myTagProperty.getDefaultValue(SUMO_ATTR_END));
        }
        // number
        if ((vehicleParameters->parametersSet & VEHPARS_NUMBER_SET) == 0) {
            setAttribute(SUMO_ATTR_NUMBER, myTagProperty.getDefaultValue(SUMO_ATTR_NUMBER));
        }
        // vehicles/person/container per hour
        if (((vehicleParameters->parametersSet & VEHPARS_PERIOD_SET) == 0) &&
                ((vehicleParameters->parametersSet & VEHPARS_POISSON_SET) == 0) &&
                ((vehicleParameters->parametersSet & VEHPARS_VPH_SET) == 0)) {
            setAttribute(SUMO_ATTR_PERIOD, myTagProperty.getDefaultValue(SUMO_ATTR_PERIOD));
        }
        // probability
        if ((vehicleParameters->parametersSet & VEHPARS_PROB_SET) == 0) {
            setAttribute(SUMO_ATTR_PROB, myTagProperty.getDefaultValue(SUMO_ATTR_PROB));
        }
        // poisson
        if (vehicleParameters->repetitionOffset < 0) {
            toggleAttribute(SUMO_ATTR_PERIOD, false);
            toggleAttribute(GNE_ATTR_POISSON, true);
            setAttribute(GNE_ATTR_POISSON, time2string(vehicleParameters->repetitionOffset * -1));
        }
    }
}


void
GNEDemandElement::buildMenuCommandRouteLength(GUIGLObjectPopupMenu* ret) const {
    std::vector<GNEEdge*> edges;
    if (myTagProperty.isRoute()) {
        edges = getParentEdges();
    } else if ((getParentDemandElements().size() > 1) && getParentDemandElements().at(1)->getTagProperty().isRoute()) {
        edges = getParentDemandElements().at(1)->getParentEdges();
    } else if ((getChildDemandElements().size() > 0) && getChildDemandElements().front()->getTagProperty().isRoute()) {
        edges = getChildDemandElements().front()->getParentEdges();
    } else if (getParentEdges().size() > 0) {
        edges = getParentEdges();
    }
    // calculate path
    const auto path = myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getVClass(), edges);
    // check path size
    if (path.size() > 0) {
        double length = 0;
        for (const auto& edge : path) {
            length += edge->getNBEdge()->getFinalLength();
        }
        for (int i = 0; i < ((int)path.size() - 1); i++) {
            length += path.at(i)->getLanes().front()->getLane2laneConnections().getLane2laneGeometry(path.at(i + 1)->getLanes().front()).getShape().length();
        }
        GUIDesigns::buildFXMenuCommand(ret, "Route length: " + toString(length), nullptr, ret, MID_COPY_NAME);
    }
}

/****************************************************************************/
