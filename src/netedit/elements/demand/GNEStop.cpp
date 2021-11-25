/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Representation of Stops in NETEDIT
/****************************************************************************/
#include <cmath>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEStop.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStop::GNEStop(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_STOP, tag, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEStop::GNEStop(SumoXMLTag tag, GNENet* net, const SUMOVehicleParameter::Stop& stopParameter, GNEAdditional* stoppingPlace, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, net, GLO_STOP, tag, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {stoppingPlace}, {}, {}, {stopParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEStop::GNEStop(GNENet* net, const SUMOVehicleParameter::Stop& stopParameter, GNELane* lane, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, net, GLO_STOP, SUMO_TAG_STOP_LANE, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {lane}, {}, {}, {}, {stopParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEStop::~GNEStop() {}


GNEMoveOperation*
GNEStop::getMoveOperation() {
    if (myTagProperty.getTag() == SUMO_TAG_STOP_LANE) {
        // get allow change lane
        const bool allowChangeLane = myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane();
        // fist check if we're moving only extremes
        if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
                (myNet->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_MOVE) &&
                myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
            // get snap radius
            const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
            // get mouse position
            const Position mousePosition = myNet->getViewNet()->getPositionInformation();
            // check if we clicked over start or end position
            if ((startPos != INVALID_DOUBLE) && (myDemandElementGeometry.getShape().front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius))) {
                // move only start position
                return new GNEMoveOperation(this, getParentLanes().front(), startPos, getParentLanes().front()->getLaneShape().length2D() - POSITION_EPS,
                                            allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST);
            } else if ((endPos != INVALID_DOUBLE) && (myDemandElementGeometry.getShape().back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius))) {
                // move only end position
                return new GNEMoveOperation(this, getParentLanes().front(), 0, endPos,
                                            allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND);
            } else {
                return nullptr;
            }
        } else if ((startPos != INVALID_DOUBLE) && (endPos != INVALID_DOUBLE)) {
            // move both start and end positions
            return new GNEMoveOperation(this, getParentLanes().front(), startPos, endPos,
                                        allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVEBOTH);
        } else if (startPos != INVALID_DOUBLE) {
            // move only start position
            return new GNEMoveOperation(this, getParentLanes().front(), startPos, getParentLanes().front()->getLaneShape().length2D() - POSITION_EPS,
                                        allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST);
        } else if (startPos != INVALID_DOUBLE) {
            // move only end position
            return new GNEMoveOperation(this, getParentLanes().front(), 0, endPos,
                                        allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND);
        } else {
            // start and end positions undefined, then nothing to move
            return nullptr;
        }
    } else {
        return nullptr;
    }
}


std::string
GNEStop::getBegin() const {
    return "";
}


void
GNEStop::writeDemandElement(OutputDevice& device) const {
    write(device);
}


bool
GNEStop::isDemandElementValid() const {
    // only Stops placed over lanes can be invalid
    if (myTagProperty.getTag() != SUMO_TAG_STOP_LANE) {
        return true;
    } else if (friendlyPos) {
        // with friendly position enabled position are "always fixed"
        return true;
    } else {
        // obtain lane length
        double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength() * getParentLanes().front()->getLengthGeometryFactor();
        // declare a copy of start and end positions
        double startPosCopy = startPos;
        double endPosCopy = endPos;
        // check if position has to be fixed
        if (startPosCopy < 0) {
            startPosCopy += laneLength;
        }
        if (endPosCopy < 0) {
            endPosCopy += laneLength;
        }
        // check values
        return ((startPosCopy >= 0) && (endPosCopy <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) && ((endPosCopy - startPosCopy) >= POSITION_EPS));
    }
}


std::string
GNEStop::getDemandElementProblem() const {
    // declare a copy of start and end positions
    double startPosCopy = startPos;
    double endPosCopy = endPos;
    // obtain lane length
    double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // check if position has to be fixed
    if (startPosCopy < 0) {
        startPosCopy += laneLength;
    }
    if (endPosCopy < 0) {
        endPosCopy += laneLength;
    }
    // declare variables
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if (startPosCopy < 0) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    } else if (startPosCopy > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
    }
    if (endPosCopy < 0) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    } else if (endPosCopy > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
    }
    // check separator
    if ((errorStart.size() > 0) && (errorEnd.size() > 0)) {
        separator = " and ";
    }
    return errorStart + separator + errorEnd;
}


void
GNEStop::fixDemandElementProblem() {
    //
}


SUMOVehicleClass
GNEStop::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEStop::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.stopColor;
}


