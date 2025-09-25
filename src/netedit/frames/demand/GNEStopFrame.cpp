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
/// @file    GNEStopFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// The Widget for add Stops elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNETagSelector.h>
#include <utils/foxtools/MFXDynamicLabel.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEStopFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEStopFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEStopFrame::HelpCreation::HelpCreation(GNEStopFrame* StopFrameParent) :
    MFXGroupBoxModule(StopFrameParent, TL("Help")),
    myStopFrameParent(StopFrameParent) {
    myInformationLabel = new MFXDynamicLabel(getCollapsableFrame(), "", 0, GUIDesignLabelFrameInformation);
}


GNEStopFrame::HelpCreation::~HelpCreation() {}


void
GNEStopFrame::HelpCreation::showHelpCreation() {
    // first update help creation
    updateHelpCreation();
    // show module
    show();
}


void
GNEStopFrame::HelpCreation::hideHelpCreation() {
    hide();
}


void
GNEStopFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected Stop type
    if (myStopFrameParent->myStopTagSelector->getCurrentTemplateAC()) {
        switch (myStopFrameParent->myStopTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag()) {
            case GNE_TAG_STOP_BUSSTOP:
            case GNE_TAG_WAYPOINT_BUSSTOP:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a bus stop to create a stop.");
                break;
            case GNE_TAG_STOP_TRAINSTOP:
            case GNE_TAG_WAYPOINT_TRAINSTOP:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a train stop to create a stop.");
                break;
            case GNE_TAG_STOP_CONTAINERSTOP:
            case GNE_TAG_WAYPOINT_CONTAINERSTOP:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a container stop to create a stop.");
                break;
            case GNE_TAG_STOP_CHARGINGSTATION:
            case GNE_TAG_WAYPOINT_CHARGINGSTATION:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a charging station to create a stop.");
                break;
            case GNE_TAG_STOP_PARKINGAREA:
            case GNE_TAG_WAYPOINT_PARKINGAREA:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a parking area to create a stop.");
                break;
            case GNE_TAG_STOP_LANE:
            case GNE_TAG_WAYPOINT_LANE:
                information
                        << "- " << TL("Shift+Click to select parent") << "\n"
                        << "- " << TL("Click over a lane to create a stop.");
                break;
            default:
                information
                        << "- " << TL("No stop parents in current network.");
                break;
        }
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEStopFrame - methods
// ---------------------------------------------------------------------------

GNEStopFrame::GNEStopFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Stops")),
    myStopParentBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // Create Stop parent selector
    myStopParentSelector = new GNEDemandElementSelector(this, {GNETagProperties::Type::VEHICLE, GNETagProperties::Type::ROUTE}, {GNE_TAG_ROUTE_EMBEDDED});

    // Create item Selector module for Stops
    myStopTagSelector = new GNETagSelector(this, GNETagProperties::Type::STOP_VEHICLE, GNE_TAG_STOP_LANE);

    // Create attributes editor
    myAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // Create Help Creation Module
    myHelpCreation = new HelpCreation(this);

    // refresh myStopParentMatchBox
    myStopParentSelector->refreshDemandElementSelector();
}


GNEStopFrame::~GNEStopFrame() {
    delete myStopParentBaseObject;
}


void
GNEStopFrame::show() {
    // first check if stop frame modules can be shown
    bool validStopParent = false;
    // check if at least there an item that supports an stop
    for (auto i = myStopParentSelector->getAllowedTags().begin(); (i != myStopParentSelector->getAllowedTags().end()) && (validStopParent == false); i++) {
        if (myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(*i).size() > 0) {
            validStopParent = true;
        }
    }
    // show or hide modules depending of validStopParent
    if (validStopParent) {
        // refresh tag selector
        myStopTagSelector->refreshTagSelector();
        // refresh vType selector
        myStopParentSelector->refreshDemandElementSelector();
        // refresh tag selector
        myStopTagSelector->refreshTagSelector();
        // show
        myStopParentSelector->showDemandElementSelector();
        myStopTagSelector->showTagSelector();
    } else {
        // hide modules (except help creation)
        myStopParentSelector->hideDemandElementSelector();
        myStopTagSelector->hideTagSelector();
        myAttributesEditor->hideAttributesEditor();
        // show help creation module
        myHelpCreation->showHelpCreation();
    }
    // reset last position
    myLastClickedPosition = Position::INVALID;
    // show frame
    GNEFrame::show();
}


