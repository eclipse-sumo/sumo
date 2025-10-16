/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
// Representation of Stops in netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_ToggleAttribute.h>
#include <netedit/elements/moving/GNEMoveElementLaneDouble.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEStop.h"

// ===========================================================================
// member method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNEStop::GNEStop(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, -1),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_STARTPOS, startPos, SUMO_ATTR_ENDPOS, endPos, friendlyPos)),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // enable parking for stops in parkin)gAreas
    if ((tag == GNE_TAG_STOP_PARKINGAREA) || (tag == GNE_TAG_WAYPOINT_PARKINGAREA)) {
        parametersSet |= STOP_PARKING_SET;
    }
    // set parking
    if (parametersSet & STOP_PARKING_SET) {
        parking = ParkingType::OFFROAD;
    }
    // set waypoint speed
    myTagProperty->isVehicleWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}


GNEStop::GNEStop(SumoXMLTag tag, GNEDemandElement* stopParent, GNEAdditional* stoppingPlace, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(stopParent, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    SUMOVehicleParameter::Stop(stopParameter),
    GNEDemandElementPlan(this, -1, -1),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_STARTPOS, startPos, SUMO_ATTR_ENDPOS, endPos, friendlyPos)),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // set parents
    setParent<GNEAdditional*>(stoppingPlace);
    setParent<GNEDemandElement*>(stopParent);
    // set triggered values
    if (triggered) {
        parametersSet |= STOP_TRIGGER_SET;
        if (awaitedPersons.size() > 0) {
            parametersSet |= STOP_EXPECTED_SET;
        }
    } else if (containerTriggered) {
        parametersSet |= STOP_TRIGGER_SET;
        parametersSet |= STOP_CONTAINER_TRIGGER_SET;
        if (awaitedPersons.size() > 0) {
            parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
        }
    } else if (joinTriggered) {
        parametersSet |= STOP_TRIGGER_SET;
    }
    // set parking
    if (parametersSet & STOP_PARKING_SET) {
        parking = ParkingType::OFFROAD;
    }
    // set tripID and line
    (stopParameter.tripId.size() > 0) ? parametersSet |= STOP_TRIP_ID_SET : parametersSet &= ~STOP_TRIP_ID_SET;
    (stopParameter.line.size() > 0) ? parametersSet |= STOP_LINE_SET : parametersSet &= ~STOP_LINE_SET;
    stopParameter.onDemand ? parametersSet |= STOP_ONDEMAND_SET : parametersSet &= ~STOP_ONDEMAND_SET;
    // set waypoint speed
    myTagProperty->isVehicleWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}


GNEStop::GNEStop(SumoXMLTag tag, GNEDemandElement* stopParent, GNELane* lane, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(stopParent, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    SUMOVehicleParameter::Stop(stopParameter),
    GNEDemandElementPlan(this, -1, -1),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_STARTPOS, startPos, SUMO_ATTR_ENDPOS, endPos, friendlyPos)),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // set parents
    setParent<GNELane*>(lane);
    setParent<GNEDemandElement*>(stopParent);
    // set triggered values
    if (triggered) {
        parametersSet |= STOP_TRIGGER_SET;
        if (awaitedPersons.size() > 0) {
            parametersSet |= STOP_EXPECTED_SET;
        }
    } else if (containerTriggered) {
        parametersSet |= STOP_TRIGGER_SET;
        parametersSet |= STOP_CONTAINER_TRIGGER_SET;
        if (awaitedPersons.size() > 0) {
            parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
        }
    } else if (joinTriggered) {
        parametersSet |= STOP_TRIGGER_SET;
    }
    // set parking
    if (parametersSet & STOP_PARKING_SET) {
        parking = ParkingType::OFFROAD;
    }
    // set trigger
    (stopParameter.tripId.size() > 0) ? parametersSet |= STOP_TRIP_ID_SET : parametersSet &= ~STOP_TRIP_ID_SET;
    // set tripID and line
    (stopParameter.tripId.size() > 0) ? parametersSet |= STOP_TRIP_ID_SET : parametersSet &= ~STOP_TRIP_ID_SET;
    (stopParameter.line.size() > 0) ? parametersSet |= STOP_LINE_SET : parametersSet &= ~STOP_LINE_SET;
    stopParameter.onDemand ? parametersSet |= STOP_ONDEMAND_SET : parametersSet &= ~STOP_ONDEMAND_SET;
    // set waypoint speed
    myTagProperty->isVehicleWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNEStop::~GNEStop() {}