void
GNEStop::updateGeometry() {
    // update geometry depending of parent
    if (getParentLanes().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myDemandElementGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
    } else if (getParentAdditionals().size() > 0) {
        // use geometry of additional (busStop)
        myDemandElementGeometry = getParentAdditionals().at(0)->getAdditionalGeometry();
    }
    // recompute geometry of all Demand elements related with this this stop
    if (getParentDemandElements().front()->getTagProperty().isRoute()) {
        getParentDemandElements().front()->updateGeometry();
    }
}


Position
GNEStop::getPositionInView() const {
    if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getLaneShape().positionAtOffset((startPos + endPos) / 2.0);
    } else if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getPositionInView();
    } else {
        throw ProcessError("Invalid Stop parent");
    }
}


std::string
GNEStop::getParentName() const {
    if (getParentDemandElements().size() > 0) {
        return getParentDemandElements().front()->getID();
    } else if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getID();
    } else if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getID();
    } else {
        throw ProcessError("Invalid parent");
    }
}


double
GNEStop::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEStop::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().at(0)->getCenteringBoundary();
    } else if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myDemandElementGeometry.getShape().size() > 0) {
        Boundary b = myDemandElementGeometry.getShape().getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEStop::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // declare flag to enable or disable draw person plan
    bool drawPersonPlan = false;
    if (myTagProperty.isStop()) {
        if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
                myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this)) {
            drawPersonPlan = true;
        }
    } else if (myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentDemandElements().front())) {
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedPerson() == getParentDemandElements().front()) {
        drawPersonPlan = true;
    } else if (!myNet->getViewNet()->getInspectedAttributeCarriers().empty() &&
               (myNet->getViewNet()->getInspectedAttributeCarriers().front()->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT))) {
        drawPersonPlan = true;
    }
    // check if stop can be drawn
    if (drawPersonPlan) {
        // Obtain exaggeration of the draw
        const double exaggeration = getExaggeration(s);
        // declare value to save stop color
        RGBColor stopColor;
        // Set color
        if (drawUsingSelectColor()) {
            stopColor = s.colorSettings.selectedRouteColor;
        } else {
            stopColor = s.colorSettings.stopColor;
        }
        // get lane
        const auto& stopLane = getParentLanes().size() > 0 ? getParentLanes().front() : nullptr;
        // get lane width
        const double width = stopLane ? stopLane->getParentEdge()->getNBEdge()->getLaneWidth(stopLane->getIndex()) * 0.5 : exaggeration * 0.8;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a layer matrix
        GLHelper::pushMatrix();
        // set Color
        GLHelper::setColor(stopColor);
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // draw depending of details
        if (s.drawDetail(s.detailSettings.stopsDetails, exaggeration) && stopLane) {
            // Draw top and bot lines using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myDemandElementGeometry.getShape(),
                                   myDemandElementGeometry.getShapeRotations(),
                                   myDemandElementGeometry.getShapeLengths(),
                                   exaggeration * 0.1, 0, width);
            GLHelper::drawBoxLines(myDemandElementGeometry.getShape(),
                                   myDemandElementGeometry.getShapeRotations(),
                                   myDemandElementGeometry.getShapeLengths(),
                                   exaggeration * 0.1, 0, width * -1);
            // Add a detail matrix
            GLHelper::pushMatrix();
            // move to geometry front
            glTranslated(myDemandElementGeometry.getShape().back().x(), myDemandElementGeometry.getShape().back().y(), 0.1);
            // rotate
            if (myDemandElementGeometry.getShapeRotations().size() > 0) {
                glRotated(myDemandElementGeometry.getShapeRotations().back(), 0, 0, 1);
            }
            // move again
            glTranslated(0, exaggeration * 0.5, 0);
            // draw stop front
            GLHelper::drawBoxLine(Position(0, 0), 0, exaggeration * 0.5, width);
            // move to "S" position
            glTranslated(0, 1, 0.1);
            // only draw text if isn't being drawn for selecting
            if (s.drawForRectangleSelection) {
                GLHelper::setColor(stopColor);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
            } else if (s.drawDetail(s.detailSettings.stopsText, exaggeration)) {
                // draw "S" symbol
                GLHelper::drawText("S", Position(), .1, 2.8, stopColor);
                // move to subtitle positin
                glTranslated(0, 1.4, 0);
                // draw subtitle depending of tag
                GLHelper::drawText("lane", Position(), .1, 1, stopColor, 180);
            }
            // pop detail matrix
            GLHelper::popMatrix();
            // draw geometry points
            drawGeometryPoints(s, stopColor);
        } else {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration taked from stoppingPlace parent
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, width);
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // Draw name if isn't being drawn for selecting
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myDemandElementGeometry.getShape(),
                    width, exaggeration, true, true);
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myDemandElementGeometry.getShape(),
                    width, exaggeration, true, true);
        }
        // draw person parent if this stop if their first person plan child
        if ((getParentDemandElements().size() == 1) && getParentDemandElements().front()->getChildDemandElements().front() == this) {
            getParentDemandElements().front()->drawGL(s);
        }
    }
}


