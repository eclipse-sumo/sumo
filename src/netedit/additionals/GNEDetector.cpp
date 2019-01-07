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

#include <netedit/netelements/GNELane.h>

#include "GNEDetector.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                         double pos, double freq, const std::string& filename, const std::string& vehicleTypes, const std::string& name, bool friendlyPos, bool blockMovement) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement),
    myPositionOverLane(pos),
    myFreq(freq),
    myFilename(filename),
    myVehicleTypes(vehicleTypes),
    myFriendlyPosition(friendlyPos) {
}


GNEDetector::GNEDetector(GNEAdditional* additionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                         double pos, double freq, const std::string& filename, const std::string& name, bool friendlyPos, bool blockMovement) :
    GNEAdditional(additionalParent, viewNet, type, tag, name, blockMovement),
    myPositionOverLane(pos),
    myFreq(freq),
    myFilename(filename),
    myFriendlyPosition(friendlyPos) {
}


GNEDetector::~GNEDetector() {}


double
GNEDetector::getPositionOverLane() const {
    return myPositionOverLane;
}


Position
GNEDetector::getPositionInView() const {
    if (myPositionOverLane < 0) {
        return getLane()->getShape().front();
    } else if (myPositionOverLane > getLane()->getShape().length()) {
        return getLane()->getShape().back();
    } else {
        return getLane()->getShape().positionAtOffset(myPositionOverLane);
    }
}


std::string
GNEDetector::getParentName() const {
    return getLane()->getMicrosimID();
}


std::string
GNEDetector::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEDetector::getHierarchyName() const {
    return getTagStr();
}

/****************************************************************************/
