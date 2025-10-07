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
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// Builds trigger objects for netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_TAZSourceSink.h>
#include <netedit/dialogs/basic/GNEOverwriteElement.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEAccess.h"
#include "GNEAdditionalHandler.h"
#include "GNEBusStop.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEEntryExitDetector.h"
#include "GNEInductionLoopDetector.h"
#include "GNEInstantInductionLoopDetector.h"
#include "GNELaneAreaDetector.h"
#include "GNEMultiEntryExitDetector.h"
#include "GNEOverheadWire.h"
#include "GNEPOI.h"
#include "GNEParkingArea.h"
#include "GNEParkingAreaReroute.h"
#include "GNEParkingSpace.h"
#include "GNEPoly.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERerouterSymbol.h"
#include "GNERouteProbReroute.h"
#include "GNERouteProbe.h"
#include "GNETAZ.h"
#include "GNETractionSubstation.h"
#include "GNEVaporizer.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNEVariableSpeedSignSymbol.h"

// ===========================================================================
// GNEAdditionalHandler method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(GNENet* net, const std::string& filename, const bool allowUndoRedo) :
    AdditionalHandler(filename),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {
}


bool
GNEAdditionalHandler::postParserTasks() {
    // nothing to do
    return true;
}


bool
GNEAdditionalHandler::buildBusStop(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id,
                                   const std::string& laneID, const double startPos, const double endPos, const std::string& name,
                                   const std::vector<std::string>& lines, const int personCapacity, const double parkingLength,
                                   const RGBColor& color, const bool friendlyPosition, const double angle,
                                   const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::busStops, id);
    if (!checkElement(SUMO_TAG_BUS_STOP, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_BUS_STOP, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_BUS_STOP, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLaneDoublePosition(startPos, endPos, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosition)) {
            return writeErrorInvalidPosition(SUMO_TAG_BUS_STOP, id);
        } else if (!checkNegative(SUMO_TAG_BUS_STOP, id, SUMO_ATTR_PERSON_CAPACITY, personCapacity, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_BUS_STOP, id, SUMO_ATTR_PARKING_LENGTH, parkingLength, true)) {
            return false;
        } else {
            // build busStop
            GNEAdditional* busStop = GNEBusStop::buildBusStop(id, myNet, myFilename, lane, startPos, endPos, name, lines, personCapacity,
                                     parkingLength, color, friendlyPosition, angle, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(busStop, TL("add bus stop '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(busStop);
                lane->addChildElement(busStop);
                busStop->incRef("buildBusStop");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildTrainStop(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id,
                                     const std::string& laneID, const double startPos, const double endPos, const std::string& name,
                                     const std::vector<std::string>& lines, const int personCapacity, const double parkingLength,
                                     const RGBColor& color, const bool friendlyPosition, const double angle,
                                     const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::busStops, id);
    if (!checkElement(SUMO_TAG_TRAIN_STOP, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_TRAIN_STOP, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_TRAIN_STOP, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLaneDoublePosition(startPos, endPos, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosition)) {
            return writeErrorInvalidPosition(SUMO_TAG_TRAIN_STOP, id);
        } else if (!checkNegative(SUMO_TAG_TRAIN_STOP, id, SUMO_ATTR_PERSON_CAPACITY, personCapacity, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_TRAIN_STOP, id, SUMO_ATTR_PARKING_LENGTH, parkingLength, true)) {
            return false;
        } else {
            // build trainStop
            GNEAdditional* trainStop = GNEBusStop::buildTrainStop(id, myNet, myFilename, lane, startPos, endPos, name, lines, personCapacity,
                                       parkingLength, color, friendlyPosition, angle, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(trainStop, TL("add train stop '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(trainStop, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(trainStop);
                lane->addChildElement(trainStop);
                trainStop->incRef("buildTrainStop");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildAccess(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& laneID,
                                  const std::string& pos, const double length, const bool friendlyPos, const Parameterised::Map& parameters) {
    // get lane
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
    // get busStop (or trainStop)
    const auto busStop = getAdditionalParent(sumoBaseObject, SUMO_TAG_BUS_STOP);
    const auto trainStop = getAdditionalParent(sumoBaseObject, SUMO_TAG_TRAIN_STOP);
    const auto containerStop = getAdditionalParent(sumoBaseObject, SUMO_TAG_CONTAINER_STOP);
    // check parent
    if ((busStop == nullptr) && (trainStop == nullptr) && (containerStop == nullptr)) {
        return writeErrorInvalidParent(SUMO_TAG_ACCESS, "", sumoBaseObject->getParentSumoBaseObject()->getTag(), sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    }
    GNEAdditional* accessParent = busStop ? busStop : trainStop ? trainStop : containerStop;
    // pos double
    bool validPos = true;
    double posDouble = 0;
    if (lane) {
        if (GNEAttributeCarrier::canParse<double>(pos)) {
            posDouble = GNEAttributeCarrier::parse<double>(pos);
            validPos = checkLanePosition(posDouble, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos);
        } else if (pos == "random" || pos == "doors" || pos == "carriage") {
            posDouble = INVALID_DOUBLE;
        } else if (pos.empty()) {
            posDouble = 0;
        } else {
            validPos = false;
        }
    }
    // Check if lane is correct
    if (lane == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_ACCESS, "", SUMO_TAG_LANE, laneID);
    } else if (!validPos) {
        return writeErrorInvalidPosition(SUMO_TAG_ACCESS, accessParent->getID());
    } else if ((length != -1) && !checkNegative(SUMO_TAG_ACCESS, accessParent->getID(), SUMO_ATTR_LENGTH, length, true)) {
        return false;
    } else if (!accessExists(accessParent, lane->getParentEdge())) {
        return writeError(TLF("Could not build access in netedit; % '%' already owns an access in the edge '%'", accessParent->getTagStr(), accessParent->getID(), lane->getParentEdge()->getID()));
    } else if (!containerStop && !lane->allowPedestrians()) {
        // only for busStops and trainStops
        return writeError(TLF("Could not build access in netedit; The lane '%' doesn't support pedestrians", lane->getID()));
    } else {
        // build access
        GNEAdditional* access = new GNEAccess(accessParent, lane, posDouble, pos, friendlyPos, length, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(access, TL("add access in '") + accessParent->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(access, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(access);
            lane->addChildElement(access);
            accessParent->addChildElement(access);
            access->incRef("buildAccess");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildContainerStop(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& laneID,
        const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines, const int containerCapacity,
        const double parkingLength, const RGBColor& color, const bool friendlyPosition, const double angle, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_CONTAINER_STOP}, id);
    if (!checkElement(SUMO_TAG_CONTAINER_STOP, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_CONTAINER_STOP, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_CONTAINER_STOP, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLaneDoublePosition(startPos, endPos, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosition)) {
            return writeErrorInvalidPosition(SUMO_TAG_CONTAINER_STOP, id);
        } else if (!checkNegative(SUMO_TAG_CONTAINER_STOP, id, SUMO_ATTR_CONTAINER_CAPACITY, containerCapacity, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_CONTAINER_STOP, id, SUMO_ATTR_PARKING_LENGTH, parkingLength, true)) {
            return false;
        } else {
            // build containerStop
            GNEAdditional* containerStop = new GNEContainerStop(id, myNet, myFilename, lane, startPos, endPos, name, lines, containerCapacity, parkingLength,
                    color, friendlyPosition, angle, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(containerStop, TL("add container stop '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(containerStop);
                lane->addChildElement(containerStop);
                containerStop->incRef("buildContainerStop");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildChargingStation(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id,
        const std::string& laneID, const double startPos, const double endPos, const std::string& name, const double chargingPower,
        const double efficiency, const bool chargeInTransit, const SUMOTime chargeDelay, const std::string& chargeType,
        const SUMOTime waitingTime, const bool friendlyPosition, const std::string& parkingAreaID, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_CHARGING_STATION}, id);
    if (!checkElement(SUMO_TAG_CHARGING_STATION, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_CHARGING_STATION, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_CHARGING_STATION, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLaneDoublePosition(startPos, endPos, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosition)) {
            return writeErrorInvalidPosition(SUMO_TAG_CHARGING_STATION, id);
        } else if (!checkNegative(SUMO_TAG_CHARGING_STATION, id, SUMO_ATTR_CHARGINGPOWER, chargingPower, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_CHARGING_STATION, id, SUMO_ATTR_CHARGEDELAY, chargeDelay, true)) {
            return false;
        } else if (!SUMOXMLDefinitions::ChargeTypes.hasString(chargeType)) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Invalid charge type '%' .", toString(SUMO_TAG_CHARGING_STATION), id, chargeType));
        } else {
            // build chargingStation
            GNEAdditional* chargingStation = new GNEChargingStation(id, myNet, myFilename, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit,
                    chargeDelay, chargeType, waitingTime, parkingAreaID, friendlyPosition, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(chargingStation, TL("add charging station '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(chargingStation);
                lane->addChildElement(chargingStation);
                chargingStation->incRef("buildChargingStation");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildParkingArea(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& laneID,
                                       const double startPos, const double endPos, const std::string& departPos, const std::string& name,
                                       const std::vector<std::string>& badges, const bool friendlyPosition, const int roadSideCapacity, const bool onRoad,
                                       const double width, const double length, const double angle, const bool lefthand, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_PARKING_AREA}, id);
    if (!checkElement(SUMO_TAG_PARKING_AREA, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_PARKING_AREA, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // get departPos double
        const double departPosDouble = GNEAttributeCarrier::canParse<double>(departPos) ? GNEAttributeCarrier::parse<double>(departPos) : 0;
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_PARKING_AREA, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLaneDoublePosition(startPos, endPos, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosition)) {
            return writeErrorInvalidPosition(SUMO_TAG_PARKING_AREA, id);
        } else if (!checkNegative(SUMO_TAG_PARKING_AREA, id, SUMO_ATTR_ROADSIDE_CAPACITY, roadSideCapacity, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_PARKING_AREA, id, SUMO_ATTR_WIDTH, width, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_PARKING_AREA, id, SUMO_ATTR_LENGTH, length, true)) {
            return false;
        } else if ((departPosDouble < 0) || (departPosDouble > lane->getParentEdge()->getNBEdge()->getFinalLength())) {
            return writeError(TLF("Could not build parking area with ID '%' in netedit; Invalid departPos over lane.", id));
        } else {
            // build parkingArea
            GNEAdditional* parkingArea = new GNEParkingArea(id, myNet, myFilename, lane, startPos, endPos, GNEAttributeCarrier::canParse<double>(departPos) ? departPos : "",
                    name, badges, friendlyPosition, roadSideCapacity, onRoad,
                    (width == 0) ? SUMO_const_laneWidth : width, length, angle, lefthand, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(parkingArea, TL("add parking area '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingArea, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(parkingArea);
                lane->addChildElement(parkingArea);
                parkingArea->incRef("buildParkingArea");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildParkingSpace(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const double x, const double y, const double z,
                                        const std::string& name, const std::string& width, const std::string& length, const std::string& angle, const double slope,
                                        const Parameterised::Map& parameters) {
    // check width and heights
    if (!width.empty() && !GNEAttributeCarrier::canParse<double>(width)) {
        return writeError(TL("Could not build parking space in netedit; attribute width cannot be parse to float."));
    } else if (!length.empty() && !GNEAttributeCarrier::canParse<double>(length)) {
        return writeError(TL("Could not build parking space in netedit; attribute length cannot be parse to float."));
    } else if (!angle.empty() && !GNEAttributeCarrier::canParse<double>(angle)) {
        return writeError(TL("Could not build parking space in netedit; attribute angle cannot be parse to float."));
    } else {
        // get lane
        GNEAdditional* parkingArea = getAdditionalParent(sumoBaseObject, SUMO_TAG_PARKING_AREA);
        // get double values
        const double widthDouble = width.empty() ? INVALID_DOUBLE : GNEAttributeCarrier::parse<double>(width);
        const double lengthDouble = length.empty() ? INVALID_DOUBLE : GNEAttributeCarrier::parse<double>(length);
        const double angleDouble = angle.empty() ? INVALID_DOUBLE : GNEAttributeCarrier::parse<double>(angle);
        // check lane
        if (parkingArea == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_PARKING_SPACE, "", SUMO_TAG_PARKING_AREA, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
        } else if ((widthDouble != INVALID_DOUBLE) && !checkNegative(SUMO_TAG_PARKING_SPACE, parkingArea->getID(), SUMO_ATTR_WIDTH, widthDouble, true)) {
            return false;
        } else if ((lengthDouble != INVALID_DOUBLE) && !checkNegative(SUMO_TAG_PARKING_SPACE, parkingArea->getID(), SUMO_ATTR_LENGTH, lengthDouble, true)) {
            return false;
        } else {
            // build parkingSpace
            GNEAdditional* parkingSpace = new GNEParkingSpace(parkingArea, Position(x, y, z), widthDouble, lengthDouble, angleDouble, slope, name, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(parkingSpace, TL("add parking space in '") + parkingArea->getID() + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingSpace, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(parkingSpace);
                parkingArea->addChildElement(parkingSpace);
                parkingSpace->incRef("buildParkingSpace");
            }
            // update geometry (due boundaries)
            parkingSpace->updateGeometry();
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildE1Detector(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& laneID,
                                      const double position, const SUMOTime period, const std::string& file, const std::vector<std::string>& vehicleTypes,
                                      const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                                      const bool friendlyPos, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_INDUCTION_LOOP}, id);
    if (!checkElement(SUMO_TAG_INDUCTION_LOOP, element)) {
        return false;
    } else if (!checkValidDetectorID(SUMO_TAG_INDUCTION_LOOP, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_INDUCTION_LOOP, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLanePosition(position, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
            return writeErrorInvalidPosition(SUMO_TAG_INDUCTION_LOOP, id);
        } else if (!checkNegative(SUMO_TAG_INDUCTION_LOOP, id, SUMO_ATTR_PERIOD, period, true)) {
            return false;
        } else if (!checkFileName(SUMO_TAG_INDUCTION_LOOP, id, SUMO_ATTR_FILE, file)) {
            return false;
        } else if (!checkListOfVehicleTypes(SUMO_TAG_INDUCTION_LOOP, id, vehicleTypes)) {
            return false;
        } else {
            // build E1
            GNEAdditional* detectorE1 = new GNEInductionLoopDetector(id, myNet, myFilename, lane, position, period, file, vehicleTypes,
                    nextEdges, detectPersons, name, friendlyPos, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(detectorE1, TL("add induction loop '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(detectorE1);
                lane->addChildElement(detectorE1);
                detectorE1->incRef("buildDetectorE1");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildSingleLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& laneID,
        const double pos, const double length, const SUMOTime period, const std::string& trafficLight, const std::string& filename,
        const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges, const std::string& detectPersons,
        const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold,
        const bool friendlyPos, const bool show, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::laneAreaDetectors, id);
    if (!checkElement(SUMO_TAG_LANE_AREA_DETECTOR, element)) {
        return false;
    } else if (!checkValidDetectorID(SUMO_TAG_LANE_AREA_DETECTOR, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_TAG_LANE, laneID);
        } else {
            // check friendlyPos in small lanes
            const bool friendlyPosCheck = checkFriendlyPosSmallLanes(pos, length, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos);
            if (!checkLanePosition(pos, length, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPosCheck)) {
                return writeErrorInvalidPosition(SUMO_TAG_LANE_AREA_DETECTOR, id);
            } else if (!checkNegative(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_LENGTH, length, true)) {
                return false;
            } else if ((period != -1) && !checkNegative(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_PERIOD, period, true)) {
                return false;
            } else if ((trafficLight.size() > 0) && !(SUMOXMLDefinitions::isValidNetID(trafficLight))) {
                // temporal
                return writeError(TLF("Could not build lane area detector with ID '%' in netedit; invalid traffic light ID.", id));
            } else if (!checkNegative(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_HALTING_TIME_THRESHOLD, timeThreshold, true)) {
                return false;
            } else if (!checkNegative(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold, true)) {
                return false;
            } else if (!checkNegative(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_JAM_DIST_THRESHOLD, jamThreshold, true)) {
                return false;
            } else if (!checkFileName(SUMO_TAG_LANE_AREA_DETECTOR, id, SUMO_ATTR_FILE, filename)) {
                return false;
            } else if (!checkListOfVehicleTypes(SUMO_TAG_LANE_AREA_DETECTOR, id, vehicleTypes)) {
                return false;
            } else {
                // build E2 single lane
                GNEAdditional* detectorE2 = new GNELaneAreaDetector(id, myNet, myFilename, lane, pos, length, period, trafficLight, filename,
                        vehicleTypes, nextEdges, detectPersons, name, timeThreshold,
                        speedThreshold, jamThreshold, friendlyPosCheck, show, parameters);
                // insert depending of allowUndoRedo
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(detectorE2, TL("add lane area detector '") + id + "'");
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertAdditional(detectorE2);
                    lane->addChildElement(detectorE2);
                    detectorE2->incRef("buildDetectorE2");
                }
                return true;
            }
        }
    }
}


bool
GNEAdditionalHandler::buildMultiLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::vector<std::string>& laneIDs,
        const double pos, const double endPos, const SUMOTime period, const std::string& trafficLight, const std::string& filename,
        const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges, const std::string& detectPersons,
        const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold,
        const bool friendlyPos, const bool show, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::laneAreaDetectors, id);
    if (!checkElement(GNE_TAG_MULTI_LANE_AREA_DETECTOR, element)) {
        return false;
    } else if (!checkValidDetectorID(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id)) {
        return false;
    } else {
        // get lanes
        const auto lanes = parseLanes(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, laneIDs);
        // check lanes
        if (lanes.empty()) {
            return false;
        } else {
            // calculate path
            if (!GNEAdditional::areLaneConsecutives(lanes)) {
                return writeError(TLF("Could not build lane area detector with ID '%' in netedit; Lanes aren't consecutives.", id));
            } else if (!checkMultiLanePosition(
                           pos, lanes.front()->getParentEdge()->getNBEdge()->getFinalLength(),
                           endPos, lanes.back()->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
                return writeErrorInvalidPosition(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id);
            } else if ((period != -1) && !checkNegative(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, SUMO_ATTR_PERIOD, period, true)) {
                return false;
            } else if ((trafficLight.size() > 0) && !(SUMOXMLDefinitions::isValidNetID(trafficLight))) {
                // temporal
                return writeError(TLF("Could not build lane area detector with ID '%' in netedit; invalid traffic light ID.", id));
            } else if (!checkNegative(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, SUMO_ATTR_HALTING_TIME_THRESHOLD, timeThreshold, true)) {
                return false;
            } else if (!checkNegative(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold, true)) {
                return false;
            } else if (!checkNegative(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, SUMO_ATTR_JAM_DIST_THRESHOLD, jamThreshold, true)) {
                return false;
            } else if (!checkFileName(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, SUMO_ATTR_FILE, filename)) {
                return false;
            } else if (!checkListOfVehicleTypes(GNE_TAG_MULTI_LANE_AREA_DETECTOR, id, vehicleTypes)) {
                return false;
            } else {
                // build E2 multilane detector
                GNEAdditional* detectorE2 = new GNELaneAreaDetector(id, myNet, myFilename, lanes, pos, endPos, period, trafficLight, filename,
                        vehicleTypes, nextEdges, detectPersons, name, timeThreshold,
                        speedThreshold, jamThreshold, friendlyPos, show, parameters);
                // insert depending of allowUndoRedo
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(detectorE2, TL("add lane area detector '") + id + "'");
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertAdditional(detectorE2);
                    for (const auto& lane : lanes) {
                        lane->addChildElement(detectorE2);
                    }
                    detectorE2->incRef("buildDetectorE2Multilane");
                }
                return true;
            }
        }
    }
}


bool
GNEAdditionalHandler::buildDetectorE3(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const Position& pos, const SUMOTime period,
                                      const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
                                      const std::string& detectPersons, const std::string& name, const SUMOTime timeThreshold, const double speedThreshold,
                                      const bool openEntry, const bool expectedArrival, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_ENTRY_EXIT_DETECTOR}, id);
    if (!checkElement(SUMO_TAG_ENTRY_EXIT_DETECTOR, element)) {
        return false;
    } else if (!checkValidDetectorID(SUMO_TAG_ENTRY_EXIT_DETECTOR, id)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_ENTRY_EXIT_DETECTOR, id, SUMO_ATTR_PERIOD, period, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_ENTRY_EXIT_DETECTOR, id, SUMO_ATTR_HALTING_TIME_THRESHOLD, timeThreshold, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_ENTRY_EXIT_DETECTOR, id, SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold, true)) {
        return false;
    } else if (!checkFileName(SUMO_TAG_ENTRY_EXIT_DETECTOR, id, SUMO_ATTR_FILE, filename)) {
        return false;
    } else if (!checkListOfVehicleTypes(SUMO_TAG_ENTRY_EXIT_DETECTOR, id, vehicleTypes)) {
        return false;
    } else {
        // build E3
        GNEAdditional* E3 = new GNEMultiEntryExitDetector(id, myNet, myFilename, pos, period, filename, vehicleTypes, nextEdges, detectPersons,
                name, timeThreshold, speedThreshold, openEntry, expectedArrival, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(E3, TL("add entry-exit detector '") + id + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(E3, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(E3);
            E3->incRef("buildDetectorE3");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorEntry(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& laneID, const double pos,
        const bool friendlyPos, const Parameterised::Map& parameters) {
    // get lane
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
    // get E3 parent
    GNEAdditional* E3 = getAdditionalParent(sumoBaseObject, SUMO_TAG_ENTRY_EXIT_DETECTOR);
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DET_ENTRY, "", SUMO_TAG_LANE, laneID);
    } else if (E3 == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DET_ENTRY, "", SUMO_TAG_ENTRY_EXIT_DETECTOR, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (!checkLanePosition(pos, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
        return writeErrorInvalidPosition(SUMO_TAG_DET_ENTRY, E3->getID());
    } else {
        // build entry instant
        GNEAdditional* entry = new GNEEntryExitDetector(SUMO_TAG_DET_ENTRY, E3, lane, pos, friendlyPos, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(entry, TL("add entry detector in '") + E3->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(entry);
            lane->addChildElement(entry);
            E3->addChildElement(entry);
            entry->incRef("buildDetectorEntry");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorExit(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& laneID, const double pos,
                                        const bool friendlyPos, const Parameterised::Map& parameters) {
    // get lane
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
    // get E3 parent
    GNEAdditional* E3 = getAdditionalParent(sumoBaseObject, SUMO_TAG_ENTRY_EXIT_DETECTOR);
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DET_EXIT, "", SUMO_TAG_LANE, laneID);
    } else if (E3 == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DET_EXIT, "", SUMO_TAG_ENTRY_EXIT_DETECTOR, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (!checkLanePosition(pos, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
        return writeErrorInvalidPosition(SUMO_TAG_DET_EXIT, E3->getID());
    } else {
        // build exit instant
        GNEAdditional* exit = new GNEEntryExitDetector(SUMO_TAG_DET_EXIT, E3, lane, pos, friendlyPos, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(exit, TL("add exit detector in '") + E3->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(exit);
            lane->addChildElement(exit);
            E3->addChildElement(exit);
            exit->incRef("buildDetectorExit");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorE1Instant(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& laneID, double pos,
        const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const bool friendlyPos, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_INSTANT_INDUCTION_LOOP}, id);
    if (!checkElement(SUMO_TAG_INSTANT_INDUCTION_LOOP, element)) {
        return false;
    } else if (!checkValidDetectorID(SUMO_TAG_INSTANT_INDUCTION_LOOP, id)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, SUMO_TAG_LANE, laneID);
        } else if (!checkFileName(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, SUMO_ATTR_FILE, filename)) {
            return false;
        } else if (!checkLanePosition(pos, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
            return writeErrorInvalidPosition(SUMO_TAG_INSTANT_INDUCTION_LOOP, id);
        } else {
            // build E1 instant
            GNEAdditional* detectorE1Instant = new GNEInstantInductionLoopDetector(id, myNet, myFilename, lane, pos, filename, vehicleTypes, nextEdges,
                    detectPersons, name, friendlyPos, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(detectorE1Instant, TL("add instant induction loop '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE1Instant, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(detectorE1Instant);
                lane->addChildElement(detectorE1Instant);
                detectorE1Instant->incRef("buildDetectorE1Instant");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildLaneCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& laneID, const double pos,
        const std::string& name, const std::string& outfile, const SUMOTime period, const std::string& routeprobeID, const double jamThreshold, const std::vector<std::string>& vTypes,
        const Parameterised::Map& parameters) {
    // get lane
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
    // get routeProbe
    GNEAdditional* routeProbe = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, routeprobeID, false);
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::calibrators, id);
    if (!checkElement(GNE_TAG_CALIBRATOR_LANE, element)) {
        return false;
    } else if (!checkValidAdditionalID(GNE_TAG_CALIBRATOR_LANE, id)) {
        return false;
    } else if ((routeprobeID.size() > 0) && (routeProbe == nullptr)) {
        return writeErrorInvalidParent(GNE_TAG_CALIBRATOR_LANE, id, SUMO_TAG_ROUTEPROBE, routeprobeID);
    } else if (lane == nullptr) {
        return writeErrorInvalidParent(GNE_TAG_CALIBRATOR_LANE, id, SUMO_TAG_LANE, laneID);
    } else {
        // check lane
        if (!checkLanePosition(pos, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), false)) {
            return writeErrorInvalidPosition(GNE_TAG_CALIBRATOR_LANE, id);
        } else if (!checkNegative(GNE_TAG_CALIBRATOR_LANE, id, SUMO_ATTR_PERIOD, period, true)) {
            return false;
        } else if (!checkNegative(GNE_TAG_CALIBRATOR_LANE, id, SUMO_ATTR_JAM_DIST_THRESHOLD, jamThreshold, true)) {
            return false;
        } else {
            // build Calibrator
            GNEAdditional* calibrator = (routeProbe == nullptr) ?
                                        new GNECalibrator(id, myNet, myFilename, lane, pos, period, name, outfile, jamThreshold, vTypes, parameters) :
                                        new GNECalibrator(id, myNet, myFilename, lane, pos, period, name, outfile, routeProbe, jamThreshold, vTypes, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(calibrator, TL("add lane calibrator '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
                myNet->getViewNet()->getUndoList()->end();
                // check if center after creation
                if (sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) &&
                        sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)) {
                    myNet->getViewNet()->centerTo(calibrator->getPositionInView(), false);
                }
            } else {
                myNet->getAttributeCarriers()->insertAdditional(calibrator);
                lane->addChildElement(calibrator);
                if (routeProbe) {
                    routeProbe->addChildElement(calibrator);
                }
                calibrator->incRef("buildCalibrator");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildEdgeCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& edgeID, const double pos,
        const std::string& name, const std::string& outfile, const SUMOTime period, const std::string& routeprobeID, const double jamThreshold, const std::vector<std::string>& vTypes,
        const Parameterised::Map& parameters) {
    // get edge
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    // get routeProbe
    GNEAdditional* routeProbe = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, routeprobeID, false);
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::calibrators, id);
    if (!checkElement(SUMO_TAG_CALIBRATOR, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_CALIBRATOR, id)) {
        return false;
    } else if ((routeprobeID.size() > 0) && (routeProbe == nullptr)) {
        return writeErrorInvalidParent(SUMO_TAG_CALIBRATOR, id, SUMO_TAG_ROUTEPROBE, routeprobeID);
    } else if (edge == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_CALIBRATOR, id, SUMO_TAG_EDGE, edgeID);
    } else {
        if (!checkLanePosition(pos, 0, edge->getChildLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), false)) {
            return writeErrorInvalidPosition(SUMO_TAG_CALIBRATOR, id);
        } else if (!checkNegative(SUMO_TAG_CALIBRATOR, id, SUMO_ATTR_PERIOD, period, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_CALIBRATOR, id, SUMO_ATTR_JAM_DIST_THRESHOLD, jamThreshold, true)) {
            return false;
        } else {
            // build Calibrator
            GNEAdditional* calibrator = (routeProbe == nullptr) ?
                                        new GNECalibrator(id, myNet, myFilename, edge, pos, period, name, outfile, jamThreshold, vTypes, parameters) :
                                        new GNECalibrator(id, myNet, myFilename, edge, pos, period, name, outfile, routeProbe, jamThreshold, vTypes, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(calibrator, TL("add calibrator '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
                myNet->getViewNet()->getUndoList()->end();
                // check if center after creation
                if (sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) &&
                        sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)) {
                    myNet->getViewNet()->centerTo(calibrator->getPositionInView(), false);
                }
            } else {
                myNet->getAttributeCarriers()->insertAdditional(calibrator);
                edge->addChildElement(calibrator);
                if (routeProbe) {
                    routeProbe->addChildElement(calibrator);
                }
                calibrator->incRef("buildCalibrator");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildCalibratorFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameter) {
    // get vType
    GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameter.vtypeid.empty() ? DEFAULT_VTYPE_ID : vehicleParameter.vtypeid, false);
    // get route
    GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameter.routeid, false);
    // get calibrator parent
    GNEAdditional* calibrator = myNet->getAttributeCarriers()->retrieveAdditional(sumoBaseObject->getParentSumoBaseObject()->getTag(), sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    // check parents
    if (vType == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_FLOW, "", SUMO_TAG_VTYPE, vehicleParameter.vtypeid);
    } else if (route == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_FLOW, "", SUMO_TAG_ROUTE, vehicleParameter.routeid);
    } else if (calibrator == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_FLOW, "", SUMO_TAG_CALIBRATOR, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else {
        // create calibrator flow
        GNEAdditional* flow = new GNECalibratorFlow(calibrator, vType, route, vehicleParameter);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(flow, TL("add calibrator flow in '") + calibrator->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(flow, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(flow);
            calibrator->addChildElement(flow);
            route->addChildElement(flow);
            vType->addChildElement(flow);
            flow->incRef("buildCalibratorFlow");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildRerouter(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const Position& pos,
                                    const std::vector<std::string>& edgeIDs, const double prob, const std::string& name,
                                    const bool off, const bool optional, const SUMOTime timeThreshold,
                                    const std::vector<std::string>& vTypes, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_REROUTER}, id);
    if (!checkElement(SUMO_TAG_REROUTER, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_REROUTER, id)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_REROUTER, id, SUMO_ATTR_PROB, prob, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_REROUTER, id, SUMO_ATTR_HALTING_TIME_THRESHOLD, timeThreshold, true)) {
        return false;
    } else if (!checkListOfVehicleTypes(SUMO_TAG_REROUTER, id, vTypes)) {
        return false;
    } else {
        // parse edges
        std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_REROUTER, id, edgeIDs);
        // check edges
        if (edges.empty()) {
            return false;
        } else {
            GNEAdditional* rerouter = nullptr;
            // continue depending of position
            if (pos == Position::INVALID) {
                if (edges.size() > 0) {
                    PositionVector laneShape = edges.front()->getChildLanes().front()->getLaneShape();
                    // move to side
                    laneShape.move2side(3);
                    // create rerouter
                    rerouter = new GNERerouter(id, myNet, myFilename, laneShape.positionAtOffset2D(laneShape.length2D() - 6), name, prob, off, optional, timeThreshold, vTypes, parameters);
                } else {
                    rerouter = new GNERerouter(id, myNet, myFilename, Position(0, 0), name, prob, off, optional, timeThreshold, vTypes, parameters);
                }
            } else {
                rerouter = new GNERerouter(id, myNet, myFilename, pos, name, prob, off, optional, timeThreshold, vTypes, parameters);
            }
            // create rerouter Symbols
            std::vector<GNEAdditional*> rerouterSymbols;
            for (const auto& edge : edges) {
                rerouterSymbols.push_back(new GNERerouterSymbol(rerouter, edge));
            }
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(rerouter, TL("add rerouter '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
                // add symbols
                for (const auto& rerouterSymbol : rerouterSymbols) {
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouterSymbol, true), true);
                }
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(rerouter);
                rerouter->incRef("buildRerouter");
                // add symbols into rerouter
                for (const auto& rerouterSymbol : rerouterSymbols) {
                    rerouter->addChildElement(rerouterSymbol);
                }
                // add symbols into edges
                for (int i = 0; i < (int)edges.size(); i++) {
                    edges.at(i)->addChildElement(rerouterSymbols.at(i));
                }
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildRerouterInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime begin, const SUMOTime end) {
    // get rerouter parent
    GNEAdditional* rerouter = getAdditionalParent(sumoBaseObject, SUMO_TAG_REROUTER);
    // check if rerouter exist
    if (rerouter == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_INTERVAL, "", SUMO_TAG_REROUTER, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (!checkNegative(SUMO_TAG_INTERVAL, rerouter->getID(), SUMO_ATTR_BEGIN, begin, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_INTERVAL, rerouter->getID(), SUMO_ATTR_END, end, true)) {
        return false;
    } else if (end < begin) {
        return writeError(TLF("Could not build interval with ID '%' in netedit; begin is greater than end.", rerouter->getID()));
    } else {
        // check if new interval will produce a overlapping
        if (checkOverlappingRerouterIntervals(rerouter, begin, end)) {
            // create rerouter interval and add it into rerouter parent
            GNEAdditional* rerouterInterval = new GNERerouterInterval(rerouter, begin, end);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(rerouterInterval, TL("add rerouter interval in '") + rerouter->getID() + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouterInterval, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                rerouter->addChildElement(rerouterInterval);
                rerouterInterval->incRef("buildRerouterInterval");
            }
        } else {
            return writeError(TLF("Could not build interval with begin '%' and end '%' in '%' due overlapping.", toString(begin), toString(end), rerouter->getID()));
        }
        // update centering boundary of rerouter parent
        rerouter->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildClosingLaneReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& closedLaneID, SVCPermissions permissions) {
    // get rerouter interval parent
    GNEAdditional* rerouterInterval = getRerouterIntervalParent(sumoBaseObject);
    // get closed lane
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(closedLaneID, false);
    // check parents
    if (lane == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_CLOSING_LANE_REROUTE, "", SUMO_TAG_LANE, closedLaneID);
    } else if (rerouterInterval == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_CLOSING_LANE_REROUTE, "", SUMO_TAG_INTERVAL, "");
    } else {
        // create closing lane reroute
        GNEAdditional* closingLaneReroute = new GNEClosingLaneReroute(rerouterInterval, lane, permissions);
        // add it to interval parent depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(closingLaneReroute, TL("add closing lane reroute in '") + lane->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            rerouterInterval->addChildElement(closingLaneReroute);
            closingLaneReroute->incRef("buildClosingLaneReroute");
        }
        // update centering boundary of rerouter parent
        rerouterInterval->getParentAdditionals().front()->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildClosingReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& closedEdgeID, SVCPermissions permissions) {
    // get rerouter interval parent
    GNEAdditional* rerouterInterval = getRerouterIntervalParent(sumoBaseObject);
    // get closed edge
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(closedEdgeID, false);
    // check parents
    if (edge == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_CLOSING_REROUTE, "", SUMO_TAG_EDGE, closedEdgeID);
    } else if (rerouterInterval == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_CLOSING_REROUTE, "", SUMO_TAG_INTERVAL, "");
    } else {
        // create closing reroute
        GNEAdditional* closingLaneReroute = new GNEClosingReroute(rerouterInterval, edge, permissions);
        // add it to interval parent depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(closingLaneReroute, TL("add closing reroute in '") + edge->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            rerouterInterval->addChildElement(closingLaneReroute);
            closingLaneReroute->incRef("buildClosingLaneReroute");
        }
        // update centering boundary of rerouter parent
        rerouterInterval->getParentAdditionals().front()->updateCenteringBoundary(true);
        return true;
    }
}

bool
GNEAdditionalHandler::buildDestProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& newEdgeDestinationID, const double probability) {
    // get rerouter interval parent
    GNEAdditional* rerouterInterval = getRerouterIntervalParent(sumoBaseObject);
    // get edge
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(newEdgeDestinationID, false);
    // check parents
    if (edge == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DEST_PROB_REROUTE, "", SUMO_TAG_EDGE, newEdgeDestinationID);
    } else if (rerouterInterval == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_DEST_PROB_REROUTE, "", SUMO_TAG_INTERVAL, "");
    } else {
        // create dest probability reroute
        GNEAdditional* destProbReroute = new GNEDestProbReroute(rerouterInterval, edge, probability);
        // add it to interval parent depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(destProbReroute, TL("add dest prob reroute in '") + edge->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(destProbReroute, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            rerouterInterval->addChildElement(destProbReroute);
            destProbReroute->incRef("builDestProbReroute");
        }
        // update centering boundary of rerouter parent
        rerouterInterval->getParentAdditionals().front()->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildParkingAreaReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& newParkignAreaID, const double probability, const bool visible) {
    // get rerouter interval parent
    GNEAdditional* rerouterInterval = getRerouterIntervalParent(sumoBaseObject);
    // get parking area
    GNEAdditional* parkingArea = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, newParkignAreaID, false);
    // check parents
    if (parkingArea == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_PARKING_AREA_REROUTE, "", SUMO_TAG_PARKING_AREA, newParkignAreaID);
    } else if (rerouterInterval == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_PARKING_AREA_REROUTE, "", SUMO_TAG_INTERVAL, "");
    } else {
        // create parking area reroute
        GNEAdditional* parkingAreaReroute = new GNEParkingAreaReroute(rerouterInterval, parkingArea, probability, visible);
        // add it to interval parent depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(parkingAreaReroute, TL("add parking area reroute in '") + parkingArea->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingAreaReroute, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            rerouterInterval->addChildElement(parkingAreaReroute);
            parkingAreaReroute->incRef("builParkingAreaReroute");
        }
        // update centering boundary of rerouter parent
        rerouterInterval->getParentAdditionals().front()->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildRouteProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& newRouteID, const double probability) {
    // get rerouter interval parent
    GNEAdditional* rerouterInterval = getRerouterIntervalParent(sumoBaseObject);
    // get route parent
    GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, newRouteID, false);
    // check parents
    if (route == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_ROUTE_PROB_REROUTE, "", SUMO_TAG_ROUTE, newRouteID);
    } else if (rerouterInterval == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_ROUTE_PROB_REROUTE, "", SUMO_TAG_INTERVAL, "");
    } else {
        // create rout prob reroute
        GNEAdditional* routeProbReroute = new GNERouteProbReroute(rerouterInterval, route, probability);
        // add it to interval parent depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(routeProbReroute, TL("add route prob reroute in '") + route->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(routeProbReroute, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            rerouterInterval->addChildElement(routeProbReroute);
            routeProbReroute->incRef("buildRouteProbReroute");
        }
        // update centering boundary of rerouter parent
        rerouterInterval->getParentAdditionals().front()->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildRouteProbe(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& edgeID, const SUMOTime period,
                                      const std::string& name, const std::string& file, const SUMOTime begin, const std::vector<std::string>& vTypes,
                                      const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_ROUTEPROBE}, id);
    if (!checkElement(SUMO_TAG_ROUTEPROBE, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_ROUTEPROBE, id)) {
        return false;
    } else {
        // get edge
        GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
        // check lane
        if (edge == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_ROUTEPROBE, id, SUMO_TAG_EDGE, edgeID);
        } else if (!checkNegative(SUMO_TAG_ROUTEPROBE, id, SUMO_ATTR_PERIOD, period, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_ROUTEPROBE, id, SUMO_ATTR_BEGIN, begin, true)) {
            return false;
        } else if (!checkFileName(SUMO_TAG_ROUTEPROBE, id, SUMO_ATTR_FILE, file)) {
            return false;
        } else {
            // build route probe
            GNEAdditional* routeProbe = new GNERouteProbe(id, myNet, myFilename, edge, period, name, file, begin, vTypes, parameters);
            // insert depending of allowUndoRedo
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(routeProbe, TL("add route probe '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
                myNet->getViewNet()->getUndoList()->end();
                // check if center after creation
                if (sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) &&
                        sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)) {
                    myNet->getViewNet()->centerTo(routeProbe->getPositionInView(), false);
                }
            } else {
                myNet->getAttributeCarriers()->insertAdditional(routeProbe);
                edge->addChildElement(routeProbe);
                routeProbe->incRef("buildRouteProbe");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSign(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const Position& pos,
        const std::vector<std::string>& laneIDs, const std::string& name, const std::vector<std::string>& vTypes, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_VSS}, id);
    if (!checkElement(SUMO_TAG_VSS, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_VSS, id)) {
        return false;
    } else {
        // parse lanes
        std::vector<GNELane*> lanes = parseLanes(SUMO_TAG_VSS, id, laneIDs);
        // check lane
        if (lanes.empty()) {
            return false;
        } else {
            // check vTypes
            if (!checkListOfVehicleTypes(SUMO_TAG_VSS, id, vTypes)) {
                return false;
            } else {
                // create VSS
                GNEAdditional* variableSpeedSign = new GNEVariableSpeedSign(id, myNet, myFilename, pos, name, vTypes, parameters);
                // create VSS Symbols
                std::vector<GNEAdditional*> VSSSymbols;
                for (const auto& lane : lanes) {
                    VSSSymbols.push_back(new GNEVariableSpeedSignSymbol(variableSpeedSign, lane));
                }
                // insert depending of allowUndoRedo
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(variableSpeedSign, TL("add Variable Speed Sign '") + id + "'");
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
                    for (const auto& VSSSymbol : VSSSymbols) {
                        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(VSSSymbol, true), true);
                    }
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertAdditional(variableSpeedSign);
                    variableSpeedSign->incRef("buildVariableSpeedSign");
                    // add symbols into VSS
                    for (const auto& VSSSymbol : VSSSymbols) {
                        variableSpeedSign->addChildElement(VSSSymbol);
                    }
                    // add symbols into lanes
                    for (int i = 0; i < (int)lanes.size(); i++) {
                        lanes.at(i)->addChildElement(VSSSymbols.at(i));
                    }
                }
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSignStep(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime time, const double speed) {
    // get VSS parent
    GNEAdditional* VSS = getAdditionalParent(sumoBaseObject, SUMO_TAG_VSS);
    // check lane
    if (VSS == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_STEP, "", SUMO_TAG_VSS, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (!checkNegative(SUMO_TAG_STEP, VSS->getID(), SUMO_ATTR_TIME, time, true)) {
        return false;
    } else {
        // create Variable Speed Sign
        GNEAdditional* variableSpeedSignStep = new GNEVariableSpeedSignStep(VSS, time, speed);
        // add it depending of allow undoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(variableSpeedSignStep, TL("add VSS Step in '") + VSS->getID() + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(variableSpeedSignStep, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            VSS->addChildElement(variableSpeedSignStep);
            variableSpeedSignStep->incRef("buildVariableSpeedSignStep");
        }
        // update centering boundary of VSS parent
        VSS->updateCenteringBoundary(true);
        return true;
    }
}


bool
GNEAdditionalHandler::buildVaporizer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID, const SUMOTime beginTime,
                                     const SUMOTime endTime, const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_VAPORIZER}, edgeID);
    if (!checkElement(SUMO_TAG_VAPORIZER, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_VAPORIZER, edgeID)) {
        return false;
    } else {
        // get edge
        GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
        // check lane
        if (edge == nullptr) {
            return writeErrorInvalidParent(SUMO_TAG_VAPORIZER, "", SUMO_TAG_EDGE, edgeID);
        } else if (!checkNegative(SUMO_TAG_VAPORIZER, edge->getID(), SUMO_ATTR_BEGIN, beginTime, true)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_VAPORIZER, edge->getID(), SUMO_ATTR_END, endTime, true)) {
            return false;
        } else if (endTime < beginTime) {
            return writeError(TLF("Could not build Vaporizer with ID '%' in netedit; begin is greater than end.", edge->getID()));
        } else {
            // build vaporizer
            GNEAdditional* vaporizer = new GNEVaporizer(myNet, myFilename, edge, beginTime, endTime, name, parameters);
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(vaporizer, TL("add vaporizer in '") + edge->getID() + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
                myNet->getViewNet()->getUndoList()->end();
                // check if center after creation
                if (sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) &&
                        sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)) {
                    myNet->getViewNet()->centerTo(vaporizer->getPositionInView(), false);
                }
            } else {
                myNet->getAttributeCarriers()->insertAdditional(vaporizer);
                edge->addChildElement(vaporizer);
                vaporizer->incRef("buildVaporizer");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildTAZ(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const PositionVector& shape,
                               const Position& center, const bool fill, const RGBColor& color, const std::vector<std::string>& edgeIDs,
                               const std::string& name, const Parameterised::Map& parameters) {
    // parse edges
    const std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_TAZ, id, edgeIDs);
    if (edges.size() != edgeIDs.size()) {
        return false;
    } else {
        // check TAZShape
        PositionVector TAZShape = shape;
        if (TAZShape.size() == 0) {
            // declare boundary
            Boundary TAZBoundary;
            for (const auto& edge : edges) {
                TAZBoundary.add(edge->getCenteringBoundary());
            }
            // iterate over children and add sourceSinkEdge boundaries to make a taz shape
            for (const auto& sourceSink : sumoBaseObject->getSumoBaseObjectChildren()) {
                // check that child is a source or sink elements (to avoid other elements)
                if ((sourceSink->getTag() == SUMO_TAG_TAZSOURCE) || (sourceSink->getTag() == SUMO_TAG_TAZSINK)) {
                    const GNEEdge* sourceSinkEdge = myNet->getAttributeCarriers()->retrieveEdge(sourceSink->getStringAttribute(SUMO_ATTR_ID), false);
                    if (sourceSinkEdge) {
                        TAZBoundary.add(sourceSinkEdge->getCenteringBoundary());
                    }
                }
            }
            // update TAZShape
            TAZShape = TAZBoundary.getShape(true);
        }
        // check TAZ
        const auto element = retrieveAdditionalElement({SUMO_TAG_TAZ}, id);
        if (!checkElement(SUMO_TAG_TAZ, element)) {
            return false;
        } else if (!checkValidAdditionalID(SUMO_TAG_TAZ, id)) {
            return false;
        } else if (TAZShape.size() == 0) {
            return writeError(TLF("Could not build TAZ with ID '%' in netedit; Invalid Shape.", id));
        } else {
            // build TAZ with the given shape
            const Position center2 = center == Position::INVALID ? TAZShape.getCentroid() : center;
            GNEAdditional* TAZ = new GNETAZ(id, myNet, myFilename, TAZShape, center2, fill, color, name, parameters);
            // disable updating geometry of TAZ children during insertion (because in large nets provokes slowdowns)
            myNet->disableUpdateGeometry();
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(TAZ, TL("add TAZ '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(TAZ, true), true);
                // create TAZEdges
                for (const auto& edge : edges) {
                    // create TAZ Source using GNEChange_Additional
                    GNETAZSourceSink* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZSourceSink(TAZSource, true), true);
                    // create TAZ Sink using GNEChange_Additional
                    GNETAZSourceSink* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZSourceSink(TAZSink, true), true);
                }
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertAdditional(TAZ);
                TAZ->incRef("buildTAZ");
                for (const auto& edge : edges) {
                    // create TAZ Source
                    GNETAZSourceSink* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
                    myNet->getAttributeCarriers()->insertTAZSourceSink(TAZSource);
                    TAZSource->incRef("buildTAZ");
                    TAZ->addChildElement(TAZSource);
                    edge->addChildElement(TAZSource);
                    // create TAZ Sink
                    GNETAZSourceSink* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
                    myNet->getAttributeCarriers()->insertTAZSourceSink(TAZSink);
                    TAZSink->incRef("buildTAZ");
                    TAZ->addChildElement(TAZSink);
                    edge->addChildElement(TAZSink);
                }
            }
            // enable updating geometry again and update geometry of TAZ
            myNet->enableUpdateGeometry();
            // update TAZ parent
            TAZ->updateGeometry();
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildTAZSource(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID, const double departWeight) {
    // get TAZ parent
    GNEAdditional* TAZ = getAdditionalParent(sumoBaseObject, SUMO_TAG_TAZ);
    // get edge
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    // check parents
    if (TAZ == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_SOURCE, edgeID, SUMO_TAG_TAZ, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (edge == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_SOURCE, edgeID, SUMO_TAG_EDGE, TAZ->getID());
    } else {
        // declare TAZ Source
        GNETAZSourceSink* existentTAZSource = nullptr;
        // first check if already exist a TAZ Source for the given edge and TAZ
        for (auto it = edge->getChildTAZSourceSinks().begin(); (it != edge->getChildTAZSourceSinks().end()) && !existentTAZSource; it++) {
            if (((*it)->getTagProperty()->getTag() == SUMO_TAG_TAZSOURCE) && ((*it)->getParentAdditionals().front() == TAZ)) {
                existentTAZSource = (*it);
            }
        }
        // check if TAZSource has to be created
        if (existentTAZSource == nullptr) {
            // Create TAZ only with departWeight
            GNETAZSourceSink* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, departWeight);
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(TAZ, TL("add TAZ Source in '") + TAZ->getID() + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZSourceSink(TAZSource, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertTAZSourceSink(TAZSource);
                TAZ->addChildElement(TAZSource);
                edge->addChildElement(TAZSource);
                TAZSource->incRef("buildTAZSource");
            }
        } else {
            // update TAZ Attribute depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(TAZ, TL("update TAZ Source in '") + TAZ->getID() + "'");
                existentTAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), myNet->getViewNet()->getUndoList());
                myNet->getViewNet()->getUndoList()->end();
            } else {
                existentTAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), nullptr);
            }
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildTAZSink(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID, const double arrivalWeight) {
    // get TAZ parent
    GNEAdditional* TAZ = getAdditionalParent(sumoBaseObject, SUMO_TAG_TAZ);
    // get edge
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    // check parents
    if (TAZ == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_SOURCE, edgeID, SUMO_TAG_TAZ, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    } else if (edge == nullptr) {
        return writeErrorInvalidParent(SUMO_TAG_SOURCE, edgeID, SUMO_TAG_EDGE, TAZ->getID());
    } else {
        // declare TAZ Sink
        GNETAZSourceSink* existentTAZSink = nullptr;
        // first check if already exist a TAZ Sink for the given edge and TAZ
        for (auto it = edge->getChildTAZSourceSinks().begin(); (it != edge->getChildTAZSourceSinks().end()) && !existentTAZSink; it++) {
            if (((*it)->getTagProperty()->getTag() == SUMO_TAG_TAZSINK) && ((*it)->getParentAdditionals().front() == TAZ)) {
                existentTAZSink = (*it);
            }
        }
        // check if TAZSink has to be created
        if (existentTAZSink == nullptr) {
            // Create TAZ only with departWeight
            GNETAZSourceSink* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, arrivalWeight);
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(TAZ, TL("add TAZ Sink in '") + TAZ->getID() + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZSourceSink(TAZSink, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertTAZSourceSink(TAZSink);
                TAZ->addChildElement(TAZSink);
                edge->addChildElement(TAZSink);
                TAZSink->incRef("buildTAZSink");
            }
        } else {
            // update TAZ Attribute depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(TAZ, TL("update TAZ Sink in '") + TAZ->getID() + "'");
                existentTAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), myNet->getViewNet()->getUndoList());
                myNet->getViewNet()->getUndoList()->end();
            } else {
                existentTAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), nullptr);
            }
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildTractionSubstation(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const Position& pos,
        const double voltage, const double currentLimit, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_TRACTION_SUBSTATION}, id);
    if (!checkElement(SUMO_TAG_TRACTION_SUBSTATION, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_TRACTION_SUBSTATION, id)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_TRACTION_SUBSTATION, id, SUMO_ATTR_VOLTAGE, voltage, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_TRACTION_SUBSTATION, id, SUMO_ATTR_CURRENTLIMIT, currentLimit, true)) {
        return false;
    } else {
        // build traction substation
        GNEAdditional* tractionSubstation = new GNETractionSubstation(id, myNet, myFilename, pos, voltage, currentLimit, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(tractionSubstation, TL("add traction substation '") + id + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(tractionSubstation, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(tractionSubstation);
            tractionSubstation->incRef("buildTractionSubstation");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildOverheadWire(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& substationId,
                                        const std::vector<std::string>& laneIDs, const double startPos, const double endPos, const bool friendlyPos,
                                        const std::vector<std::string>& forbiddenInnerLanes, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement({SUMO_TAG_OVERHEAD_WIRE_SECTION}, id);
    if (!checkElement(SUMO_TAG_OVERHEAD_WIRE_SECTION, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_OVERHEAD_WIRE_SECTION, id)) {
        return false;
    } else {
        // get lanes
        const auto lanes = parseLanes(SUMO_TAG_OVERHEAD_WIRE_SECTION, id, laneIDs);
        // get traction substation
        const auto tractionSubstation = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRACTION_SUBSTATION, substationId, false);
        // check lanes
        if (lanes.empty()) {
            return false;
        } else {
            // calculate path
            if (!GNEAdditional::areLaneConsecutives(lanes)) {
                return writeError(TLF("Could not build overhead wire with ID '%' in netedit; Lanes aren't consecutives.", id));
            } else if (!checkMultiLanePosition(
                           startPos, lanes.front()->getParentEdge()->getNBEdge()->getFinalLength(),
                           endPos, lanes.back()->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
                return writeErrorInvalidPosition(SUMO_TAG_OVERHEAD_WIRE_SECTION, id);
            } else if (tractionSubstation == nullptr) {
                return writeErrorInvalidParent(SUMO_TAG_OVERHEAD_WIRE_SECTION, "", SUMO_TAG_TRACTION_SUBSTATION, substationId);
            } else {
                // build Overhead Wire
                GNEAdditional* overheadWire = new GNEOverheadWire(id, myNet, myFilename, lanes, tractionSubstation, startPos, endPos, friendlyPos, forbiddenInnerLanes, parameters);
                // insert depending of allowUndoRedo
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(overheadWire, TL("add overhead wire '") + id + "'");
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(overheadWire, true), true);
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertAdditional(overheadWire);
                    for (const auto& lane : lanes) {
                        lane->addChildElement(overheadWire);
                    }
                    overheadWire->incRef("buildOverheadWire");
                }
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildOverheadWireClamp(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& /* id */, const std::string& /* overheadWireIDStartClamp */,
        const std::string& /* laneIDStartClamp */, const std::string& /* overheadWireIDEndClamp */, const std::string& /* laneIDEndClamp */,
        const Parameterised::Map& /* parameters */) {
    //
    return false;
}


bool
GNEAdditionalHandler::buildPolygon(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type,
                                   const RGBColor& color, double layer, double angle, const std::string& imgFile, const PositionVector& shape,
                                   bool geo, bool fill, double lineWidth, const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    if (type == "jupedsim.walkable_area") {
        return buildJpsWalkableArea(sumoBaseObject, id, shape, geo, name, parameters);
    } else if (type == "jupedsim.obstacle") {
        return buildJpsObstacle(sumoBaseObject, id, shape, geo, name, parameters);
    } else {
        // check conditions
        const auto element = retrieveAdditionalElement(NamespaceIDs::polygons, id);
        if (!checkElement(SUMO_TAG_POLY, element)) {
            return false;
        } else if (!checkValidAdditionalID(SUMO_TAG_POLY, id)) {
            return false;
        } else if (!checkNegative(SUMO_TAG_POLY, id, SUMO_ATTR_LINEWIDTH, lineWidth, true)) {
            return false;
        } else {
            // create poly
            GNEPoly* poly = new GNEPoly(id, myNet, myFilename, type, shape, geo, fill, lineWidth, color, layer, angle, imgFile, name, parameters);
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(poly, TL("add polygon '") + id + "'");
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(poly, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                // insert shape without allowing undo/redo
                myNet->getAttributeCarriers()->insertAdditional(poly);
                poly->incRef("addPolygon");
            }
            return true;
        }
    }
}


bool
GNEAdditionalHandler::buildPOI(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& type,
                               const RGBColor& color, const double x, const double y, const std::string& icon, double layer, double angle,
                               const std::string& imgFile, double width, double height, const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::POIs, id);
    if (!checkElement(SUMO_TAG_POI, element)) {
        return false;
    } else if (!checkValidAdditionalID(SUMO_TAG_POI, id)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_POI, id, SUMO_ATTR_WIDTH, width, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_POI, id, SUMO_ATTR_HEIGHT, height, true)) {
        return false;
    } else if (!checkFileName(SUMO_TAG_POI, id, SUMO_ATTR_IMGFILE, imgFile)) {
        return false;
    } else {
        // parse position
        const auto pos = Position(x, y);
        // parse icon
        const auto POIIcon = SUMOXMLDefinitions::POIIcons.hasString(icon) ? SUMOXMLDefinitions::POIIcons.get(icon) : POIIcon::NONE;
        // create POI
        GNEPOI* POI = new GNEPOI(id, myNet, myFilename, type, color, pos, false, POIIcon, layer, angle, imgFile, width, height, name, parameters);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(POI, TLF("add POI '%'", id));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(POI, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertAdditional(POI);
            POI->incRef("addPOI");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildPOILane(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& type,
                                   const RGBColor& color, const std::string& laneID, double posOverLane, const bool friendlyPos, double posLat,
                                   const std::string& icon, double layer, double angle, const std::string& imgFile, double width, double height,
                                   const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::POIs, id);
    if (!checkElement(GNE_TAG_POILANE, element)) {
        return false;
    } else if (!checkValidAdditionalID(GNE_TAG_POILANE, id)) {
        return false;
    } else if (!checkNegative(GNE_TAG_POILANE, id, SUMO_ATTR_WIDTH, width, true)) {
        return false;
    } else if (!checkNegative(GNE_TAG_POILANE, id, SUMO_ATTR_HEIGHT, height, true)) {
        return false;
    } else if (!checkFileName(GNE_TAG_POILANE, id, SUMO_ATTR_IMGFILE, imgFile)) {
        return false;
    } else {
        // get lane
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // check lane
        if (lane == nullptr) {
            return writeErrorInvalidParent(GNE_TAG_POILANE, id, SUMO_TAG_LANE, laneID);
        } else if (!checkLanePosition(posOverLane, 0, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
            return writeErrorInvalidPosition(GNE_TAG_POILANE, id);
        } else {
            // parse icon
            const auto POIIcon = SUMOXMLDefinitions::POIIcons.hasString(icon) ? SUMOXMLDefinitions::POIIcons.get(icon) : POIIcon::NONE;
            // create POI (use GNEAdditional instead GNEPOI for add child references)
            GNEAdditional* POILane = new GNEPOI(id, myNet, myFilename, type, color, lane, posOverLane, friendlyPos, posLat, POIIcon, layer,
                                                angle, imgFile, width, height, name, parameters);
            // add it depending of allow undoRed
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(POILane, TLF("add POI lane '%'", id));
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(POILane, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                // insert shape without allowing undo/redo
                myNet->getAttributeCarriers()->insertAdditional(POILane);
                lane->addChildElement(POILane);
                POILane->incRef("buildPOILane");
            }
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildPOIGeo(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const std::string& type,
                                  const RGBColor& color, const double lon, const double lat, const std::string& icon, double layer,
                                  double angle, const std::string& imgFile, double width, double height, const std::string& name,
                                  const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::POIs, id);
    if (!checkElement(GNE_TAG_POIGEO, element)) {
        return false;
    } else if (!checkValidAdditionalID(GNE_TAG_POIGEO, id)) {
        return false;
    } else if (!checkNegative(GNE_TAG_POIGEO, id, SUMO_ATTR_WIDTH, width, true)) {
        return false;
    } else if (!checkNegative(GNE_TAG_POIGEO, id, SUMO_ATTR_HEIGHT, height, true)) {
        return false;
    } else if (!checkFileName(GNE_TAG_POIGEO, id, SUMO_ATTR_IMGFILE, imgFile)) {
        return false;
    } else if (GeoConvHelper::getFinal().getProjString() == "!") {
        return writeError(TLF("Could not build POI with ID '%' in netedit", id) + std::string("; ") + TL("Network requires a geo projection."));
    } else {
        // parse position
        const auto pos = Position(lon, lat);
        // parse icon
        const auto POIIcon = SUMOXMLDefinitions::POIIcons.hasString(icon) ? SUMOXMLDefinitions::POIIcons.get(icon) : POIIcon::NONE;
        // create POIGEO
        GNEPOI* POIGEO = new GNEPOI(id, myNet, myFilename, type, color, pos, true, POIIcon, layer, angle, imgFile, width, height, name, parameters);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(POIGEO, TLF("add POI GEO '%'", id));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(POIGEO, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertAdditional(POIGEO);
            POIGEO->incRef("buildPOIGeo");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildJpsWalkableArea(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const PositionVector& shape,
        bool geo, const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::polygons, id);
    if (!checkElement(GNE_TAG_JPS_WALKABLEAREA, element)) {
        return false;
    } else if (!checkValidAdditionalID(GNE_TAG_JPS_WALKABLEAREA, id)) {
        return false;
    } else {
        // create walkable area
        GNEPoly* walkableArea = new GNEPoly(GNE_TAG_JPS_WALKABLEAREA, id, myNet, myFilename, shape, geo, name, parameters);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(walkableArea, TL("add jps walkable area '") + id + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(walkableArea, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertAdditional(walkableArea);
            walkableArea->incRef("addWalkableArea");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildJpsObstacle(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, const PositionVector& shape,
                                       bool geo, const std::string& name, const Parameterised::Map& parameters) {
    // check conditions
    const auto element = retrieveAdditionalElement(NamespaceIDs::polygons, id);
    if (!checkElement(GNE_TAG_JPS_OBSTACLE, element)) {
        return false;
    } else if (!checkValidAdditionalID(GNE_TAG_JPS_OBSTACLE, id)) {
        return false;
    } else {
        // create walkable area
        GNEPoly* obstacle = new GNEPoly(GNE_TAG_JPS_OBSTACLE, id, myNet, myFilename, shape, geo, name, parameters);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(obstacle, TL("add jps obstacle '") + id + "'");
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(obstacle, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertAdditional(obstacle);
            obstacle->incRef("addObstacle");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::accessExists(const GNEAdditional* stoppingPlaceParent, const GNEEdge* edge) {
    // check if exist another access for the same parent in the given edge
    for (const auto& access : stoppingPlaceParent->getChildAdditionals()) {
        // check tag
        if (access->getTagProperty()->getTag() == SUMO_TAG_ACCESS) {
            // check all siblings of the lane
            for (const auto& lane : edge->getChildLanes()) {
                if (access->getAttribute(SUMO_ATTR_LANE) == lane->getID()) {
                    return false;
                }
            }
        }
    }
    return true;
}


bool
GNEAdditionalHandler::checkOverlappingRerouterIntervals(GNEAdditional* rerouter, SUMOTime newBegin, SUMOTime newEnd) {
    // declare a vector to keep sorted rerouter children
    std::vector<std::pair<SUMOTime, SUMOTime>> sortedIntervals;
    // iterate over child additional
    for (const auto& rerouterChild : rerouter->getChildAdditionals()) {
        if (!rerouterChild->getTagProperty()->isSymbol()) {
            sortedIntervals.push_back(std::make_pair((SUMOTime)0., (SUMOTime)0.));
            // set begin and end
            sortedIntervals.back().first = TIME2STEPS(rerouterChild->getAttributeDouble(SUMO_ATTR_BEGIN));
            sortedIntervals.back().second = TIME2STEPS(rerouterChild->getAttributeDouble(SUMO_ATTR_END));
        }
    }
    // add new intervals
    sortedIntervals.push_back(std::make_pair(newBegin, newEnd));
    // sort children
    std::sort(sortedIntervals.begin(), sortedIntervals.end());
    // check overlapping after sorting
    for (int i = 0; i < (int)sortedIntervals.size() - 1; i++) {
        if (sortedIntervals.at(i).second > sortedIntervals.at(i + 1).first) {
            return false;
        }
    }
    return true;
}


bool
GNEAdditionalHandler::checkLanePosition(double pos, const double length, const double laneLength, const bool friendlyPos) {
    if (friendlyPos) {
        return true;
    }
    // adjust from and to (negative means that start at the end of lane and count backward)
    if (pos < 0) {
        pos += laneLength;
    }
    // check extremes
    if ((pos < 0) || (pos > laneLength)) {
        return false;
    }
    // check pos + length
    if ((pos + length) > laneLength) {
        return false;
    }
    // all OK
    return true;
}


bool
GNEAdditionalHandler::checkFriendlyPosSmallLanes(double pos, const double length, const double laneLength, const bool friendlyPos) {
    if (friendlyPos == true) {
        return true;
    } else if (OptionsCont::getOptions().getBool("e2.friendlyPos.automatic")) {
        // adjust from and to (negative means that start at the end of lane and count backward)
        if (pos < 0) {
            pos += laneLength;
        }
        // check extremes
        if ((pos < 0) || (pos > laneLength)) {
            return true;
        }
        // check pos + length
        if ((pos + length) > laneLength) {
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::checkLaneDoublePosition(double from, double to, const double laneLength, const bool friendlyPos) {
    if (friendlyPos) {
        return true;
    }
    // adjust from and to (negative means that start at the end of lane and count backward)
    if (from == INVALID_DOUBLE) {
        from = 0;
    }
    if (to == INVALID_DOUBLE) {
        to = laneLength;
    }
    if (from < 0) {
        from += laneLength;
    }
    if (to < 0) {
        to += laneLength;
    }
    if ((to - from) < POSITION_EPS) {
        return false;
    }
    if ((from < 0) || (from > laneLength)) {
        return false;
    }
    if ((to < 0) || (to > laneLength)) {
        return false;
    }
    return true;
}


void
GNEAdditionalHandler::fixLaneDoublePosition(double& from, double& to, const double laneLength) {
    // adjust from (negative means that start at the end of lane and count backward)
    if (from == INVALID_DOUBLE) {
        from = 0;
    }
    if (to == INVALID_DOUBLE) {
        to = laneLength;
    }
    if (from < 0) {
        from += laneLength;
    }
    if (from < 0) {
        from = 0;
    } else if (from > laneLength) {
        from = laneLength;
    }
    // adjust to
    if (to < 0) {
        to += laneLength;
    }
    if (to < 0) {
        to = 0;
    } else if (to > laneLength) {
        to = laneLength;
    }
    // to has more priorty as from, and distance between from and to must be >= POSITION_EPS
    if ((to - from) < POSITION_EPS) {
        if (to >= POSITION_EPS) {
            from = to - POSITION_EPS;
        } else {
            from = 0;
            to = POSITION_EPS;
        }
    }
}


bool
GNEAdditionalHandler::checkMultiLanePosition(double fromPos, const double fromLaneLength, const double toPos, const double tolaneLength, const bool friendlyPos) {
    if (friendlyPos) {
        return true;
    } else {
        return (checkLanePosition(fromPos, 0, fromLaneLength, false) && checkLanePosition(toPos, 0, tolaneLength, false));
    }
}


GNEAdditional*
GNEAdditionalHandler::getAdditionalParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SumoXMLTag tag) const {
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        return nullptr;
    } else if (!sumoBaseObject->getParentSumoBaseObject()->hasStringAttribute(SUMO_ATTR_ID)) {
        return nullptr;
    } else {
        return myNet->getAttributeCarriers()->retrieveAdditional(tag, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    }
}


GNEAdditional*
GNEAdditionalHandler::getRerouterIntervalParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        // parent interval doesn't exist
        return nullptr;
    } else if (sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject() == nullptr) {
        // rerouter parent doesn't exist
        return nullptr;
    } else if (!sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->hasStringAttribute(SUMO_ATTR_ID) ||   // rerouter ID
               !sumoBaseObject->getParentSumoBaseObject()->hasTimeAttribute(SUMO_ATTR_BEGIN) || // interval begin
               !sumoBaseObject->getParentSumoBaseObject()->hasTimeAttribute(SUMO_ATTR_END)) {   // interval end
        return nullptr;
    } else {
        return myNet->getAttributeCarriers()->retrieveRerouterInterval(
                   sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID),
                   sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN),
                   sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    }
}


std::vector<GNEEdge*>
GNEAdditionalHandler::parseEdges(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& edgeIDs) {
    std::vector<GNEEdge*> edges;
    for (const auto& edgeID : edgeIDs) {
        GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
        // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
        if (edge == nullptr) {
            writeError(TLF("Could not build % with ID '%' in netedit; % with ID '%' doesn't exist.", toString(tag), id, toString(SUMO_TAG_EDGE), edgeID));
            edges.clear();
            return edges;
        } else {
            edges.push_back(edge);
        }
    }
    return edges;
}


std::vector<GNELane*>
GNEAdditionalHandler::parseLanes(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& laneIDs) {
    std::vector<GNELane*> lanes;
    for (const auto& laneID : laneIDs) {
        GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
        // empty lanes aren't allowed. If lane is empty, write error, clear lanes and stop
        if (lane == nullptr) {
            writeError(TLF("Could not build % with ID '%' in netedit; % with ID '%' doesn't exist.", toString(tag), id, toString(SUMO_TAG_LANE), laneID));
            lanes.clear();
            return lanes;
        } else {
            lanes.push_back(lane);
        }
    }
    return lanes;
}


GNEAdditional*
GNEAdditionalHandler::retrieveAdditionalElement(const std::vector<SumoXMLTag> tags, const std::string& id) {
    for (const auto& tag : tags) {
        // retrieve additional element
        auto additionalElement = myNet->getAttributeCarriers()->retrieveAdditional(tag, id, false);
        if (additionalElement) {
            return additionalElement;
        }
    }
    return nullptr;
}


bool
GNEAdditionalHandler::checkElement(const SumoXMLTag tag, GNEAdditional* additionalElement) {
    if (additionalElement) {
        if (myOverwriteElements) {
            // delete element
            myNet->deleteAdditional(additionalElement, myNet->getViewNet()->getUndoList());
        } else if (myRemainElements) {
            // duplicated demand
            return writeWarningDuplicated(tag, additionalElement->getID(), additionalElement->getTagProperty()->getTag());
        } else {
            // open overwrite dialog
            GNEOverwriteElement overwriteElementDialog(this, additionalElement);
            // continue depending of result
            if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::ACCEPT) {
                // delete element
                myNet->deleteAdditional(additionalElement, myNet->getViewNet()->getUndoList());
            } else if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::CANCEL) {
                // duplicated demand
                return writeWarningDuplicated(tag, additionalElement->getID(), additionalElement->getTagProperty()->getTag());
            } else {
                return false;
            }
        }
    }
    return true;
}

/****************************************************************************/
