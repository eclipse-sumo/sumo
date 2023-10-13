/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEStopPlan.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2023
///
// Representation of Stops in netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_ToggleAttribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEStopPlan.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStopPlan::GNEStopPlan(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_STOP, tag, GUIIconSubSys::getIcon(GUIIcon::STOP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    GNEDemandElementPlan(this, -1, -1),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // reset default values
    resetDefaultValues();
    // enable parking for stops in parkin)gAreas
    if ((tag == GNE_TAG_STOP_PARKINGAREA) || (tag == GNE_TAG_WAYPOINT_PARKINGAREA)) {
        parametersSet |= STOP_PARKING_SET;
    }
    // set parking
    if (parametersSet & STOP_PARKING_SET) {
        parking = ParkingType::OFFROAD;
    }
    // set waypoint speed
    myTagProperty.isWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}


GNEStopPlan::GNEStopPlan(SumoXMLTag tag, GNENet* net, GNEDemandElement* stopParent, GNEAdditional* stoppingPlace, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(stopParent, net, GLO_STOP, tag, GUIIconSubSys::getIcon(GUIIcon::STOP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {stoppingPlace}, {stopParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    GNEDemandElementPlan(this, -1, -1),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // enable parking for stops in parkingAreas
    if ((tag == GNE_TAG_STOP_PARKINGAREA) || (tag == GNE_TAG_WAYPOINT_PARKINGAREA)) {
        parametersSet |= STOP_PARKING_SET;
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
    myTagProperty.isWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}


GNEStopPlan::GNEStopPlan(SumoXMLTag tag, GNENet* net, GNEDemandElement* stopParent, GNEEdge* edge, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(stopParent, net, GLO_STOP, tag, GUIIconSubSys::getIcon(GUIIcon::STOP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {edge}, {}, {}, {stopParent}, {}),
    GNEDemandElementPlan(this, -1, -1),
    myCreationIndex(myNet->getAttributeCarriers()->getStopIndex()) {
    // enable parking for stops in parkingAreas
    if ((tag == GNE_TAG_STOP_PARKINGAREA) || (tag == GNE_TAG_WAYPOINT_PARKINGAREA)) {
        parametersSet |= STOP_PARKING_SET;
    }
    // set flags
    if (parametersSet & STOP_PARKING_SET) {
        parking = ParkingType::OFFROAD;
    }
    triggered = (parametersSet & STOP_TRIGGER_SET);
    containerTriggered = (parametersSet & STOP_CONTAINER_TRIGGER_SET);
    joinTriggered = (parametersSet & STOP_JOIN_SET);
    // set tripID and line
    (stopParameter.tripId.size() > 0) ? parametersSet |= STOP_TRIP_ID_SET : parametersSet &= ~STOP_TRIP_ID_SET;
    (stopParameter.line.size() > 0) ? parametersSet |= STOP_LINE_SET : parametersSet &= ~STOP_LINE_SET;
    stopParameter.onDemand ? parametersSet |= STOP_ONDEMAND_SET : parametersSet &= ~STOP_ONDEMAND_SET;
    // set waypoint speed
    myTagProperty.isWaypoint() ? parametersSet |= STOP_SPEED_SET : parametersSet &= ~STOP_SPEED_SET;
    // set jump
    (jump != -1) ? parametersSet |= STOP_JUMP_SET : parametersSet &= ~STOP_JUMP_SET;
    // set locator sufix
    setStopMicrosimID();
}


GNEStopPlan::~GNEStopPlan() {}


GNEMoveOperation*
GNEStopPlan::getMoveOperation() {
    if ((myTagProperty.getTag() == GNE_TAG_STOPPERSON_EDGE) || (myTagProperty.getTag() == GNE_TAG_STOPCONTAINER_EDGE)) {
        // return move operation for additional placed over shape
        return new GNEMoveOperation(this, getParentEdges().front()->getLanes().front(), endPos, false);
    } else if ((myTagProperty.getTag() == GNE_TAG_STOP_LANE) || (myTagProperty.getTag() == GNE_TAG_WAYPOINT_LANE)) {
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


void
GNEStopPlan::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_STOP);
    if (getParentAdditionals().size() > 0) {
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP) {
            device.writeAttr(SUMO_ATTR_TRAIN_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP) {
            device.writeAttr(SUMO_ATTR_CONTAINER_STOP, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION) {
            device.writeAttr(SUMO_ATTR_CHARGING_STATION, getParentAdditionals().front()->getID());
        }
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA) {
            device.writeAttr(SUMO_ATTR_PARKING_AREA, getParentAdditionals().front()->getID());
        }
    } else {
        if (getParentLanes().size() > 0) {
            device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
        } else {
            device.writeAttr(SUMO_ATTR_EDGE, getParentEdges().front()->getID());
        }
        if ((parametersSet & STOP_START_SET) != 0) {
            device.writeAttr(SUMO_ATTR_STARTPOS, startPos);
        }
        if ((parametersSet & STOP_END_SET) != 0) {
            device.writeAttr(SUMO_ATTR_ENDPOS, endPos);
        }
    }
    // write rest of attributes
    write(device, true, false);
}


GNEDemandElement::Problem
GNEStopPlan::isDemandElementValid() const {
    if (myTagProperty.isStopPerson() || myTagProperty.isStopContainer()) {
        // get lane
        const GNELane* firstLane = getFirstAllowedLane();
        // only Stops placed over lanes can be invalid
        if (myTagProperty.getTag() != GNE_TAG_STOP_LANE) {
            return isPersonPlanValid();
        } else if (friendlyPos) {
            // with friendly position enabled position are "always fixed"
            return isPersonPlanValid();
        } else if (firstLane != nullptr) {
            // obtain lane length
            const double laneLength = getParentEdges().front()->getNBEdge()->getFinalLength() * firstLane->getLengthGeometryFactor();
            // declare end pos fixed
            const double endPosFixed = (endPos < 0) ? (endPos + laneLength) : endPos;
            // check values
            if ((endPosFixed <= getParentEdges().front()->getNBEdge()->getFinalLength()) && (endPosFixed > 0)) {
                return isPersonPlanValid();
            } else {
                return Problem::INVALID_STOPPOSITION;
            }
        } else {
            return Problem::INVALID_ELEMENT;
        }
    } else if (getPathStopIndex() == -1) {
        return Problem::STOP_DOWNSTREAM;
    } else {
        // only Stops placed over lanes can be invalid
        if (myTagProperty.getTag() != GNE_TAG_STOP_LANE) {
            return Problem::OK;
        } else if (friendlyPos) {
            // with friendly position enabled position are "always fixed"
            return Problem::OK;
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
            if ((startPosCopy >= 0) && (endPosCopy <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) && ((endPosCopy - startPosCopy) >= POSITION_EPS)) {
                return Problem::OK;
            } else {
                return Problem::INVALID_STOPPOSITION;
            }
        }
    }
}


std::string
GNEStopPlan::getDemandElementProblem() const {
    if (friendlyPos || (getParentAdditionals().size() > 0)) {
        return getPersonPlanProblem();
    } else {
        // obtain lane length
        const double laneLength = getParentEdges().front()->getNBEdge()->getFinalLength();
        // declare end pos fixed
        const double endPosFixed = (endPos < 0) ? (endPos + laneLength) : endPos;
        // check positions over lane
        if (endPosFixed < 0) {
            return (toString(SUMO_ATTR_ENDPOS) + " < 0");
        } else if (endPosFixed > getParentEdges().front()->getNBEdge()->getFinalLength()) {
            return (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
        } else {
            return getPersonPlanProblem();
        }
    }
}


void
GNEStopPlan::fixDemandElementProblem() {
    //
}


SUMOVehicleClass
GNEStopPlan::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEStopPlan::getColor() const {
    if (getTagProperty().isPersonPlan() || getTagProperty().isContainerPlan()) {
        return myNet->getViewNet()->getVisualisationSettings().colorSettings.stopPersonColor;
    } else if (myNet->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // get inspected AC
        const auto AC = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // check if is a route or a vehicle
        if ((AC->getTagProperty().isRoute() || AC->getTagProperty().isVehicle()) && (AC != getParentDemandElements().front())) {
            return RGBColor::GREY;
        }
    } else if (myNet->getViewNet()->getViewParent()->getStopFrame()->shown()) {
        if (myNet->getViewNet()->getViewParent()->getStopFrame()->getStopParentSelector()->getCurrentDemandElement() != getParentDemandElements().front()) {
            return RGBColor::GREY;
        }
    }
    // return default color
    if (myTagProperty.isWaypoint()) {
        return myNet->getViewNet()->getVisualisationSettings().colorSettings.waypointColor;
    } else {
        return myNet->getViewNet()->getVisualisationSettings().colorSettings.stopColor;
    }
}


void
GNEStopPlan::updateGeometry() {
    // update geometry depending of parent
    if (getParentLanes().size() > 0) {
        // Cut shape using as delimitators fixed start position and fixed end position
        myDemandElementGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
    } else if (getParentAdditionals().size() > 0) {
        if (getTagProperty().isPersonPlan() || getTagProperty().isContainerPlan()) {
            // get busStop shape
            const PositionVector& busStopShape = getParentAdditionals().front()->getAdditionalGeometry().getShape();
            // update demand element geometry using both positions
            myDemandElementGeometry.updateGeometry(busStopShape, busStopShape.length2D() - 0.6, busStopShape.length2D(), 0);
        } else {
            // use geometry of additional (busStop)
            myDemandElementGeometry = getParentAdditionals().at(0)->getAdditionalGeometry();
        }
    } else if (getParentEdges().size() > 0) {
        // get front and back lane
        const GNELane* frontLane = getParentEdges().front()->getLanes().front();
        const GNELane* backLane = getParentEdges().front()->getLanes().back();
        // get lane drawing constants
        GNELane::LaneDrawingConstants laneDrawingConstantsFront(myNet->getViewNet()->getVisualisationSettings(), frontLane);
        GNELane::LaneDrawingConstants laneDrawingConstantBack(myNet->getViewNet()->getVisualisationSettings(), backLane);
        // calculate front position
        const Position frontPosition = frontLane->getLaneShape().positionAtOffset2D(getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS), laneDrawingConstantsFront.halfWidth);
        // calulate length between both shapes
        const double length = backLane->getLaneShape().distance2D(frontPosition, true);
        // calculate back position
        const Position backPosition = frontLane->getLaneShape().positionAtOffset2D(getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS), (length + laneDrawingConstantBack.halfWidth - laneDrawingConstantsFront.halfWidth) * -1);
        // update demand element geometry using both positions
        myDemandElementGeometry.updateGeometry({frontPosition, backPosition});
    }
    /*
        // recompute geometry of all Demand elements related with this this stop
        if (getParentDemandElements().front()->getTagProperty().isRoute()) {
            getParentDemandElements().front()->updateGeometry();
        }
    */
}


Position
GNEStopPlan::getPositionInView() const {
    // check if is placed over a busStop
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getPositionInView();
    } else {
        // get lane
        const GNELane* personLane = getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
        // get position over lane shape
        if (endPos <= 0) {
            return personLane->getLaneShape().front();
        } else if (endPos >= personLane->getLaneShape().length2D()) {
            return personLane->getLaneShape().back();
        } else {
            return personLane->getLaneShape().positionAtOffset2D(endPos);
        }
    }
}


std::string
GNEStopPlan::getParentName() const {
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
GNEStopPlan::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEStopPlan::getCenteringBoundary() const {
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
GNEStopPlan::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEStopPlan::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double exaggeration = getExaggeration(s);
    // check if stop can be draw
    if ((getTagProperty().isStopPerson() && checkDrawPersonPlan()) ||
            (getTagProperty().isStopContainer() && checkDrawContainerPlan())) {
        // check if draw stopPerson over busStop oder over lane
        if (getParentAdditionals().size() > 0) {
            drawStopPersonOverStoppingPlace(s, exaggeration);
        } else {
            drawStopPersonOverEdge(s, exaggeration);
        }
    }
}


void
GNEStopPlan::computePathElement() {
    // only update geometry
    updateGeometry();
}


void
GNEStopPlan::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


void
GNEStopPlan::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


GNELane*
GNEStopPlan::getFirstPathLane() const {
    // check if stop is placed over a busStop
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else if (getParentEdges().size() > 0) {
        return getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
    } else {
        return getParentLanes().front();
    }
}


GNELane*
GNEStopPlan::getLastPathLane() const {
    // first and last path lane are the same
    return getFirstPathLane();
}


std::string
GNEStopPlan::getAttribute(SumoXMLAttr key) const {
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
        case SUMO_ATTR_ACTTYPE:
            return actType;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEStopPlan::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
        case GNE_ATTR_PLAN_GEOMETRY_STARTPOS:
            if (getParentAdditionals().size() > 0) {
                return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_STARTPOS);
            } else {
                return startPos;
            }
        case SUMO_ATTR_ENDPOS:
        case GNE_ATTR_PLAN_GEOMETRY_ENDPOS:
            if (getParentAdditionals().size() > 0) {
                return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            } else {
                return endPos;
            }
        case SUMO_ATTR_INDEX: // for writting sorted
            return (double)myCreationIndex;
        case GNE_ATTR_STOPINDEX: {
            // extract all stops of demandElement parent
            std::vector<GNEDemandElement*> stops, filteredStops;
            for (const auto& parent : getParentDemandElements().front()->getChildDemandElements()) {
                if (parent->getTagProperty().isStop()) {
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
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNEStopPlan::getAttributePosition(SumoXMLAttr key) const {
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
            throw InvalidArgument(getTagStr() + " doesn't have a position attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopPlan::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_ACTTYPE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStopPlan::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
            if (canParse<SUMOTime>(value)) {
                return parse<SUMOTime>(value) >= 0;
            } else {
                return false;
            }
        case SUMO_ATTR_ACTTYPE:
            return true;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopPlan::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
            undoList->add(new GNEChange_ToggleAttribute(this, key, true), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopPlan::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
            undoList->add(new GNEChange_ToggleAttribute(this, key, false), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStopPlan::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DURATION:
            return (parametersSet & STOP_DURATION_SET) != 0;
        case SUMO_ATTR_UNTIL:
            return (parametersSet & STOP_UNTIL_SET) != 0;
        default:
            return false;
    }
}


std::string
GNEStopPlan::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStopPlan::getHierarchyName() const {
    if (getParentAdditionals().size() > 0) {
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
            return "BusStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP) {
            return "TrainStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP) {
            return "containerStop: " + getParentAdditionals().front()->getID();
        } else if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION) {
            return "chargingStation: " + getParentAdditionals().front()->getID();
        } else {
            return "parkingArea: " + getParentAdditionals().front()->getID();
        }
    } else if (getParentEdges().size() > 0) {
        return "edge: " + getParentEdges().front()->getID();
    } else if (getParentLanes().size() > 0) {
        return "lane: " + getParentLanes().front()->getID();
    } else {
        return "";
    }
}


const Parameterised::Map&
GNEStopPlan::getACParametersMap() const {
    return getParametersMap();
}


double
GNEStopPlan::getStartGeometryPositionOverLane() const {
    double fixedPos = startPos;
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    if (fixedPos < 0) {
        fixedPos += len;
    }
    return fixedPos * getParentLanes().front()->getLengthGeometryFactor();
}


double
GNEStopPlan::getEndGeometryPositionOverLane() const {
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

const GNELane*
GNEStopPlan::getFirstAllowedLane() const {
    if (getParentEdges().empty()) {
        return nullptr;
    }
    for (const auto& pLane : getParentEdges().front()->getLanes()) {
        if (pLane->allowPedestrians()) {
            return pLane;
        }
    }
    return getParentEdges().front()->getLanes().front();
}


void
GNEStopPlan::drawStopPersonOverEdge(const GUIVisualizationSettings& s, const double exaggeration) const {
    // declare stop color
    const RGBColor stopColor = drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : s.colorSettings.stopColor;
    // avoid draw invisible elements
    if (stopColor.alpha() != 0) {
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add layer matrix matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // declare stop color
        // declare central line color
        const RGBColor centralLineColor = drawUsingSelectColor() ? stopColor.changedBrightness(-32) : RGBColor::WHITE;
        // set base color
        GLHelper::setColor(stopColor);
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, 0.3 * exaggeration);
        // move to front
        glTranslated(0, 0, .1);
        // set central color
        GLHelper::setColor(centralLineColor);
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, 0.05 * exaggeration);
        // move to icon position and front
        glTranslated(myDemandElementGeometry.getShape().front().x(), myDemandElementGeometry.getShape().front().y(), .1);
        // rotate over lane
        GUIGeometry::rotateOverLane((myDemandElementGeometry.getShapeRotations().front() * -1) + 90);
        // move again
        glTranslated(0, s.additionalSettings.vaporizerSize * exaggeration, 0);
        // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
        if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
            // set color
            glColor3d(1, 1, 1);
            // rotate texture
            glRotated(180, 0, 0, 1);
            // draw texture
            if (drawUsingSelectColor()) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPPERSON_SELECTED), s.additionalSettings.vaporizerSize * exaggeration);
            } else {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPPERSON), s.additionalSettings.vaporizerSize * exaggeration);
            }
        } else {
            // rotate
            glRotated(22.5, 0, 0, 1);
            // set stop color
            GLHelper::setColor(stopColor);
            // move matrix
            glTranslated(0, 0, 0);
            // draw filled circle
            GLHelper::drawFilledCircle(0.1 + s.additionalSettings.vaporizerSize, 8);
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
    }
    // check if mouse is over element
    mouseWithinGeometry(myDemandElementGeometry.getShape(), 0.3);
    // draw dotted geometry
    myContour.drawDottedContourExtruded(s, myDemandElementGeometry.getShape(), 0.3, exaggeration, true, true,
                                        s.dottedContourSettings.segmentWidth);
}


void
GNEStopPlan::drawStopPersonOverStoppingPlace(const GUIVisualizationSettings& s, const double exaggeration) const {
    // declare stop color
    const RGBColor stopColor = drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : s.colorSettings.stopColor;
    // avoid draw invisible elements
    if (stopColor.alpha() != 0) {
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add layer matrix matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // set base color
        GLHelper::setColor(stopColor);
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        if (getParentAdditionals().front()->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP) {
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, s.stoppingPlaceSettings.trainStopWidth * exaggeration);
        } else {
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, s.stoppingPlaceSettings.busStopWidth * exaggeration);
        }
        // move to icon position and front
        glTranslated(myDemandElementGeometry.getShape().getLineCenter().x(), myDemandElementGeometry.getShape().getLineCenter().y(), .1);
        // rotate over lane
        GUIGeometry::rotateOverLane((myDemandElementGeometry.getShapeRotations().front() * -1) + 90);
        // move again
        glTranslated(s.stoppingPlaceSettings.busStopWidth * exaggeration * -2, 0, 0);
        // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
        if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
            // set color
            glColor3d(1, 1, 1);
            // rotate texture
            glRotated(-90, 0, 0, 1);
            // draw texture
            if (drawUsingSelectColor()) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPPERSON_SELECTED),
                                                   s.additionalSettings.vaporizerSize * exaggeration);
            } else {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPPERSON),
                                                   s.additionalSettings.vaporizerSize * exaggeration);
            }
        } else {
            // rotate
            glRotated(22.5, 0, 0, 1);
            // set stop color
            GLHelper::setColor(stopColor);
            // move matrix
            glTranslated(0, 0, 0);
            // draw filled circle
            GLHelper::drawFilledCircle(0.1 + s.additionalSettings.vaporizerSize, 8);
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
        // draw dotted geometry
        myContour.drawDottedContourExtruded(s, myDemandElementGeometry.getShape(), 0.3, exaggeration, true, true,
                                            s.dottedContourSettings.segmentWidth);
    }
}


