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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewNet.h>

#include "GNEStoppingPlace.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNEStoppingPlace::myCircleWidth = 1.1;
const double GNEStoppingPlace::myCircleWidthSquared = 1.21;
const double GNEStoppingPlace::myCircleInWidth = 0.9;
const double GNEStoppingPlace::myCircleInText = 1.6;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, bool friendlyPosition, bool blockMovement) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement),
    myLane(lane),
    myStartPosition(startPos),
    myEndPosition(endPos),
    myFriendlyPosition(friendlyPosition) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


Position
GNEStoppingPlace::getPositionInView() const {
    double startPos = canParse<double>(myStartPosition) ? parse<double>(myStartPosition) : 0;
    double endPos = canParse<double>(myEndPosition) ? parse<double>(myEndPosition) : myLane->getShape().length();
    if (myStartPosition.empty() && myEndPosition.empty()) {
        return myLane->getShape().positionAtOffset(myLane->getShape().length() / 2);
    } else if (myStartPosition.empty()) {
        return myLane->getShape().positionAtOffset(endPos);
    } else if (myEndPosition.empty()) {
        return myLane->getShape().positionAtOffset(startPos);
    } else {
        return myLane->getShape().positionAtOffset((startPos + endPos) / 2.0);
    }
}


void
GNEStoppingPlace::moveGeometry(const Position& oldPos, const Position& offset) {
    // only move if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        // Calculate new position using old position
        Position newStoppingPlaceCenterPosition = oldPos;
        newStoppingPlaceCenterPosition.add(offset);
        double newStoppingPlaceCenter = myLane->getShape().nearest_offset_to_point2D(newStoppingPlaceCenterPosition, false);
        // move stopping palce depending if start or end position is defined
        if (!myStartPosition.empty() && !myEndPosition.empty()) {
            double halfStoppingPlaceLength = (parse<double>(myEndPosition) - parse<double>(myStartPosition)) / 2.0;
            // change start and end position of stopping place
            myStartPosition = toString(newStoppingPlaceCenter - halfStoppingPlaceLength);
            myEndPosition = toString(newStoppingPlaceCenter + halfStoppingPlaceLength);
        } else if (myStartPosition.empty()) {
            myEndPosition = toString(newStoppingPlaceCenter);
        } else {
            myStartPosition = toString(newStoppingPlaceCenter);
        }
        // Update geometry
        updateGeometry(false);
    }
}


void
GNEStoppingPlace::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    // only commit geometry moving if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        // calculate old stopping place center
        double oldStoppingPlaceCenterOffset = myLane->getShape().nearest_offset_to_point2D(oldPos, false);
        undoList->p_begin("position of " + toString(getTag()));
        // change myStartPosition or myEndPosition depending if they are defined
        if (!myStartPosition.empty() && !myEndPosition.empty()) {
            double halfStoppingPlaceLength = (parse<double>(myEndPosition) - parse<double>(myStartPosition)) / 2.0;
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myStartPosition), true, toString(oldStoppingPlaceCenterOffset - halfStoppingPlaceLength)));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, myEndPosition, true, toString(oldStoppingPlaceCenterOffset + halfStoppingPlaceLength)));
        } else if (myStartPosition.empty()) {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, myEndPosition, true, toString(oldStoppingPlaceCenterOffset)));
        } else {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myStartPosition), true, toString(oldStoppingPlaceCenterOffset)));
        }
        // only change end position if its set
        if (!myEndPosition.empty()) {
        }
        undoList->p_end();
    }
}


GNELane*
GNEStoppingPlace::getLane() const {
    return myLane;
}


double
GNEStoppingPlace::getStartPosition() const {
    if (canParse<double>(myStartPosition)) {
        return parse<double>(myStartPosition);
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndPosition() const {
    if (canParse<double>(myEndPosition)) {
        return parse<double>(myEndPosition);
    } else {
        return myLane->getLaneShapeLength();
    }
}


bool
GNEStoppingPlace::areStoppingPlacesPositionsFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        if (myStartPosition.empty() && myEndPosition.empty()) {
            return true;
        } else if (myStartPosition.empty()) {
            return (canParse<double>(myEndPosition) && (parse<double>(myEndPosition) <= myLane->getParentEdge().getNBEdge()->getFinalLength()));
        } else if (myEndPosition.empty()) {
            return (canParse<double>(myStartPosition) && (parse<double>(myStartPosition) >= 0));
        } else {
            return canParse<double>(myStartPosition) && canParse<double>(myEndPosition) &&
                   (parse<double>(myStartPosition) >= 0) &&
                   (parse<double>(myEndPosition) <= myLane->getParentEdge().getNBEdge()->getFinalLength()) &&
                   ((parse<double>(myEndPosition) - parse<double>(myStartPosition)) >= POSITION_EPS);
        }
    }
}


std::string
GNEStoppingPlace::getParentName() const {
    return myLane->getMicrosimID();
}


void
GNEStoppingPlace::setStoppingPlaceGeometry(double movingToSide) {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myShape = myLane->getShape();

    // Move shape to side
    myShape.move2side(movingToSide * offsetSign);

    // set start position
    double startPosFixed;
    if (!canParse<double>(myStartPosition)) {
        startPosFixed = 0;
    } else if (parse<double>(myStartPosition) < 0) {
        startPosFixed = 0;
    } else if (parse<double>(myStartPosition) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
        startPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        startPosFixed = parse<double>(myStartPosition);
    }

    // set end position
    double endPosFixed;
    if (!canParse<double>(myEndPosition)) {
        endPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else if (parse<double>(myEndPosition) < 0) {
        endPosFixed = 0;
    } else if (parse<double>(myEndPosition) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
        endPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        endPosFixed = parse<double>(myEndPosition);
    }

    // Cut shape using as delimitators fixed start position and fixed end position
    myShape = myShape.getSubpart(startPosFixed * myLane->getLengthGeometryFactor(), endPosFixed * myLane->getLengthGeometryFactor());

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


std::string
GNEStoppingPlace::getPopUpID() const {
    return toString(getTag()) + ": " + getID();
}


std::string
GNEStoppingPlace::getHierarchyName() const {
    return toString(getTag());
}

/****************************************************************************/
