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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEDetector.h"
#include "GNELane.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane,
                         double pos, double freq, const std::string& filename, bool friendlyPos, GNEAdditional* additionalParent) :
    GNEAdditional(id, viewNet, tag, icon, true, additionalParent),
    myLane(lane),
    myPositionOverLane(pos / lane->getLaneParametricLength()),
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
GNEDetector::getAbsolutePositionOverLane() const {
    return myPositionOverLane * myLane->getLaneParametricLength();
}


void
GNEDetector::moveGeometry(const Position& oldPos, const Position& offset) {
    // Calculate new position using old position
    Position newPosition = oldPos;
    newPosition.add(offset);
    myPositionOverLane = myLane->getShape().nearest_offset_to_point2D(newPosition, false) / myLane->getLaneShapeLength();
    // Update geometry
    updateGeometry();
}


void
GNEDetector::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    if (!myBlocked) {
        // restore old position before commit new position
        double originalPosOverLane = myLane->getShape().nearest_offset_to_point2D(oldPos, false);
        undoList->p_begin("position of " + toString(getTag()));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPositionOverLane * myLane->getLaneParametricLength()), true, toString(originalPosOverLane)));
        undoList->p_end();
    }
}


Position
GNEDetector::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length());
}


const std::string&
GNEDetector::getParentName() const {
    return myLane->getMicrosimID();
}


/****************************************************************************/
