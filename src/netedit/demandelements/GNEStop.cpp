/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// Representation of Stops in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <cmath>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/additionals/GNEStoppingPlace.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNEAdditional.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEStop.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStop::GNEStop(SumoXMLTag tag, GNEViewNet* viewNet, const SUMOVehicleParameter::Stop& stopParameter, GNEAdditional* stoppingPlace, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, GLO_STOP, tag, {}, {}, {}, {}, {stoppingPlace}, {stopParent}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myFriendlyPosition(false) {
}


GNEStop::GNEStop(GNEViewNet* viewNet, const SUMOVehicleParameter::Stop& stopParameter, GNELane* lane, bool friendlyPosition, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, GLO_STOP, SUMO_TAG_STOP_LANE, {}, {lane}, {}, {}, {}, {stopParent}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myFriendlyPosition(friendlyPosition) {
}


GNEStop::~GNEStop() {}


std::string
GNEStop::getBegin() const {
    return "";
}


const RGBColor&
GNEStop::getColor() const {
    return RGBColor::BLACK;
}


void
GNEStop::writeDemandElement(OutputDevice& device) const {
    write(device);
}


void
GNEStop::moveGeometry(const Position& offset) {
    // only move if at leats start or end positions is defined
    if ((getLaneParents().size() > 0) && ((parametersSet & STOP_START_SET) || (parametersSet & STOP_END_SET))) {
        // Calculate new position using old position
        Position newPosition = myMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        double offsetLane = getLaneParents().front()->getShape().nearest_offset_to_point2D(newPosition, false) - getLaneParents().front()->getShape().nearest_offset_to_point2D(myMove.originalViewPosition, false);
        // check if both position has to be moved
        if ((parametersSet & STOP_START_SET) && (parametersSet & STOP_END_SET)) {
            // calculate stoppingPlace lenght and lane lenght (After apply geometry factor)
            double stoppingPlaceLenght = fabs(parse<double>(myMove.secondOriginalPosition) - parse<double>(myMove.firstOriginalLanePosition));
            double laneLengt = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
            // avoid changing stopping place's lenght
            if ((parse<double>(myMove.firstOriginalLanePosition) + offsetLane) < 0) {
                startPos = 0;
                endPos = stoppingPlaceLenght;
                std::cout << "A" << std::endl;
            } else if ((parse<double>(myMove.secondOriginalPosition) + offsetLane) > laneLengt) {
                startPos = laneLengt - stoppingPlaceLenght;
                endPos = laneLengt;
                std::cout << "B" << std::endl;
            } else {
                startPos = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
                endPos = parse<double>(myMove.secondOriginalPosition) + offsetLane;
                std::cout << toString(offsetLane) << " | " << toString(startPos) << " " << toString(endPos) << std::endl;
            }
        } else {
            // check if start position must be moved
            if ((parametersSet & STOP_START_SET)) {
                startPos = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
            }
            // check if start position must be moved
            if ((parametersSet & STOP_END_SET)) {
                endPos = parse<double>(myMove.secondOriginalPosition) + offsetLane;
            }
        }
        // Update geometry
        updateGeometry();
    }
}


void
GNEStop::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if at leats start or end positions is defined
    if ((getLaneParents().size() > 0) && ((parametersSet & STOP_START_SET) || (parametersSet & STOP_END_SET))) {
        undoList->p_begin("position of " + getTagStr());
        if (parametersSet & STOP_START_SET) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_STARTPOS, toString(startPos), true, myMove.firstOriginalLanePosition));
        }
        if (parametersSet & STOP_END_SET) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ENDPOS, toString(endPos), true, myMove.secondOriginalPosition));
        }
        undoList->p_end();
    }
}


void
GNEStop::updateGeometry() {
    // Clear all containers
    myGeometry.clearGeometry();
    //only update Stops over lanes, because other uses the geometry of stopping place parent
    if (getLaneParents().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myGeometry.shape = getLaneParents().front()->getShape().getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());
        // Get calculate lenghts and rotations
        myGeometry.calculateShapeRotationsAndLengths();
    } else if (getAdditionalParents().size() > 0) {
        // copy geometry of additional
        myGeometry.shape = getAdditionalParents().at(0)->getAdditionalGeometry().shape;
        myGeometry.shapeLengths = getAdditionalParents().at(0)->getAdditionalGeometry().shapeLengths;
        myGeometry.shapeRotations = getAdditionalParents().at(0)->getAdditionalGeometry().shapeRotations;
    }
}


