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
/// @file    GNEMoveElementView.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over view
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElementView.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementView::GNEMoveElementView(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                                       SumoXMLAttr posAttr, Position& position) :
    GNEMoveElement(element),
    myPosAttr(posAttr),
    myPosOverView(position),
    myAttributesFormat(attributesFormat) {
}

GNEMoveElementView::~GNEMoveElementView() {}


GNEMoveOperation*
GNEMoveElementView::getMoveOperation() {
    // move entire space
    return new GNEMoveOperation(this, myPosOverView);
}


std::string
GNEMoveElementView::getMovingAttribute(SumoXMLAttr key) const {
    if (key == myPosAttr) {
        return toString(myPosOverView);
    } else {
        return myMovedElement->getCommonAttribute(key);
    }
}


double
GNEMoveElementView::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementView::getMovingAttributePosition(SumoXMLAttr key) const {
    if (key == myPosAttr) {
        return myPosOverView;
    } else {
        return myMovedElement->getCommonAttributePosition(key);
    }
}


PositionVector
GNEMoveElementView::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementView::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (key == myPosAttr) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else {
        myMovedElement->setCommonAttribute(key, value, undoList);
    }
}


bool
GNEMoveElementView::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    if (key == myPosAttr) {
        return GNEAttributeCarrier::canParse<Position>(value);
    } else {
        return myMovedElement->isCommonAttributeValid(key, value);
    }
}


void
GNEMoveElementView::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    if (key == myPosAttr) {
        myPosOverView =  GNEAttributeCarrier::parse<Position>(value);
    } else {
        myMovedElement->setCommonAttribute(key, value);
    }
}


void
GNEMoveElementView::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementView::writeMoveAttributes(OutputDevice& device) const {
    // position format
    if (myAttributesFormat == AttributesFormat::POSITION) {
        device.writeAttr(SUMO_ATTR_POSITION, myPosOverView);
    } else {
        // x-y format
        if (myAttributesFormat == AttributesFormat::CARTESIAN) {
            device.writeAttr(SUMO_ATTR_X, myPosOverView.x());
            device.writeAttr(SUMO_ATTR_Y, myPosOverView.y());
        }
        // geo format
        if (myAttributesFormat == AttributesFormat::GEO) {
            device.setPrecision(gPrecisionGeo);
            device.writeAttr(SUMO_ATTR_LON, myMovedElement->getAttributeDouble(SUMO_ATTR_LON));
            device.writeAttr(SUMO_ATTR_LAT, myMovedElement->getAttributeDouble(SUMO_ATTR_LAT));
            device.setPrecision();
        }
        // z
        if (myPosOverView.z() != 0) {
            device.writeAttr(SUMO_ATTR_Z, myPosOverView.z());
        }
    }
}


void
GNEMoveElementView::setMoveShape(const GNEMoveResult& moveResult) {
    myPosOverView = moveResult.shapeToUpdate.front();
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementView::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
    GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
    undoList->end();
}

/****************************************************************************/
