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
/// @file    GNEAdditionalHandlerBeta.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// Builds trigger objects for netedit
/****************************************************************************/
#include <config.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_TAZElement.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEAdditionalHandlerBeta.h"
#include "GNEBusStop.h"
#include "GNEAccess.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntryExit.h"
#include "GNEDetectorE1Instant.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNERerouter.h"
#include "GNERerouterSymbol.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNEParkingAreaReroute.h"
#include "GNERouteProbe.h"
#include "GNEVaporizer.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignSymbol.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNETAZ.h"
#include "GNETAZSourceSink.h"


// ===========================================================================
// GNEAdditionalHandlerBeta method definitions
// ===========================================================================

GNEAdditionalHandlerBeta::GNEAdditionalHandlerBeta(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    AdditionalHandler(file),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEAdditionalHandlerBeta::~GNEAdditionalHandlerBeta() {
}


void 
GNEAdditionalHandlerBeta::buildBusStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, 
    const std::vector<std::string>& lines, const int personCapacity, const double parkingLength, const bool friendlyPosition, 
    const std::map<std::string, std::string> &parameters) {
    // first check if busStop exist
    if (myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build busStop
        GNEAdditional* busStop = new GNEBusStop(id, lane, myNet, /*startPos*/ 0, /*endPos*/ 1, /*parametersSet*/ 0, name, lines, 
                                                personCapacity, parkingLength, friendlyPosition, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(busStop);
            lane->addChildElement(busStop);
            busStop->incRef("buildBusStop");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildAccess(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, 
    const double pos, const std::string& length, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get lane
    GNEAdditional *busStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // Check if busStop parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (busStop == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent doesn't exist.");
    } else if (!accessCanBeCreated(busStop, lane->getParentEdge())) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent already owns a Acces in the edge '" + lane->getParentEdge()->getID() + "'");
    } else if (!lane->allowPedestrians()) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; The lane '" + lane->getID() + "' doesn't support pedestrians");
    } else {
        // build access
        GNEAdditional* access = new GNEAccess(busStop, lane, myNet, pos, length, friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_ACCESS));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(access, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(access);
            lane->addChildElement(access);
            busStop->addChildElement(access);
            access->incRef("buildAccess");
        }
    }
}


