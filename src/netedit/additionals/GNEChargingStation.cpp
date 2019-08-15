/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEChargingStation.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane* lane, GNEViewNet* viewNet, const std::string& startPos, const std::string& endPos, const std::string& name,
                                       double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay, bool friendlyPosition, bool blockMovement) :
    GNEStoppingPlace(id, viewNet, GLO_CHARGING_STATION, SUMO_TAG_CHARGING_STATION, lane, startPos, endPos, name, friendlyPosition, blockMovement),
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
    PositionVector tmpShape = myGeometry.shape;

    // Move shape to side
    tmpShape.move2side(1.5 * offsetSign);

    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();

    // Set block icon position
    myBlockIcon.position = myGeometry.shape.getLineCenter();

    // Set block icon rotation, and using their rotation for sign
    myBlockIcon.setRotation(getLaneParents().front());
}


Boundary
GNEChargingStation::getCenteringBoundary() const {
    return myGeometry.shape.getBoxBoundary().grow(10);
}


void
GNEChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // Get exaggeration
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // Push name
    glPushName(getGlID());
    // Push base matrix
    glPushMatrix();
    // Traslate matrix
    glTranslated(0, 0, getType());
    // Set Color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.colorSettings.chargingStation);
    }
    // Draw base
    GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, exaggeration);
    // Check if the distance is enought to draw details and if is being drawn for selecting
    if (s.drawForSelecting) {
        // only draw circle depending of distance between sign and mouse cursor
        if (myViewNet->getPositionInformation().distanceSquaredTo2D(mySignPos) <= (myCircleWidthSquared + 2)) {
            // Add a draw matrix for details
            glPushMatrix();
            // Start drawing sign traslating matrix to signal position
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            // scale matrix depending of the exaggeration
            glScaled(exaggeration, exaggeration, 1);
            // set color
            GLHelper::setColor(s.colorSettings.chargingStation);
            // Draw circle
            GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else if (s.drawDetail(s.detailSettings.stoppingPlaceDetails, exaggeration)) {
        // Push matrix for details
        glPushMatrix();
        // draw power depending of detailSettings
        if (s.drawDetail(s.detailSettings.stoppingPlaceText, exaggeration)) {
            // push a new matrix for charging power
            glPushMatrix();
            // draw line with a color depending of the selection status
            if (drawUsingSelectColor()) {
                GLHelper::drawText((toString(myChargingPower) + " W").c_str(), mySignPos + Position(1.2, 0), .1, 1.f, s.colorSettings.selectionColor, myBlockIcon.rotation, FONS_ALIGN_LEFT);
            } else {
                GLHelper::drawText((toString(myChargingPower) + " W").c_str(), mySignPos + Position(1.2, 0), .1, 1.f, s.colorSettings.chargingStation, myBlockIcon.rotation, FONS_ALIGN_LEFT);
            }
            // pop matrix for charging power
            glPopMatrix();
        }
        // Set position over sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        // Scale matrix
        glScaled(exaggeration, exaggeration, 1);
        // Set base color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.colorSettings.chargingStation);
        }
        // Draw extern
        GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
        // Move to top
        glTranslated(0, 0, .1);
        // Set sign color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectionColor);
        } else {
            GLHelper::setColor(s.colorSettings.chargingStation_sign);
        }
        // Draw internt sign
        GLHelper::drawFilledCircle(myCircleInWidth, s.getCircleResolution());
        // Draw sign 'C' depending of detail settings
        if (s.drawDetail(s.detailSettings.stoppingPlaceText, exaggeration)) {
            if (drawUsingSelectColor()) {
                GLHelper::drawText("C", Position(), .1, myCircleInText, s.colorSettings.selectedAdditionalColor, myBlockIcon.rotation);
            } else {
                GLHelper::drawText("C", Position(), .1, myCircleInText, s.colorSettings.chargingStation, myBlockIcon.rotation);
            }
        }
        // Pop sign matrix
        glPopMatrix();
        // Draw icon
        myBlockIcon.drawIcon(s, exaggeration);
    }
    // Pop base matrix
    glPopMatrix();
    // Draw name if isn't being drawn for selecting
    drawName(getPositionInView(), s.scale, s.addName);
    if (s.addFullName.show && (myAdditionalName != "") && !s.drawForSelecting) {
        GLHelper::drawText(myAdditionalName, mySignPos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, myBlockIcon.rotation);
    }
    // check if dotted contour has to be drawn
    if (myViewNet->getDottedAC() == this) {
        GLHelper::drawShapeDottedContourAroundShape(s, getType(), myGeometry.shape, exaggeration);
    }
    // Pop name matrix
    glPopName();
    // draw demand element children
    for (const auto& i : getDemandElementChildren()) {
        if (!i->getTagProperty().isPlacedInRTree()) {
            i->drawGL(s);
        }
    }
}


std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return getLaneParents().front()->getID();
        case SUMO_ATTR_STARTPOS:
            return toString(myStartPosition);
        case SUMO_ATTR_ENDPOS:
            return myEndPosition;
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_CHARGINGPOWER:
            return toString(myChargingPower);
        case SUMO_ATTR_EFFICIENCY:
            return toString(myEfficiency);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return toString(myChargeInTransit);
        case SUMO_ATTR_CHARGEDELAY:
            return time2string(myChargeDelay);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return checkStoppinPlacePosition(value, myEndPosition, getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return checkStoppinPlacePosition(myStartPosition, value, getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGINGPOWER:
            return (canParse<double>(value) && parse<double>(value) >= 0);
        case SUMO_ATTR_EFFICIENCY:
            return (canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGEDELAY:
            return canParse<SUMOTime>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            changeLaneParents(this, value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPosition = value;
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosition = value;
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
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
            myChargeDelay = parse<SUMOTime>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + "attribute '" + toString(key) + "' not allowed");
    }
}


/****************************************************************************/
