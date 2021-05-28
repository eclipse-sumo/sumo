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
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// Builds trigger objects for netedit
/****************************************************************************/
#include <config.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_TAZElement.h>
#include <netedit/changes/GNEChange_Shape.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEAdditionalHandler.h"
#include "GNEAccess.h"
#include "GNEBusStop.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE1Instant.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntryExit.h"
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
#include "GNETAZSourceSink.h"
#include "GNEVaporizer.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNEVariableSpeedSignSymbol.h"


// ===========================================================================
// GNEAdditionalHandler method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    AdditionalHandler(file),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {
}


void 
GNEAdditionalHandler::buildBusStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
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
        GNEAdditional* busStop = new GNEBusStop(id, lane, myNet, startPos, endPos, name, lines, personCapacity, 
                                                parkingLength, friendlyPosition, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildAccess(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, 
    const double pos, const double length, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get lane
    GNEAdditional *busStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
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
        GNEAdditional* access = new GNEAccess(busStop, lane, myNet, pos, length, friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildContainerStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, 
    const std::vector<std::string>& lines, const bool friendlyPosition, const std::map<std::string, std::string> &parameters) {
    // first check if containerStop exist
    if (myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build containerStop
        GNEAdditional* containerStop = new GNEContainerStop(id, lane, myNet, startPos, endPos, name, lines, friendlyPosition, 
                                                            parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildChargingStation(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
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
        GNEAdditional* chargingStation = new GNEChargingStation(id, lane, myNet, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, 
                                                                chargeDelay, friendlyPosition, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildParkingArea(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const std::string &startPos, const std::string &endPos, const std::string &departPos, const std::string& name, const bool friendlyPosition, 
    const int roadSideCapacity, const bool onRoad, const double width, const double length, const double angle, const std::map<std::string, std::string> &parameters) {
    // first check if parkingArea exist
    if (myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build parkingArea
        GNEAdditional* parkingArea = new GNEParkingArea(id, lane, myNet, startPos, endPos, departPos, name, friendlyPosition, roadSideCapacity,
                                                        onRoad, width, length, angle, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildParkingSpace(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const double x, const double y, const double z, 
    const std::string& name, const std::string &width, const std::string &length, const std::string &angle, const double slope, 
    const std::map<std::string, std::string> &parameters) {
    // get lane
    GNEAdditional *parkingArea = myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // build parkingSpace
    GNEAdditional* parkingSpace = new GNEParkingSpace(myNet, parkingArea, x, y, z, width, length, angle, slope, name, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildE1Detector(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, const std::string &laneID, 
    const double position, const SUMOTime frequency, const std::string &file, const std::vector<std::string> &vehicleTypes, const std::string &name, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if E1 exist
    if (myNet->retrieveAdditional(SUMO_TAG_E1DETECTOR, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build E1
        GNEAdditional* detectorE1 = new GNEDetectorE1(id, lane, myNet, position, frequency, file, vehicleTypes, name, friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildSingleLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const double pos, const double length, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if E2 exist
    if ((myNet->retrieveAdditional(SUMO_TAG_E2DETECTOR, id, false) == nullptr) && 
        (myNet->retrieveAdditional(GNE_TAG_E2DETECTOR_MULTILANE, id, false) == nullptr)) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build E2 single lane
        GNEAdditional* detectorE2 = new GNEDetectorE2(id, lane, myNet, pos, length, freq, trafficLight, filename, 
                                                      vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, 
                                                      friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildMultiLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::vector<std::string>& laneIDs, 
    const double pos, const double endPos, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    // first check if Multilane E2 exist
    if ((myNet->retrieveAdditional(SUMO_TAG_E2DETECTOR, id, false) == nullptr) && 
        (myNet->retrieveAdditional(GNE_TAG_E2DETECTOR_MULTILANE, id, false) == nullptr)) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lanes
        std::vector<GNELane*> lanes;
        for (const auto &lane : laneIDs) {
            lanes.push_back(myNet->retrieveLane(lane));
        }
        // build E2 multilane detector
        GNEAdditional* detectorE2 = new GNEDetectorE2(id, lanes, myNet, pos, endPos, freq, trafficLight, filename, 
                                                      vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, 
                                                      friendlyPos, parameters, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(GNE_TAG_E2DETECTOR_MULTILANE));
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
        throw ProcessError("Could not build " + toString(GNE_TAG_E2DETECTOR_MULTILANE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandler::buildDetectorE3(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, const SUMOTime freq, 
    const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold, const double speedThreshold, 
    const std::map<std::string, std::string> &parameters) {
    // get NETEDIT parameters
    NeteditParameters neteditParameters(sumoBaseObject);
    // build E3
    GNEAdditional* E3 = new GNEDetectorE3(id, myNet, pos, freq, filename, vehicleTypes, name, timeThreshold, speedThreshold, parameters, neteditParameters.blockMovement);
    // insert depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(E3, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        myNet->getAttributeCarriers()->insertAdditional(E3);
        E3->incRef("buildDetectorE3");
    }
}


void 
GNEAdditionalHandler::buildDetectorEntry(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos, 
        const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get E3 parent
    GNEAdditional *E3 = myNet->retrieveAdditional(SUMO_TAG_E3DETECTOR, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3 == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // build entry instant
        GNEAdditional* entry = new GNEDetectorEntryExit(SUMO_TAG_DET_ENTRY, myNet, E3, lane, pos, friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildDetectorExit(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos,
        const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // get lane
    GNELane *lane = myNet->retrieveLane(laneID);
    // get E3 parent
    GNEAdditional *E3 = myNet->retrieveAdditional(SUMO_TAG_E3DETECTOR, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3 == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // build exit instant
        GNEAdditional* exit = new GNEDetectorEntryExit(SUMO_TAG_DET_EXIT, myNet, E3, lane, pos, friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildDetectorE1Instant(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, double pos, 
    const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    // check if E1 instant exist
    if (myNet->retrieveAdditional(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build E1 instant
        GNEAdditional* detectorE1Instant = new GNEDetectorE1Instant(id, lane, myNet, pos, filename, vehicleTypes, name, friendlyPos, parameters, neteditParameters.blockMovement);
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
GNEAdditionalHandler::buildLaneCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, const double pos, 
    const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, const double jamThreshold, const std::vector<std::string> &vTypes, 
    const std::map<std::string, std::string> &parameters) {
    // check if lane calibrator exist
    if ((myNet->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) && 
        (myNet->retrieveAdditional(SUMO_TAG_LANECALIBRATOR, id, false) == nullptr)) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // build Calibrator
        GNEAdditional* calibrator = new GNECalibrator(id, myNet, lane, pos, freq, name, outfile, routeprobe, jamThreshold, vTypes, parameters, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
            // center after creation
            if (neteditParameters.centerAfterCreation) {
                myNet->getViewNet()->centerTo(calibrator->getPositionInView(), false);
            }
        } else {
            myNet->getAttributeCarriers()->insertAdditional(calibrator);
            lane->addChildElement(calibrator);
            calibrator->incRef("buildCalibrator");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void
GNEAdditionalHandler::buildEdgeCalibrator(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &edgeID, const double pos, 
    const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, const double jamThreshold, const std::vector<std::string> &vTypes, 
    const std::map<std::string, std::string> &parameters) {
    // check if lane calibrator exist
    if ((myNet->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) && 
        (myNet->retrieveAdditional(SUMO_TAG_LANECALIBRATOR, id, false) == nullptr)) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get edge
        GNEEdge *edge = myNet->retrieveEdge(edgeID);
        // build Calibrator
        GNEAdditional* calibrator = new GNECalibrator(id, myNet, edge, pos, freq, name, outfile, routeprobe, jamThreshold, vTypes, parameters, neteditParameters.blockMovement);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
            // center after creation
            if (neteditParameters.centerAfterCreation) {
                myNet->getViewNet()->centerTo(calibrator->getPositionInView(), false);
            }
        } else {
            myNet->getAttributeCarriers()->insertAdditional(calibrator);
            edge->addChildElement(calibrator);
            calibrator->incRef("buildCalibrator");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void
GNEAdditionalHandler::buildCalibratorFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &vTypeID, const std::string &routeID,
    const std::string& vehsPerHour, const std::string& speed, const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, 
    const std::string& arrivalLane, const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, const int personNumber, const int containerNumber, 
    const bool reroute, const std::string& departPosLat, const std::string& arrivalPosLat, const SUMOTime begin, const SUMOTime end, const std::map<std::string, std::string> &parameters) {
    // get vType
    GNEDemandElement *vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vTypeID);
    // get route
    GNEDemandElement *route = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, routeID);
    // get calibrator parent
    GNEAdditional *calibrator = myNet->retrieveAdditional(sumoBaseObject->getTag(), sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // create calibrator flow
    GNEAdditional* flow = new GNECalibratorFlow(calibrator, vType, route, vehsPerHour, speed, color, departLane, departPos, departSpeed,
            arrivalLane, arrivalPos, arrivalSpeed, line, personNumber, containerNumber, reroute, departPosLat, arrivalPosLat, begin, end, parameters);
    // insert depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + flow->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(flow, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        calibrator->addChildElement(flow);
        flow->incRef("buildCalibratorFlow");
    }
}


void 
GNEAdditionalHandler::buildRerouter(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, 
    const std::vector<std::string>& edgeIDs, const double prob, const std::string& name, const std::string& file, const bool off, const SUMOTime timeThreshold, 
    const std::vector<std::string>& vTypes, const std::map<std::string, std::string> &parameters) {
    // check if lane calibrator exist
    if (myNet->retrieveAdditional(SUMO_TAG_REROUTER, id, false) == nullptr) {
        // parse edges
        std::vector<GNEEdge*> edges;
        for (const auto &edge : edgeIDs) {
            edges.push_back(myNet->retrieveEdge(edge));
        }
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // create reroute
        GNEAdditional* rerouter = new GNERerouter(id, myNet, pos, name, file, prob, off, timeThreshold, vTypes, parameters, neteditParameters.blockMovement);
        // create rerouter Symbols
        std::vector<GNEAdditional*> rerouterSymbols;
        for (const auto& edge : edges) {
            rerouterSymbols.push_back(new GNERerouterSymbol(rerouter, edge));
        }
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
            // add symbols
            for (const auto& rerouterSymbol : rerouterSymbols) {
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouterSymbol, true), true);
            }
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(rerouter);
            // add symbols
            for (int i = 0; i < (int)edges.size(); i++) {
                edges.at(i)->addChildElement(rerouterSymbols.at(i));
                rerouterSymbols.at(i)->incRef("buildRerouterSymbol");
            }
            rerouter->incRef("buildRerouter");
        }
/*
        // parse rerouter children
        if (!file.empty()) {
            // we assume that rerouter values files is placed in the same folder as the additional file
            std::string currentAdditionalFilename = FileHelpers::getFilePath(OptionsCont::getOptions().getString("additional-files"));
            // Create additional handler for parse rerouter values
            GNEAdditionalHandler rerouterValuesHandler(currentAdditionalFilename + file, net, rerouter);
            // disable validation for rerouters
            XMLSubSys::setValidation("never", "auto", "auto");
            // Run parser
            if (!XMLSubSys::runParser(rerouterValuesHandler, currentAdditionalFilename + file, false)) {
                WRITE_MESSAGE("Loading of " + file + " failed.");
            }
            // enable validation for rerouters
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
*/
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandler::buildRerouterInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime begin, const SUMOTime end) {
    // get rerouter parent
    GNEAdditional *rerouter = myNet->retrieveAdditional(SUMO_TAG_REROUTER, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // check if new interval will produce a overlapping
    if (checkOverlappingRerouterIntervals(rerouter, begin, end)) {
        // create rerouter interval and add it into rerouter parent
        GNEAdditional* rerouterInterval = new GNERerouterInterval(rerouter, begin, end);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + rerouterInterval->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(rerouterInterval, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            rerouter->addChildElement(rerouterInterval);
            rerouterInterval->incRef("buildRerouterInterval");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INTERVAL) + " with begin '" + toString(begin) + "' and '" + toString(end) + "' in '" + rerouter->getID() + "' due overlapping.");
    }
}


void 
GNEAdditionalHandler::buildClosingLaneReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &closedLaneID, SVCPermissions permissions) {
    // get rerouter id
    const std::string rerouterID = sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID);
    // get rerouter interval parent
    GNEAdditional *rerouterInterval = myNet->retrieveRerouterInterval(rerouterID, sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN), sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    // get closed lane
    GNELane *lane = myNet->retrieveLane(closedLaneID);
    // create closing lane reorute
    GNEAdditional* closingLaneReroute = new GNEClosingLaneReroute(rerouterInterval, lane, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + closingLaneReroute->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        rerouterInterval->addChildElement(closingLaneReroute);
        closingLaneReroute->incRef("buildClosingLaneReroute");
    }
}


void 
GNEAdditionalHandler::buildClosingReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &closedEdgeID, SVCPermissions permissions) {
    // get rerouter id
    const std::string rerouterID = sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID);
    // get rerouter interval parent
    GNEAdditional *rerouterInterval = myNet->retrieveRerouterInterval(rerouterID, sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN), sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    // get closed edge
    GNEEdge *edge = myNet->retrieveEdge(closedEdgeID);
    // create closing reroute
    GNEAdditional* closingLaneReroute = new GNEClosingReroute(rerouterInterval, edge, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + closingLaneReroute->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        rerouterInterval->addChildElement(closingLaneReroute);
        closingLaneReroute->incRef("buildClosingLaneReroute");
    }
}

void 
GNEAdditionalHandler::buildDestProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &newEdgeDestinationID, const double probability) {
    // get rerouter id
    const std::string rerouterID = sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID);
    // get rerouter interval parent
    GNEAdditional *rerouterInterval = myNet->retrieveRerouterInterval(rerouterID, sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN), sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    // get edge
    GNEEdge *edge = myNet->retrieveEdge(newEdgeDestinationID);
    // create dest probability reroute
    GNEAdditional* destProbReroute = new GNEDestProbReroute(rerouterInterval, edge, probability);
    // add it to interval parent depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + destProbReroute->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(destProbReroute, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        rerouterInterval->addChildElement(destProbReroute);
        destProbReroute->incRef("builDestProbReroute");
    }
}


void 
GNEAdditionalHandler::buildParkingAreaReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &newParkignAreaID, const double probability, const bool visible) {
    // get rerouter id
    const std::string rerouterID = sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID);
    // get rerouter interval parent
    GNEAdditional *rerouterInterval = myNet->retrieveRerouterInterval(rerouterID, sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN), sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    // get parking area
    GNEAdditional *parkingArea = myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, newParkignAreaID);
    // create parking area reroute
    GNEAdditional* parkingAreaReroute = new GNEParkingAreaReroute(rerouterInterval, parkingArea, probability, visible);
    // add it to interval parent depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + parkingAreaReroute->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingAreaReroute, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        rerouterInterval->addChildElement(parkingAreaReroute);
        parkingAreaReroute->incRef("builParkingAreaReroute");
    }
}


void
GNEAdditionalHandler::buildRouteProbReroute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& newRouteID, const double probability) {
    // get rerouter id
    const std::string rerouterID = sumoBaseObject->getParentSumoBaseObject()->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID);
    // get rerouter interval parent
    GNEAdditional *rerouterInterval = myNet->retrieveRerouterInterval(rerouterID, sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_BEGIN), sumoBaseObject->getParentSumoBaseObject()->getTimeAttribute(SUMO_ATTR_END));
    // create rout prob reroute
    GNEAdditional* routeProbReroute = new GNERouteProbReroute(rerouterInterval, newRouteID, probability);
    // add it to interval parent depending of allowUndoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + routeProbReroute->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(routeProbReroute, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        rerouterInterval->addChildElement(routeProbReroute);
        routeProbReroute->incRef("buildRouteProbReroute");
    }
}


void 
GNEAdditionalHandler::buildRouteProbe(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &edgeID, const SUMOTime freq, 
    const std::string& name, const std::string& file, const SUMOTime begin, const std::map<std::string, std::string> &parameters) {
    // check if routeprobe exist
    if (myNet->retrieveAdditional(SUMO_TAG_ROUTEPROBE, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get edge
        GNEEdge *edge = myNet->retrieveEdge(edgeID);
        // build route probe
        GNEAdditional* routeProbe = new GNERouteProbe(id, myNet, edge, freq, name, file, begin, parameters);
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
            // center after creation
            if (neteditParameters.centerAfterCreation) {
                myNet->getViewNet()->centerTo(routeProbe->getPositionInView(), false);
            }
        } else {
            myNet->getAttributeCarriers()->insertAdditional(routeProbe);
            edge->addChildElement(routeProbe);
            routeProbe->incRef("buildRouteProbe");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void
GNEAdditionalHandler::buildVariableSpeedSign(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, 
    const std::vector<std::string>& laneIDs, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if VSS exist
    if (myNet->retrieveAdditional(SUMO_TAG_VSS, id, false) == nullptr) {
        // parse lanes
        std::vector<GNELane*> lanes;
        for (const auto &lane : laneIDs) {
            lanes.push_back(myNet->retrieveLane(lane));
        }
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // create VSS
        GNEAdditional* variableSpeedSign = new GNEVariableSpeedSign(id, myNet, pos, name, parameters, neteditParameters.blockMovement);
        // create VSS Symbols
        std::vector<GNEAdditional*> VSSSymbols;
        for (const auto& lane : lanes) {
            VSSSymbols.push_back(new GNEVariableSpeedSignSymbol(variableSpeedSign, lane));
        }
        // insert depending of allowUndoRedo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
            for (const auto& VSSSymbol : VSSSymbols) {
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(VSSSymbol, true), true);
            }
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertAdditional(variableSpeedSign);
            // add symbols
            for (int i = 0; i < (int)lanes.size(); i++) {
                lanes.at(i)->addChildElement(VSSSymbols.at(i));
                VSSSymbols.at(i)->incRef("buildVariableSpeedSignSymbol");
            }
            variableSpeedSign->incRef("buildVariableSpeedSign");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandler::buildVariableSpeedSignStep(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOTime time, const std::string &speed) {
    // get VSS parent
    GNEAdditional *VSS = myNet->retrieveAdditional(SUMO_TAG_VSS, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // create Variable Speed Sign
    GNEAdditional* variableSpeedSignStep = new GNEVariableSpeedSignStep(VSS, time, speed);
    // add it depending of allow undoRedo
    if (myAllowUndoRedo) {
        myNet->getViewNet()->getUndoList()->p_begin("add " + variableSpeedSignStep->getTagStr());
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(variableSpeedSignStep, true), true);
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        VSS->addChildElement(variableSpeedSignStep);
        variableSpeedSignStep->incRef("buildVariableSpeedSignStep");
    }
}


void 
GNEAdditionalHandler::buildVaporizer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const SUMOTime from, 
    const SUMOTime endTime, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if Vaporizer exist
    if (myNet->retrieveAdditional(SUMO_TAG_VAPORIZER, edgeID, false) == nullptr) {
        // get edge
        GNEEdge *edge = myNet->retrieveEdge(edgeID);
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // build vaporizer
        GNEAdditional* vaporizer = new GNEVaporizer(myNet, edge, from, endTime, name, parameters);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
            // center after creation
            if (neteditParameters.centerAfterCreation) {
                myNet->getViewNet()->centerTo(vaporizer->getPositionInView(), false);
            }
        } else {
            myNet->getAttributeCarriers()->insertAdditional(vaporizer);
            edge->addChildElement(vaporizer);
            vaporizer->incRef("buildVaporizer");
        }
    }
}


void 
GNEAdditionalHandler::buildTAZ(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const PositionVector& shape, 
    const RGBColor& color, const std::vector<std::string>& edgeIDs, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if VSS exist
    if (myNet->retrieveTAZElement(SUMO_TAG_TAZ, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // parse edges
        std::vector<GNEEdge*> edges;
        for (const auto &edge : edgeIDs) {
            edges.push_back(myNet->retrieveEdge(edge));
        }
        // build TAZ
        GNETAZElement* TAZ = new GNETAZ(id, myNet, shape, color,name, parameters, neteditParameters.blockMovement);
        // disable updating geometry of TAZ children during insertion (because in large nets provokes slowdowns)
        myNet->disableUpdateGeometry();
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZ));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZ, true), true);
            // create TAZEdges
            for (const auto& edge : edges) {
                // create TAZ Source using GNEChange_Additional
                GNETAZElement* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
                myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSource, true), true);
                // create TAZ Sink using GNEChange_Additional
                GNETAZElement* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
                myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSink, true), true);
            }
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertTAZElement(TAZ);
            TAZ->incRef("buildTAZ");
            for (const auto& edge : edges) {
                // create TAZ Source
                GNETAZElement* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
                TAZSource->incRef("buildTAZ");
                TAZ->addChildElement(TAZSource);
                // create TAZ Sink
                GNETAZElement* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
                TAZSink->incRef("buildTAZ");
                TAZ->addChildElement(TAZSink);
            }
        }
        // enable updating geometry again and update geometry of TAZ
        myNet->enableUpdateGeometry();
        // update TAZ Frame
        TAZ->updateGeometry();
        TAZ->updateParentAdditional();
    }
}


void 
GNEAdditionalHandler::buildTAZSource(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const double departWeight) {
    // get TAZ parent
    GNETAZElement *TAZ = myNet->retrieveTAZElement(SUMO_TAG_TAZ, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // get edge
    GNEEdge *edge = myNet->retrieveEdge(edgeID);
    // declare TAZ Sink
    GNETAZElement* TAZSink = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (const auto& TAZElement : TAZ->getChildTAZElements()) {
        if ((TAZElement->getTagProperty().getTag() == SUMO_TAG_TAZSINK) && (TAZElement->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSink = TAZElement;
        }
    }
    // check if TAZSink has to be created
    if (TAZSink == nullptr) {
        // Create TAZ with weight 0 (default)
        TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSINK));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSink, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertTAZElement(TAZSink);
            TAZSink->incRef("buildTAZSource");
        }
    }
    // now check check if TAZSource exist
    GNETAZElement* TAZSource = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (const auto& TAZElement : TAZ->getChildTAZElements()) {
        if ((TAZElement->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) && (TAZElement->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSource = TAZElement;
        }
    }
    // check if TAZSource has to be created
    if (TAZSource == nullptr) {
        // Create TAZ only with departWeight
        TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, departWeight);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSOURCE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSource, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertTAZElement(TAZSource);
            TAZSource->incRef("buildTAZSource");
        }
    } else {
        // update TAZ Attribute depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("update " + toString(SUMO_TAG_TAZSOURCE));
            TAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            TAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), nullptr);
            TAZSource->incRef("buildTAZSource");
        }
    }
}


