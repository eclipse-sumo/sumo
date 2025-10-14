/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveElementPlan.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/demand/GNEDemandElementPlan.h>
#include <netedit/elements/moving/GNEMoveElementVehicle.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElementPlan.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementPlan::GNEMoveElementPlan(GNEDemandElement* planElement, double& departPos) :
    GNEMoveElement(planElement),
    myPlanElement(planElement),
    myArrivalPosition(departPos) {
}


GNEMoveElementPlan::~GNEMoveElementPlan() {}


GNEMoveOperation*
GNEMoveElementPlan::getMoveOperation() {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // only move personTrips defined over edges
    if (tagProperty->planToEdge() || tagProperty->planConsecutiveEdges() || tagProperty->planEdge()) {
        // get geometry end pos
        const Position geometryEndPos = myPlanElement->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
        // calculate circle width squared
        const double circleWidthSquared = GNEMoveElementVehicle::arrivalPositionDiameter * GNEMoveElementVehicle::arrivalPositionDiameter;
        // check if we clicked over a geometry end pos
        if (myPlanElement->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
            // continue depending of parent edges
            if (myPlanElement->getParentEdges().size() > 0) {
                return new GNEMoveOperation(this, myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(myPlanElement->getVClass()), myArrivalPosition, false);
            } else {
                return new GNEMoveOperation(this, myPlanElement->getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(myPlanElement->getVClass()), myArrivalPosition, false);
            }
        }
    }
    return nullptr;
}


std::string
GNEMoveElementPlan::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementPlan::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementPlan::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementPlan::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementPlan::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementPlan::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementPlan::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementPlan::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementPlan::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newLastPos;
    // update geometry
    myPlanElement->updateGeometry();
}


void
GNEMoveElementPlan::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(myPlanElement, TLF("arrivalPos of %", myPlanElement->getTagStr()));
    // now adjust start position
    myPlanElement->setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