void
GNEStop::computePathElement() {
    // nothing to compute
}


void
GNEStop::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


void
GNEStop::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


GNELane*
GNEStop::getFirstPathLane() const {
    // check if stop is placed over a busStop
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentLanes().front();
    }
}


GNELane*
GNEStop::getLastPathLane() const {
    // first and last path lane are the same
    return getFirstPathLane();
}


std::string
GNEStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DURATION:
            if (parametersSet & STOP_DURATION_SET) {
                return time2string(duration);
            } else {
                return "";
            }
        case SUMO_ATTR_UNTIL:
            if (parametersSet & STOP_UNTIL_SET) {
                return time2string(until);
            } else {
                return "";
            }
        case SUMO_ATTR_EXTENSION:
            if (parametersSet & STOP_EXTENSION_SET) {
                return time2string(extension);
            } else {
                return "";
            }
        case SUMO_ATTR_TRIGGERED:
            // this is an special case
            if (parametersSet & STOP_TRIGGER_SET) {
                return "1";
            } else {
                return "0";
            }
        case SUMO_ATTR_CONTAINER_TRIGGERED:
            // this is an special case
            if (parametersSet & STOP_CONTAINER_TRIGGER_SET) {
                return "1";
            } else {
                return "0";
            }
        case SUMO_ATTR_EXPECTED:
            if (parametersSet & STOP_EXPECTED_SET) {
                return toString(awaitedPersons);
            } else {
                return "";
            }
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            if (parametersSet & STOP_EXPECTED_CONTAINERS_SET) {
                return toString(awaitedContainers);
            } else {
                return "";
            }
        case SUMO_ATTR_PARKING:
            return toString(parking);
        case SUMO_ATTR_ACTTYPE:
            return actType;
        case SUMO_ATTR_TRIP_ID:
            if (parametersSet & STOP_TRIP_ID_SET) {
                return tripId;
            } else {
                return "";
            }
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
            return getParentAdditionals().front()->getID();
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_STARTPOS:
            return toString(startPos);
        case SUMO_ATTR_ENDPOS:
            return toString(endPos);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(friendlyPos);
        case SUMO_ATTR_POSITION_LAT:
            if (posLat == INVALID_DOUBLE) {
                return "";
            } else {
                return toString(posLat);
            }
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARENT:
            return getParentDemandElements().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEStop::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            return startPos;
        case SUMO_ATTR_ENDPOS:
            return endPos;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNEStop::getAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
}


void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
        case SUMO_ATTR_TRIGGERED:
        case SUMO_ATTR_CONTAINER_TRIGGERED:
        case SUMO_ATTR_EXPECTED:
        case SUMO_ATTR_EXPECTED_CONTAINERS:
        case SUMO_ATTR_PARKING:
        case SUMO_ATTR_ACTTYPE:
        case SUMO_ATTR_TRIP_ID:
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_POSITION_LAT:
        //
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStop::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            if (canParse<SUMOTime>(value)) {
                return parse<SUMOTime>(value) >= 0;
            } else {
                return false;
            }
        case SUMO_ATTR_TRIGGERED:
            return canParse<bool>(value);
        case SUMO_ATTR_CONTAINER_TRIGGERED:
            return canParse<bool>(value);
        case SUMO_ATTR_EXPECTED:
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            if (value.empty()) {
                return true;
            } else {
                std::vector<std::string> IDs = parse<std::vector<std::string>>(value);
                for (const auto& i : IDs) {
                    if (SUMOXMLDefinitions::isValidVehicleID(i) == false) {
                        return false;
                    }
                }
                return true;
            }
        case SUMO_ATTR_PARKING:
            return canParse<bool>(value);
        case SUMO_ATTR_ACTTYPE:
            return true;
        case SUMO_ATTR_TRIP_ID:
            return SUMOXMLDefinitions::isValidVehicleID(value);
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_CONTAINER_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_CHARGING_STATION:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, value, false) != nullptr);
        case SUMO_ATTR_PARKING_AREA:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
            if (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(parse<double>(value), endPos, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPos);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(startPos, parse<double>(value), getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPos);
            } else {
                return false;
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_POSITION_LAT:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            newParametersSet |= STOP_DURATION_SET;
            break;
        case SUMO_ATTR_UNTIL:
            newParametersSet |= STOP_UNTIL_SET;
            break;
        case SUMO_ATTR_EXTENSION:
            newParametersSet |= STOP_EXTENSION_SET;
            break;
        case SUMO_ATTR_EXPECTED:
            newParametersSet |= STOP_TRIGGER_SET;
            break;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            newParametersSet |= STOP_CONTAINER_TRIGGER_SET;
            break;
        case SUMO_ATTR_PARKING:
            newParametersSet |= STOP_PARKING_SET;
            break;
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, parametersSet, newParametersSet), true);
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        default:
            break;
    }
}


