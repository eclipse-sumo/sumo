/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/network/GNEEdge.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEDemandElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myViewNet(viewNet),
    myStackedLabelNumber(0) {
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, demandElementParent->generateChildID(tag)),
    GNEAttributeCarrier(tag),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myViewNet(viewNet) {
}


GNEDemandElement::~GNEDemandElement() {}


const std::string& 
GNEDemandElement::getID() const {
    return getMicrosimID();
}


std::string
GNEDemandElement::generateChildID(SumoXMLTag childTag) {
    int counter = (int)getChildDemandElements().size();
    while (myViewNet->getNet()->retrieveDemandElement(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


const GNEGeometry::Geometry&
GNEDemandElement::getDemandElementGeometry() {
    return myDemandElementGeometry;
}


const GNEGeometry::SegmentGeometry&
GNEDemandElement::getDemandElementSegmentGeometry() const {
    return myDemandElementSegmentGeometry;
}


void
GNEDemandElement::updateDemandElementGeometry(const GNELane* lane, const double posOverLane) {
    myDemandElementGeometry.updateGeometry(lane, posOverLane);
}


void
GNEDemandElement::updateDemandElementStackLabel(const int stack) {
    myStackedLabelNumber = stack;
}


const GNEGeometry::SegmentGeometry&
GNEDemandElement::getDemandElementSegmentSpreadGeometry() const {
    return mySpreadSegmentGeometry;
}


void
GNEDemandElement::updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane) {
    mySpreadGeometry.updateGeometry(lane, posOverLane);
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


GNEViewNet*
GNEDemandElement::getViewNet() const {
    return myViewNet;
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
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
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
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


GNELane*
GNEDemandElement::getFirstAllowedVehicleLane() const {
    // first check if current demand element has parent edges
    if (myTagProperty.getTag() == SUMO_TAG_WALK_ROUTE) {
        // use route edges
        return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
    } else if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == SUMO_TAG_ROUTEFLOW)) {
        // check if vehicle use a embedded route
        if (getParentDemandElements().size() == 2) {
            return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else if (getChildDemandElements().size() > 0) {
            return getChildDemandElements().front()->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if (getParentEdges().size() > 0) {
        // obtain Lane depending of attribute "departLane"
        if (myTagProperty.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
            // obtain depart lane
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
        } else if (myTagProperty.isRide()) {
            // special case for rides
            return getParentEdges().front()->getLaneByDisallowedVClass(getVClass());
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
    // first check if current demand element has parent edges
    if (myTagProperty.getTag() == SUMO_TAG_WALK_ROUTE) {
        // use route edges
        return getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass());
    } else if (getParentEdges().size() > 0) {
        if ((myTagProperty.getTag() == SUMO_TAG_PERSONTRIP_BUSSTOP) ||
                (myTagProperty.getTag() == SUMO_TAG_WALK_BUSSTOP) ||
                (myTagProperty.getTag() == SUMO_TAG_RIDE_BUSSTOP)) {
            // return busStop lane
            return getParentAdditionals().front()->getParentLanes().front();
        } else if (myTagProperty.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
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
        } else if (myTagProperty.isRide()) {
            // special case for rides
            return getParentEdges().back()->getLaneByDisallowedVClass(getVClass());
        } else {
            // in other case, always return the first allowed
            return getParentEdges().back()->getLaneByAllowedVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


void
GNEDemandElement::calculatePersonPlanLaneStartEndPos(double& startPos, double& endPos) const {
    // obtain pointer to current busStop
    GNEAdditional* busStop = getParentAdditionals().size() > 0 ? getParentAdditionals().front() : nullptr;
    // declare pointers for previous elements
    GNEAdditional* previousBusStop = nullptr;
    GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    // declare pointer to next person plan
    GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
    // obtain departlane throught previous element
    if (previousPersonPlan && (previousPersonPlan->getParentAdditionals().size() > 0)) {
        // set previous busStop
        previousBusStop = previousPersonPlan->getParentAdditionals().front();
    }
    // adjust startPos depending of previous busStop
    if (previousBusStop) {
        startPos = previousBusStop->getAttributeDouble(SUMO_ATTR_ENDPOS);
    } else if (previousPersonPlan) {
        // check if previous element is a stop or another person plan (walk, ride, trip...)
        if (previousPersonPlan->getTagProperty().isPersonStop()) {
            startPos = previousPersonPlan->getAttributeDouble(SUMO_ATTR_ENDPOS);
        } else {
            startPos = previousPersonPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        }
    } else {
        // if this is the first person plan, use departPos of pedestrian
        startPos = getParentDemandElements().front()->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
    }
    // adjust endPos depending of next busStop
    if (busStop) {
        endPos = busStop->getAttributeDouble(SUMO_ATTR_STARTPOS);
    } else if (nextPersonPlan && nextPersonPlan->getTagProperty().isPersonStop()) {
        endPos = nextPersonPlan->getAttributeDouble(SUMO_ATTR_STARTPOS);
    } else {
        // if this is the last element, simply use arrival position
        endPos = getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    }
}


void
GNEDemandElement::calculatePersonPlanPositionStartEndPos(Position& startPos, Position& endPos) const {
    // obtain previous demand element
    GNEDemandElement* previousDemandElmement = getParentDemandElements().front()->getPreviousChildDemandElement(this);
    if (previousDemandElmement) {
        // update startPos
        if ((previousDemandElmement->getParentAdditionals().size() > 0) &&
                (previousDemandElmement->getParentAdditionals().front()->getAdditionalGeometry().getShape().size() > 0)) {
            // Previous demand element ends in an busStop
            startPos = previousDemandElmement->getParentAdditionals().front()->getAdditionalGeometry().getShape().back();
        } else if (previousDemandElmement->getTagProperty().isPersonStop() && (previousDemandElmement->getDemandElementGeometry().getShape().size() > 0)) {
            // Previous demand element ends in an Stop
            startPos = previousDemandElmement->getDemandElementGeometry().getShape().back();
        } else if ((previousDemandElmement->getDemandElementSegmentGeometry().size() > 0) &&
                   (previousDemandElmement->getDemandElementSegmentGeometry().back().getShape().size() > 0)) {
            // add last shape segment of previous segment geometry
            startPos = previousDemandElmement->getDemandElementSegmentGeometry().back().getShape().back();
        }
    }
    // check if demand element ends in an busStop
    if ((getParentAdditionals().size() > 0) && (getParentAdditionals().front()->getAdditionalGeometry().getShape().size() > 0)) {
        endPos = getParentAdditionals().front()->getAdditionalGeometry().getShape().front();
    } else {
        // obtain next demand element
        GNEDemandElement* nextDemandElmement = getParentDemandElements().front()->getNextChildDemandElement(this);
        if (nextDemandElmement) {
            // update end pos
            if (nextDemandElmement->getTagProperty().isPersonStop() && (nextDemandElmement->getDemandElementGeometry().getShape().size() > 0)) {
                // previous demand element ends in an Stop
                endPos = nextDemandElmement->getDemandElementGeometry().getShape().front();
            }
        }
    }
}


bool
GNEDemandElement::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEDemandElement::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().isCurrentSupermodeDemand())) {
        return true;
    } else {
        return false;
    }
}


bool
GNEDemandElement::checkChildDemandElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildDemandElementRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


/****************************************************************************/