void 
GNEAdditionalHandlerBeta::buildContainerStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, 
    const std::vector<std::string>& lines, const bool friendlyPosition, const std::map<std::string, std::string> &parameters) {
    // first check if containerStop exist
    if (myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build containerStop
        GNEAdditional* containerStop = new GNEContainerStop(id, lane, myNet, /*startPos*/ 0, /*endPos*/ 1, /*parametersSet*/ 0, name, lines, 
                                                            friendlyPosition, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(containerStop);
            lane->addChildElement(containerStop);
            containerStop->incRef("buildContainerStop");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void
GNEAdditionalHandlerBeta::buildChargingStation(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, const double chargingPower, 
    const double efficiency, const bool chargeInTransit, const SUMOTime chargeDelay, const bool friendlyPosition, 
    const std::map<std::string, std::string> &parameters) {
    // first check if containerStop exist
    if (myNet->retrieveAdditional(SUMO_TAG_CHARGING_STATION, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build chargingStation
        GNEAdditional* chargingStation = new GNEChargingStation(id, lane, myNet, /*startPos*/ 0, /*endPos*/ 1, /*parametersSet*/ 0, 
                                                                name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition,
                                                                neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(chargingStation);
            lane->addChildElement(chargingStation);
            chargingStation->incRef("buildChargingStation");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " with ID '" + id + "' in netedit; probably declared twice.");
    }

}


void 
GNEAdditionalHandlerBeta::buildParkingArea(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const std::string &startPos, const std::string &endPos, const std::string& name, const bool friendlyPosition, const int roadSideCapacity, 
    const bool onRoad, const double width, const std::string& length, const double angle, const std::map<std::string, std::string> &parameters) {
    // first check if parkingArea exist
    if (myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build parkingArea
        GNEAdditional* parkingArea = new GNEParkingArea(id, lane, myNet, /*startPos*/ 0, /*endPos*/ 1, /*parametersSet*/ 0, name, 
                                                        friendlyPosition, roadSideCapacity, onRoad, width, length, angle, 
                                                        neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_AREA));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingArea, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(parkingArea);
            lane->addChildElement(parkingArea);
            parkingArea->incRef("buildParkingArea");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_PARKING_AREA) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildParkingSpace(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const Position &pos, const double width, 
    const double length, const double angle, const double slope, const std::map<std::string, std::string> &parameters) {
    // get lane
    GNEAdditional *parkingArea = myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // build parkingSpace
    GNEAdditional* parkingSpace = new GNEParkingSpace(myNet, parkingArea, pos, width, length, angle, slope, neteditParameters.blockMovement);
    // insert depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_SPACE));
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingSpace, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        myNet->getAttributeCarriers()->insertAdditional(parkingSpace);
        parkingArea->addChildElement(parkingSpace);
        parkingSpace->incRef("buildParkingSpace");
    }
    // update geometry (due boundaries)
    parkingSpace->updateGeometry();
}


void 
GNEAdditionalHandlerBeta::buildE1Detector(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, const std::string &laneId, 
    const double position, const SUMOTime frequency, const std::string &file, const std::string &vehicleTypes, const std::string &name, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if E1 exist
    if (myNet->retrieveAdditional(SUMO_TAG_E1DETECTOR, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneId);
        // build E1
        GNEAdditional* detectorE1 = new GNEDetectorE1(id, lane, myNet, position, frequency, file, vehicleTypes, name, friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(detectorE1);
            lane->addChildElement(detectorE1);
            detectorE1->incRef("buildDetectorE1");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildSingleLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const double pos, const double length, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::string& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if E2 exist
    if (myNet->retrieveAdditional(SUMO_TAG_E2DETECTOR, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build E2 single lane
        GNEAdditional* detectorE2 = new GNEDetectorE2(id, lane, myNet, pos, length, freq, trafficLight, filename, 
                                                      vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, 
                                                      friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(detectorE2);
            lane->addChildElement(detectorE2);
            detectorE2->incRef("buildDetectorE2");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildMultiLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::vector<std::string>& laneIDs, 
    const double pos, const double endPos, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::string& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if E2 exist
    if (myNet->retrieveAdditional(SUMO_TAG_E2DETECTOR_MULTILANE, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lanes
        std::vector<GNELane*> lanes;
        for (const auto &lane : laneIDs) {
            lanes.push_back(myNet->retrieveLane(lane));
        }
        // build E2 multilane detector
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lanes, myNet, pos, endPos, freq, trafficLight, filename, 
                                                      vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, 
                                                      friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR_MULTILANE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(detectorE2);
            for (const auto& lane : lanes) {
                lane->addChildElement(detectorE2);
            }
            detectorE2->incRef("buildDetectorE2Multilane");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR_MULTILANE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildDetectorE3(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, const SUMOTime freq, 
    const std::string& filename, const std::string& vehicleTypes, const std::string& name,  SUMOTime timeThreshold, const double speedThreshold, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildDetectorEntry(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos, const bool friendlyPos) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get lane
    GNEAdditional *E3 = myNet->retrieveAdditional(SUMO_TAG_E3DETECTOR, sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3 == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        // build entry instant
        GNEAdditional* entry = new GNEDetectorEntryExit(SUMO_TAG_DET_ENTRY, myNet, E3, lane, pos, friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(entry);
            lane->addChildElement(entry);
            E3->addChildElement(entry);
            entry->incRef("buildDetectorEntry");
        }
    }
}


void 
GNEAdditionalHandlerBeta::buildDetectorExit(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos, const bool friendlyPos) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get lane
    GNEAdditional *E3 = myNet->retrieveAdditional(SUMO_TAG_E3DETECTOR, sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3 == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        // build exit instant
        GNEAdditional* exit = new GNEDetectorEntryExit(SUMO_TAG_DET_EXIT, myNet, E3, lane, pos, friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(exit);
            lane->addChildElement(exit);
            E3->addChildElement(exit);
            exit->incRef("buildDetectorExit");
        }
    }
}


void 
GNEAdditionalHandlerBeta::buildDetectorE1Instant(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, double pos, 
    const std::string& filename, const std::string& vehicleTypes, const std::string& name, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // check if E1 instant exist
    if (myNet->retrieveAdditional(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build E1 instant
        GNEAdditional* detectorE1Instant = new GNEDetectorE1Instant(id, lane, myNet, pos, filename, vehicleTypes, name, friendlyPos, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(detectorE1Instant, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(detectorE1Instant);
            lane->addChildElement(detectorE1Instant);
            detectorE1Instant->incRef("buildDetectorE1Instant");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandlerBeta::buildLaneCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, const double pos, 
    const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, const std::map<std::string, std::string> &parameters) {
    //
}


void
GNEAdditionalHandlerBeta::buildEdgeCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &edgeID, const double pos, 
    const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, const std::map<std::string, std::string> &parameters) {
    //
}


void
GNEAdditionalHandlerBeta::buildCalibratorFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &vTypeID, const std::string &routeID,
    const std::string& vehsPerHour, const std::string& speed, const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, 
    const std::string& arrivalLane, const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, const int personNumber, const int containerNumber, 
    const bool reroute, const std::string& departPosLat, const std::string& arrivalPosLat, const SUMOTime begin, const SUMOTime end, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildRerouter(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, 
    const std::vector<std::string>& edgeIDs, const double prob, const std::string& name, const std::string& file, const bool off, const SUMOTime timeThreshold, 
    const std::string& vTypes, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildRerouterInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime begin, const SUMOTime end) {
    //
}


void 
GNEAdditionalHandlerBeta::buildClosingLaneReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &closedLane, SVCPermissions permissions) {
    //
}


void 
GNEAdditionalHandlerBeta::buildClosingReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &closedEdgeID, SVCPermissions permissions) {
    //
}

void 
GNEAdditionalHandlerBeta::buildDestProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &newEdgeDestinationID, const double probability) {
    //
}


void 
GNEAdditionalHandlerBeta::buildParkingAreaReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &newParkignAreaID, const double probability, const bool visible) {
    //
}


void
GNEAdditionalHandlerBeta::buildRouteProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& newRouteID, const double probability) {
    //
}


void 
GNEAdditionalHandlerBeta::buildRouteProbe(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &edgeID, const std::string& freq, 
    const std::string& name, const std::string& file, const SUMOTime begin, const std::map<std::string, std::string> &parameters) {
    //
}


void
GNEAdditionalHandlerBeta::buildVariableSpeedSign(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, 
    const std::vector<std::string>& destLaneIDs, const std::string& name, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildVariableSpeedSignStep(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime time, const double speed) {
    //
}


void 
GNEAdditionalHandlerBeta::buildVaporizer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const SUMOTime start, 
    const SUMOTime endTime, const std::string& name, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildTAZ(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const PositionVector& shape, 
    const RGBColor& color, const std::vector<std::string>& edgeIDs, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildTAZSource(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const double departWeight, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildTAZSink(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const double arrivalWeight, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildPolygon(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, double layer, double angle, const std::string& imgFile, bool relativePath, const PositionVector& shape, bool geo, bool fill, 
    double lineWidth, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildPOI(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, const Position& pos, bool geo, const std::string& lane, double posOverLane, double posLat, double layer, double angle,
    const std::string& imgFile, bool relativePath, double width, double height, const std::map<std::string, std::string> &parameters) {
    //
}


bool
GNEAdditionalHandlerBeta::accessCanBeCreated(GNEAdditional* busStopParent, GNEEdge* edge) {
    // check if exist another acces for the same busStop in the given edge
    for (const auto & additional: busStopParent->getChildAdditionals()) {
        for (const auto &lane : edge->getLanes()) {
            if (additional->getAttribute(SUMO_ATTR_LANE) == lane->getID()) {
                return false;
            }
        }
    }
    return true;
}


bool
GNEAdditionalHandlerBeta::checkOverlappingRerouterIntervals(GNEAdditional* rerouter, SUMOTime newBegin, SUMOTime newEnd) {
    // declare a vector to keep sorted rerouter children
    std::vector<std::pair<SUMOTime, SUMOTime>> sortedIntervals;
    // iterate over child additional
    for (const auto& rerouterChild : rerouter->getChildAdditionals()) {
        if (!rerouterChild->getTagProperty().isSymbol()) {
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

// ===========================================================================
// GNEAdditionalHandlerBeta::NeteditParameters method definitions
// ===========================================================================

GNEAdditionalHandlerBeta::NeteditParameters::NeteditParameters(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) :
    blockMovement(sumoBaseObject->hasBoolAttribute(GNE_ATTR_BLOCK_MOVEMENT)? sumoBaseObject->getBoolAttribute(GNE_ATTR_BLOCK_MOVEMENT) : false),
    select(sumoBaseObject->hasBoolAttribute(GNE_ATTR_SELECTED)? sumoBaseObject->getBoolAttribute(GNE_ATTR_SELECTED) : false),
    centerAfterCreation(sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)? sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) : false) {
}


GNEAdditionalHandlerBeta::NeteditParameters::~NeteditParameters() {}


GNEAdditionalHandlerBeta::NeteditParameters::NeteditParameters() :
    blockMovement(false),
    select(false),
    centerAfterCreation(false) {
}

/****************************************************************************/