void
GNEStop::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            newParametersSet &= ~STOP_DURATION_SET;
            break;
        case SUMO_ATTR_UNTIL:
            newParametersSet &= ~STOP_UNTIL_SET;
            break;
        case SUMO_ATTR_EXTENSION:
            newParametersSet &= ~STOP_EXTENSION_SET;
            break;
        case SUMO_ATTR_EXPECTED:
            newParametersSet &= ~STOP_TRIGGER_SET;
            break;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            newParametersSet &= ~STOP_CONTAINER_TRIGGER_SET;
            break;
        case SUMO_ATTR_PARKING:
            newParametersSet &= ~STOP_PARKING_SET;
            break;
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, parametersSet, newParametersSet), true);
}


bool
GNEStop::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        // Currently stops parents cannot be edited
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
            return false;
        case SUMO_ATTR_DURATION:
            return (parametersSet & STOP_DURATION_SET) != 0;
        case SUMO_ATTR_UNTIL:
            return (parametersSet & STOP_UNTIL_SET) != 0;
        case SUMO_ATTR_EXTENSION:
            return (parametersSet & STOP_EXTENSION_SET) != 0;
        case SUMO_ATTR_EXPECTED:
            return (parametersSet & STOP_TRIGGER_SET) != 0;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            return (parametersSet & STOP_CONTAINER_TRIGGER_SET) != 0;
        case SUMO_ATTR_PARKING:
            return (parametersSet & STOP_PARKING_SET) != 0;
        default:
            return true;
    }
}


std::string
GNEStop::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStop::getHierarchyName() const {
    if (getParentAdditionals().size() > 0) {
        return "vehicle stop: " + getParentAdditionals().front()->getTagStr();
    } else {
        return "vehicle stop: lane";
    }
}


const std::map<std::string, std::string>&
GNEStop::getACParametersMap() const {
    return getParametersMap();
}

double
GNEStop::getStartGeometryPositionOverLane() const {
    double fixedPos = startPos;
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    if (fixedPos < 0) {
        fixedPos += len;
    }
    return fixedPos * getParentLanes().front()->getLengthGeometryFactor();
}


