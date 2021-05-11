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

GNEAdditionalHandlerBeta::GNEAdditionalHandlerBeta(const std::string& file, GNENet* net, GNEAdditional* additionalParent) :
    AdditionalHandler(file),
    myNet(net) {
}


GNEAdditionalHandlerBeta::~GNEAdditionalHandlerBeta() {
}


void 
GNEAdditionalHandlerBeta::buildBusStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, 
    const std::vector<std::string>& lines, const int personCapacity, const double parkingLength, const bool friendlyPosition, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildAccess(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, 
    const double pos, const std::string& length, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildContainerStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, 
    const std::vector<std::string>& lines, const bool friendlyPosition, const std::map<std::string, std::string> &parameters) {
    //
}


void
GNEAdditionalHandlerBeta::buildChargingStation(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, 
    const std::string &laneID, const std::string &startPos, const std::string &endPos, const std::string& name, const double chargingPower, 
    const double efficiency, const bool chargeInTransit, const SUMOTime chargeDelay, const bool friendlyPosition, 
    const std::map<std::string, std::string> &parameters) {
    //

}


void 
GNEAdditionalHandlerBeta::buildParkingArea(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const std::string &startPos, const std::string &endPos, const std::string& name, const bool friendlyPosition, const int roadSideCapacity, 
    const bool onRoad, const double width, const std::string& length, const double angle, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildParkingSpace(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const Position &pos, const double width, 
    const double length, const double angle, const double slope, const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildE1Detector(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, const std::string &laneId, 
    const double position, const SUMOTime frequency, const std::string &file, const std::string &vehicleTypes, const std::string &name, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildSingleLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, 
    const double pos, const double length, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::string& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildMultiLaneDetectorE2(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::vector<std::string>& lanes, 
    const double pos, const double endPos, const std::string& freq, const std::string& trafficLight, const std::string& filename, const std::string& vehicleTypes, 
    const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildDetectorE3(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const Position &pos, const SUMOTime freq, 
    const std::string& filename, const std::string& vehicleTypes, const std::string& name,  SUMOTime timeThreshold, const double speedThreshold, 
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
GNEAdditionalHandlerBeta::buildDetectorEntry(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos, const bool friendlyPos) {
    //
}


void 
GNEAdditionalHandlerBeta::buildDetectorExit(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &laneID, const double pos, const bool friendlyPos) {
    //
}


void 
GNEAdditionalHandlerBeta::buildDetectorE1Instant(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const std::string &laneID, double pos, 
    const std::string& filename, const std::string& vehicleTypes, const std::string& name, const bool friendlyPos, const std::map<std::string, std::string> &parameters) {
    //
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
    // check that busStopParent is a busStop
    assert(busStopParent->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    // check if exist another acces for the same busStop in the given edge
    for (auto i : busStopParent->getChildAdditionals()) {
        for (auto j : edge->getLanes()) {
            if (i->getAttribute(SUMO_ATTR_LANE) == j->getID()) {
                return false;
            }
        }
    }
    return true;
}


bool
GNEAdditionalHandlerBeta::checkOverlappingRerouterIntervals(GNEAdditional* rerouter, SUMOTime newBegin, SUMOTime newEnd) {
    // check that rerouter is correct
    assert(rerouter->getTagProperty().getTag() == SUMO_TAG_REROUTER);
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

/****************************************************************************/
