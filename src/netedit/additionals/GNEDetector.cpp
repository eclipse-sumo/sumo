/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEAdditionalHandler.h>

#include "GNEDetector.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                         double pos, SUMOTime freq, const std::string& filename, const std::string& vehicleTypes,
                         const std::string& name, bool friendlyPos, bool blockMovement, const std::vector<GNELane*>& laneParents) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement, {}, laneParents, {}, {}, {}, {}, {}, {}, {}, {}),
              myPositionOverLane(pos),
              myFreq(freq),
              myFilename(filename),
              myVehicleTypes(vehicleTypes),
myFriendlyPosition(friendlyPos) {
}


GNEDetector::GNEDetector(GNEAdditional* additionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                         double pos, SUMOTime freq, const std::string& filename,
                         const std::string& name, bool friendlyPos, bool blockMovement, const std::vector<GNELane*>& laneParents) :
    GNEAdditional(additionalParent, viewNet, type, tag, name, blockMovement,
{}, laneParents, {}, {additionalParent}, {}, {}, {}, {}, {}, {}),
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


GNELane*
GNEDetector::getLane() const {
    return getLaneParents().front();
}


Position
GNEDetector::getPositionInView() const {
    return getLane()->getGeometry().shape.positionAtOffset(getGeometryPositionOverLane());
}


Boundary
GNEDetector::getCenteringBoundary() const {
    if (myGeometry.multiShapeUnified.size() > 0) {
        return myGeometry.multiShapeUnified.getBoxBoundary().grow(10);
    } else {
        return myGeometry.shape.getBoxBoundary().grow(10);
    }
}


double
GNEDetector::getGeometryPositionOverLane() const {
    double fixedPos = myPositionOverLane;
    const double len = getLane()->getParentEdge().getNBEdge()->getFinalLength();
    if (fixedPos < 0) {
        fixedPos += len;
    }
    GNEAdditionalHandler::checkAndFixDetectorPosition(fixedPos, len, true);
    return fixedPos * getLane()->getLengthGeometryFactor();
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