void 
GNEAdditionalHandler::buildTAZSink(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edgeID, const double arrivalWeight) {
    // get TAZ parent
    GNETAZElement *TAZ = myNet->retrieveTAZElement(SUMO_TAG_TAZ, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
    // get edge
    GNEEdge *edge = myNet->retrieveEdge(edgeID);
    // declare TAZ source
    GNETAZElement* TAZSource = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (const auto& TAZElement : TAZ->getChildTAZElements()) {
        if ((TAZElement->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) && (TAZElement->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSource = TAZElement;
        }
    }
    // check if TAZSource has to be created
    if (TAZSource == nullptr) {
        // Create TAZ with empty value
        TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSOURCE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSource, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertTAZElement(TAZSource);
            TAZSource->incRef("buildTAZSink");
        }
    }
    GNETAZElement* TAZSink = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (const auto& TAZElement : TAZ->getChildTAZElements()) {
        if ((TAZElement->getTagProperty().getTag() == SUMO_TAG_TAZSINK) && (TAZElement->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSink = TAZElement;
        }
    }
    // check if TAZSink has to be created
    if (TAZSink == nullptr) {
        // Create TAZ only with arrivalWeight
        TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, arrivalWeight);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSINK));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_TAZElement(TAZSink, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertTAZElement(TAZSink);
            TAZSink->incRef("buildTAZSink");
        }
    } else {
        // update TAZ Attribute depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("update " + toString(SUMO_TAG_TAZSINK));
            TAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            TAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), nullptr);
            TAZSink->incRef("buildTAZSink");
        }
    }
}


