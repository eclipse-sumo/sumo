/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEBusStop.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBusStop::GNEBusStop(const std::string& id, GNELane* lane, GNEViewNet* viewNet, const double startPos, const double endPos, const int parametersSet,
        const std::string& name, const std::vector<std::string>& lines, int personCapacity, double parkingLength, bool friendlyPosition, bool blockMovement) :
    GNEStoppingPlace(id, viewNet, GLO_BUS_STOP, SUMO_TAG_BUS_STOP, lane, startPos, endPos, parametersSet, name, friendlyPosition, blockMovement),
    myLines(lines),
    myPersonCapacity(personCapacity),
    myParkingLength(parkingLength)
{ }


GNEBusStop::~GNEBusStop() {}


void
GNEBusStop::updateGeometry() {
    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry(getParentLanes().front()->getParentEdge()->getNBEdge()->getLaneWidth(getParentLanes().front()->getIndex()) / 2);

    // Obtain a copy of the shape
    PositionVector tmpShape = myAdditionalGeometry.getShape();

    // Move shape to side
    tmpShape.move2side(myViewNet->getVisualisationSettings().stoppingPlaceSettings.stoppingPlaceSignOffset * offsetSign);

    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();

    // Set block icon position
    myBlockIcon.position = myAdditionalGeometry.getShape().getLineCenter();

    // Set block icon rotation, and using their rotation for sign
    myBlockIcon.setRotation(getParentLanes().front());

    // obtain parent edge
    const GNEEdge* edge = getParentLanes().front()->getParentEdge();

    // update child demand elements geometry
    for (const auto& i : getChildDemandElements()) {
        // special case for person trips
        if (i->getTagProperty().isPersonTrip()) {
            // update previous and next person plan
            GNEDemandElement* previousDemandElement = i->getParentDemandElements().front()->getPreviousChildDemandElement(i);
            if (previousDemandElement) {
                previousDemandElement->updatePartialGeometry(edge);
            }
            GNEDemandElement* nextDemandElement = i->getParentDemandElements().front()->getNextChildDemandElement(i);
            if (nextDemandElement) {
                nextDemandElement->updatePartialGeometry(edge);
            }
        }
        i->updatePartialGeometry(edge);
    }

    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


void
GNEBusStop::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(),
                                          myAdditionalGeometry.getShape(),
                                          myViewNet->getVisualisationSettings().stoppingPlaceSettings.busStopWidth);
}


Boundary
GNEBusStop::getCenteringBoundary() const {
    return myAdditionalGeometry.getShape().getBoxBoundary().grow(10);
}


