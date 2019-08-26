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
#include <netedit/additionals/GNEStoppingPlace.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEStop.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStop::GNEStop(SumoXMLTag tag, GNEViewNet* viewNet, const SUMOVehicleParameter::Stop& stopParameter, GNEAdditional* stoppingPlace, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, stopParent->getTagProperty().isPerson() ? GLO_PERSONSTOP : GLO_STOP, tag,
{}, {}, {}, {stoppingPlace}, {stopParent}, {}, {}, {}, {}, {}),
SUMOVehicleParameter::Stop(stopParameter),
myFriendlyPosition(false) {
}


GNEStop::GNEStop(GNEViewNet* viewNet, const SUMOVehicleParameter::Stop& stopParameter, GNELane* lane, bool friendlyPosition, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet,
                     stopParent->getTagProperty().isPerson() ? GLO_PERSONSTOP : GLO_STOP,
                     stopParent->getTagProperty().isPerson() ? SUMO_TAG_PERSONSTOP_LANE : SUMO_TAG_STOP_LANE,
{}, {lane}, {}, {}, {stopParent}, {}, {}, {}, {}, {}),
SUMOVehicleParameter::Stop(stopParameter),
myFriendlyPosition(friendlyPosition) {
}


GNEStop::~GNEStop() {}


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
    } else if (myFriendlyPosition) {
        // with friendly position enabled position are "always fixed"
        return true;
    } else {
        // obtain lane length
        double laneLength = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
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
    // obtain lane length
    double laneLength = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
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


GNEEdge*
GNEStop::getFromEdge() const {
    if (getAdditionalParents().size() > 0) {
        return &getAdditionalParents().front()->getLaneParents().front()->getParentEdge();
    } else {
        return &getLaneParents().front()->getParentEdge();
    }
}


GNEEdge*
GNEStop::getToEdge() const {
    if (getAdditionalParents().size() > 0) {
        return &getAdditionalParents().front()->getLaneParents().front()->getParentEdge();
    } else {
        return &getLaneParents().front()->getParentEdge();
    }
}


SUMOVehicleClass
GNEStop::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


const RGBColor&
GNEStop::getColor() const {
    if (myTagProperty.isPersonStop()) {
        return myViewNet->getVisualisationSettings()->colorSettings.personStops;
    } else {
        return myViewNet->getVisualisationSettings()->colorSettings.stops;
    }
}


void
GNEStop::compute() {
    // Nothing to compute
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
            // calculate stoppingPlace length and lane length (After apply geometry factor)
            double stoppingPlaceLength = fabs(parse<double>(myStopMove.secondOriginalPosition) - parse<double>(myStopMove.firstOriginalLanePosition));
            double laneLengt = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
            // avoid changing stopping place's length
            if ((parse<double>(myStopMove.firstOriginalLanePosition) + offsetLane) < 0) {
                startPos = 0;
                endPos = stoppingPlaceLength;
            } else if ((parse<double>(myStopMove.secondOriginalPosition) + offsetLane) > laneLengt) {
                startPos = laneLengt - stoppingPlaceLength;
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
        // update person or vehicle frame
        getDemandElementParents().front()->markSegmentGeometryDeprecated();
        getDemandElementParents().front()->updateGeometry();
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
        // update person or vehicle frame
        getDemandElementParents().front()->markSegmentGeometryDeprecated();
        getDemandElementParents().front()->updateGeometry();
    }
}


void
GNEStop::updateGeometry() {
    // Clear all containers
    myDemandElementGeometry.clearGeometry();
    //only update Stops over lanes, because other uses the geometry of stopping place parent
    if (getLaneParents().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myDemandElementGeometry.shape = getLaneParents().front()->getGeometry().shape.getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());
        // Get calculate lengths and rotations
        myDemandElementGeometry.calculateShapeRotationsAndLengths();
    } else if (getAdditionalParents().size() > 0) {
        // copy geometry of additional
        myDemandElementGeometry.shape = getAdditionalParents().at(0)->getAdditionalGeometry().shape;
        myDemandElementGeometry.shapeLengths = getAdditionalParents().at(0)->getAdditionalGeometry().shapeLengths;
        myDemandElementGeometry.shapeRotations = getAdditionalParents().at(0)->getAdditionalGeometry().shapeRotations;
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
    } else if (myDemandElementGeometry.shape.size() > 0) {
        Boundary b = myDemandElementGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // declare flag to enable or disable draw person plan
    bool drawPersonPlan = false;
    if (myTagProperty.isStop() || myTagProperty.isPersonStop()) {
        if (myViewNet->getNetworkViewOptions().showDemandElements() && myViewNet->getDemandViewOptions().showNonInspectedDemandElements(this)) {
            drawPersonPlan = true;
        }
    } else if (myViewNet->getDemandViewOptions().showAllPersonPlans()) {
        drawPersonPlan = true;
    } else if (myViewNet->getDottedAC() == getDemandElementParents().front()) {
        drawPersonPlan = true;
    } else if (myViewNet->getDemandViewOptions().getLockedPerson() == getDemandElementParents().front()) {
        drawPersonPlan = true;
    } else if (myViewNet->getDottedAC() && myViewNet->getDottedAC()->getTagProperty().isPersonPlan() &&
               (myViewNet->getDottedAC()->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT))) {
        drawPersonPlan = true;
    }
    // check if stop can be drawn
    if (drawPersonPlan) {
        // Obtain exaggeration of the draw
        const double exaggeration = s.addSize.getExaggeration(s, this);
        // declare value to save stop color
        RGBColor stopColor;
        // Set color
        if (drawUsingSelectColor()) {
            if (myTagProperty.isPersonStop()) {
                stopColor = s.colorSettings.selectedPersonPlanColor;
            } else {
                stopColor = s.colorSettings.selectedRouteColor;
            }
        } else if (myTagProperty.isPersonStop()) {
            stopColor = s.colorSettings.personStops;
        } else {
            stopColor = s.colorSettings.stops;
        }
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // set Color
        GLHelper::setColor(stopColor);
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // draw depending of details
        if (s.drawDetail(s.detailSettings.stopsDetails, exaggeration) && getLaneParents().size() > 0) {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myDemandElementGeometry.shape, myDemandElementGeometry.shapeRotations, myDemandElementGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            GLHelper::drawBoxLines(myDemandElementGeometry.shape, myDemandElementGeometry.shapeRotations, myDemandElementGeometry.shapeLengths, exaggeration * 0.1, 0,
                                   getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * -0.5);
            // pop draw matrix
            glPopMatrix();
            // Add a draw matrix
            glPushMatrix();
            // move to geometry front
            glTranslated(myDemandElementGeometry.shape.back().x(), myDemandElementGeometry.shape.back().y(), getType());
            glRotated(myDemandElementGeometry.shapeRotations.back(), 0, 0, 1);
            // draw front of Stop depending if it's placed over a lane or over a stoppingPlace
            if (getLaneParents().size() > 0) {
                // draw front of Stop
                GLHelper::drawBoxLine(Position(0, 0), 0, exaggeration * 0.5,
                                      getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
            } else {
                // draw front of Stop
                GLHelper::drawBoxLine(Position(0, 0), 0, exaggeration * 0.5, exaggeration);
            }
            // only draw text if isn't being drawn for selecting
            if (s.drawDetail(s.detailSettings.stopsText, exaggeration) && !s.drawForSelecting) {
                // move to "S" position
                glTranslated(0, 1, 0);
                // draw "S" symbol
                GLHelper::drawText("S", Position(), .1, 2.8, stopColor);
                // move to subtitle positin
                glTranslated(0, 1.4, 0);
                // draw subtitle depending of tag
                GLHelper::drawText("lane", Position(), .1, 1, stopColor, 180);
            }
            // pop draw matrix
            glPopMatrix();
            // Draw name if isn't being drawn for selecting
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
            // check if dotted contour has to be drawn
            if (myViewNet->getDottedAC() == this) {
                // draw dooted contour depending if it's placed over a lane or over a stoppingPlace
                if (getLaneParents().size() > 0) {
                    GLHelper::drawShapeDottedContourAroundShape(s, getType(), myDemandElementGeometry.shape, getLaneParents().front()->getParentEdge().getNBEdge()->getLaneWidth(getLaneParents().front()->getIndex()) * 0.5);
                } else {
                    GLHelper::drawShapeDottedContourAroundShape(s, getType(), myDemandElementGeometry.shape, exaggeration);
                }
            }
        } else {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GLHelper::drawBoxLines(myDemandElementGeometry.shape, myDemandElementGeometry.shapeRotations, myDemandElementGeometry.shapeLengths, exaggeration * 0.8);
            // pop draw matrix
            glPopMatrix();
        }
        // Pop name
        glPopName();
        // draw person parent if this stop if their first person plan child
        if ((getDemandElementParents().size() == 1) && getDemandElementParents().front()->getDemandElementChildren().front() == this) {
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
            return "waiting";
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
        case GNE_ATTR_PARENT:
            return getDemandElementParents().front()->getID();
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
                std::vector<std::string> IDs = parse<std::vector<std::string>>(value);
                for (const auto &i : IDs) {
                    if (SUMOXMLDefinitions::isValidVehicleID(i) == false) {
                        return false;
                    }
                }
                return true;
            }
        case SUMO_ATTR_PARKING:
            return canParse<bool>(value);
        case SUMO_ATTR_ACTTYPE:
            return (value == "waiting");
        case SUMO_ATTR_TRIP_ID:
            return SUMOXMLDefinitions::isValidVehicleID(value);
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


void
GNEStop::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_EXPECTED:
            newParametersSet |= STOP_TRIGGER_SET;
            break;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            newParametersSet |= STOP_CONTAINER_TRIGGER_SET;
            break;
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, myViewNet->getNet(), parametersSet, newParametersSet), true);
}


bool
GNEStop::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_EXPECTED:
            return (parametersSet & STOP_TRIGGER_SET) != 0;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            return (parametersSet & STOP_CONTAINER_TRIGGER_SET) != 0;
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
    std::string stopType;
    // first distinguish between person stops and vehicles stops
    if (getDemandElementParents().front()->getTagProperty().isPerson()) {
        stopType = "person stop";
    } else {
        stopType = "vehicle stop";
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


std::string
GNEStop::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEStop::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEStop::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
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


void
GNEStop::setEnabledAttribute(const int enabledAttributes) {
    parametersSet = enabledAttributes;
}

/****************************************************************************/
