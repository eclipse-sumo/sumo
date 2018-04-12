/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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
#include <foreign/fontstash/fontstash.h>
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
#include <utils/common/MsgHandler.h>

#include "GNEChargingStation.h"
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewNet.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double startPos, double endPos, const std::string& name,
                                       double chargingPower, double efficiency, bool chargeInTransit, const double chargeDelay, bool friendlyPosition, bool blockMovement) :
    GNEStoppingPlace(id, viewNet, SUMO_TAG_CHARGING_STATION, ICON_CHARGINGSTATION, lane, startPos, endPos, name, friendlyPosition, blockMovement),
    myChargingPower(chargingPower),
    myEfficiency(efficiency),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(chargeDelay) {
}


GNEChargingStation::~GNEChargingStation() {}


void
GNEChargingStation::updateGeometry() {
    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry(0);

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
    myViewNet->getNet()->refreshElement(this);
}


void
GNEChargingStation::writeAdditional(OutputDevice& device) const {
    // Write additional
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, getAbsoluteStartPosition());
    device.writeAttr(SUMO_ATTR_ENDPOS, getAbsoluteEndPosition());
    if (myName.empty() == false) {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    device.writeAttr(SUMO_ATTR_CHARGINGPOWER, myChargingPower);
    device.writeAttr(SUMO_ATTR_EFFICIENCY, myEfficiency);
    device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, myChargeInTransit);
    device.writeAttr(SUMO_ATTR_CHARGEDELAY, myChargeDelay);
    // write block movement attribute only if it's enabled
    if (myBlockMovement) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlockMovement);
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
    // obtain circle resolution
    int circleResolution = getCircleResolution(s);
    // Get exaggeration
    const double exaggeration = s.addSize.getExaggeration(s);
    // Push name
    glPushName(getGlID());
    // Push base matrix
    glPushMatrix();
    // Traslate matrix
    glTranslated(0, 0, getType());
    // Set Color
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.SUMO_color_chargingStation);
    }
    // Draw base
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);
    // Check if the distance is enought to draw details and if is being drawn for selecting
    if(s.drawForSelecting) {
        // only draw circle depending of distance between sign and mouse cursor
        if(myViewNet->getPositionInformation().distanceSquaredTo(mySignPos) <= (myCircleWidthSquared + 2)) {
            // Add a draw matrix for details
            glPushMatrix();
            // Start drawing sign traslating matrix to signal position
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            // scale matrix depending of the exaggeration
            glScaled(exaggeration, exaggeration, 1);
            // set color
            GLHelper::setColor(s.SUMO_color_chargingStation);
            // Draw circle
            GLHelper::drawFilledCircle(myCircleWidth, circleResolution);
            // pop draw matrix
            glPopMatrix();
        }
    } else if (s.scale * exaggeration >= 10) {
        // Push matrix for details
        glPushMatrix();
        // push a new matrix for charging power
        glPushMatrix();
        // draw line with a color depending of the selection status
        if (isAdditionalSelected()) {
            GLHelper::drawText((toString(myChargingPower) + " W").c_str(), mySignPos + Position(1.2, 0), .1, 1.f, myViewNet->getNet()->selectionColor, myBlockIconRotation, FONS_ALIGN_LEFT);
        } else {
            GLHelper::drawText((toString(myChargingPower) + " W").c_str(), mySignPos + Position(1.2, 0), .1, 1.f, s.SUMO_color_chargingStation, myBlockIconRotation, FONS_ALIGN_LEFT);
        }
        // pop matrix for charging power
        glPopMatrix();
        // Set position over sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        // Scale matrix
        glScaled(exaggeration, exaggeration, 1);
        // Set base color
        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.SUMO_color_chargingStation);
        }
        // Draw extern
        GLHelper::drawFilledCircle(myCircleWidth, circleResolution);
        // Move to top
        glTranslated(0, 0, .1);
        // Set sign color
        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectionColor);
        } else {
            GLHelper::setColor(s.SUMO_color_chargingStation_sign);
        }
        // Draw internt sign
        GLHelper::drawFilledCircle(myCircleInWidth, circleResolution);
        // Draw sign 'C'
        if (s.scale * exaggeration >= 4.5) {
            if (isAdditionalSelected()) {
                GLHelper::drawText("C", Position(), .1, myCircleInText, myViewNet->getNet()->selectedAdditionalColor, myBlockIconRotation);
            } else {
                GLHelper::drawText("C", Position(), .1, myCircleInText, s.SUMO_color_chargingStation, myBlockIconRotation);
            }
        }
        // Pop sign matrix
        glPopMatrix();
        // Draw icon
        GNEAdditional::drawLockIcon();
    }
    // Pop base matrix
    glPopMatrix();
    // Draw name if isn't being drawn for selecting
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    if (s.addFullName.show && (myName != "") && !s.drawForSelecting) {
        GLHelper::drawText(myName, mySignPos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, myBlockIconRotation);
    }    
    // Pop name matrix
    glPopName();
}


std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
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
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAdditionalSelected());
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
        case GNE_ATTR_SELECTED:
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
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (canParse<double>(value)) {
                // Check that new start Position is smaller that end position
                return ((parse<double>(value) / myLane->getLaneParametricLength()) < myEndPosRelative);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (canParse<double>(value)) {
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
        case GNE_ATTR_SELECTED:
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
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPosRelative = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosRelative = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_NAME:
            myName = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
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
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if(parse<bool>(value)) {
                selectAdditional();
            } else {
                unselectAdditional();
            }
            break;
        default:
            throw InvalidArgument(toString(getTag()) + "attribute '" + toString(key) + "' not allowed");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}


/****************************************************************************/