bool
GNEStopFrame::addStop(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check stop type
    if (myStopTagSelector->getCurrentTemplateAC() == nullptr) {
        WRITE_WARNING(TL("Selected Stop type isn't valid."));
        return false;
    }
    // check last position
    if ((myViewNet->getPositionInformation() == myLastClickedPosition) && !myViewNet->getMouseButtonKeyPressed().shiftKeyPressed()) {
        WRITE_WARNING(TL("Shift + click to create two additionals in the same position"));
        return false;
    }
    // check if we're selecting a new stop parent
    if (mouseButtonKeyPressed.shiftKeyPressed()) {
        if (viewObjects.getDemandElementFront() &&
                (viewObjects.getDemandElementFront()->getTagProperty()->isVehicle() || viewObjects.getDemandElementFront()->getTagProperty()->getTag() == SUMO_TAG_ROUTE)) {
            myStopParentSelector->setDemandElement(viewObjects.getDemandElementFront());
            WRITE_WARNINGF(TL("Selected % '%' as stop parent."), viewObjects.getDemandElementFront()->getTagStr(), viewObjects.getDemandElementFront()->getID());
            return true;
        } else {
            WRITE_WARNING(TL("Selected Stop parent isn't valid."));
            return false;
        }
    } else {
        // now check if stop parent selector is valid
        if (myStopParentSelector->getCurrentDemandElement() == nullptr) {
            WRITE_WARNING(TL("Current selected Stop parent isn't valid."));
            return false;
        }
        // create stop base object
        getStopParameter(myStopTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag(),
                         viewObjects.getLaneFront(), viewObjects.getAdditionalFront());
        if (myStopParentBaseObject->getTag() != SUMO_TAG_NOTHING) {
            // declare route handler
            GNERouteHandler routeHandler(myViewNet->getNet(), myStopParentBaseObject->hasStringAttribute(GNE_ATTR_DEMAND_FILE) ?
                                         myStopParentBaseObject->getStringAttribute(GNE_ATTR_DEMAND_FILE) : "",
                                         myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
            // build stop
            routeHandler.buildStop(myStopParentBaseObject->getSumoBaseObjectChildren().front(), myPlanParameters,
                                   myStopParentBaseObject->getSumoBaseObjectChildren().front()->getStopParameter());
            // show all trips
            if (myStopTagSelector->getCurrentTemplateAC()->getTagProperty()->isVehicleStop()) {
                myViewNet->getDemandViewOptions().menuCheckShowAllTrips->setChecked(TRUE);
            } else {
                myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            }
            // stop successfully created, then return true
            return true;
        } else {
            return false;
        }
    }
}

bool
GNEStopFrame::getStopParameter(const SumoXMLTag stopTag, const GNELane* lane, const GNEAdditional* stoppingPlace) {
    // first clear containers
    myStopParentBaseObject->clear();
    myPlanParameters.clear();
    // declare stop parameters
    SUMOVehicleParameter::Stop stop;
    // first check that current selected Stop is valid
    if (stopTag == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("Current selected Stop type isn't valid."));
        return false;
    } else if ((stopTag == GNE_TAG_STOP_LANE) || (stopTag == GNE_TAG_WAYPOINT_LANE)) {
        if (lane) {
            stop.lane = lane->getID();
            if ((stopTag == GNE_TAG_WAYPOINT_LANE) && (stop.speed == 0)) {
                stop.speed = lane->getSpeed();
            }
            const Position viewPosSnapped = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());
            const double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(viewPosSnapped) / lane->getLengthGeometryFactor();
            stop.startPos = mousePositionOverLane - 10;
            if (stop.startPos < 0) {
                stop.startPos = 0;
            }
            stop.parametersSet |= STOP_START_SET;
            stop.endPos = mousePositionOverLane;
            stop.parametersSet |= STOP_END_SET;
        } else {
            WRITE_WARNING("Click over a " + toString(SUMO_TAG_LANE) + " to create a stop placed in a " + toString(SUMO_TAG_LANE));
            return false;
        }
    } else if ((stopTag == GNE_TAG_STOPPERSON_EDGE) || (stopTag == GNE_TAG_STOPCONTAINER_EDGE)) {
        if (lane) {
            stop.edge = lane->getParentEdge()->getID();
            myPlanParameters.toEdge = stop.edge;
        } else {
            WRITE_WARNING("Click over a " + toString(SUMO_TAG_EDGE) + " to create a stop placed in a " + toString(SUMO_TAG_EDGE));
            return false;
        }
    } else if (stoppingPlace) {
        if (stoppingPlace->getTagProperty()->getTag() == SUMO_TAG_BUS_STOP) {
            if ((stopTag != GNE_TAG_STOP_BUSSTOP) && (stopTag != GNE_TAG_WAYPOINT_BUSSTOP) && (stopTag != GNE_TAG_STOPPERSON_BUSSTOP)) {
                WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a " + stoppingPlace->getTagProperty()->getTagStr());
                return false;
            } else {
                stop.busstop = stoppingPlace->getID();
                myPlanParameters.toBusStop = stop.busstop;
                if ((stopTag == GNE_TAG_WAYPOINT_BUSSTOP) && (stop.speed == 0)) {
                    stop.speed = stoppingPlace->getParentLanes().front()->getSpeed();
                }
                stop.startPos = 0;
                stop.endPos = 0;
            }
        } else if (stoppingPlace->getTagProperty()->getTag() == SUMO_TAG_TRAIN_STOP) {
            if ((stopTag != GNE_TAG_STOP_TRAINSTOP) && (stopTag != GNE_TAG_WAYPOINT_TRAINSTOP)) {
                WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a " + stoppingPlace->getTagProperty()->getTagStr());
                return false;
            } else {
                stop.busstop = stoppingPlace->getID();
                myPlanParameters.toTrainStop = stop.busstop;
                if ((stopTag == GNE_TAG_WAYPOINT_TRAINSTOP) && (stop.speed == 0)) {
                    stop.speed = stoppingPlace->getParentLanes().front()->getSpeed();
                }
                stop.startPos = 0;
                stop.endPos = 0;
            }
        } else if (stoppingPlace->getTagProperty()->getTag() == SUMO_TAG_CONTAINER_STOP) {
            if ((stopTag != GNE_TAG_STOP_CONTAINERSTOP) && (stopTag != GNE_TAG_WAYPOINT_CONTAINERSTOP)) {
                WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a " + stoppingPlace->getTagProperty()->getTagStr());
                return false;
            } else {
                stop.containerstop = stoppingPlace->getID();
                myPlanParameters.toContainerStop = stop.containerstop;
                if ((stopTag == GNE_TAG_WAYPOINT_CONTAINERSTOP) && (stop.speed == 0)) {
                    stop.speed = stoppingPlace->getParentLanes().front()->getSpeed();
                }
                stop.startPos = 0;
                stop.endPos = 0;
            }
        } else if (stoppingPlace->getTagProperty()->getTag() == SUMO_TAG_CHARGING_STATION) {
            if ((stopTag != GNE_TAG_STOP_CHARGINGSTATION) && (stopTag != GNE_TAG_WAYPOINT_CHARGINGSTATION)) {
                WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a " + stoppingPlace->getTagProperty()->getTagStr());
                return false;
            } else {
                stop.chargingStation = stoppingPlace->getID();
                myPlanParameters.toChargingStation = stop.chargingStation;
                if ((stopTag == GNE_TAG_WAYPOINT_CHARGINGSTATION) && (stop.speed == 0)) {
                    stop.speed = stoppingPlace->getParentLanes().front()->getSpeed();
                }
                stop.startPos = 0;
                stop.endPos = 0;
            }
        } else if (stoppingPlace->getTagProperty()->getTag() == SUMO_TAG_PARKING_AREA) {
            if ((stopTag != GNE_TAG_STOP_PARKINGAREA) && (stopTag != GNE_TAG_WAYPOINT_PARKINGAREA)) {
                WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a " + stoppingPlace->getTagProperty()->getTagStr());
                return false;
            } else {
                stop.parkingarea = stoppingPlace->getID();
                myPlanParameters.toParkingArea = stop.parkingarea;
                if ((stopTag == GNE_TAG_WAYPOINT_PARKINGAREA) && (stop.speed == 0)) {
                    stop.speed = stoppingPlace->getParentLanes().front()->getSpeed();
                }
                stop.startPos = 0;
                stop.endPos = 0;
            }
        }
    } else {
        if ((stopTag == GNE_TAG_STOP_BUSSTOP) || (stopTag == GNE_TAG_WAYPOINT_BUSSTOP)) {
            WRITE_WARNING("Click over a " + toString(GNE_TAG_STOP_BUSSTOP) + " to create a stop placed in a " + toString(GNE_TAG_STOP_BUSSTOP));
        } else if ((stopTag == GNE_TAG_STOP_TRAINSTOP) || (stopTag == GNE_TAG_WAYPOINT_TRAINSTOP)) {
            WRITE_WARNING("Click over a " + toString(GNE_TAG_STOP_TRAINSTOP) + " to create a stop placed in a " + toString(GNE_TAG_STOP_TRAINSTOP));
        } else if ((stopTag == GNE_TAG_STOP_CONTAINERSTOP) || (stopTag == GNE_TAG_WAYPOINT_CONTAINERSTOP)) {
            WRITE_WARNING("Click over a " + toString(SUMO_TAG_CONTAINER_STOP) + " to create a stop placed in a " + toString(SUMO_TAG_CONTAINER_STOP));
        } else if ((stopTag == GNE_TAG_STOP_CHARGINGSTATION) || (stopTag == GNE_TAG_WAYPOINT_CHARGINGSTATION)) {
            WRITE_WARNING("Click over a " + toString(SUMO_TAG_CHARGING_STATION) + " to create a stop placed in a " + toString(SUMO_TAG_CHARGING_STATION));
        } else if ((stopTag == GNE_TAG_STOP_PARKINGAREA) || (stopTag == GNE_TAG_WAYPOINT_PARKINGAREA)) {
            WRITE_WARNING("Click over a " + toString(SUMO_TAG_PARKING_AREA) + " to create a stop placed in a " + toString(SUMO_TAG_PARKING_AREA));
        } else if (stopTag == GNE_TAG_STOPPERSON_BUSSTOP) {
            WRITE_WARNING("Click over a " + toString(GNE_TAG_STOP_BUSSTOP) + " to create a person stop placed in a " + toString(GNE_TAG_STOP_BUSSTOP));
        } else if (stopTag == GNE_TAG_STOPPERSON_TRAINSTOP) {
            WRITE_WARNING("Click over a " + toString(GNE_TAG_STOP_TRAINSTOP) + " to create a person stop placed in a " + toString(GNE_TAG_STOP_TRAINSTOP));
        }
        return false;
    }
    // check if stop attributes are valid
    if (!myAttributesEditor->checkAttributes(true)) {
        return false;
    }
    // get stop parent
    const GNEDemandElement* stopParent = myStopParentSelector->getCurrentDemandElement();
    // if stopParent is a route, check that stop is placed over a route's edge
    if (stopParent->isRoute() && lane) {
        bool found = false;
        for (const auto& edge : stopParent->getParentEdges()) {
            if (edge == lane->getParentEdge()) {
                found = true;
            }
        }
        if (!found) {
            WRITE_WARNING(TL("Stop must be placed over a route's edge"));
            return false;
        }
    }
    // same if stoParent is a vehicle/flow with embedded route
    if (stopParent->getChildDemandElements().size() > 0 && stopParent->getChildDemandElements().front()->getTagProperty()->isRoute() && lane) {
        bool found = false;
        for (const auto& edge : stopParent->getChildDemandElements().front()->getParentEdges()) {
            if (edge == lane->getParentEdge()) {
                found = true;
            }
        }
        if (!found) {
            WRITE_WARNING(TL("Stop must be placed over an embedded route's edge"));
            return false;
        }
    }
    // set parent tag and id
    myStopParentBaseObject->setTag(stopParent->getTagProperty()->getTag());
    myStopParentBaseObject->addStringAttribute(SUMO_ATTR_ID, stopParent->getID());
    // add route, from and to attributes
    if (stopParent->getTagProperty()->hasAttribute(SUMO_ATTR_ROUTE)) {
        myStopParentBaseObject->addStringAttribute(SUMO_ATTR_ROUTE, stopParent->getAttribute(SUMO_ATTR_ROUTE));
    }
    if (stopParent->getTagProperty()->hasAttribute(SUMO_ATTR_FROM)) {
        myStopParentBaseObject->addStringAttribute(SUMO_ATTR_FROM, stopParent->getAttribute(SUMO_ATTR_FROM));
    }
    if (stopParent->getTagProperty()->hasAttribute(SUMO_ATTR_TO)) {
        myStopParentBaseObject->addStringAttribute(SUMO_ATTR_TO, stopParent->getAttribute(SUMO_ATTR_TO));
    }
    // create stop object
    CommonXMLStructure::SumoBaseObject* stopBaseObject = new CommonXMLStructure::SumoBaseObject(myStopParentBaseObject);
    // get stop attributes
    myAttributesEditor->fillSumoBaseObject(stopBaseObject);
    // obtain friendly position
    if (stopBaseObject->hasBoolAttribute(SUMO_ATTR_FRIENDLY_POS)) {
        stop.friendlyPos = stopBaseObject->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS);
    }
    // obtain posLat
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_POSITION_LAT)) {
        if (GNEAttributeCarrier::canParse<double>(stopBaseObject->getStringAttribute(SUMO_ATTR_POSITION_LAT))) {
            stop.posLat = GNEAttributeCarrier::parse<double>(stopBaseObject->getStringAttribute(SUMO_ATTR_POSITION_LAT));
            stop.parametersSet |= STOP_POSLAT_SET;
        } else {
            stop.posLat = INVALID_DOUBLE;
        }
    }
    // obtain actType
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_ACTTYPE)) {
        stop.actType = stopBaseObject->getStringAttribute(SUMO_ATTR_ACTTYPE);
    }
    // fill rest of parameters depending if it was edited
    if (stopBaseObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
        stop.duration = stopBaseObject->getTimeAttribute(SUMO_ATTR_DURATION);
        if (stop.duration >= 0) {
            stop.parametersSet |= STOP_DURATION_SET;
        }
    }
    if (stopBaseObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
        stop.until = stopBaseObject->getTimeAttribute(SUMO_ATTR_UNTIL);
        if (stop.until >= 0) {
            stop.parametersSet |= STOP_UNTIL_SET;
        }
    }
    if (stopBaseObject->hasTimeAttribute(SUMO_ATTR_EXTENSION)) {
        stop.extension = stopBaseObject->getTimeAttribute(SUMO_ATTR_EXTENSION);
        if (stop.extension >= 0) {
            stop.parametersSet |= STOP_EXTENSION_SET;
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_TRIGGERED)) {
        if ((stopBaseObject->getStringAttribute(SUMO_ATTR_TRIGGERED) == "person") || (stopBaseObject->getStringAttribute(SUMO_ATTR_TRIGGERED) == "true")) {
            stop.parametersSet |= STOP_TRIGGER_SET;
            stop.triggered = true;
        } else if (stopBaseObject->getStringAttribute(SUMO_ATTR_TRIGGERED) == "container") {
            stop.parametersSet |= STOP_CONTAINER_TRIGGER_SET;
            stop.containerTriggered = true;
        } else if (stopBaseObject->getStringAttribute(SUMO_ATTR_TRIGGERED) == "join") {
            stop.parametersSet |= STOP_JOIN_SET;
            stop.joinTriggered = true;
        }
    }
    if (stopBaseObject->hasStringListAttribute(SUMO_ATTR_EXPECTED)) {
        const auto expected = stopBaseObject->getStringListAttribute(SUMO_ATTR_EXPECTED);
        if (expected.size() > 0) {
            if (stop.triggered) {
                for (const auto& id : expected) {
                    stop.awaitedPersons.insert(id);
                }
                stop.parametersSet |= STOP_EXPECTED_SET;
            } else if (stop.containerTriggered) {
                for (const auto& id : expected) {
                    stop.awaitedContainers.insert(id);
                }
                stop.parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
            }
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_JOIN)) {
        stop.join = stopBaseObject->getStringAttribute(SUMO_ATTR_JOIN);
        if (stop.join.size() > 0) {
            stop.parametersSet |= STOP_JOIN_SET;
        }
    }
    if (stopBaseObject->hasStringListAttribute(SUMO_ATTR_PERMITTED)) {
        const auto permitted = stopBaseObject->getStringListAttribute(SUMO_ATTR_PERMITTED);
        if (permitted.size() > 0) {
            stop.parametersSet |= STOP_PERMITTED_SET;
            for (const auto& permit : permitted) {
                stop.permitted.insert(permit);
            }
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_PARKING)) {
        if (stopBaseObject->getStringAttribute(SUMO_ATTR_PARKING) == "true") {
            stop.parking = ParkingType::ONROAD;
            stop.parametersSet |= STOP_PARKING_SET;
        } else if (stopBaseObject->getStringAttribute(SUMO_ATTR_PARKING) == "opportunistic") {
            stop.parking = ParkingType::OPPORTUNISTIC;
            stop.parametersSet |= STOP_PARKING_SET;
        } else {
            stop.parking = ParkingType::OFFROAD;
        }
    }
    if (stopBaseObject->hasTimeAttribute(SUMO_ATTR_JUMP)) {
        stop.jump = stopBaseObject->getTimeAttribute(SUMO_ATTR_JUMP);
        if (stop.jump >= 0) {
            stop.parametersSet |= STOP_JUMP_SET;
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_SPLIT)) {
        stop.split = stopBaseObject->getStringAttribute(SUMO_ATTR_SPLIT);
        if (stop.split.size() > 0) {
            stop.parametersSet |= STOP_SPLIT_SET;
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_TRIP_ID)) {
        stop.tripId = stopBaseObject->getStringAttribute(SUMO_ATTR_TRIP_ID);
        if (stop.tripId.size() > 0) {
            stop.parametersSet |= STOP_TRIP_ID_SET;
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_LINE)) {
        stop.line = stopBaseObject->getStringAttribute(SUMO_ATTR_LINE);
        if (stop.line.size() > 0) {
            stop.parametersSet |= STOP_LINE_SET;
        }
    }
    if (stopBaseObject->hasBoolAttribute(SUMO_ATTR_ONDEMAND)) {
        stop.onDemand = stopBaseObject->getBoolAttribute(SUMO_ATTR_ONDEMAND);
        if (stop.onDemand) {
            stop.parametersSet |= STOP_ONDEMAND_SET;
        }
    }
    if (stopBaseObject->hasDoubleAttribute(SUMO_ATTR_SPEED) && (stopBaseObject->getDoubleAttribute(SUMO_ATTR_SPEED) > 0)) {
        stop.speed = stopBaseObject->getDoubleAttribute(SUMO_ATTR_SPEED);
        if (stop.speed > 0) {
            stop.parametersSet |= STOP_SPEED_SET;
        }
    }
    if (stopBaseObject->hasStringAttribute(SUMO_ATTR_INDEX)) {
        if (stopBaseObject->getStringAttribute(SUMO_ATTR_INDEX) == "fit") {
            stop.index = STOP_INDEX_FIT;
        } else if (stopBaseObject->getStringAttribute(SUMO_ATTR_INDEX) == "end") {
            stop.index = STOP_INDEX_END;
        } else if (GNEAttributeCarrier::canParse<int>(stopBaseObject->getStringAttribute(SUMO_ATTR_INDEX))) {
            stop.index = GNEAttributeCarrier::parse<int>(stopBaseObject->getStringAttribute(SUMO_ATTR_INDEX));
        } else {
            stop.index = STOP_INDEX_END;
        }
    } else {
        stop.index = STOP_INDEX_END;
    }
    // refresh attributes editor
    myAttributesEditor->refreshAttributesEditor();
    // set tag
    stopBaseObject->setTag(stopTag);
    stopBaseObject->setStopParameter(stop);
    return true;
}