GNEMoveElement*
GNEStop::getMoveElement() const {
    return myMoveElementLaneDouble;
}


Parameterised*
GNEStop::getParameters() {
    return this;
}


const Parameterised*
GNEStop::getParameters() const {
    return this;
}


void
GNEStop::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_STOP);
    if (getParentAdditionals().size() > 0) {
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_BUS_STOP) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_TRAIN_STOP) {
            device.writeAttr(SUMO_ATTR_TRAIN_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_CONTAINER_STOP) {
            device.writeAttr(SUMO_ATTR_CONTAINER_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_CHARGING_STATION) {
            device.writeAttr(SUMO_ATTR_CHARGING_STATION, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_PARKING_AREA) {
            device.writeAttr(SUMO_ATTR_PARKING_AREA, getParentAdditionals().front()->getID());
        }
    } else {
        if (getParentLanes().size() > 0) {
            device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
        }
        if (startPos != INVALID_DOUBLE) {
            device.writeAttr(SUMO_ATTR_STARTPOS, startPos);
        }
        if (endPos != INVALID_DOUBLE) {
            device.writeAttr(SUMO_ATTR_ENDPOS, endPos);
        }
    }
    // write rest of attributes
    write(device, true, false);
}


GNEDemandElement::Problem
GNEStop::isDemandElementValid() const {
    if (getPathStopIndex() == -1) {
        return Problem::STOP_DOWNSTREAM;
    } else if ((getParentLanes().size() > 0) && !myMoveElementLaneDouble->isMoveElementValid()) {
        return Problem::INVALID_STOPPOSITION;
    } else {
        return Problem::OK;
    }
}


std::string
GNEStop::getDemandElementProblem() const {
    if (getPathStopIndex() == -1) {
        return ("Downstream stop");
    } else if (getParentLanes().size() > 0) {
        return myMoveElementLaneDouble->getMovingProblem();
    } else {
        return "";
    }
}


void
GNEStop::fixDemandElementProblem() {
    // currently only for stops over lanes
    if (GNEStop::isDemandElementValid() == Problem::INVALID_STOPPOSITION) {
        myMoveElementLaneDouble->fixMovingProblem();
    }
}


SUMOVehicleClass
GNEStop::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEStop::getColor() const {
    // get inspected AC
    const auto inspectedAC = myNet->getViewNet()->getInspectedElements().getFirstAC();
    if (inspectedAC) {
        // check if is a route or a vehicle
        if ((inspectedAC->getTagProperty()->isRoute() || inspectedAC->getTagProperty()->isVehicle()) && (inspectedAC != getParentDemandElements().front())) {
            return RGBColor::GREY;
        }
    } else if (myNet->getViewNet()->getViewParent()->getStopFrame()->shown()) {
        if (myNet->getViewNet()->getViewParent()->getStopFrame()->getStopParentSelector()->getCurrentDemandElement() != getParentDemandElements().front()) {
            return RGBColor::GREY;
        }
    }
    // return default color
    if (myTagProperty->isVehicleWaypoint()) {
        return myNet->getViewNet()->getVisualisationSettings().colorSettings.waypointColor;
    } else {
        return myNet->getViewNet()->getVisualisationSettings().colorSettings.stopColor;
    }
}


void
GNEStop::updateGeometry() {
    // update geometry depending of parent
    if (getParentLanes().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myDemandElementGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLaneDouble->myMovingLateralOffset);
    } else if (getParentAdditionals().size() > 0) {
        // use geometry of additional (busStop)
        myDemandElementGeometry = getParentAdditionals().at(0)->getAdditionalGeometry();
    }
}


Position
GNEStop::getPositionInView() const {
    if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getLaneShape().positionAtOffset((startPos + endPos) / 2.0);
    } else if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getPositionInView();
    } else {
        throw ProcessError(TL("Invalid Stop parent"));
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
        throw ProcessError(TL("Invalid parent"));
    }
}


double
GNEStop::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEStop::getCenteringBoundary() const {
    Boundary b;
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().at(0)->getCenteringBoundary();
    } else if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myDemandElementGeometry.getShape().size() > 0) {
        b = myDemandElementGeometry.getShape().getBoxBoundary();
    } else {
        b.add(getPositionInView());
    }
    b.grow(20);
    return b;
}


