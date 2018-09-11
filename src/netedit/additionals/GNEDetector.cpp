/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

#include "GNEDetector.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane,
                         double pos, double freq, const std::string& filename, const std::string& vehicleTypes, const std::string& name, bool friendlyPos, bool blockMovement) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement),
    myLane(lane),
    myPositionOverLane(pos),
    myFreq(freq),
    myFilename(filename),
    myVehicleTypes(vehicleTypes),
    myFriendlyPosition(friendlyPos) {
}


GNEDetector::GNEDetector(GNEAdditional* additionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane,
                         double pos, double freq, const std::string& filename, const std::string& name, bool friendlyPos, bool blockMovement) :
    GNEAdditional(additionalParent, viewNet, type, tag, name, blockMovement),
    myLane(lane),
    myPositionOverLane(pos),
    myFreq(freq),
    myFilename(filename),
    myFriendlyPosition(friendlyPos) {
}


GNEDetector::~GNEDetector() {}


GNELane*
GNEDetector::getLane() const {
    return myLane;
}


double
GNEDetector::getPositionOverLane() const {
    return myPositionOverLane;
}


void
GNEDetector::moveGeometry(const Position& oldPos, const Position& offset) {
    // Calculate new position using old position
    Position newPosition = oldPos;
    newPosition.add(offset);
    myPositionOverLane = myLane->getShape().nearest_offset_to_point2D(newPosition, false);
    // Update geometry
    updateGeometry(false);
}


void
GNEDetector::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    // restore old position before commit new position
    double originalPosOverLane = myLane->getShape().nearest_offset_to_point2D(oldPos, false);
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPositionOverLane), true, toString(originalPosOverLane)));
    undoList->p_end();
}


Position
GNEDetector::getPositionInView() const {
    if (myPositionOverLane < 0) {
        return myLane->getShape().front();
    } else if (myPositionOverLane > myLane->getShape().length()) {
        return myLane->getShape().back();
    } else {
        return myLane->getShape().positionAtOffset(myPositionOverLane);
    }
}


std::string
GNEDetector::getParentName() const {
    return myLane->getMicrosimID();
}


std::string
GNEDetector::getPopUpID() const {
    return toString(getTag()) + ": " + getID();
}


std::string
GNEDetector::getHierarchyName() const {
    return toString(getTag());
}

/****************************************************************************/
