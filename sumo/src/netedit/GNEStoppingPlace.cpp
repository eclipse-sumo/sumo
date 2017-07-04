/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
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

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, double startPos, double endPos, const std::string &name, bool friendlyPosition) :
    GNEAdditional(id, viewNet, Position(), tag, icon),
    myStartPosRelative(startPos / lane->getLaneParametricLength()),
    myEndPosRelative(endPos / lane->getLaneParametricLength()),
    myName(name),
    myFriendlyPosition(friendlyPosition),
    mySignColor(RGBColor::YELLOW),
    mySignColorSelected(RGBColor::BLUE),
    myTextColor(RGBColor::CYAN),
    myTextColorSelected(RGBColor::BLUE) {
    // This additional belongs to a Lane
    myLane = lane;
}


GNEStoppingPlace::~GNEStoppingPlace() {
}


Position
GNEStoppingPlace::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myLane->getPositionRelativeToParametricLength(myPosition.x()));
}


void
GNEStoppingPlace::moveGeometry(const Position &newPosition) {
    // First we need to change the absolute new positions to a relative positions
    double lenghtDifference = myLane->getLaneParametricLength() / myLane->getLaneShapeLength();
    double relativePos = newPosition.x() / myLane->getLaneParametricLength() * lenghtDifference;
    double stoppingPlaceLenght = myEndPosRelative - myStartPosRelative;
    // change start position of stopping place
    if((myStartPosRelative + relativePos) < 0) {
        myStartPosRelative = 0;
        myEndPosRelative = stoppingPlaceLenght;
    } else if (myEndPosRelative + relativePos > 1) {
        myStartPosRelative = 1 - stoppingPlaceLenght;
        myEndPosRelative = 1;
    } else {
        myStartPosRelative += relativePos;
        myEndPosRelative += relativePos;
    }
    // Update geometry
    updateGeometry();
}


void
GNEStoppingPlace::commmitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myStartPosRelative * myLane->getLaneParametricLength()), true, toString(oldPos.x())));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(myEndPosRelative * myLane->getLaneParametricLength()), true, toString(oldPos.y())));
    undoList->p_end();
    // Refresh element
    myViewNet->getNet()->refreshAdditional(this);
}


double
GNEStoppingPlace::getStartPosition() const {
    return myStartPosRelative;
}


double
GNEStoppingPlace::getEndPosition() const {
    return myEndPosRelative;
}


bool  
GNEStoppingPlace::fixStoppingPlacePosition() {
    if(myFriendlyPosition) {
        throw InvalidArgument("StoppingPlace position cannot be fixed if friendlyPos is enabled");
    } else {
        return GNEAdditionalHandler::checkAndFixStoppinPlacePosition(myStartPosRelative, myEndPosRelative, myLane->getLaneShapeLength(), POSITION_EPS, myFriendlyPosition);
    }
}


bool 
GNEStoppingPlace::areStoppingPlacesPositionsFixed() {
    // with friendly position enabled position are "always fixed"
    if(myFriendlyPosition) {
        return true;
    } else {
        double tmpStarPos = myStartPosRelative;
        double tmpEndPos = myEndPosRelative;
        // Check if positions can be fixed
        if (GNEAdditionalHandler::checkAndFixStoppinPlacePosition(tmpStarPos, tmpEndPos, myLane->getLaneShapeLength(), POSITION_EPS, myFriendlyPosition)) {
            return ((tmpStarPos == myStartPosRelative) && (tmpEndPos == myEndPosRelative));
        } else {
            return false;
        }
    }
}


const std::string&
GNEStoppingPlace::getStoppingPlaceName() const {
    return myName;
}


void 
GNEStoppingPlace::setStoppingPlaceName(const std::string &name) {
    myName = name;
}


const std::string&
GNEStoppingPlace::getParentName() const {
    return myLane->getMicrosimID();
}

/****************************************************************************/