void
GNEStop::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // check if draw an stop for person/containers or for vehicles/routes
    if (canDrawVehicleStop()) {
        // get exaggeration
        const auto exaggeration = getExaggeration(s);
        // get lane
        const auto& stopLane = getParentLanes().size() > 0 ? getParentLanes().front() : nullptr;
        // get lane width
        const double width = stopLane ? stopLane->getParentEdge()->getNBEdge()->getLaneWidth(stopLane->getIndex()) * 0.5 : exaggeration * 0.8;
        // get detail level
        const auto d = s.getDetailLevel(exaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // get color
            const auto color = drawUsingSelectColor() ? s.colorSettings.selectedRouteColor : getColor();
            // Add a layer matrix
            GLHelper::pushMatrix();
            // set Color
            GLHelper::setColor(color);
            // Start with the drawing of the area traslating matrix to origin
            drawInLayer(getType());
            // draw depending if is over lane or over stoppingP
            if (getParentLanes().size() > 0) {
                drawStopOverLane(s, d, color, width, exaggeration);
            } else {
                drawStopOverStoppingPlace(d, color, width, exaggeration);
            }
            // pop layer matrix
            GLHelper::popMatrix();
            if (s.showParkingInfo) {
                // draw above demand elements
                GLHelper::pushMatrix();
                glTranslated(myDemandElementGeometry.getShape().back().x(), myDemandElementGeometry.getShape().back().y(), GLO_VEHICLELABELS);
                drawStopLabel(s);
                GLHelper::popMatrix();
            }
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), exaggeration);
            // draw dotted contour
            myStopContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour and draw dotted geometry
        if (getParentAdditionals().size() > 0) {
            myStopContour.calculateContourExtrudedShape(s, d, this, myDemandElementGeometry.getShape(), getType(), width, exaggeration, true, true,
                    0, nullptr, getParentAdditionals().front()->getParentLanes().front()->getParentEdge());
        } else {
            myStopContour.calculateContourExtrudedShape(s, d, this, myDemandElementGeometry.getShape(), getType(), width, exaggeration, true, true,
                    0, nullptr, getParentLanes().front()->getParentEdge());
        }
    }
}


void
GNEStop::drawStopLabel(const GUIVisualizationSettings& s) const {
    const SUMOVehicleParameter::Stop& stop = *this;
    std::string label;
    if (stop.speed > 0) {
        label += "waypoint";
    } else if (stop.busstop != "") {
        label += "busStop:" + stop.busstop;
    } else if (stop.containerstop != "") {
        label += "containerStop:" + stop.containerstop;
    } else if (stop.parkingarea != "") {
        label += "parkingArea:" + stop.parkingarea;
    } else if (stop.chargingStation != "") {
        label += "chargingStation:" + stop.chargingStation;
    } else if (stop.overheadWireSegment != "") {
        label += "overheadWireSegment:" + stop.overheadWireSegment;
    } else {
        label += "stop";
    }
    if (stop.triggered || stop.containerTriggered || stop.joinTriggered) {
        label += " triggered:";
        if (stop.triggered) {
            label += "person";
            if (!stop.awaitedPersons.empty()) {
                label += "(" + toString(stop.awaitedPersons) + ")";
            }
        }
        if (stop.containerTriggered) {
            label += "container";
            if (!stop.awaitedContainers.empty()) {
                label += "(" + toString(stop.awaitedContainers) + ")";
            }
        }
        if (stop.joinTriggered) {
            label += "join";
            if (stop.join != "") {
                label += "(" + stop.join + ")";
            }
        }
    }
    if (stop.arrival >= 0) {
        label += " arrival:" + time2string(stop.arrival);
    }
    if (stop.until >= 0) {
        label += " until:" + time2string(stop.until);
    }
    if (stop.started >= 0) {
        label += " started:" + time2string(stop.started);
    }
    if (stop.ended >= 0) {
        label += " ended:" + time2string(stop.ended);
    }
    if (stop.duration >= 0 || stop.duration > 0) {
        if (STEPS2TIME(stop.duration) > 3600 * 24) {
            label += " duration:1day+";
        } else {
            label += " duration:" + time2string(stop.duration);
        }
    }
    if (stop.actType != "") {
        label += " actType:" + stop.actType;
    }
    GLHelper::drawTextSettings(s.vehicleText, label, Position(0, 0), s.scale, s.angle, 0);
}


void
GNEStop::computePathElement() {
    // only update geometry
    updateGeometry();
}


void
GNEStop::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
}