Position
GNEStop::getPositionInView() const {
    if (getLaneParents().size() > 0) {
        // calculate start and end positions as absolute values
        double start = fabs(parametersSet & STOP_START_SET ? startPos : 0);
        double end = fabs(parametersSet & STOP_END_SET ? endPos : getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
        // obtain position in view depending if both positions are defined
        if (!(parametersSet & STOP_START_SET) && !(parametersSet & STOP_END_SET)) {
            return getLaneParents().front()->getShape().positionAtOffset(getLaneParents().front()->getShape().length() / 2);
        } else if (!(parametersSet & STOP_START_SET)) {
            return getLaneParents().front()->getShape().positionAtOffset(end);
        } else if (!(parametersSet & STOP_END_SET)) {
            return getLaneParents().front()->getShape().positionAtOffset(start);
        } else {
            return getLaneParents().front()->getShape().positionAtOffset((start + end) / 2.0);
        }
    } else if (getDemandElementParents().size() > 0) {
        return getDemandElementParents().front()->getPositionInView();
    } else {
        throw ProcessError("Invalid Stop parent");
    }
}


std::string
GNEStop::getParentName() const {
    if (getDemandElementParents().size() > 0) {
        return getDemandElementParents().front()->getID();
    } else if (getAdditionalParents().size() > 0) {
        return getAdditionalParents().front()->getID();
    } else if (getLaneParents().size() > 0) {
        return getLaneParents().front()->getID();
    } else {
        throw ProcessError("Invalid parent");
    }
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptions().showDemandElements()) {
        // Obtain exaggeration of the draw
        const double exaggeration = s.addSize.getExaggeration(s, this);
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.SUMO_color_stops);
        }
        // draw lines depending if it's placed over a lane or over a stoppingPlace
        if (getLaneParents().size() > 0) {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * -0.5);
        } else {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, exaggeration * 0.1, 0, exaggeration * -1);
            GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, exaggeration * 0.1, 0, exaggeration);
        }
        // pop draw matrix
        glPopMatrix();
        // Add a draw matrix
        glPushMatrix();
        // move to geometry front
        glTranslated(myGeometry.shape.back().x(), myGeometry.shape.back().y(), getType());
        glRotated(myGeometry.shapeRotations.back(), 0, 0, 1);
        // draw front of Stop depending if it's placed over a lane or over a stoppingPlace
        if (getLaneParents().size() > 0) {
            // draw front of Stop
            GLHelper::drawBoxLine(Position(0, 0), 0, exaggeration * 0.5,
                                  getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
        } else {
            // draw front of Stop
            GLHelper::drawBoxLine(Position(0, 0), 0, exaggeration * 0.5, exaggeration);
        }
        // move to "S" position
        glTranslated(0, 1, 0);
        // draw "S" symbol
        GLHelper::drawText("S", Position(), .1, 2.8, s.SUMO_color_stops);
        // move to subtitle positin
        glTranslated(0, 1.4, 0);
        // draw subtitle depending of tag
        if (myTagProperty.getTag() == SUMO_TAG_STOP_BUSSTOP) {
            GLHelper::drawText("busStop", Position(), .1, .5, s.SUMO_color_stops, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_CONTAINERSTOP) {
            GLHelper::drawText("container", Position(), .1, .5, s.SUMO_color_stops, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Stop", Position(), .1, .5, s.SUMO_color_stops, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_CHARGINGSTATION) {
            GLHelper::drawText("charging", Position(), .1, .5, s.SUMO_color_stops, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Station", Position(), .1, .5, s.SUMO_color_stops, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_PARKINGAREA) {
            GLHelper::drawText("parking", Position(), .1, .5, s.SUMO_color_stops, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Area", Position(), .1, .5, s.SUMO_color_stops, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_LANE) {
            GLHelper::drawText("lane", Position(), .1, 1, s.SUMO_color_stops, 180);
        }
        // pop draw matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
            // draw dooted contour depending if it's placed over a lane or over a stoppingPlace
            if (getLaneParents().size() > 0) {
                GLHelper::drawShapeDottedContour(getType(), myGeometry.shape, getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            } else {
                GLHelper::drawShapeDottedContour(getType(), myGeometry.shape, exaggeration);
            }
        }
        // Pop name
        glPopName();
    }
}


void
GNEStop::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEStop::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_DURATION:
            return time2string(duration);
        case SUMO_ATTR_UNTIL:
            return time2string(until);
        case SUMO_ATTR_INDEX:
            if (index == STOP_INDEX_END) {
                return "end";
            } else if (index == STOP_INDEX_FIT) {
                return "fit";
            } else {
                return toString(index);
            }
        case SUMO_ATTR_TRIGGERED:
            if (parametersSet & STOP_TRIGGER_SET) {
                return toString(triggered);
            } else {
                return "";
            }
        case SUMO_ATTR_CONTAINER_TRIGGERED:
            if (parametersSet & STOP_CONTAINER_TRIGGER_SET) {
                return toString(containerTriggered);
            } else {
                return "";
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
            if (parametersSet & STOP_PARKING_SET) {
                return toString(parking);
            } else {
                return "";
            }
        case SUMO_ATTR_ACTTYPE:
            return "";  // CHECK
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
            return getAdditionalParents().front()->getID();
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
            return getLaneParents().front()->getID();
        case SUMO_ATTR_STARTPOS:
            if (parametersSet & STOP_START_SET) {
                return toString(startPos);
            } else {
                return "";
            }
        case SUMO_ATTR_ENDPOS:
            if (parametersSet & STOP_END_SET) {
                return toString(endPos);
            } else {
                return "";
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_INDEX:
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
        //
        case GNE_ATTR_GENERIC:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
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
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
            if (canParse<SUMOTime>(value)) {
                return parse<SUMOTime>(value) >= 0;
            } else {
                return false;
            }
        case SUMO_ATTR_INDEX:
            if ((value == "fit") || (value == "end")) {
                return true;
            } else if (canParse<int>(value)) {
                return (parse<int>(value) >= 0);
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
                return canParse<std::vector<std::string> >(value);
            }
        case SUMO_ATTR_PARKING:
            return canParse<bool>(value);
        case SUMO_ATTR_ACTTYPE:
            return false;  // CHECK
        case SUMO_ATTR_TRIP_ID:
            return SUMOXMLDefinitions::isValidAttribute(value);
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_CONTAINER_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_CHARGING_STATION:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, value, false) != nullptr);
        case SUMO_ATTR_PARKING_AREA:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        // specific of stops over lanes
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
                return GNEStoppingPlace::checkStoppinPlacePosition(value, toString(endPos), getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return GNEStoppingPlace::checkStoppinPlacePosition(toString(startPos), value, getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStop::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_EXPECTED:
            return triggered;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            return containerTriggered;
        default:
            return true;
    };
}


std::string
GNEStop::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStop::getHierarchyName() const {
    if (getAdditionalParents().size() > 0) {
        return "stop: " + getAdditionalParents().front()->getTagStr();
    } else {
        return "stop: lane";
    }
}


double
GNEStop::getStartGeometryPositionOverLane() const {
    if (parametersSet & STOP_START_SET) {
        double fixedPos = endPos;
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    } else {
        return 0;
    }
}


double
GNEStop::getEndGeometryPositionOverLane() const {
    if (parametersSet & STOP_END_SET) {
        double fixedPos = startPos;
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    } else {
        return 0;
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_DURATION:
            duration = string2time(value);
            break;
        case SUMO_ATTR_UNTIL:
            until = string2time(value);
            break;
        case SUMO_ATTR_INDEX:
            if (value == "fit") {
                index = STOP_INDEX_FIT;
            } else if (value == "end") {
                index = STOP_INDEX_END;
            } else {
                index = parse<int>(value);
            }
            break;
        case SUMO_ATTR_TRIGGERED:
            if (value.empty()) {
                parametersSet &= ~STOP_TRIGGER_SET;
            } else {
                triggered = parse<bool>(value);
                parametersSet |= STOP_TRIGGER_SET;
            }
            break;
        case SUMO_ATTR_CONTAINER_TRIGGERED:
            if (value.empty()) {
                parametersSet &= ~STOP_CONTAINER_TRIGGER_SET;
            } else {
                containerTriggered = parse<bool>(value);
                parametersSet |= STOP_CONTAINER_TRIGGER_SET;
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
            if (value.empty()) {
                parametersSet &= ~STOP_PARKING_SET;
            } else {
                parking = parse<bool>(value);
                parametersSet |= STOP_PARKING_SET;
            }
            break;
        case SUMO_ATTR_ACTTYPE:
            // CHECK
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
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
            changeAdditionalParent(this, value, 0);
            break;
        // specific of Stops over lanes
        case SUMO_ATTR_LANE:
            changeLaneParents(this, value);
            break;
        case SUMO_ATTR_STARTPOS:
            if (value.empty()) {
                parametersSet &= ~STOP_START_SET;
            } else {
                startPos = parse<double>(value);
                parametersSet |= STOP_START_SET;
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
                parametersSet &= ~STOP_END_SET;
            } else {
                endPos = parse<double>(value);
                parametersSet |= STOP_END_SET;
            }
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        //
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // check if updated attribute requieres update geometry
    if (myTagProperty.hasAttribute(key) && myTagProperty.getAttributeProperties(key).requiereUpdateGeometry()) {
        updateGeometry();
    }
}

/****************************************************************************/
