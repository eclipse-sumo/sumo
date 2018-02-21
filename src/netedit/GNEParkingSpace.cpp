/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEParkingSpace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <foreign/fontstash/fontstash.h>
#include <iostream>
#include <string>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEChange_Attribute.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNEViewNet* viewNet, GNEParkingArea* parkingAreaParent, double x, double y, double z, double width, double length, double angle) :
    GNEAdditional(parkingAreaParent->generateParkingSpaceID(), viewNet, SUMO_TAG_PARKING_SPACE, ICON_PARKINGSPACE, true, parkingAreaParent),
    myPosition(Position(x,y)),
    myZ(z),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
}


GNEParkingSpace::~GNEParkingSpace() {}


void
GNEParkingSpace::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_X, myPosition.x());
    device.writeAttr(SUMO_ATTR_Y, myPosition.y());
    if(myZ != 0) {
        device.writeAttr(SUMO_ATTR_Z, myZ);
    }
    if(toString(myWidth) != myAdditionalParent->getAttribute(SUMO_ATTR_WIDTH)) {
        device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    }
    if(toString(myLength) != myAdditionalParent->getAttribute(SUMO_ATTR_LENGTH)) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    if(toString(myAngle) != myAdditionalParent->getAttribute(SUMO_ATTR_ANGLE)) {
        device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    }
    // Close tag
    device.closeTag();
}


void 
GNEParkingSpace::moveGeometry(const Position & oldPos, const Position & offset) {
    // restore old position, apply offset and update Geometry
    myPosition = oldPos;
    myPosition.add(offset);
    updateGeometry();
}


void 
GNEParkingSpace::commitGeometryMoving(const Position & oldPos, GNEUndoList * undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition), true, toString(oldPos)));
    undoList->p_end();
}


void
GNEParkingSpace::updateGeometry() {
    myShape.clear();

    double w = myWidth / 2. - 0.;
    double h = myLength;
    myShape.push_back(myPosition + Position(- w, + 0, 0.));
    myShape.push_back(myPosition + Position(+ w, + 0, 0.));
    myShape.push_back(myPosition + Position(+ w, + h, 0.));
    myShape.push_back(myPosition + Position(- w, + h, 0.));
    myShape.push_back(myPosition + Position(- w, + 0, 0.));


    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position 
GNEParkingSpace::getPositionInView() const {
    return myPosition;
}


const std::string&
GNEParkingSpace::getParentName() const {
    return myAdditionalParent->getID();
}


void
GNEParkingSpace::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    RGBColor grey(177, 184, 186, 171);
    RGBColor blue(83, 89, 172, 255);
    RGBColor red(255, 0, 0, 255);
    RGBColor green(0, 255, 0, 255);
    const double exaggeration = s.addSize.getExaggeration(s);
    glPushMatrix();
    /*
    geom.push_back(Position(pos.x(), pos.y(), pos.z()));
    geom.push_back(Position(pos.x() + (*l).second.myWidth, pos.y(), pos.z()));
    geom.push_back(Position(pos.x() + (*l).second.myWidth, pos.y() - (*l).second.myLength, pos.z()));
    geom.push_back(Position(pos.x(), pos.y() - (*l).second.myLength, pos.z()));
    geom.push_back(Position(pos.x(), pos.y(), pos.z()));
    */
    GLHelper::setColor(/*(*i).second.vehicle == 0 ? green :*/ red);
    GLHelper::drawBoxLines(myShape, 0.1 * exaggeration);
    glPopMatrix();
    glPopName();
}


std::string
GNEParkingSpace::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_Z:
            return toString(myZ);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_ANGLE:
            return toString(myAngle);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        case GNE_ATTR_PARENT:
            return myAdditionalParent->getID();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_Z:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_PARENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingSpace::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_Z:
            return canParse<double>(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_PARENT:
            return (myViewNet->getNet()->getAdditional(SUMO_TAG_PARKING_AREA, value) != NULL);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            break;
        case SUMO_ATTR_Z:
            myZ = parse<double>(value);
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_ANGLE:
            myAngle = parse<double>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            changeAdditionalParent(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}


/****************************************************************************/