void
GNEStop::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
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
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_DURATION:
            if (isAttributeEnabled(key)) {
                return time2string(duration);
            } else {
                return "";
            }
        case SUMO_ATTR_UNTIL:
            if (isAttributeEnabled(key)) {
                return time2string(until);
            } else {
                return "";
            }
        case SUMO_ATTR_EXTENSION:
            if (isAttributeEnabled(key)) {
                return time2string(extension);
            } else {
                return "";
            }
        case SUMO_ATTR_TRIGGERED:
            if (triggered) {
                return "person";
            } else if (containerTriggered) {
                return "container";
            } else if (joinTriggered) {
                return "join";
            } else {
                return "false";
            }
        case SUMO_ATTR_EXPECTED:
            if (triggered) {
                return toString(awaitedPersons);
            } else if (containerTriggered) {
                return toString(awaitedContainers);
            } else {
                return "";
            }
        case SUMO_ATTR_JOIN:
            if (joinTriggered) {
                return join;
            } else {
                return "";
            }
        case SUMO_ATTR_PERMITTED:
            return toString(permitted);
        case SUMO_ATTR_PARKING:
            if (parametersSet & STOP_PARKING_SET) {
                return "true";
            } else {
                return "false";
            }
        case SUMO_ATTR_ACTTYPE:
            return actType;
        case SUMO_ATTR_TRIP_ID:
            return tripId;
        case SUMO_ATTR_LINE:
            return line;
        case SUMO_ATTR_ONDEMAND:
            return toString(onDemand);
        case SUMO_ATTR_JUMP:
            if (parametersSet & STOP_JUMP_SET) {
                return time2string(jump);
            } else {
                return "";
            }
        // only for waypoints
        case SUMO_ATTR_SPEED:
            return toString(speed);
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_TRAIN_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
            return getParentAdditionals().front()->getID();
        // specific of stops over lanes
        case SUMO_ATTR_POSITION_LAT:
            if (posLat == INVALID_DOUBLE) {
                return "";
            } else {
                return toString(posLat);
            }
        case SUMO_ATTR_SPLIT:
            return split;
        //
        case GNE_ATTR_PARENT:
            return getParentDemandElements().front()->getID();
        case GNE_ATTR_STOPINDEX: {
            // extract all stops of demandElement parent
            std::vector<GNEDemandElement*> stops;
            for (const auto& parent : getParentDemandElements().front()->getChildDemandElements()) {
                if (parent->getTagProperty()->isVehicleStop()) {
                    stops.push_back(parent);
                }
            }
            // find index in stops
            for (int i = 0; i < (int)stops.size(); i++) {
                if (stops.at(i) == this) {
                    return toString(i);
                }
            }
            return "invalid index";
        }
        case GNE_ATTR_PATHSTOPINDEX:
            return toString(getPathStopIndex());
        default:
            return myMoveElementLaneDouble->getMovingAttribute(key);
    }
}


double
GNEStop::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_INDEX: // for writting sorted
            return (double)myCreationIndex;
        case GNE_ATTR_STOPINDEX: {
            // extract all stops of demandElement parent
            std::vector<GNEDemandElement*> stops, filteredStops;
            for (const auto& parent : getParentDemandElements().front()->getChildDemandElements()) {
                if (parent->getTagProperty()->isVehicleStop()) {
                    stops.push_back(parent);
                }
            }
            // now filter stops with the same startPos
            for (const auto& stop : stops) {
                if (stop->getAttributeDouble(SUMO_ATTR_STARTPOS) == getAttributeDouble(SUMO_ATTR_STARTPOS)) {
                    filteredStops.push_back(stop);
                }
            }
            // get index
            for (int i = 0; i < (int)filteredStops.size(); i++) {
                if (filteredStops.at(i) == this) {
                    return i;
                }
            }
            return 0;
        }
        case GNE_ATTR_PATHSTOPINDEX:
            return (double)getPathStopIndex();
        default:
            return myMoveElementLaneDouble->getMovingAttributeDouble(key);
    }
}