void 
GNEAdditionalHandler::buildPolygon(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, double layer, double angle, const std::string& imgFile, bool relativePath, const PositionVector& shape, bool geo, bool fill, 
    double lineWidth, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if poly exist
    if (myNet->retrieveShape(SUMO_TAG_POLY, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // create poly
        GNEPoly* poly = new GNEPoly(myNet, id, type, shape, geo, fill, lineWidth, color, layer, angle, imgFile, relativePath, name, parameters, neteditParameters.blockMovement);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + toString(SUMO_TAG_POLY));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Shape(poly, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertShape(poly);
            poly->incRef("addPolygon");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_POLY) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandler::buildPOI(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, const double x, const double y, double layer, double angle, const std::string& imgFile, bool relativePath, 
    double width, double height, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if ID is duplicated
    if (myNet->retrieveShape(SUMO_TAG_POI, id, false) == nullptr) {
         // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // create POI
        GNEPOI* POI = new GNEPOI(myNet, id, type, color, x, y, false, layer, angle, imgFile, relativePath, width, height, name, parameters, neteditParameters.blockMovement);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + POI->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Shape(POI, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertShape(POI);
            POI->incRef("addPOI");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_POI) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


void 
GNEAdditionalHandler::buildPOILane(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, const std::string& laneID, double posOverLane, double posLat, double layer, double angle, const std::string& imgFile, 
    bool relativePath, double width, double height, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if ID is duplicated
    if (myNet->retrieveShape(SUMO_TAG_POLY, id, false) == nullptr) {
        // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // get lane
        GNELane *lane = myNet->retrieveLane(laneID);
        // create POI
        GNEShape* POILane = new GNEPOI(myNet, id, type, color, lane, posOverLane, posLat, layer, angle, imgFile, relativePath, width, height, name, parameters, neteditParameters.blockMovement);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + POILane->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Shape(POILane, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertShape(POILane);
            lane->addChildElement(POILane);
            POILane->incRef("buildPOILane");
        }
    }
}


void
GNEAdditionalHandler::buildPOIGeo(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string& type, 
    const RGBColor& color, const double lon, const double lat, double layer, double angle, const std::string& imgFile, bool relativePath, 
    double width, double height, const std::string& name, const std::map<std::string, std::string> &parameters) {
    // check if ID is duplicated
    if (myNet->retrieveShape(SUMO_TAG_POI, id, false) == nullptr) {
         // get NETEDIT parameters
        NeteditParameters neteditParameters(sumoBaseObject);
        // create POIGEO
        GNEPOI* POIGEO = new GNEPOI(myNet, id, type, color, lon, lat, true, layer, angle, imgFile, relativePath, width, height, name, parameters, neteditParameters.blockMovement);
        // add it depending of allow undoRed
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + POIGEO->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_Shape(POIGEO, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // insert shape without allowing undo/redo
            myNet->getAttributeCarriers()->insertShape(POIGEO);
            POIGEO->incRef("buildPOIGeo");
        }
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_POI) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::accessCanBeCreated(GNEAdditional* busStopParent, GNEEdge* edge) {
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
GNEAdditionalHandler::checkOverlappingRerouterIntervals(GNEAdditional* rerouter, SUMOTime newBegin, SUMOTime newEnd) {
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


bool 
GNEAdditionalHandler::checkAndFixDetectorPosition(double& pos, const double laneLength, const bool friendlyPos) {
    if (fabs(pos) > laneLength) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength - 0.01;
        }
    }
    return true;
}


bool 
GNEAdditionalHandler::fixE2DetectorPosition(double& pos, double& length, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || ((pos + length) > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength - 0.01;
            length = 0;
        } else if ((pos + length) > laneLength) {
            length = laneLength - pos - 0.01;
        }
    }
    return true;
}

// ===========================================================================
// GNEAdditionalHandler::NeteditParameters method definitions
// ===========================================================================

GNEAdditionalHandler::NeteditParameters::NeteditParameters(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) :
    blockMovement(sumoBaseObject->hasBoolAttribute(GNE_ATTR_BLOCK_MOVEMENT)? sumoBaseObject->getBoolAttribute(GNE_ATTR_BLOCK_MOVEMENT) : false),
    select(sumoBaseObject->hasBoolAttribute(GNE_ATTR_SELECTED)? sumoBaseObject->getBoolAttribute(GNE_ATTR_SELECTED) : false),
    centerAfterCreation(sumoBaseObject->hasBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION)? sumoBaseObject->getBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION) : false) {
}


GNEAdditionalHandler::NeteditParameters::~NeteditParameters() {}


GNEAdditionalHandler::NeteditParameters::NeteditParameters() :
    blockMovement(false),
    select(false),
    centerAfterCreation(false) {
}

/****************************************************************************/
