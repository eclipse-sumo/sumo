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
    GNEPathManager::PathElement(GNEPathManager::PathElement::DEMAND_ELEMENT),
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
    GNEPathManager::PathElement(GNEPathManager::PathElement::DEMAND_ELEMENT),
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


bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myNet->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


GNELane*
GNEDemandElement::getFirstAllowedVehicleLane() const {
    // check demand element
    if (!myTagProperty.isVehicle()) {
        throw ProcessError("Only allowed for vehicles");
    } else if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        // check if vehicle use a embedded route
        if (getParentDemandElements().size() == 2) {
            return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if ((myTagProperty.getTag() == GNE_TAG_VEHICLE_WITHROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        if (getChildDemandElements().size() > 0) {
            return getChildDemandElements().front()->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if (getParentEdges().size() > 0) {
        if (myTagProperty.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
            // obtain Lane depending of attribute "departLane"
            std::string departLane = getAttribute(SUMO_ATTR_DEPARTLANE);
            //  check depart lane
            if ((departLane == "random") || (departLane == "free") || (departLane == "allowed") || (departLane == "best") || (departLane == "first")) {
                return getParentEdges().front()->getLaneByAllowedVClass(getVClass());
            }
            // obtain index
            const int departLaneIndex = parse<int>(getAttribute(SUMO_ATTR_DEPARTLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((departLaneIndex >= 0) && (departLaneIndex < getParentEdges().front()->getNBEdge()->getNumLanes())) {
                return getParentEdges().front()->getLanes().at(departLaneIndex);
            } else {
                return nullptr;
            }
        } else {
            // in other case, always return the first allowed
            return getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


GNELane*
GNEDemandElement::getLastAllowedVehicleLane() const {
    // check demand element
    if (!myTagProperty.isVehicle()) {
        throw ProcessError("Only allowed for vehicles");
    } else if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        // check if vehicle use a embedded route
        if (getParentDemandElements().size() == 2) {
            return getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if ((myTagProperty.getTag() == GNE_TAG_VEHICLE_WITHROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        if (getChildDemandElements().size() > 0) {
            return getChildDemandElements().front()->getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if (getParentEdges().size() > 0) {
        if (myTagProperty.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
            // obtain Lane depending of attribute "arrivalLane"
            std::string arrivalLane = getAttribute(SUMO_ATTR_ARRIVALLANE);
            //  check depart lane
            if (arrivalLane == "current") {
                return getParentEdges().back()->getLaneByAllowedVClass(getVClass());
            }
            // obtain index
            const int arrivalLaneIndex = parse<int>(getAttribute(SUMO_ATTR_ARRIVALLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((arrivalLaneIndex >= 0) && (arrivalLaneIndex < getParentEdges().back()->getNBEdge()->getNumLanes())) {
                return getParentEdges().back()->getLanes().at(arrivalLaneIndex);
            } else {
                return nullptr;
            }
        } else {
            // in other case, always return the first allowed
            return getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


const Position
GNEDemandElement::getBeginPosition(const double pedestrianDepartPos) const {
    if (myTagProperty.isPersonStop()) {
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
    if (myTagProperty.isPersonStop()) {
        if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this)) {
            return true;
        }
    } else if (myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        return true;
    } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentDemandElements().front())) {
        return true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedPerson() == getParentDemandElements().front()) {
        return true;
    } else if (!myNet->getViewNet()->getInspectedAttributeCarriers().empty() && myNet->getViewNet()->getInspectedAttributeCarriers().front()->getTagProperty().isPersonPlan() &&
        (myNet->getViewNet()->getInspectedAttributeCarriers().front()->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT))) {
        return true;
    }
    return false;
}


GNELane*
GNEDemandElement::getFirstPersonPlanLane() const {
    switch (myTagProperty.getTag()) {
        // check first edge elements
        case GNE_TAG_PERSONTRIP_EDGE:
        case GNE_TAG_WALK_EDGE:
        case GNE_TAG_PERSONTRIP_BUSSTOP:
        case GNE_TAG_WALK_BUSSTOP:
        case GNE_TAG_WALK_EDGES:
        case GNE_TAG_PERSONSTOP_EDGE:
            return getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        // check rides
        case GNE_TAG_RIDE_EDGE:
        case GNE_TAG_RIDE_BUSSTOP:
            return getParentEdges().front()->getLaneByDisallowedVClass(getVClass());
        // check personStop (bus)
        case GNE_TAG_PERSONSTOP_BUSSTOP:
            return getParentAdditionals().front()->getParentLanes().front();
        // check route walk
        case GNE_TAG_WALK_ROUTE:
            return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        default:
            throw ProcessError("Invalid person plan tag");
    }
}


GNELane* 
GNEDemandElement::getLastPersonPlanLane() const {
    switch (myTagProperty.getTag()) {
        // check edge elements
        case GNE_TAG_PERSONTRIP_EDGE:
        case GNE_TAG_WALK_EDGE:
        case GNE_TAG_WALK_EDGES:
        case GNE_TAG_PERSONSTOP_EDGE:
            return getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        // check ride
        case GNE_TAG_RIDE_EDGE:
            return getParentEdges().back()->getLaneByDisallowedVClass(getVClass());
        // check busStops elements
        case GNE_TAG_PERSONTRIP_BUSSTOP:
        case GNE_TAG_RIDE_BUSSTOP:
        case GNE_TAG_WALK_BUSSTOP:
        case GNE_TAG_PERSONSTOP_BUSSTOP:
            return getParentAdditionals().front()->getParentLanes().front();
        // check route walks
        case GNE_TAG_WALK_ROUTE:
            return getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        default:
            throw ProcessError("Invalid person plan tag");
    }
}


double
GNEDemandElement::getPersonPlanDepartValue() const {
    // get previous person Plan
    const GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    // check if this is the first person plan
    if (previousPersonPlan) {
        if (previousPersonPlan->getParentAdditionals().size() > 0) {
            // use busStop end
            return previousPersonPlan->getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
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
GNEDemandElement::getPersonPlanDepartPos() const {
    // get previous person Plan
    const GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    // check if this is the first person plan
    if (previousPersonPlan) {
        if (previousPersonPlan->getParentAdditionals().size() > 0) {
            // use busStop end
            return previousPersonPlan->getParentAdditionals().front()->getAdditionalGeometry().getShape().back();
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
GNEDemandElement::getPersonPlanArrivalValue() const {
    if(getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_STARTPOS);
    } else {
        return getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    }
}


Position 
GNEDemandElement::getPersonPlanArrivalPos() const {
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getAdditionalGeometry().getShape().front();
    } else {
        return getAttributePosition(SUMO_ATTR_ARRIVALPOS);
    }
}


void
GNEDemandElement::drawPersonPlanPartial(const GUIVisualizationSettings& s, const GNELane* lane, const double offsetFront, 
    const int options, const double personPlanWidth, const RGBColor& personPlanColor) const {
    /// @brief get person parent
    const GNEDemandElement* personParent = getParentDemandElements().front();
    // check if draw person plan element can be drawn
    if (drawPersonPlan()) {
        // get inspected attribute carriers
        const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
        // get inspected person plan
        const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (personPlanInspected == this) || (personPlanInspected == personParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, lane) * personPlanWidth * (duplicateWidth? 2 : 1);
        // get segment flags
        const bool firstSegment = (options & GNEPathManager::PathElement::Options::FIRST_SEGMENT) != 0;
        const bool lastSegment = (options & GNEPathManager::PathElement::Options::LAST_SEGMENT) != 0;
        // declare path geometry
        GNEGeometry::Geometry personPlanGeometry;
        // update pathGeometry depending of first and last segment
        if (firstSegment && lastSegment) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(), 
                getPersonPlanDepartValue(), getPersonPlanArrivalValue(),    // extrem positions
                getPersonPlanDepartPos(), getPersonPlanArrivalPos());       // extra positions
        } else if (firstSegment) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(), 
                getPersonPlanDepartValue(), -1,                 // extrem positions
                getPersonPlanDepartPos(), Position::INVALID);   // extra positions
        } else if (lastSegment) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(), 
                -1, getPersonPlanArrivalValue(),                // extrem positions
                Position::INVALID, getPersonPlanArrivalPos());  // extra positions
        } else {
            personPlanGeometry = lane->getLaneGeometry();
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
        if (lastSegment) {
            // calculate circle width
            const double circleRadius = (duplicateWidth ? myPersonPlanArrivalPositionDiameter : (myPersonPlanArrivalPositionDiameter / 2.0));
            const double circleWidth = circleRadius * MIN2((double)0.5, s.laneWidthExaggeration);
            const double circleWidthSquared = circleWidth * circleWidth;
            // get geometryEndPos
            const Position geometryEndPos = getPersonPlanArrivalPos();
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
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() ||
            myNet->getViewNet()->isAttributeCarrierInspected(this) ||
            (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            // declare trim geometry to draw
            const GNEGeometry::DottedGeometry pathDottedGeometry((firstSegment || lastSegment) ? GNEGeometry::DottedGeometry(s, personPlanGeometry.getShape(), false) : lane->getDottedLaneGeometry());
            // draw inspected dotted contour
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, pathDottedGeometry, pathWidth, firstSegment, lastSegment);
            }
            // draw front dotted contour
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, pathDottedGeometry, pathWidth, firstSegment, lastSegment);
            }
        }
    }
    // draw person parent if this is the edge first edge and this is the first plan
    if ((getFirstPersonPlanLane()->getParentEdge() == lane->getParentEdge()) && 
        (personParent->getChildDemandElements().front() == this)) {
        personParent->drawGL(s);
    }
}


void
GNEDemandElement::drawPersonPlanPartial(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const double offsetFront,
    const int options, const double personPlanWidth, const RGBColor& personPlanColor) const {
    // get inspected attribute carriers
    const auto& inspectedACs = myNet->getViewNet()->getInspectedAttributeCarriers();
    // get inspected person plan
    const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
    const GNEDemandElement* personParent = getParentDemandElements().front();
    // declare flag to enable or disable draw person plan
    bool drawPersonPlan = false;
    if (myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        // all person plan has to be drawn
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedPerson() == personParent) {
        // if person parent is locked, draw all their person plans children
        drawPersonPlan = true;
    } else if (personPlanInspected == personParent) {
        // draw if we're inspecting person parent
        drawPersonPlan = true;
    } else if (personPlanInspected == this) {
        // draw if we're inspecting this demand element
        drawPersonPlan = true;
    }
    // check if draw person plan elements can be drawn
    if (drawPersonPlan) {
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
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // draw lane2lane dotted geometry
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), pathWidth, false, false);
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