Position
GNEStop::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_PLAN_GEOMETRY_ENDPOS: {
            if (getParentAdditionals().size() > 0) {
                // return first position of busStop
                return getParentAdditionals().front()->getAdditionalGeometry().getShape().front();
            } else {
                // get lane shape
                const PositionVector& laneShape = getLastPathLane()->getLaneShape();
                // continue depending of arrival position
                if (endPos == 0) {
                    return laneShape.front();
                } else if ((endPos == -1) || (endPos >= laneShape.length2D())) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(endPos);
                }
            }
        }
        default:
            return getCommonAttributePosition(key);
    }
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
        case SUMO_ATTR_EXPECTED:
        case SUMO_ATTR_JOIN:
        case SUMO_ATTR_PERMITTED:
        case SUMO_ATTR_PARKING:
        case SUMO_ATTR_ACTTYPE:
        case SUMO_ATTR_TRIP_ID:
        case SUMO_ATTR_LINE:
        case SUMO_ATTR_ONDEMAND:
        case SUMO_ATTR_JUMP:
        // only for waypoints
        case SUMO_ATTR_SPEED:
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_TRAIN_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
        // specific of stops over lanes
        case SUMO_ATTR_POSITION_LAT:
        case SUMO_ATTR_SPLIT:
        // other
        case GNE_ATTR_PARENT:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementLaneDouble->setMovingAttribute(key, value, undoList);
            break;
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
        case SUMO_ATTR_TRIGGERED: {
            const auto expectedValues = myTagProperty->getAttributeProperties(key)->getDiscreteValues();
            const auto triggeredValues = parse<std::vector<std::string> >(value);
            for (const auto& triggeredValue : triggeredValues) {
                if (std::find(expectedValues.begin(), expectedValues.end(), triggeredValue) == expectedValues.end()) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_EXPECTED:
            if (value.empty()) {
                return false;
            } else {
                const std::vector<std::string> expectedValues = parse<std::vector<std::string> >(value);
                for (const auto& expectedValue : expectedValues) {
                    if (!SUMOXMLDefinitions::isValidVehicleID(expectedValue)) {
                        return false;
                    }
                }
                return true;
            }
        case SUMO_ATTR_JOIN:
            if (value.empty()) {
                return false;
            } else {
                return SUMOXMLDefinitions::isValidVehicleID(value);
            }
        case SUMO_ATTR_PERMITTED: {
            const std::vector<std::string> expectedValues = parse<std::vector<std::string> >(value);
            for (const auto& expectedValue : expectedValues) {
                if (!SUMOXMLDefinitions::isValidVehicleID(expectedValue)) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_PARKING: {
            if (value == "opportunistic") {
                return false; // Currrently deactivated opportunistic in netedit waiting for the implementation in SUMO
            }
            const auto expectedValues = myTagProperty->getAttributeProperties(key)->getDiscreteValues();
            const auto triggeredValues = parse<std::vector<std::string> >(value);
            for (const auto& triggeredValue : triggeredValues) {
                if (std::find(expectedValues.begin(), expectedValues.end(), triggeredValue) == expectedValues.end()) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_ACTTYPE:
            return true;
        case SUMO_ATTR_TRIP_ID:
            return SUMOXMLDefinitions::isValidVehicleID(value);
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_ONDEMAND:
            return canParse<bool>(value);
        case SUMO_ATTR_JUMP:
            if ((value == "-1") || (value.empty())) {
                return true;
            } else if (canParse<double>(value)) {
                return parse<double>(value) >= 0;
            } else {
                return false;
            }
        // only for waypoints
        case SUMO_ATTR_SPEED:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_TRAIN_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, value, false) != nullptr);
        case SUMO_ATTR_CONTAINER_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_CHARGING_STATION:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, value, false) != nullptr);
        case SUMO_ATTR_PARKING_AREA:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        // specific of stops over lanes
        case SUMO_ATTR_POSITION_LAT:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_SPLIT:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidVehicleID(value);
            }
        //
        case GNE_ATTR_PARENT:
            return false;
        default:
            return myMoveElementLaneDouble->isMovingAttributeValid(key, value);
    }
}


void
GNEStop::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            undoList->add(new GNEChange_ToggleAttribute(this, key, true), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            undoList->add(new GNEChange_ToggleAttribute(this, key, false), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStop::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        // Currently stops parents cannot be edited
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_TRAIN_STOP:
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
            return triggered || containerTriggered;
        case SUMO_ATTR_JOIN:
            return joinTriggered;
        case SUMO_ATTR_PARKING:
            // for stops/waypoints over parking areas, always enabled
            if ((myTagProperty->getTag() == GNE_TAG_STOP_PARKINGAREA) || (myTagProperty->getTag() == GNE_TAG_WAYPOINT_PARKINGAREA)) {
                return false;
            } else {
                return true;
            }
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
        if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_BUS_STOP) {
            return "BusStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_TRAIN_STOP) {
            return "TrainStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_CONTAINER_STOP) {
            return "containerStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_CHARGING_STATION) {
            return "chargingStation: " + getParentAdditionals().front()->getID();
        } else {
            return "parkingArea: " + getParentAdditionals().front()->getID();
        }
    } else if (getParentLanes().size() > 0) {
        return "lane: " + getParentLanes().front()->getID();
    } else {
        return "";
    }
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
// protected
// ===========================================================================