bool
GNEStopPlan::drawIndex() const {
    // get stop frame
    const auto stopFrame = myNet->getViewNet()->getViewParent()->getStopFrame();
    // check conditions
    if (myNet->getViewNet()->isAttributeCarrierInspected(getParentDemandElements().front())) {
        return true;
    } else if (stopFrame->shown() && (stopFrame->getStopParentSelector()->getCurrentDemandElement() == getParentDemandElements().front())) {
        return true;
    } else {
        return false;
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStopPlan::setAttribute(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_ACTTYPE:
            actType = value;
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopPlan::toggleAttribute(SumoXMLAttr key, const bool value) {
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
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopPlan::setMoveShape(const GNEMoveResult& moveResult) {
    // change endPos
    endPos = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEStopPlan::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(this, "position of " + getTagStr());
    if (myTagProperty.isStopPerson() || myTagProperty.isStopContainer()) {
        // now adjust endPos position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set attributes depending of operation type
        if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) {
            // set only start position
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        } else if (moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) {
            // set only end position
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
        } else {
            // set both
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos), undoList);
            // check if lane has to be changed
            if (moveResult.newFirstLane) {
                // set new lane
                setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
            }
        }
    }
    // end change attribute
    undoList->end();
}


int
GNEStopPlan::getPathStopIndex() const {
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
GNEStopPlan::setStopMicrosimID() {
    if (getParentAdditionals().size() > 0) {
        setDemandElementID(getMicrosimID() + " (" + getParentAdditionals().front()->getTagStr() + ")");
    } else if (getParentLanes().size() > 0) {
        setDemandElementID(getMicrosimID() + " (" + getParentLanes().front()->getTagStr() + ")");
    } else if (getParentEdges().size() > 0) {
        setDemandElementID(getMicrosimID() + " (" + getParentEdges().front()->getTagStr() + ")");
    }
}

/****************************************************************************/
