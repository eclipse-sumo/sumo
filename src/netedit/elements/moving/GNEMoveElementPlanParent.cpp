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
/// @file    GNEMoveElementPlanParent.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/

#include <foreign/fontstash/fontstash.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/moving/GNEMoveElement.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEMoveElementPlanParent.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementPlanParent::GNEMoveElementPlanParent(GNEDemandElement* planParent,
        double& departPos, DepartPosDefinition& departPosProcedure) :
    GNEMoveElement(planParent),
    myPlanParent(planParent),
    myDepartPos(departPos),
    myDepartPosProcedure(departPosProcedure) {
}


GNEMoveElementPlanParent::~GNEMoveElementPlanParent() {}


GNEMoveOperation*
GNEMoveElementPlanParent::getMoveOperation() {
    const auto firstContainerPlan = myPlanParent->getChildDemandElements().front();
    // check first person plan
    if (firstContainerPlan->getTagProperty()->isPlanStopPerson()) {
        return nullptr;
    } else if (firstContainerPlan->getParentEdges().size() > 0) {
        // get lane
        const GNELane* lane = firstContainerPlan->getParentEdges().front()->getLaneByAllowedVClass(myPlanParent->getVClass());
        // declare departPos
        double posOverLane = 0;
        if (GNEAttributeCarrier::canParse<double>(myPlanParent->getAttribute(SUMO_ATTR_DEPARTPOS))) {
            posOverLane = GNEAttributeCarrier::parse<double>(myPlanParent->getAttribute(SUMO_ATTR_DEPARTPOS));
        }
        // return move operation
        return new GNEMoveOperation(this, lane, posOverLane, false);
    } else {
        return nullptr;
    }
}


std::string
GNEMoveElementPlanParent::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementPlanParent::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementPlanParent::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementPlanParent::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementPlanParent::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementPlanParent::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementPlanParent::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementPlanParent::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementPlanParent::setMoveShape(const GNEMoveResult& moveResult) {
    // change departPos
    myDepartPosProcedure = DepartPosDefinition::GIVEN;
    myDepartPos = moveResult.newFirstPos;
    // update geometry
    myPlanParent->updateGeometry();
}


void
GNEMoveElementPlanParent::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(myPlanParent, TLF("departPos of %", myPlanParent->getTagStr()));
    // now set departPos
    myPlanParent->setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
