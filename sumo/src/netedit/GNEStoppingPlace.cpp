/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GNELane* lane, SUMOReal startPos, SUMOReal endPos, bool blocked) :
    GNEAdditional(id, viewNet, Position(), tag, NULL, blocked),
    myLane(lane),
    myStartPos(startPos),
    myEndPos(endPos),
    mySignColor(RGBColor::YELLOW),
    mySignColorSelected(RGBColor::BLUE),
    myTextColor(RGBColor::CYAN),
    myTextColorSelected(RGBColor::BLUE) {
    myLane->addAdditional(this);
}


GNEStoppingPlace::~GNEStoppingPlace() {
    if(myLane) {
        myLane->removeAdditional(this);
    }
}


Position 
GNEStoppingPlace::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myPosition.x());
}


void
GNEStoppingPlace::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // Due a stoppingplace is placed over an lane ignore Warning of posy
    UNUSED_PARAMETER(posy);
    // if item isn't blocked
    if(myBlocked == false) {
        // Move to Right if distance is positive, to left if distance is negative
        if( ((posx > 0) && ((myEndPos + posx) < myLane->getLaneShapeLenght())) || ((posx < 0) && ((myStartPos + posx) > 0)) ) {
            // change attribute
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myStartPos + posx)));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(myEndPos + posx)));
        }
    }
}


GNELane*
GNEStoppingPlace::getLane() const {
    return myLane;
}


void
GNEStoppingPlace::removeLaneReference() {
    myLane = NULL;
}

void 
GNEStoppingPlace::changeLane(GNELane *newLane) {
    myLane->removeAdditional(this);
    myLane = newLane;
    myLane->addAdditional(this);
    updateGeometry();
    getViewNet()->update();
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
    if(startPos < 0) {
        throw InvalidArgument("Start position '" + toString(startPos) + "' not allowed. Must be greather than 0");
    } else if(startPos >= myEndPos) {
        throw InvalidArgument("Start position '" + toString(startPos) + "' not allowed. Must be smaller than endPos '" + toString(myEndPos) + "'");
    } else if ((myEndPos - startPos) < 1) {
        throw InvalidArgument("Start position '" + toString(startPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    } else {
        myStartPos = startPos;
    }
}


void
GNEStoppingPlace::setEndPosition(SUMOReal endPos) {
    if(endPos > myLane->getLaneShapeLenght()) {
        throw InvalidArgument("End position '" + toString(endPos) + "' not allowed. Must be smaller than lane length");
    } else if(myStartPos >= endPos) {
        throw InvalidArgument("End position '" + toString(endPos) + "' not allowed. Must be smaller than endPos '" + toString(myEndPos) + "'");
    } else if ((endPos - myStartPos) < 1) {
        throw InvalidArgument("End position '" + toString(endPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    } else {
        myEndPos = endPos;
    }
}


const std::string&
GNEStoppingPlace::getParentName() const {
        return myLane->getMicrosimID();
}

/****************************************************************************/