void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double busStopExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (s.drawAdditionals(busStopExaggeration) && myViewNet->getDataViewOptions().showAdditionals()) {
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // Set color of the base
        if (mySpecialColor) {
            GLHelper::setColor(*mySpecialColor);
        } else if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.stoppingPlaceSettings.busStopColor);
        }
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        GNEGeometry::drawGeometry(myViewNet, myAdditionalGeometry, s.stoppingPlaceSettings.busStopWidth * busStopExaggeration);
        // Check if the distance is enought to draw details and if is being drawn for selecting
        if (s.drawForRectangleSelection) {
            // only draw circle depending of distance between sign and mouse cursor
            if (myViewNet->getPositionInformation().distanceSquaredTo2D(mySignPos) <= (myCircleWidthSquared + 2)) {
                // Add a draw matrix for details
                glPushMatrix();
                // Start drawing sign traslating matrix to signal position
                glTranslated(mySignPos.x(), mySignPos.y(), 0);
                // scale matrix depending of the exaggeration
                glScaled(busStopExaggeration, busStopExaggeration, 1);
                // set color
                GLHelper::setColor(s.stoppingPlaceSettings.busStopColor);
                // Draw circle
                GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
                // pop draw matrix
                glPopMatrix();
            }
        } else if (s.drawDetail(s.detailSettings.stoppingPlaceDetails, busStopExaggeration)) {
            // draw lines between BusStops and Acces
            for (auto i : getChildAdditionals()) {
                GLHelper::drawBoxLine(i->getAdditionalGeometry().getPosition(),
                                      RAD2DEG(mySignPos.angleTo2D(i->getAdditionalGeometry().getPosition())) - 90, mySignPos.distanceTo2D(i->getAdditionalGeometry().getPosition()), .05);
            }
            // Add a draw matrix for details
            glPushMatrix();
            // draw lines depending of detailSettings
            if (s.drawDetail(s.detailSettings.stoppingPlaceText, busStopExaggeration) && !s.drawForPositionSelection) {
                // Iterate over every line
                for (int i = 0; i < (int)myLines.size(); ++i) {
                    // push a new matrix for every line
                    glPushMatrix();
                    // Rotate and traslaste
                    glTranslated(mySignPos.x(), mySignPos.y(), 0);
                    glRotated(-1 * myBlockIcon.rotation, 0, 0, 1);
                    // draw line with a color depending of the selection status
                    if (drawUsingSelectColor()) {
                        GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, s.colorSettings.selectionColor, 0, FONS_ALIGN_LEFT);
                    } else {
                        GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, s.stoppingPlaceSettings.busStopColor, 0, FONS_ALIGN_LEFT);
                    }
                    // pop matrix for every line
                    glPopMatrix();
                }
            }
            // Start drawing sign traslating matrix to signal position
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            // scale matrix depending of the exaggeration
            glScaled(busStopExaggeration, busStopExaggeration, 1);
            // Set color of the externe circle
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
            } else {
                GLHelper::setColor(s.stoppingPlaceSettings.busStopColor);
            }
            // Draw circle
            GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
            // Traslate to front
            glTranslated(0, 0, .1);
            // Set color of the interne circle
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectionColor);
            } else {
                GLHelper::setColor(s.stoppingPlaceSettings.busStopColorSign);
            }
            // draw another circle in the same position, but a little bit more small
            GLHelper::drawFilledCircle(myCircleInWidth, s.getCircleResolution());
            // draw H depending of detailSettings
            if (s.drawDetail(s.detailSettings.stoppingPlaceText, busStopExaggeration) && !s.drawForPositionSelection) {
                if (drawUsingSelectColor()) {
                    GLHelper::drawText("H", Position(), .1, myCircleInText, s.colorSettings.selectedAdditionalColor, myBlockIcon.rotation);
                } else {
                    GLHelper::drawText("H", Position(), .1, myCircleInText, s.stoppingPlaceSettings.busStopColor, myBlockIcon.rotation);
                }
            }
            // pop draw matrix
            glPopMatrix();
            // Show Lock icon depending of the Edit mode
            myBlockIcon.drawIcon(s, busStopExaggeration);
        }
        // pop draw matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        drawName(getPositionInView(), s.scale, s.addName);
        if (s.addFullName.show && (myAdditionalName != "") && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
            GLHelper::drawText(myAdditionalName, mySignPos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, myBlockIcon.rotation);
        }
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GNEGeometry::drawShapeDottedContour(s, getType(), busStopExaggeration, myDottedGeometry);
        }
        // Pop name
        glPopName();
        // draw child demand elements
        for (const auto& i : getChildDemandElements()) {
            if (!i->getTagProperty().isPlacedInRTree()) {
                i->drawGL(s);
            }
        }
    }
}


std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_STARTPOS:
            if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
                return toString(myStartPosition);
            } else {
                return "";
            }
        case SUMO_ATTR_ENDPOS:
            if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
                return toString(myEndPosition);
            } else {
                return "";
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_PERSON_CAPACITY:
            return toString(myPersonCapacity);
        case SUMO_ATTR_PARKING_LENGTH:
            return toString(myParkingLength);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            // change ID of BusStop
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            // Change Ids of all Acces children
            for (auto i : getChildAdditionals()) {
                i->setAttribute(SUMO_ATTR_ID, generateChildID(SUMO_TAG_ACCESS), undoList);
            }
            break;
        }
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_PERSON_CAPACITY:
        case SUMO_ATTR_PARKING_LENGTH:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
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
                return SUMORouteHandler::isStopPosValid(parse<double>(value), myEndPosition, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(myStartPosition, parse<double>(value), getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_PERSON_CAPACITY:
            return canParse<int>(value) && (parse<int>(value) > 0 || parse<int>(value) == -1);
        case SUMO_ATTR_PARKING_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_LANE:
            replaceParentLanes(this, value);
            break;
        case SUMO_ATTR_STARTPOS:
            if (!value.empty()) {
                myStartPosition = parse<double>(value);
                myParametersSet |= STOPPINGPLACE_STARTPOS_SET;
            } else {
                myParametersSet &= ~STOPPINGPLACE_STARTPOS_SET;
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (!value.empty()) {
                myEndPosition = parse<double>(value);
                myParametersSet |= STOPPINGPLACE_ENDPOS_SET;
            } else {
                myParametersSet &= ~STOPPINGPLACE_ENDPOS_SET;
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            myPersonCapacity = GNEAttributeCarrier::parse<int>(value);
            break;
        case SUMO_ATTR_PARKING_LENGTH:
            myParkingLength = GNEAttributeCarrier::parse<double>(value);
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
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