GNEDemandElementSelector*
GNEStopFrame::getStopParentSelector() const {
    return myStopParentSelector;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEStopFrame::tagSelected() {
    if (myStopTagSelector->getCurrentTemplateAC()) {
        // show Stop type selector module
        myAttributesEditor->showAttributesEditor(myStopTagSelector->getCurrentTemplateAC(), true);
        myHelpCreation->showHelpCreation();
    } else {
        // hide all modules if stop parent isn't valid
        myAttributesEditor->hideAttributesEditor();
        myHelpCreation->hideHelpCreation();
    }
}


void
GNEStopFrame::demandElementSelected() {
    // show or hide modules depending if current selected stop parent is valid
    if (myStopParentSelector->getCurrentDemandElement()) {
        myStopTagSelector->showTagSelector();
        if (myStopTagSelector->getCurrentTemplateAC()) {
            // show modules
            myAttributesEditor->showAttributesEditor(myStopTagSelector->getCurrentTemplateAC(), true);
            myHelpCreation->showHelpCreation();
        } else {
            myAttributesEditor->hideAttributesEditor();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        // hide modules
        myStopTagSelector->hideTagSelector();
        myAttributesEditor->hideAttributesEditor();
        myHelpCreation->hideHelpCreation();
    }
}

/****************************************************************************/
