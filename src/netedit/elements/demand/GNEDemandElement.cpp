/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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

GNEDemandElement::GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEShape*>& shapeParents,
                                   const std::vector<GNETAZElement*>& TAZElementParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, id),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(GNEPathManager::PathElement::Options::DEMAND_ELEMENT),
    myStackedLabelNumber(0) {
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEShape*>& shapeParents,
                                   const std::vector<GNETAZElement*>& TAZElementParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, demandElementParent->getID()),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(GNEPathManager::PathElement::Options::DEMAND_ELEMENT),
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


const GNEGeometry::Geometry&
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


bool
GNEDemandElement::isDemandElementValid() const {
    return true;
}


std::string
GNEDemandElement::getDemandElementProblem() const {
    return "";
}


void
GNEDemandElement::fixDemandElementProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEDemandElement::openDemandElementDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have an demand element dialog");
}


std::string
GNEDemandElement::getBegin() const {
    throw InvalidArgument(getTagStr() + " doesn't have an begin time");
}


GUIGLObjectPopupMenu*
GNEDemandElement::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
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
                return previousPersonPlan->getParentAdditionals().front()->getAttributeDouble(GNE_ATTR_CENTER);
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
            return getParentAdditionals().front()->getAttributeDouble(GNE_ATTR_CENTER);
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

// ---------------------------------------------------------------------------
// GNEDemandElement - protected methods
// ---------------------------------------------------------------------------

bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myNet->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
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
        GNEGeometry::Geometry personPlanGeometry;
        // only calculate geometry if segment is valid
        if (segment->isValid()) {
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
        }
        // get color
        const RGBColor& pathColor = drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : personPlanColor;
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront);
        // Set color
        GLHelper::setColor(pathColor);
        // draw geometry
        GNEGeometry::drawGeometry(myNet->getViewNet(), personPlanGeometry, pathWidth);
        // Pop last matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // Pop name
        glPopName();
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
                glPushMatrix();
                // Start with the drawing of the area traslating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to pos and move to upper using GLO_PERSONTRIP (to avoid overlapping)
                glTranslated(geometryEndPos.x(), geometryEndPos.y(), 0);
                // Set person plan color
                GLHelper::setColor(pathColor);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                // pop draw matrix
                glPopMatrix();
            }
        }
        // check if we have to draw a red line to the next segment
        if (segment->getNextSegment()) {
            // push draw matrix
            glPushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // Set red color
            GLHelper::setColor(RGBColor::RED);
            // get firstPosition (last position of current lane shape)
            const Position firstPosition = lane->getLaneShape().back();
            // get lastPosition (first position of next lane shape)
            const Position arrivalPos = segment->getNextSegment()->getPathElement()->getPathElementArrivalPos();
            // draw box line
            GLHelper::drawBoxLine(arrivalPos,
                                  RAD2DEG(firstPosition.angleTo2D(arrivalPos)) - 90,
                                  firstPosition.distanceTo2D(arrivalPos), .05);
            // pop draw matrix
            glPopMatrix();
        }
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || dottedElement) {
            // declare trim geometry to draw
            const GNEGeometry::DottedGeometry pathDottedGeometry((segment->isFirstSegment() || segment->isLastSegment()) ? GNEGeometry::DottedGeometry(s, personPlanGeometry.getShape(), false) : lane->getDottedLaneGeometry());
            // draw inspected dotted contour
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, pathDottedGeometry, pathWidth, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, pathDottedGeometry, pathWidth, segment->isFirstSegment(), segment->isLastSegment());
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
    if (drawPlan && myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, fromLane, toLane, myTagProperty.getTag())) {
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
        glPushName(getGlID());
        // push a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront);
        // check if draw lane2lane connection or a red line
        if (fromLane->getLane2laneConnections().exist(toLane)) {
            // obtain lane2lane geometry
            const GNEGeometry::Geometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
            // Set person plan color
            GLHelper::setColor(color);
            // draw lane2lane
            GNEGeometry::drawGeometry(myNet->getViewNet(), lane2laneGeometry, pathWidth);
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // draw line between end of first shape and first position of second shape
            GLHelper::drawBoxLines({fromLane->getLaneShape().back(), toLane->getLaneShape().front()}, (0.5 * pathWidth));
        }
        // Pop last matrix
        glPopMatrix();
        // Pop name
        glPopName();
        // check if shape dotted contour has to be drawn
        if (fromLane->getLane2laneConnections().exist(toLane) && (s.drawDottedContour() || dottedElement)) {
            // draw lane2lane inspected dotted geometry
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), pathWidth, false, false);
            }
            // draw lane2lane front dotted geometry
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), pathWidth, false, false);
            }
        }
    }
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
GNEDemandElement::replaceFirstParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[0] = myNet->retrieveEdge(value);
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
    parentEdges[(int)parentEdges.size() - 1] = myNet->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEDemandElement::replaceAdditionalParent(SumoXMLTag tag, const std::string& value) {
    std::vector<GNEAdditional*> parentAdditionals = getParentAdditionals();
    parentAdditionals[0] = myNet->retrieveAdditional(tag, value);
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEDemandElement::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


bool
GNEDemandElement::checkChildDemandElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildDemandElementRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}

/****************************************************************************/
