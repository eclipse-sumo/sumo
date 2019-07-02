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
    GNEDemandElement(stopParent, viewNet, stopParent->getTagProperty().isPerson()? GLO_PERSONSTOP : GLO_STOP, tag, 
    {}, {}, {}, {stoppingPlace}, {stopParent}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myFriendlyPosition(false) {
}


GNEStop::GNEStop(GNEViewNet* viewNet, const SUMOVehicleParameter::Stop& stopParameter, GNELane* lane, bool friendlyPosition, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, 
        stopParent->getTagProperty().isPerson()? GLO_PERSONSTOP : GLO_STOP, 
        stopParent->getTagProperty().isPerson()? SUMO_TAG_PERSONSTOP_LANE : SUMO_TAG_STOP_LANE,
    {}, {lane}, {}, {}, {stopParent}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myFriendlyPosition(friendlyPosition) {
}


GNEStop::~GNEStop() {}


SUMOVehicleClass 
GNEStop::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


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


bool
GNEStop::isDemandElementValid() const {
    // only Stops placed over lanes can be invalid
    if(myTagProperty.getTag() != SUMO_TAG_STOP_LANE) {
        return true;
    } else if (myFriendlyPosition) {
        // with friendly position enabled position are "always fixed"
        return true;
    } else {
        // obtain lane length
        double laneLenght = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
        // declare a copy of start and end positions
        double startPosCopy = startPos;
        double endPosCopy = endPos;
        // check if position has to be fixed
        if (startPosCopy < 0) {
            startPosCopy += laneLenght;
        }
        if (endPosCopy < 0) {
            endPosCopy += laneLenght;
        }
        // check values
        if (!(parametersSet & STOP_START_SET) && !(parametersSet & STOP_END_SET)) {
            return true;
        } else if (!(parametersSet & STOP_START_SET)) {
            return (endPosCopy <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
        } else if (!(parametersSet & STOP_END_SET)) {
            return (startPosCopy >= 0);
        } else {
            return ((startPosCopy >= 0) && (endPosCopy <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) && ((endPosCopy - startPosCopy) >= POSITION_EPS));
        }
    }
}


std::string 
GNEStop::getDemandElementProblem() const {
    // declare a copy of start and end positions
    double startPosCopy = startPos;
    double endPosCopy = endPos;
    // obtain lane lenght
    double laneLenght = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    // check if position has to be fixed
    if (startPosCopy < 0) {
        startPosCopy += laneLenght;
    }
    if (endPosCopy < 0) {
        endPosCopy += laneLenght;
    }
    // declare variables
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if (startPosCopy < 0) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    } else if (startPosCopy > getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
    }
    if (endPosCopy < 0) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    } else if (endPosCopy > getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) {
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


void
GNEStop::startGeometryMoving() {
    // only start geometry moving if stop is placed over a lane
    if (getLaneParents().size() > 0) {
        // always save original position over view
        myStopMove.originalViewPosition = getPositionInView();
        // save start and end position
        myStopMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_STARTPOS);
        myStopMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
        // save current centering boundary
        myStopMove.movingGeometryBoundary = getCenteringBoundary();
    }
}


void
GNEStop::endGeometryMoving() {
    // check that stop is placed over a lane and endGeometryMoving was called only once
    if ((getLaneParents().size() > 0) && myStopMove.movingGeometryBoundary.isInitialised()) {
        // reset myMovingGeometryBoundary
        myStopMove.movingGeometryBoundary.reset();
    }
}


void
GNEStop::moveGeometry(const Position& offset) {
    // only move if at leats start or end positions is defined
    if ((getLaneParents().size() > 0) && ((parametersSet & STOP_START_SET) || (parametersSet & STOP_END_SET))) {
        // Calculate new position using old position
        Position newPosition = myStopMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        double offsetLane = getLaneParents().front()->getGeometry().shape.nearest_offset_to_point2D(newPosition, false) - getLaneParents().front()->getGeometry().shape.nearest_offset_to_point2D(myStopMove.originalViewPosition, false);
        // check if both position has to be moved
        if ((parametersSet & STOP_START_SET) && (parametersSet & STOP_END_SET)) {
            // calculate stoppingPlace lenght and lane lenght (After apply geometry factor)
            double stoppingPlaceLenght = fabs(parse<double>(myStopMove.secondOriginalPosition) - parse<double>(myStopMove.firstOriginalLanePosition));
            double laneLengt = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
            // avoid changing stopping place's lenght
            if ((parse<double>(myStopMove.firstOriginalLanePosition) + offsetLane) < 0) {
                startPos = 0;
                endPos = stoppingPlaceLenght;
            } else if ((parse<double>(myStopMove.secondOriginalPosition) + offsetLane) > laneLengt) {
                startPos = laneLengt - stoppingPlaceLenght;
                endPos = laneLengt;
            } else {
                startPos = parse<double>(myStopMove.firstOriginalLanePosition) + offsetLane;
                endPos = parse<double>(myStopMove.secondOriginalPosition) + offsetLane;
            }
        } else {
            // check if start position must be moved
            if ((parametersSet & STOP_START_SET)) {
                startPos = parse<double>(myStopMove.firstOriginalLanePosition) + offsetLane;
            }
            // check if start position must be moved
            if ((parametersSet & STOP_END_SET)) {
                endPos = parse<double>(myStopMove.secondOriginalPosition) + offsetLane;
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
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_STARTPOS, toString(startPos), true, myStopMove.firstOriginalLanePosition));
        }
        if (parametersSet & STOP_END_SET) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ENDPOS, toString(endPos), true, myStopMove.secondOriginalPosition));
        }
        undoList->p_end();
    }
}


