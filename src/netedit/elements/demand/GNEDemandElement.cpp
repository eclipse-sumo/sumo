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

GNEDemandElement::GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEJunction*>& junctionParents,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNETAZElement*>& TAZElementParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEJunction*>& junctionChildren,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNETAZElement*>& TAZElementChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag, net),
    GNEHierarchicalParentElements(this, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, junctionChildren, edgeChildren, laneChildren, additionalChildren, shapeChildren, TAZElementChildren, demandElementChildren, genericDataChildren),
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
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEJunction*>& junctionChildren,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNETAZElement*>& TAZElementChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, demandElementParent->getID() + "_" + toString(tag)),
    GNEAttributeCarrier(tag, net),
    GNEHierarchicalParentElements(this, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, junctionChildren, edgeChildren, laneChildren, additionalChildren, shapeChildren, TAZElementChildren, demandElementChildren, genericDataChildren),
    myStackedLabelNumber(0) {
}


GNEDemandElement::~GNEDemandElement() {}


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
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
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
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myNet->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


GNELane*
GNEDemandElement::getFirstAllowedVehicleLane() const {
    // first check if current demand element has parent edges
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        // use route edges
        return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
    } else if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        // check if vehicle use a embedded route
        if (getParentDemandElements().size() == 2) {
            return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if ((myTagProperty.getTag() == GNE_TAG_VEHICLEWITHROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        if (getChildDemandElements().size() > 0) {
            return getChildDemandElements().front()->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        } else {
            return nullptr;
        }
    } else if (myTagProperty.personPlanStartBusStop()) {
        return getParentAdditionals().front()->getParentLanes().front();
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
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        // use route edges
        return getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass());
    } else if (myTagProperty.personPlanEndBusStop()) {
        // return busStop lane
        return getParentAdditionals().back()->getParentLanes().front();
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
GNEDemandElement::calculatePersonPlanLaneStartEndPos(double& startLanePos, double& endLanePos, Position &extraStartPosition, Position &extraEndPosition) const {
    // set default values
    startLanePos = -1;
    endLanePos = -1;
    extraStartPosition = Position::INVALID;
    extraEndPosition = Position::INVALID;
    // get previous person Plan
    const GNEDemandElement* previousPersonPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
    double endLanePosPreviousLane = 0;
    // obtain previous plan 
    if (previousPersonPlan) {
        if (previousPersonPlan->getTagProperty().personPlanEndEdge()) {
            // get arrival position
            endLanePosPreviousLane = previousPersonPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        } else {
            // get arrival pos (end busStop shape)
            endLanePosPreviousLane = previousPersonPlan->getParentAdditionals().back()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            extraStartPosition = previousPersonPlan->getParentAdditionals().back()->getAdditionalGeometry().getShape().back();
        }
    }
    // set lane start position
    if (myTagProperty.personPlanStartEdge()) {
        if (previousPersonPlan) {
            // use as startLanePos the endLanePosPreviousLane
            startLanePos = endLanePosPreviousLane;
            // obtain last allowed vehicle lane of previous person plan
            const GNELane *lastAllowedLanePrevious = previousPersonPlan->getLastAllowedVehicleLane();
            // check if both plans start in the same lane
            if (lastAllowedLanePrevious && (getFirstAllowedVehicleLane() != lastAllowedLanePrevious)) {
                extraStartPosition = lastAllowedLanePrevious->getLaneShape().positionAtOffset(startLanePos);
            }
        } else {
            // return pedestrian departPos
            startLanePos = getParentDemandElements().front()->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
        }
    } else if (myTagProperty.personPlanStartBusStop()) { 
        // use as startLanePos the busStop end position
        startLanePos = getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
        // use as extraEndPosition the end of first busStop shape
        extraStartPosition = getParentAdditionals().front()->getAdditionalGeometry().getShape().back();
    }
    // set lane end position
    if (myTagProperty.personPlanEndEdge()) {
        // use as endLane Position the arrival position
        endLanePos = getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    } else if (myTagProperty.personPlanEndBusStop()) { 
        // use as endLanePosition the busStop start position
        endLanePos = getParentAdditionals().back()->getAttributeDouble(SUMO_ATTR_STARTPOS);
        // use as extraEndPosition the begin of last busStop shape
        extraEndPosition = getParentAdditionals().back()->getAdditionalGeometry().getShape().front();
    }
    // now obtain lanes
}


bool
GNEDemandElement::checkChildDemandElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildDemandElementRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


/****************************************************************************/
