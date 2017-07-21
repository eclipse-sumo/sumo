/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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
#include <utils/common/MsgHandler.h>

#include "GNEChargingStation.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double startPos, double endPos, const std::string &name, 
                                       double chargingPower, double efficiency, bool chargeInTransit, const double chargeDelay, bool friendlyPosition) :
    GNEStoppingPlace(id, viewNet, SUMO_TAG_CHARGING_STATION, ICON_CHARGINGSTATION, lane, startPos, endPos, name, friendlyPosition),
    myChargingPower(chargingPower),
    myEfficiency(efficiency),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(chargeDelay) {
    // When a new additional element is created, updateGeometry() must be called
    updateGeometry();
    // Set Colors
    myBaseColor = RGBColor(114, 210, 252, 255);
    myBaseColorSelected = RGBColor(125, 255, 255, 255);
    mySignColor = RGBColor(255, 235, 0, 255);
    mySignColorSelected = RGBColor(255, 235, 0, 255);
    myTextColor = RGBColor(114, 210, 252, 255);
    myTextColorSelected = RGBColor(125, 255, 255, 255);
}


GNEChargingStation::~GNEChargingStation() {}


void
GNEChargingStation::updateGeometry() {
    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry();

    // Obtain a copy of the shape
    PositionVector tmpShape = myShape;

    // Move shape to side
    tmpShape.move2side(1.5 * offsetSign);

    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set block icon rotation, and using their rotation for sign
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


void
GNEChargingStation::writeAdditional(OutputDevice& device, bool volatileOptionsEnabled) const {
    // Write additional
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    // Check if another lane ID must be changed if sidewalks.guess option is enabled
    if(volatileOptionsEnabled && OptionsCont::getOptions().getBool("sidewalks.guess") && (myLane->getParentEdge().getLanes().front()->isRestricted(SVC_PEDESTRIAN) == false)) {
        // add a new extra lane to edge
        myViewNet->getNet()->duplicateLane(myLane->getParentEdge().getLanes().front(), myViewNet->getUndoList());
        // write ID (now is different because there are a new lane)
        device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
        // undo set extra lane
        myViewNet->getUndoList()->undo();
    } else {
        device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    }
    device.writeAttr(SUMO_ATTR_STARTPOS, getAbsoluteStartPosition());
    device.writeAttr(SUMO_ATTR_ENDPOS, getAbsoluteEndPosition());
    if(myName.empty() == false) {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    device.writeAttr(SUMO_ATTR_CHARGINGPOWER, myChargingPower);
    device.writeAttr(SUMO_ATTR_EFFICIENCY, myEfficiency);
    device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, myChargeInTransit);
    device.writeAttr(SUMO_ATTR_CHARGEDELAY, myChargeDelay);
    if (myBlocked) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlocked);
    }
    // Close tag
    device.closeTag();
}


double
GNEChargingStation::getChargingPower() {
    return myChargingPower;
}


double
GNEChargingStation::getEfficiency() {
    return myEfficiency;
}


bool
GNEChargingStation::getChargeInTransit() {
    return myChargeInTransit;
}


double
GNEChargingStation::getChargeDelay() {
    return myChargeDelay;
}


void
GNEChargingStation::setChargingPower(double chargingPower) {
    if (chargingPower > 0) {
        myChargingPower = chargingPower;
    } else {
        throw InvalidArgument("Value of charging Power must be greater than 0");
    }
}


void
GNEChargingStation::setEfficiency(double efficiency) {
    if (efficiency >= 0 && efficiency <= 1) {
        myEfficiency = efficiency;
    } else {
        throw InvalidArgument("Value of efficiency must be between 0 and 1");
    }
}


void
GNEChargingStation::setChargeInTransit(bool chargeInTransit) {
    myChargeInTransit = chargeInTransit;
}


void
GNEChargingStation::setChargeDelay(double chargeDelay) {
    if (chargeDelay < 0) {
        throw InvalidArgument("Value of chargeDelay cannot be negative");
    }
    myChargeDelay = chargeDelay;
}