void
GNEStop::updateGeometry() {
    // Clear all containers
    myStopGeometry.clearGeometry();
    //only update Stops over lanes, because other uses the geometry of stopping place parent
    if (getLaneParents().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myStopGeometry.shape = getLaneParents().front()->getGeometry().shape.getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());
        // Get calculate lenghts and rotations
        myStopGeometry.calculateShapeRotationsAndLengths();
    } else if (getAdditionalParents().size() > 0) {
        // copy geometry of additional
        myStopGeometry.shape = getAdditionalParents().at(0)->getAdditionalGeometry().shape;
        myStopGeometry.shapeLengths = getAdditionalParents().at(0)->getAdditionalGeometry().shapeLengths;
        myStopGeometry.shapeRotations = getAdditionalParents().at(0)->getAdditionalGeometry().shapeRotations;
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
            return getLaneParents().front()->getGeometry().shape.positionAtOffset(getLaneParents().front()->getGeometry().shape.length() / 2);
        } else if (!(parametersSet & STOP_START_SET)) {
            return getLaneParents().front()->getGeometry().shape.positionAtOffset(end);
        } else if (!(parametersSet & STOP_END_SET)) {
            return getLaneParents().front()->getGeometry().shape.positionAtOffset(start);
        } else {
            return getLaneParents().front()->getGeometry().shape.positionAtOffset((start + end) / 2.0);
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


Boundary
GNEStop::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (getAdditionalParents().size() > 0) {
        return getAdditionalParents().at(0)->getCenteringBoundary();
    } else if (myStopMove.movingGeometryBoundary.isInitialised()) {
        return myStopMove.movingGeometryBoundary;
    } else if (myStopGeometry.shape.size() > 0) {
        Boundary b = myStopGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptionsNetwork().showDemandElements() && myViewNet->getViewOptionsDemand().showNonInspectedDemandElements(this)) {
        // Obtain exaggeration of the draw
        const double exaggeration = s.addSize.getExaggeration(s, this);
        // declare value to save stop color
        RGBColor stopColor;
        // Set color
        if (drawUsingSelectColor()) {
            stopColor = s.selectedAdditionalColor;
        } else if (myTagProperty.isPersonStop()) {
            stopColor = s.SUMO_color_personStops;
        } else {
            stopColor = s.SUMO_color_stops;
        }
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.selectedAdditionalColor);
        } else if (myTagProperty.isPersonStop()) {
            GLHelper::setColor(s.SUMO_color_personStops);
        } else {
            GLHelper::setColor(s.SUMO_color_stops);
        }
        // draw lines depending if it's placed over a lane or over a stoppingPlace
        if (getLaneParents().size() > 0) {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myStopGeometry.shape, myStopGeometry.shapeRotations, myStopGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            GLHelper::drawBoxLines(myStopGeometry.shape, myStopGeometry.shapeRotations, myStopGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * -0.5);
        } else {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myStopGeometry.shape, myStopGeometry.shapeRotations, myStopGeometry.shapeLengths, exaggeration * 0.1, 0, exaggeration * -1);
            GLHelper::drawBoxLines(myStopGeometry.shape, myStopGeometry.shapeRotations, myStopGeometry.shapeLengths, exaggeration * 0.1, 0, exaggeration);
        }
        // pop draw matrix
        glPopMatrix();
        // Add a draw matrix
        glPushMatrix();
        // move to geometry front
        glTranslated(myStopGeometry.shape.back().x(), myStopGeometry.shape.back().y(), getType());
        glRotated(myStopGeometry.shapeRotations.back(), 0, 0, 1);
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
        // obtain text color

        // draw "S" symbol
        GLHelper::drawText("S", Position(), .1, 2.8, stopColor);
        // move to subtitle positin
        glTranslated(0, 1.4, 0);
        // draw subtitle depending of tag
        if (myTagProperty.getTag() == SUMO_TAG_STOP_BUSSTOP) {
            GLHelper::drawText("busStop", Position(), .1, .5, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_CONTAINERSTOP) {
            GLHelper::drawText("container", Position(), .1, .5, stopColor, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Stop", Position(), .1, .5, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_CHARGINGSTATION) {
            GLHelper::drawText("charging", Position(), .1, .5, stopColor, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Station", Position(), .1, .5, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_PARKINGAREA) {
            GLHelper::drawText("parking", Position(), .1, .5, stopColor, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("Area", Position(), .1, .5, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_STOP_LANE) {
            GLHelper::drawText("lane", Position(), .1, 1, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_PERSONSTOP_LANE) {
            GLHelper::drawText("person", Position(), .1, .7, stopColor, 180);
        } else if (myTagProperty.getTag() == SUMO_TAG_PERSONSTOP_BUSSTOP) {
            GLHelper::drawText("person", Position(), .1, .5, stopColor, 180);
            glTranslated(0, 0.5, 0);
            GLHelper::drawText("busStop", Position(), .1, .5, stopColor, 180);
        }
        // pop draw matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
            // draw dooted contour depending if it's placed over a lane or over a stoppingPlace
            if (getLaneParents().size() > 0) {
                GLHelper::drawShapeDottedContour(getType(), myStopGeometry.shape, getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            } else {
                GLHelper::drawShapeDottedContour(getType(), myStopGeometry.shape, exaggeration);
            }
        }
        // Pop name
        glPopName();
        // draw person parent if this stop if their first person plan child
        if ((getDemandElementParents().size() == 1) && getDemandElementParents().front()->isFirstDemandElementChild(this)) {
            getDemandElementParents().front()->drawGL(s);
        }
    }
}


void
GNEStop::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_STOP);
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
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_STOP);
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
        case GNE_ATTR_FIRST_CHILD:
            return toString(getDemandElementParents().front()->isFirstDemandElementChild(this));
        case GNE_ATTR_LAST_CHILD:
            return toString(getDemandElementParents().front()->isLastDemandElementChild(this));
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double 
GNEStop::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            if (parametersSet & STOP_START_SET) {
                return startPos;
            } else {
                return 0;
            }
        case SUMO_ATTR_ENDPOS:
            if (parametersSet & STOP_END_SET) {
                return endPos;
            } else {
                return getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
            }
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
GNEStop::isAttributeSet(SumoXMLAttr /*key*/) const {
    return true;
}


bool
GNEStop::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_EXPECTED:
            return (parametersSet & STOP_TRIGGER_SET) != 0;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            return (parametersSet & STOP_CONTAINER_TRIGGER_SET) != 0;
        default:
            return true;
    };
}


