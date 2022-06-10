/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDemandElement.h"


// ===========================================================================
// static defintions
// ===========================================================================
const double GNEDemandElement::myPersonPlanArrivalPositionDiameter = SUMO_const_halfLaneWidth;

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElement::GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, id),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(options),
    myStackedLabelNumber(0) {
    // check if is template
    myIsTemplate = (id == "");
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, demandElementParent->getID()),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(options),
    myStackedLabelNumber(0) {
}


GNEDemandElement::~GNEDemandElement() {}


void
GNEDemandElement::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // currently there isn't demand elements with removable geometry points
}


const std::string&
GNEDemandElement::getID() const {
    return getMicrosimID();
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


std::vector<GNEEdge*>
GNEDemandElement::getViaEdges() const {
    std::vector<GNEEdge*> middleEdges;
    // there are only middle edges if there is more than two edges
    if (getParentEdges().size() > 2) {
        // reserve middleEdges
        middleEdges.reserve(getParentEdges().size() - 2);
        // iterate over second and previous last parent edge
        for (auto i = (getParentEdges().begin() + 1); i != (getParentEdges().end() - 1); i++) {
            middleEdges.push_back(*i);
        }
    }
    return middleEdges;
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
        GUIDesigns::buildFXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
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
    } else {
        return getAttributePosition(SUMO_ATTR_ARRIVALPOS);
    }
}


bool
GNEDemandElement::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
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
    // get stops
    std::vector<GNEDemandElement*> stops;
    for (const auto& stop : getChildDemandElements()) {
        if (stop->getTagProperty().getTag() == SUMO_TAG_STOP_LANE) {
            stops.push_back(stop);
        }
    }
    // check stops
    if (stops.empty()) {
        return stops;
    } else {
        // get sorted stops
        std::vector<const GNEDemandElement*> sortedStops;
        // continue depending of route
        if (getTagProperty().getTag() == SUMO_TAG_ROUTE) {
            sortedStops = getSortedStops(getParentEdges());
        } else if (getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
            sortedStops = getSortedStops(getChildDemandElements().front()->getParentEdges());
        }
        // iterate over sortedStops
        for (const auto& sortedStop : sortedStops) {
            const auto it = std::find(stops.begin(), stops.end(), sortedStop);
            if (it != stops.end()) {
                stops.erase(it);
            }
        }
        // return stops not found in sortedStops
        return stops;
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
    if (drawPlan && myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myTagProperty.getTag())) {
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
        // Pop name
        GLHelper::popName();
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
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || isAttributeCarrierSelected());
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
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || isAttributeCarrierSelected());
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // declare trim geometry to draw
            const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? personPlanGeometry.getShape() : lane->getLaneShape();
            // draw inspected dotted contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
    // draw person parent if this is the edge first edge and this is the first plan
    if ((getFirstPathLane()->getParentEdge() == lane->getParentEdge()) &&
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
    if (drawPlan && myNet->getPathManager()->getPathDraw()->drawPathGeometry(false, fromLane, toLane, myTagProperty.getTag())) {
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
        if (fromLane->getLane2laneConnections().exist(toLane) && (dottedElement)) {
            // draw lane2lane inspected dotted geometry
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // draw lane2lane front dotted geometry
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
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
        // compare both edges
        if (previousEdge != firstEdge) {
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
        if (nextEdge != lastEdge) {
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
        // compare both edges
        if (previousEdge && firstEdge && (previousEdge != firstEdge)) {
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
        // compare both edges
        if (nextEdge && lastEdge && (nextEdge != lastEdge)) {
            return "Edge '" + lastEdge->getID() + "' is not consecutive with edge '" + nextEdge->getID() + "'";
        }
    }
    // undefined problem
    return "undefined problem";
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
GNEDemandElement::replaceMiddleParentEdges(const std::string& value, const bool updateChildReferences) {
    std::vector<GNEEdge*> middleEdges = parse<std::vector<GNEEdge*> >(getNet(), value);
    middleEdges.insert(middleEdges.begin(), getParentEdges().front());
    middleEdges.push_back(getParentEdges().back());
    // check if we have to update references in all childs, or simply update parent edges vector
    if (updateChildReferences) {
        // replace parent edges
        replaceParentElements(this, middleEdges);
    } else {
        myHierarchicalContainer.setParents<std::vector<GNEEdge*> >(middleEdges);
    }
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
    throw ProcessError("Calling non-implemented function checkChildDemandElementRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


GNEDemandElement::SortedStops::SortedStops(GNEEdge* edge_) :
    edge(edge_) {
}


void
GNEDemandElement::SortedStops::addStop(const GNEDemandElement* stop) {
    // create first pair
    auto posIndexPair = std::make_pair(stop->getAttributeDouble(SUMO_ATTR_ENDPOS), stop->getAttributeDouble(SUMO_ATTR_INDEX));
    myStops.push_back(std::make_pair(posIndexPair, stop));
    // sort stops
    std::sort(myStops.begin(), myStops.end());
}


std::vector<const GNEDemandElement*>
GNEDemandElement::getSortedStops(const std::vector<GNEEdge*>& edges) const {
    std::vector<GNEDemandElement*> stops;
    // get stops
    for (const auto& stop : getChildDemandElements()) {
        if (stop->getTagProperty().isStop()) {
            stops.push_back(stop);
        }
    }
    // create SortedStops
    std::vector<SortedStops> sortedStops;
    for (const auto& edge : edges) {
        sortedStops.push_back(SortedStops(edge));
    }
    // iterate over all stops and insert it in sortedStops
    for (const auto& stop : stops) {
        bool stopLoop = false;
        // iterate over sortedStops
        for (auto it = sortedStops.begin(); (it != sortedStops.end()) && !stopLoop; it++) {
            if ((stop->getParentAdditionals().size() > 0) && (stop->getParentAdditionals().front()->getParentLanes().front()->getParentEdge() == it->edge)) {
                it->addStop(stop);
                stopLoop = true;
            } else if ((stop->getParentLanes().size() > 0) && (stop->getParentLanes().front()->getParentEdge() == it->edge)) {
                it->addStop(stop);
                stopLoop = true;
            }
        }
    }
    // finally return sorted stops
    std::vector<const GNEDemandElement*> solution;
    for (const auto& sortedStop : sortedStops) {
        for (const auto& stop : sortedStop.myStops) {
            solution.push_back(stop.second);
        }
    }
    return solution;
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
            toogleAttribute(SUMO_ATTR_PERIOD, false);
            toogleAttribute(GNE_ATTR_POISSON, true);
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
