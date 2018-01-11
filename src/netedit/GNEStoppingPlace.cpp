/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
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

#include "GNEStoppingPlace.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, double startPos, double endPos, const std::string& name, bool friendlyPosition) :
    GNEAdditional(id, viewNet, tag, icon, true),
    myLane(lane),
    myStartPosRelative(startPos / lane->getLaneParametricLength()),
    myEndPosRelative(endPos / lane->getLaneParametricLength()),
    myName(name),
    myFriendlyPosition(friendlyPosition) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


Position
GNEStoppingPlace::getPositionInView() const {
    double stoppingPlaceCenterRelative = (myEndPosRelative + myStartPosRelative) / 2.0;
    return myLane->getShape().positionAtOffset(stoppingPlaceCenterRelative * myLane->getLaneShapeLength());
}


void
GNEStoppingPlace::moveGeometry(const Position& oldPos, const Position& offset) {
    double halfStoppingPlaceLenghtRelative = (myEndPosRelative - myStartPosRelative) / 2.0;
    // Calculate new position using old position
    Position newPosition = oldPos;
    newPosition.add(offset);
    double newStoppingPlaceCenter = myLane->getShape().nearest_offset_to_point2D(newPosition, false) / myLane->getLaneShapeLength();
    // change start position of stopping place
    myStartPosRelative = newStoppingPlaceCenter - halfStoppingPlaceLenghtRelative;
    myEndPosRelative = newStoppingPlaceCenter + halfStoppingPlaceLenghtRelative;
    // Update geometry
    updateGeometry();
}


void
GNEStoppingPlace::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    double halfStoppingPlaceLenghtRelative = (myEndPosRelative - myStartPosRelative) / 2.0;
    double oldStoppingPlaceCenterOffset = myLane->getShape().nearest_offset_to_point2D(oldPos, false) / myLane->getLaneShapeLength();
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(getAbsoluteStartPosition()), true, toString((oldStoppingPlaceCenterOffset - halfStoppingPlaceLenghtRelative) * myLane->getLaneParametricLength())));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(getAbsoluteEndPosition()), true, toString((oldStoppingPlaceCenterOffset + halfStoppingPlaceLenghtRelative) * myLane->getLaneParametricLength())));
    undoList->p_end();
}


GNELane*
GNEStoppingPlace::getLane() const {
    return myLane;
}


double
GNEStoppingPlace::getAbsoluteStartPosition() const {
    return myStartPosRelative * myLane->getLaneParametricLength();
}


double
GNEStoppingPlace::getAbsoluteEndPosition() const {
    return myEndPosRelative * myLane->getLaneParametricLength();
}


bool
GNEStoppingPlace::areStoppingPlacesPositionsFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        // floors are needed to avoid precision problems
        return ((floor(myStartPosRelative * 1000) / 1000) >= 0) &&
               ((floor(myEndPosRelative * 1000) / 1000) <= 1) &&
               ((getAbsoluteEndPosition() - getAbsoluteStartPosition()) >= POSITION_EPS);
    }
}


const std::string&
GNEStoppingPlace::getParentName() const {
    return myLane->getMicrosimID();
}


void
GNEStoppingPlace::setStoppingPlaceGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myShape = myLane->getShape();

    // Move shape to side
    myShape.move2side(1.65 * offsetSign);

    // Cut shape using as delimitators fixed start position and fixed end position
    double startPosFixed = (myStartPosRelative < 0) ? 0 : myStartPosRelative;
    double endPosFixed = (myEndPosRelative > 1) ? 1 : myEndPosRelative;
    myShape = myShape.getSubpart(startPosFixed * myShape.length() , endPosFixed * myShape.length());

    // Get number of parts of the shape
    int numberOfSegments = (int) myShape.size() - 1;

    // If number of segments is more than 0
    if (numberOfSegments >= 0) {

        // Reserve memory (To improve efficiency)
        myShapeRotations.reserve(numberOfSegments);
        myShapeLengths.reserve(numberOfSegments);

        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {

            // Obtain first position
            const Position& f = myShape[i];

            // Obtain next position
            const Position& s = myShape[i + 1];

            // Save distance between position into myShapeLengths
            myShapeLengths.push_back(f.distanceTo(s));

            // Save rotation (angle) of the vector constructed by points f and s
            myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}

/****************************************************************************/