void 
GNEStop::setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int parametersSetCopy = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (attr) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parametersSetCopy = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parametersSetCopy ^= VEHPARS_END_SET;
            parametersSetCopy |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parametersSetCopy |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set period
            parametersSetCopy |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set probability
            parametersSetCopy |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), parametersSet, parametersSetCopy));
}


std::string
GNEStop::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStop::getHierarchyName() const {
    std::string stopType;
    // first distinguish between person stops and vehicles stops
    if (getDemandElementParents().front()->getTagProperty().isPerson()) {
        stopType ="person stop";
    } else {
        stopType ="vehicle stop";
    }
    if (getAdditionalParents().size() > 0) {
        return stopType + ": " + getAdditionalParents().front()->getTagStr();
    } else {
        return stopType + ": lane";
    }
}


double
GNEStop::getStartGeometryPositionOverLane() const {
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


double
GNEStop::getEndGeometryPositionOverLane() const {
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

// ---------------------------------------------------------------------------
// GNEAdditional::StopGeometry - methods
// ---------------------------------------------------------------------------

GNEStop::StopGeometry::StopGeometry() {}


void
GNEStop::StopGeometry::clearGeometry() {
    shape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
}


void
GNEStop::StopGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {
            // Obtain first position
            const Position& f = shape[i];
            // Obtain next position
            const Position& s = shape[i + 1];
            // Save distance between position into myShapeLengths
            shapeLengths.push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            shapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
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
}

/****************************************************************************/