bool
GNEStop::canDrawVehicleStop() const {
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    if (isAttributeCarrierSelected()) {
        return true;
    } else if (inspectedElements.isACInspected(this)) {
        return true;
    } else if (inspectedElements.isACInspected(getParentDemandElements().front())) {
        return true;
    } else if (myNet->getViewNet()->getDemandViewOptions().showAllTrips()) {
        return true;
    } else if ((getParentDemandElements().front()->getTagProperty()->getTag() == GNE_TAG_VEHICLE_WITHROUTE) ||
               (getParentDemandElements().front()->getTagProperty()->getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        if (inspectedElements.isACInspected(getParentDemandElements().front()->getChildDemandElements().front())) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNEStop::drawIndex() const {
    // get stop frame
    const auto stopFrame = myNet->getViewNet()->getViewParent()->getStopFrame();
    // check conditions
    if (myNet->getViewNet()->getInspectedElements().isACInspected(getParentDemandElements().front())) {
        return true;
    } else if (stopFrame->shown() && (stopFrame->getStopParentSelector()->getCurrentDemandElement() == getParentDemandElements().front())) {
        return true;
    } else {
        return false;
    }
}



void
GNEStop::drawStopOverLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const RGBColor& color,
                          const double width, const double exaggeration) const {
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
    // move to symbol position
    glTranslated(0, 1, 0.1);
    // draw text depending of detail
    if (d <= GUIVisualizationSettings::Detail::Text) {
        // draw symbol
        GLHelper::drawText(myTagProperty->isVehicleWaypoint() ? "W" : "S", Position(), .1, 2.8, color, 180);
        // move to subtitle position
        glTranslated(0, 1.4, 0);
        // draw subtitle depending of tag
        GLHelper::drawText("lane", Position(), .1, 1, color, 180);
        // check if draw index
        if (drawIndex()) {
            // move to index position
            glTranslated(-2.1, -2.4, 0);
            glRotated(-90, 0, 0, 1);
            // draw index
            GLHelper::drawText(getAttribute(GNE_ATTR_STOPINDEX), Position(0, getAttributeDouble(GNE_ATTR_STOPINDEX) * -1, 0), .1, 1, color, 180);
        }
    } else {
        GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
    }
    // pop detail matrix
    GLHelper::popMatrix();
    // draw geometry points
    drawGeometryPoints(s, d, color);
}


void
GNEStop::drawStopOverStoppingPlace(const GUIVisualizationSettings::Detail d, const RGBColor& color,
                                   const double width, const double exaggeration) const {
    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration taked from stoppingPlace parent
    GUIGeometry::drawGeometry(d, myDemandElementGeometry, width);
    // only draw text if isn't being drawn for selecting
    if ((d <= GUIVisualizationSettings::Detail::Text) && drawIndex()) {
        // Add a detail matrix
        GLHelper::pushMatrix();
        // move to geometry front
        glTranslated(myDemandElementGeometry.getShape().back().x(), myDemandElementGeometry.getShape().back().y(), 0.1);
        // rotate
        if (myDemandElementGeometry.getShapeRotations().size() > 0) {
            glRotated(myDemandElementGeometry.getShapeRotations().back(), 0, 0, 1);
        }
        // move to index position
        glTranslated(-1.4, exaggeration * 0.5, 0.1);
        glRotated(-90, 0, 0, 1);
        // draw index
        GLHelper::drawText(getAttribute(GNE_ATTR_STOPINDEX), Position(0, getAttributeDouble(GNE_ATTR_STOPINDEX) * -1, 0), .1, 1, color, 180);
        // pop detail matrix
        GLHelper::popMatrix();
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_DURATION:
            if (value.empty()) {
                toggleAttribute(key, false);
            } else {
                toggleAttribute(key, true);
                duration = string2time(value);
            }
            break;
        case SUMO_ATTR_UNTIL:
            if (value.empty()) {
                toggleAttribute(key, false);
            } else {
                toggleAttribute(key, true);
                until = string2time(value);
            }
            break;
        case SUMO_ATTR_EXTENSION:
            if (value.empty()) {
                toggleAttribute(key, false);
            } else {
                toggleAttribute(key, true);
                extension = string2time(value);
            }
            break;
        case SUMO_ATTR_TRIGGERED:
            // reset all flags
            triggered = false;
            containerTriggered = false;
            joinTriggered = false;
            // disable all flags
            parametersSet &= ~STOP_JOIN_SET;
            parametersSet &= ~STOP_TRIGGER_SET;
            parametersSet &= ~STOP_EXPECTED_SET;
            parametersSet &= ~STOP_CONTAINER_TRIGGER_SET;
            parametersSet &= ~STOP_EXPECTED_CONTAINERS_SET;
            // check value
            if ((value == "person") || (value == "true")) {
                parametersSet |= STOP_TRIGGER_SET;
                triggered = true;
                if (awaitedPersons.size() > 0) {
                    parametersSet |= STOP_EXPECTED_SET;
                }
            } else if (value == "container") {
                parametersSet |= STOP_TRIGGER_SET;
                parametersSet |= STOP_CONTAINER_TRIGGER_SET;
                containerTriggered = true;
                if (awaitedPersons.size() > 0) {
                    parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
                }
            } else if (value == "join") {
                parametersSet |= STOP_TRIGGER_SET;
                joinTriggered = true;
            }
            break;
        case SUMO_ATTR_EXPECTED:
            if (triggered) {
                awaitedPersons = parse<std::set<std::string> >(value);
                if (awaitedPersons.size() > 0) {
                    parametersSet |= STOP_EXPECTED_SET;
                } else {
                    parametersSet &= ~STOP_EXPECTED_SET;
                }
            } else if (containerTriggered) {
                awaitedContainers = parse<std::set<std::string> >(value);
                if (awaitedContainers.size() > 0) {
                    parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
                } else {
                    parametersSet &= ~STOP_EXPECTED_CONTAINERS_SET;
                }
            }
            break;
        case SUMO_ATTR_JOIN:
            if (joinTriggered) {
                join = value;
                if (join.size() > 0) {
                    parametersSet |= STOP_JOIN_SET;
                } else {
                    parametersSet &= ~STOP_JOIN_SET;
                }
            }
            break;
        case SUMO_ATTR_PERMITTED:
            if (value.empty()) {
                parametersSet &= ~STOP_PERMITTED_SET;
            } else {
                parametersSet |= STOP_PERMITTED_SET;
                permitted = parse<std::set<std::string> >(value);
            }
            break;
        case SUMO_ATTR_PARKING:
            parking = SUMOVehicleParameter::parseParkingType(value);
            if (parking == ParkingType::ONROAD) {
                parametersSet &= ~STOP_PARKING_SET;
            } else {
                parametersSet |= STOP_PARKING_SET;
            }
            break;
        case SUMO_ATTR_ACTTYPE:
            actType = value;
            break;
        case SUMO_ATTR_TRIP_ID:
            tripId = value;
            toggleAttribute(key, (value.size() > 0));
            break;
        case SUMO_ATTR_LINE:
            line = value;
            toggleAttribute(key, (value.size() > 0));
            break;
        case SUMO_ATTR_ONDEMAND:
            if (parse<bool>(value)) {
                parametersSet |= STOP_ONDEMAND_SET;
            } else {
                parametersSet &= ~STOP_ONDEMAND_SET;
            }
            // set flag
            onDemand = ((parametersSet & STOP_ONDEMAND_SET) != 0);
            break;
        case SUMO_ATTR_JUMP:
            if ((value == "-1") || value.empty()) {
                parametersSet &= ~STOP_JUMP_SET;
                jump = -1;
            } else {
                parametersSet |= STOP_JUMP_SET;
                jump = string2time(value);
            }
            break;
        // only for waypoints
        case SUMO_ATTR_SPEED:
            speed = parse<double>(value);
            break;
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            replaceFirstParentAdditional(SUMO_TAG_BUS_STOP, value);
            updateGeometry();
            break;
        case SUMO_ATTR_TRAIN_STOP:
            replaceFirstParentAdditional(SUMO_TAG_TRAIN_STOP, value);
            updateGeometry();
            break;
        case SUMO_ATTR_CONTAINER_STOP:
            replaceFirstParentAdditional(SUMO_TAG_CONTAINER_STOP, value);
            updateGeometry();
            break;
        case SUMO_ATTR_CHARGING_STATION:
            replaceFirstParentAdditional(SUMO_TAG_CHARGING_STATION, value);
            updateGeometry();
            break;
        case SUMO_ATTR_PARKING_AREA:
            replaceFirstParentAdditional(SUMO_TAG_PARKING_AREA, value);
            updateGeometry();
            break;
        // specific of Stops over lanes
        case SUMO_ATTR_LANE:
            replaceFirstParentLane(value);
            updateGeometry();
            break;
        case SUMO_ATTR_POSITION_LAT:
            if (value.empty()) {
                posLat = INVALID_DOUBLE;
                parametersSet &= ~STOP_POSLAT_SET;
            } else {
                posLat = parse<double>(value);
                parametersSet |= STOP_POSLAT_SET;
            }
            break;
        case SUMO_ATTR_SPLIT:
            split = value;
            if (split.size() > 0) {
                parametersSet |= STOP_SPLIT_SET;
            } else {
                parametersSet &= ~STOP_SPLIT_SET;
            }
            break;
        //
        case GNE_ATTR_PARENT:
            updateGeometry();
            break;
        default:
            myMoveElementLaneDouble->setMovingAttribute(key, value);
            break;
    }
}


void
GNEStop::toggleAttribute(SumoXMLAttr key, const bool value) {
    switch (key) {
        case SUMO_ATTR_DURATION:
            if (value) {
                parametersSet |= STOP_DURATION_SET;
            } else {
                parametersSet &= ~STOP_DURATION_SET;
            }
            break;
        case SUMO_ATTR_UNTIL:
            if (value) {
                parametersSet |= STOP_UNTIL_SET;
            } else {
                parametersSet &= ~STOP_UNTIL_SET;
            }
            break;
        case SUMO_ATTR_EXTENSION:
            if (value) {
                parametersSet |= STOP_EXTENSION_SET;
            } else {
                parametersSet &= ~STOP_EXTENSION_SET;
            }
            break;
        case SUMO_ATTR_TRIP_ID:
            if (value) {
                parametersSet |= STOP_TRIP_ID_SET;
            } else {
                parametersSet &= ~STOP_TRIP_ID_SET;
            }
            break;
        case SUMO_ATTR_LINE:
            if (value) {
                parametersSet |= STOP_LINE_SET;
            } else {
                parametersSet &= ~STOP_LINE_SET;
            }
            break;
        case SUMO_ATTR_ONDEMAND:
            if (value) {
                parametersSet |= STOP_ONDEMAND_SET;
            } else {
                parametersSet &= ~STOP_ONDEMAND_SET;
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::drawGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const RGBColor& baseColor) const {
    // first check that we're in move mode and shift key is pressed
    if (drawMovingGeometryPoints()) {
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
            GLHelper::drawFilledCircleDetailled(d, s.neteditSizeSettings.additionalGeometryPointRadius);
            // pop geometry point matrix
            GLHelper::popMatrix();
        }
        if (endPos != INVALID_DOUBLE) {
            // push geometry point matrix
            GLHelper::pushMatrix();
            glTranslated(myDemandElementGeometry.getShape().back().x(), myDemandElementGeometry.getShape().back().y(), 0.1);
            // draw geometry point
            GLHelper::drawFilledCircleDetailled(d, s.neteditSizeSettings.additionalGeometryPointRadius);
            // pop geometry point matrix
            GLHelper::popMatrix();
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


int
GNEStop::getPathStopIndex() const {
    // get edge stop indexes
    const auto edgeStopIndex = getEdgeStopIndex();
    // finally find stopIndex in edgeStopIndexes
    for (const auto& edgeStop : edgeStopIndex) {
        for (const auto& stop : edgeStop.stops) {
            if (stop == this) {
                return edgeStop.stopIndex;
            }
        }
    }
    // not found, then return -1
    return -1;
}


void
GNEStop::setStopMicrosimID() {
    if (getParentAdditionals().size() > 0) {
        setDemandElementID(getMicrosimID() + " (" + getParentAdditionals().front()->getTagStr() + ")");
    } else if (getParentLanes().size() > 0) {
        setDemandElementID(getMicrosimID() + " (" + getParentLanes().front()->getTagStr() + ")");
    }
}

/****************************************************************************/
