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
#include <utils/options/OptionsCont.h>

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


bool 
GNEStoppingPlace::isAdditionalValid() const {
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
GNEStoppingPlace::getAdditionalProblem() const {
    // declare variables 
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if(canParse<double>(myStartPosition)) {
        if (parse<double>(myStartPosition) < 0) {
            errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
        } else if (parse<double>(myStartPosition) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
            errorStart = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
        }
    }
    if(canParse<double>(myEndPosition)) {
        if (parse<double>(myEndPosition) < 0) {
            errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
        } else if (parse<double>(myEndPosition) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
            errorEnd = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
        }
    }
    // check separator
    if ((errorStart.size() > 0) && (errorEnd.size() > 0)) {
        separator = " and ";
    }
    return errorStart + separator + errorEnd;
}


void 
GNEStoppingPlace::fixAdditionalProblem() {
    // declare new start and end position
    std::string newStartPos = myStartPosition;
    std::string newEndPos = myEndPosition;
    // fix start and end positions using fixStoppinPlacePosition (0.01 is used to avoid precision problems)
    GNEAdditionalHandler::fixStoppinPlacePosition(newStartPos, newEndPos, myLane->getLaneParametricLength() - 0.01, POSITION_EPS + 0.01, true);
    // set new start and end positions
    setAttribute(SUMO_ATTR_STARTPOS, newStartPos, myViewNet->getUndoList());
    setAttribute(SUMO_ATTR_ENDPOS, newEndPos, myViewNet->getUndoList());
}


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
GNEStoppingPlace::moveGeometry(const Position& offset) {
    // only move if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        // Calculate new position using old position
        Position newPosition = myMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        double offsetLane = myLane->getShape().nearest_offset_to_point2D(newPosition, false) - myLane->getShape().nearest_offset_to_point2D(myMove.originalViewPosition, false);
        // check if start position must be moved  
        if (!myStartPosition.empty()) {
            myStartPosition = toString(parse<double>(myMove.firstOriginalLanePosition) + offsetLane);
        }
        // check if start position must be moved
        if (!myStartPosition.empty()) {
            myEndPosition = toString(parse<double>(myMove.secondOriginalPosition) + offsetLane);
        }
        // Update geometry
        updateGeometry(false);
    }
}


void
GNEStoppingPlace::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        undoList->p_begin("position of " + getTagStr());
        if (!myStartPosition.empty()) {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, myStartPosition, true, myMove.firstOriginalLanePosition));
        }
        if (!myEndPosition.empty()) {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, myEndPosition, true, myMove.secondOriginalPosition));
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


std::string
GNEStoppingPlace::getParentName() const {
    return myLane->getMicrosimID();
}


void
GNEStoppingPlace::setStoppingPlaceGeometry(double movingToSide) {
    // Clear all containers
    myGeometry.clearGeometry();

    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myGeometry.shape = myLane->getShape();

    // Move shape to side
    myGeometry.shape.move2side(movingToSide * offsetSign);

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
    myGeometry.shape = myGeometry.shape.getSubpart(startPosFixed * myLane->getLengthGeometryFactor(), endPosFixed * myLane->getLengthGeometryFactor());

    // Get calculate lenghts and rotations
    myGeometry.calculateShapeRotationsAndLengths();
}


std::string
GNEStoppingPlace::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEStoppingPlace::getHierarchyName() const {
    return getTagStr();
}

/****************************************************************************/