double
GNEStop::getEndGeometryPositionOverLane() const {
    double fixedPos = endPos;
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    if (fixedPos < 0) {
        fixedPos += len;
    }
    return fixedPos * getParentLanes().front()->getLengthGeometryFactor();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_DURATION:
            if (value.empty()) {
                parametersSet &= ~STOP_DURATION_SET;
            } else {
                duration = string2time(value);
                parametersSet |= STOP_DURATION_SET;
            }
            break;
        case SUMO_ATTR_UNTIL:
            if (value.empty()) {
                parametersSet &= ~STOP_UNTIL_SET;
            } else {
                until = string2time(value);
                parametersSet |= STOP_UNTIL_SET;
            }
            break;
        case SUMO_ATTR_EXTENSION:
            if (value.empty()) {
                parametersSet &= ~STOP_EXTENSION_SET;
            } else {
                extension = string2time(value);
                parametersSet |= STOP_EXTENSION_SET;
            }
            break;
        case SUMO_ATTR_TRIGGERED:
            triggered = parse<bool>(value);
            // this is an special case: only if SUMO_ATTR_TRIGGERED is true, it will be written in XML
            if (triggered) {
                parametersSet |= STOP_TRIGGER_SET;
            } else {
                parametersSet &= ~STOP_TRIGGER_SET;
            }
            break;
        case SUMO_ATTR_CONTAINER_TRIGGERED:
            containerTriggered = parse<bool>(value);
            // this is an special case: only if SUMO_ATTR_CONTAINER_TRIGGERED is true, it will be written in XML
            if (containerTriggered) {
                parametersSet |= STOP_CONTAINER_TRIGGER_SET;
            } else {
                parametersSet &= ~STOP_CONTAINER_TRIGGER_SET;
            }
            break;
        case SUMO_ATTR_EXPECTED:
            if (value.empty()) {
                parametersSet &= ~STOP_EXPECTED_SET;
            } else {
                awaitedPersons = parse<std::set<std::string> >(value);
                parametersSet |= STOP_EXPECTED_SET;
            }
            break;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            if (value.empty()) {
                parametersSet &= ~STOP_EXPECTED_CONTAINERS_SET;
            } else {
                awaitedContainers = parse<std::set<std::string> >(value);
                parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
            }
            break;
        case SUMO_ATTR_PARKING:
            parking = parse<bool>(value);
            break;
        case SUMO_ATTR_ACTTYPE:
            actType = value;
            break;
        case SUMO_ATTR_TRIP_ID:
            if (value.empty()) {
                parametersSet &= ~STOP_TRIP_ID_SET;
            } else {
                tripId = value;
                parametersSet |= STOP_TRIP_ID_SET;
            }
            break;
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            replaceAdditionalParent(SUMO_TAG_BUS_STOP, value);
            updateGeometry();
            break;
        case SUMO_ATTR_CONTAINER_STOP:
            replaceAdditionalParent(SUMO_TAG_CONTAINER_STOP, value);
            updateGeometry();
            break;
        case SUMO_ATTR_CHARGING_STATION:
            replaceAdditionalParent(SUMO_TAG_CHARGING_STATION, value);
            updateGeometry();
            break;
        case SUMO_ATTR_PARKING_AREA:
            replaceAdditionalParent(SUMO_TAG_PARKING_AREA, value);
            updateGeometry();
            break;
        // specific of Stops over lanes
        case SUMO_ATTR_LANE:
            replaceDemandParentLanes(value);
            updateGeometry();
            break;
        case SUMO_ATTR_STARTPOS:
            startPos = parse<double>(value);
            updateGeometry();
            break;
        case SUMO_ATTR_ENDPOS:
            endPos = parse<double>(value);
            updateGeometry();
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            friendlyPos = parse<bool>(value);
            break;
        case SUMO_ATTR_POSITION_LAT:
            if (value.empty()) {
                posLat = INVALID_DOUBLE;
                parametersSet &= ~ STOP_POSLAT_SET;
            } else {
                posLat = parse<double>(value);
                parametersSet |= STOP_POSLAT_SET;
            }
            break;
        //
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::toogleAttribute(SumoXMLAttr key, const bool value) {
/*
    parametersSet = enabledAttributes;
*/
}


void
GNEStop::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) {
        // change only start position
        startPos = moveResult.newFirstPos;
        // adjust startPos
        if (startPos > (getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS)) {
            startPos = (getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS);
        }
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) {
        // change only end position
        endPos = moveResult.newFirstPos;
        // adjust endPos
        if (endPos < (getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS)) {
            endPos = (getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS);
        }
    } else {
        // change both position
        startPos = moveResult.newFirstPos;
        endPos = moveResult.newSecondPos;
        // set lateral offset
        myMoveElementLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    updateGeometry();
}


void
GNEStop::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
    // set attributes depending of operation type
    if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) {
        // set only start position
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos)));
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) {
        // set only end position
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos)));
    } else {
        // set both
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos)));
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos)));
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}


void
GNEStop::drawGeometryPoints(const GUIVisualizationSettings& s, const RGBColor& baseColor) const {
    // first check that we're in move mode and shift key is pressed
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
            (myNet->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_MOVE) &&
            myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        // calculate new color
        const RGBColor color = baseColor.changedBrightness(-50);
        // push matrix
        GLHelper::pushMatrix();
        // translated to front
        glTranslated(0, 0, 0.1);
        // set color
        GLHelper::setColor(color);
        // draw points
        if (startPos != INVALID_DOUBLE) {
            // push geometry point matrix
            GLHelper::pushMatrix();
            glTranslated(myDemandElementGeometry.getShape().front().x(), myDemandElementGeometry.getShape().front().y(), 0.1);
            // draw geometry point
            GLHelper::drawFilledCircle(s.neteditSizeSettings.additionalGeometryPointRadius, s.getCircleResolution());
            // pop geometry point matrix
            GLHelper::popMatrix();
        }
        if (endPos != INVALID_DOUBLE) {
            // push geometry point matrix
            GLHelper::pushMatrix();
            glTranslated(myDemandElementGeometry.getShape().back().x(), myDemandElementGeometry.getShape().back().y(), 0.1);
            // draw geometry point
            GLHelper::drawFilledCircle(s.neteditSizeSettings.additionalGeometryPointRadius, s.getCircleResolution());
            // pop geometry point matrix
            GLHelper::popMatrix();
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}

/****************************************************************************/
