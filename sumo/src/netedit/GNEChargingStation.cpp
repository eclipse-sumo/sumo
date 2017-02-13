/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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

#include "GNEChargingStation.h"
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

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane* lane, GNEViewNet* viewNet, SUMOReal startPos, SUMOReal endPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, const SUMOReal chargeDelay) :
    GNEStoppingPlace(id, viewNet, SUMO_TAG_CHARGING_STATION, ICON_CHARGINGSTATION, lane, startPos, endPos),
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
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Clear shape
    myShape.clear();

    // Get shape of lane parent
    myShape = myLane->getShape();

    // Cut shape using as delimitators from start position and end position
    myShape = myShape.getSubpart(myLane->getPositionRelativeToParametricLenght(myStartPos), myLane->getPositionRelativeToParametricLenght(myEndPos));

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
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }

    // Obtain a copy of the shape
    PositionVector tmpShape = myShape;

    // Move shape to side
    if (myRotationLefthand) {
        tmpShape.move2side(-1.5);
    } else {
        tmpShape.move2side(1.5);
    }

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
GNEChargingStation::writeAdditional(OutputDevice& device) const {
    // Write additional
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
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


SUMOReal
GNEChargingStation::getChargingPower() {
    return myChargingPower;
}


SUMOReal
GNEChargingStation::getEfficiency() {
    return myEfficiency;
}


bool
GNEChargingStation::getChargeInTransit() {
    return myChargeInTransit;
}


SUMOReal
GNEChargingStation::getChargeDelay() {
    return myChargeDelay;
}


void
GNEChargingStation::setChargingPower(SUMOReal chargingPower) {
    if (chargingPower > 0) {
        myChargingPower = chargingPower;
    } else {
        throw InvalidArgument("Value of charging Power must be greater than 0");
    }
}


void
GNEChargingStation::setEfficiency(SUMOReal efficiency) {
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
GNEChargingStation::setChargeDelay(SUMOReal chargeDelay) {
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
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

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

        // Define nº points (for efficiency)
        int noPoints = 9;

        // If the scale * exaggeration is more than 25, recalculate nº points
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
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);

        // Move to top
        glTranslated(0, 0, .1);

        // Set sign color
        if (isAdditionalSelected()) {
            GLHelper::setColor(mySignColorSelected);
        } else {
            GLHelper::setColor(mySignColor);
        }
        // Draw internt sign
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

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
            return toString(myStartPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
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
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
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
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) < (myEndPos - 1));
        case SUMO_ATTR_ENDPOS: {
            if (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 1 && parse<SUMOReal>(value) > myStartPos) {
                // If extension is larger than Lane
                if (parse<SUMOReal>(value) > myLane->getLaneParametricLenght()) {
                    // Ask user if want to assign the lenght of lane as endPosition
                    FXuint answer = FXMessageBox::question(getViewNet()->getApp(), MBOX_YES_NO,
                                                           (toString(SUMO_ATTR_ENDPOS) + " exceeds the size of the " + toString(SUMO_TAG_LANE)).c_str(), "%s",
                                                           (toString(SUMO_ATTR_ENDPOS) + " exceeds the size of the " + toString(SUMO_TAG_LANE) +
                                                            ". Do you want to assign the lenght of the " + toString(SUMO_TAG_LANE) + " as " + toString(SUMO_ATTR_ENDPOS) + "?").c_str());
                    if (answer == 1) { //1:yes, 2:no, 4:esc
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_CHARGINGPOWER:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_EFFICIENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) <= 1);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGEDELAY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
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
            myStartPos = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_ENDPOS:
            if (parse<SUMOReal>(value) > myLane->getLaneParametricLenght()) {
                myEndPos = myLane->getLaneParametricLenght();
            } else {
                myEndPos = parse<SUMOReal>(value);
            }
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_CHARGINGPOWER:
            myChargingPower = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_EFFICIENCY:
            myEfficiency = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_CHARGEINTRANSIT:
            myChargeInTransit = parse<bool>(value);
            break;
        case SUMO_ATTR_CHARGEDELAY:
            myChargeDelay = parse<SUMOReal>(value);
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
