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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, SUMOReal startPos, SUMOReal endPos) :
    GNEAdditional(id, viewNet, Position(), tag, icon),
    myStartPos(startPos),
    myEndPos(endPos),
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
    return myLane->getShape().positionAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x()));
}


void
GNEStoppingPlace::moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety) {
    // Due a stoppingplace is placed over an lane ignore Warning of posy
    UNUSED_PARAMETER(offsety);
    // Move to Right if distance is positive, to left if distance is negative
    if (((offsetx > 0) &&
            ((myLane->getPositionRelativeToParametricLenght(myEndPos) + offsetx) < myLane->getLaneParametricLenght())) ||
            ((offsetx < 0) && ((myLane->getPositionRelativeToParametricLenght(myStartPos) + offsetx) > 0))) {
        // change attribute
        myStartPos += offsetx;
        myEndPos += offsetx;
        // Update geometry
        updateGeometry();
    }
}


void
GNEStoppingPlace::commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(getStartPosition()), true, toString(oldPosx)));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(getEndPosition()), true, toString(oldPosy)));
    undoList->p_end();
    // Refresh element
    myViewNet->getNet()->refreshAdditional(this);
}


SUMOReal
GNEStoppingPlace::getStartPosition() const {
    return myStartPos;
}


SUMOReal
GNEStoppingPlace::getEndPosition() const {
    return myEndPos;
}


void
GNEStoppingPlace::setStartPosition(SUMOReal startPos) {
    if (startPos < 0) {
        throw InvalidArgument(toString(SUMO_ATTR_STARTPOS) + " '" + toString(startPos) + "' not allowed. Must be greater than 0");
    } else if (startPos >= myEndPos) {
        throw InvalidArgument(toString(SUMO_ATTR_STARTPOS) + " '" + toString(startPos) + "' not allowed. Must be smaller than endPos '" + toString(myEndPos) + "'");
    } else if ((myEndPos - startPos) < 1) {
        throw InvalidArgument(toString(SUMO_ATTR_STARTPOS) + " '" + toString(startPos) + "' not allowed. Lenght of StoppingPlace must be equal or greater than 1");
    } else {
        myStartPos = startPos;
    }
}


void
GNEStoppingPlace::setEndPosition(SUMOReal endPos) {
    if (endPos > myLane->getLaneShapeLenght()) {
        throw InvalidArgument(toString(SUMO_ATTR_ENDPOS) + " '" + toString(endPos) + "' not allowed. Must be smaller than lane length");
    } else if (myStartPos >= endPos) {
        throw InvalidArgument(toString(SUMO_ATTR_ENDPOS) + " '" + toString(endPos) + "' not allowed. Must be smaller than endPos '" + toString(myEndPos) + "'");
    } else if ((endPos - myStartPos) < 1) {
        throw InvalidArgument(toString(SUMO_ATTR_ENDPOS) + " '" + toString(endPos) + "' not allowed. Lenght of StoppingPlace must be equal or greater than 1");
    } else {
        myEndPos = endPos;
    }
}


const std::string&
GNEStoppingPlace::getParentName() const {
    return myLane->getMicrosimID();
}

/****************************************************************************/