void
GNEChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // Push name
    glPushName(getGlID());

    // Push base matrix
    glPushMatrix();

    // Traslate matrix
    glTranslated(0, 0, getType());

    // Set Color
    if (isAdditionalSelected()) {
        GLHelper::setColor(myBaseColorSelected);
    } else {
        GLHelper::setColor(myBaseColor);
    }

    // Get exaggeration
    const double exaggeration = s.addSize.getExaggeration(s);

    // Draw base
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {
        // Push sign matrix
        glPushMatrix();

        // Set color of the charging power
        if (isAdditionalSelected()) {
            GLHelper::setColor(myTextColorSelected);
        } else {
            GLHelper::setColor(myTextColor);
        }

        // push charging power matrix
        glPushMatrix();

        // Traslate End positionof signal
        glTranslated(mySignPos.x(), mySignPos.y(), 0);

        // Rotate 180 (Eje X -> Mirror)
        glRotated(180, 1, 0, 0);

        // Rotate again using myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, 1);

        // Set poligon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // set polyfront position on 0,0
        pfSetPosition(0, 0);

        // Set polyfront scale to 1
        pfSetScale(1.f);

        // traslate matrix
        glTranslated(1.2, 0, 0);

        // draw charging power
        pfDrawString((toString(myChargingPower) + " W").c_str());

        // pop charging power matrix
        glPopMatrix();

        // Set position over sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);

        // Define number of points (for efficiency)
        int noPoints = 9;

        // If the scale * exaggeration is more than 25, recalculate number of points
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        // Scale matrix
        glScaled(exaggeration, exaggeration, 1);

        // Set base color
        if (isAdditionalSelected()) {
            GLHelper::setColor(myBaseColorSelected);
        } else {
            GLHelper::setColor(myBaseColor);
        }

        // Draw extern
        GLHelper::drawFilledCircle((double) 1.1, noPoints);

        // Move to top
        glTranslated(0, 0, .1);

        // Set sign color
        if (isAdditionalSelected()) {
            GLHelper::setColor(mySignColorSelected);
        } else {
            GLHelper::setColor(mySignColor);
        }
        // Draw internt sign
        GLHelper::drawFilledCircle((double) 0.9, noPoints);

        // Draw sign 'C'
        if (s.scale * exaggeration >= 4.5) {
            if (isAdditionalSelected()) {
                GLHelper::drawText("C", Position(), .1, 1.6, myBaseColorSelected, myBlockIconRotation);
            } else {
                GLHelper::drawText("C", Position(), .1, 1.6, myBaseColor, myBlockIconRotation);
            }
        }
        // Pop sign matrix
        glPopMatrix();

        // Draw icon
        GNEAdditional::drawLockIcon();
    }

    // Pop base matrix
    glPopMatrix();

    // Pop name matrix
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return toString(myLane->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(getAbsoluteStartPosition());
        case SUMO_ATTR_ENDPOS:
            return toString(getAbsoluteEndPosition());
        case SUMO_ATTR_NAME:
            return myName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_CHARGINGPOWER:
            return toString(myChargingPower);
        case SUMO_ATTR_EFFICIENCY:
            return toString(myEfficiency);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return toString(myChargeInTransit);
        case SUMO_ATTR_CHARGEDELAY:
            return toString(myChargeDelay);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_CHARGINGPOWER:
        case SUMO_ATTR_EFFICIENCY:
        case SUMO_ATTR_CHARGEINTRANSIT:
        case SUMO_ATTR_CHARGEDELAY:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEChargingStation::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (isValidID(value) && (myViewNet->getNet()->getAdditional(getTag(), value) == NULL)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if(canParse<double>(value)) {
                // Check that new start Position is smaller that end position
                return ((parse<double>(value) / myLane->getLaneParametricLength()) < myEndPosRelative);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if(canParse<double>(value)) {
                // Check that new end Position is larger that end position
                return ((parse<double>(value) / myLane->getLaneParametricLength()) > myStartPosRelative);
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return true;
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGINGPOWER:
            return (canParse<double>(value) && parse<double>(value) >= 0);
        case SUMO_ATTR_EFFICIENCY:
            return (canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGEDELAY:
            return (canParse<double>(value) && parse<double>(value) >= 0);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            changeLane(value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPosRelative = floor(parse<double>(value) / myLane->getLaneParametricLength()*1000)/1000;
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosRelative = floor(parse<double>(value) / myLane->getLaneParametricLength()*1000)/1000;
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_NAME:
            myName = value;
            getViewNet()->update();
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            getViewNet()->update();
            break;
        case SUMO_ATTR_CHARGINGPOWER:
            myChargingPower = parse<double>(value);
            break;
        case SUMO_ATTR_EFFICIENCY:
            myEfficiency = parse<double>(value);
            break;
        case SUMO_ATTR_CHARGEINTRANSIT:
            myChargeInTransit = parse<bool>(value);
            break;
        case SUMO_ATTR_CHARGEDELAY:
            myChargeDelay = parse<double>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            getViewNet()->update();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + "attribute '" + toString(key) + "' not allowed");
    }
}


/****************************************************************************/
