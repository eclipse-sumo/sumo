/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSFullExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2012-04-26
///
// Dumping a hugh List of Parameters available in the Simulation
/****************************************************************************/
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <mesosim/MEVehicle.h>
#include "MSFullExport.h"


// ===========================================================================
// method definitions
// ===========================================================================

// Helper function to get interpolated position for mesoscopic vehicles
static Position
getInterpolatedMesoPosition(const MEVehicle* mesoVeh) {
    const MESegment* segment = mesoVeh->getSegment();
    if (segment == nullptr) {
        return mesoVeh->getPosition();
    }

    const double now = SIMTIME;
    const double intendedLeave = MIN2(mesoVeh->getEventTimeSeconds(), mesoVeh->getBlockTimeSeconds());
    const double entry = mesoVeh->getLastEntryTimeSeconds();

    // Calculate segment offset (position of segment start)
    double segmentOffset = 0;
    for (MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(*mesoVeh->getEdge());
            seg != nullptr && seg != segment; seg = seg->getNextSegment()) {
        segmentOffset += seg->getLength();
    }

    // Calculate interpolated position within segment
    const double length = segment->getLength();
    const double relPos = segmentOffset + length * (now - entry) / (intendedLeave - entry);

    // Convert to cartesian coordinates
    const MSLane* const lane = mesoVeh->getEdge()->getLanes()[0];
    return lane->geometryPositionAtOffset(relPos);
}

// Helper function to get interpolated position on lane for mesoscopic vehicles
static double
getInterpolatedMesoPositionOnLane(const MEVehicle* mesoVeh) {
    const MESegment* segment = mesoVeh->getSegment();
    if (segment == nullptr) {
        return mesoVeh->getPositionOnLane();
    }

    const double now = SIMTIME;
    const double intendedLeave = MIN2(mesoVeh->getEventTimeSeconds(), mesoVeh->getBlockTimeSeconds());
    const double entry = mesoVeh->getLastEntryTimeSeconds();

    // Calculate segment offset (position of segment start)
    double segmentOffset = 0;
    for (MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(*mesoVeh->getEdge());
            seg != nullptr && seg != segment; seg = seg->getNextSegment()) {
        segmentOffset += seg->getLength();
    }

    // Calculate interpolated position within segment
    const double length = segment->getLength();
    return segmentOffset + length * (now - entry) / (intendedLeave - entry);
}
void
MSFullExport::write(OutputDevice& of, SUMOTime timestep) {
    of.openTag("data") << " timestep=\"" << time2string(timestep) << "\"";
    writeVehicles(of);
    if (MSNet::getInstance()->hasPersons()) {
        writePersons(of);
    }
    writeEdge(of);
    writeTLS(of, timestep);
    of.closeTag();
}


void
MSFullExport::writeVehicles(OutputDevice& of) {
    of.openTag("vehicles");
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    const bool hasEle = MSNet::getInstance()->hasElevation();
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
        const MEVehicle* mesoVeh = MSGlobals::gUseMesoSim ? static_cast<const MEVehicle*>(veh) : nullptr;
        if (veh->isOnRoad()) {
            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));
            PollutantsInterface::Emissions emiss = PollutantsInterface::computeAll(
                    veh->getVehicleType().getEmissionClass(), veh->getSpeed(),
                    veh->getAcceleration(), veh->getSlope(),
                    veh->getEmissionParameters());
            of.openTag(SUMO_TAG_VEHICLE);
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeAttr(SUMO_ATTR_ECLASS, PollutantsInterface::getName(veh->getVehicleType().getEmissionClass()));
            of.writeAttr(SUMO_ATTR_CO2, emiss.CO2);
            of.writeAttr(SUMO_ATTR_CO, emiss.CO);
            of.writeAttr(SUMO_ATTR_HC, emiss.HC);
            of.writeAttr(SUMO_ATTR_NOX, emiss.NOx);
            of.writeAttr(SUMO_ATTR_PMX, emiss.PMx);
            of.writeAttr(SUMO_ATTR_FUEL, emiss.fuel);
            of.writeAttr(SUMO_ATTR_ELECTRICITY, emiss.electricity);
            of.writeAttr(SUMO_ATTR_NOISE, HelpersHarmonoise::computeNoise(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getAcceleration()));
            of.writeAttr(SUMO_ATTR_ROUTE, veh->getRoute().getID());
            of.writeAttr(SUMO_ATTR_TYPE, fclass);
            if (microVeh != nullptr) {
                of.writeAttr(SUMO_ATTR_WAITING, microVeh->getWaitingSeconds());
                of.writeAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID());
            }
            // Use interpolated position for meso vehicles
            if (MSGlobals::gUseMesoSim) {
                of.writeAttr(SUMO_ATTR_POSITION, getInterpolatedMesoPositionOnLane(mesoVeh));
            } else {
                of.writeAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane());
            }
            of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
            of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(veh->getAngle()));
            // Use interpolated position for meso vehicles
            Position pos;
            if (MSGlobals::gUseMesoSim) {
                pos = getInterpolatedMesoPosition(mesoVeh);
            } else {
                pos = veh->getPosition();
            }
            of.writeAttr(SUMO_ATTR_X, pos.x());
            of.writeAttr(SUMO_ATTR_Y, pos.y());
            if (hasEle) {
                of.writeAttr(SUMO_ATTR_Z, pos.z());
                of.writeAttr(SUMO_ATTR_SLOPE, veh->getSlope());
            }
            of.closeTag();
        }
    }
    of.closeTag();
}

void
MSFullExport::writePersons(OutputDevice& of) {
    MSTransportableControl& tc = MSNet::getInstance()->getPersonControl();
    const bool hasEle = MSNet::getInstance()->hasElevation();
    of.openTag("persons");
    for (auto it = tc.loadedBegin(); it != tc.loadedEnd(); ++it) {
        const MSTransportable* p = it->second;
        if (p->getCurrentStageType() != MSStageType::WAITING_FOR_DEPART) {
            const MSEdge* e = p->getEdge();
            const SUMOVehicle* v = p->getVehicle();
            Position pos = p->getPosition();
            of.openTag(SUMO_TAG_PERSON);
            of.writeAttr(SUMO_ATTR_ID, p->getID());
            of.writeAttr(SUMO_ATTR_X, pos.x());
            of.writeAttr(SUMO_ATTR_Y, pos.y());
            if (hasEle) {
                of.writeAttr("z", pos.z());
            }
            of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(p->getAngle()));
            of.writeAttr(SUMO_ATTR_SPEED, p->getSpeed());
            of.writeAttr(SUMO_ATTR_POSITION, p->getEdgePos());
            of.writeAttr(SUMO_ATTR_EDGE, e->getID());
            of.writeAttr(SUMO_ATTR_SLOPE, e->getLanes()[0]->getShape().slopeDegreeAtOffset(p->getEdgePos()));
            of.writeAttr(SUMO_ATTR_VEHICLE, v == nullptr ? "" : v->getID());
            of.writeAttr(SUMO_ATTR_TYPE, p->getVehicleType().getID());
            of.writeAttr("stage", (int)p->getCurrentStageType());
            of.closeTag();
        }
    }
    of.closeTag();
}

void
MSFullExport::writeEdge(OutputDevice& of) {
    of.openTag("edges");
    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
    const MSEdgeVector& edges = ec.getEdges();
    for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        MSEdge& edge = **e;
        if (!MSGlobals::gUsingInternalLanes && !edge.isNormal()) {
            continue;
        }
        of.openTag("edge").writeAttr("id", edge.getID()).writeAttr("traveltime", edge.getCurrentTravelTime());
        const std::vector<MSLane*>& lanes = edge.getLanes();
        for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
            writeLane(of, **lane);
        }
        of.closeTag();
    }
    of.closeTag();
}


void
MSFullExport::writeLane(OutputDevice& of, const MSLane& lane) {
    of.openTag("lane").writeAttr("id", lane.getID()).writeAttr("CO", lane.getEmissions<PollutantsInterface::CO>()).writeAttr("CO2", lane.getEmissions<PollutantsInterface::CO2>());
    of.writeAttr("NOx", lane.getEmissions<PollutantsInterface::NO_X>()).writeAttr("PMx", lane.getEmissions<PollutantsInterface::PM_X>()).writeAttr("HC", lane.getEmissions<PollutantsInterface::HC>());
    of.writeAttr("noise", lane.getHarmonoise_NoiseEmissions()).writeAttr("fuel", lane.getEmissions<PollutantsInterface::FUEL>());
    of.writeAttr("electricity", lane.getEmissions<PollutantsInterface::ELEC>()).writeAttr("maxspeed", lane.getSpeedLimit());
    of.writeAttr("meanspeed", lane.getMeanSpeed()).writeAttr("occupancy", lane.getNettoOccupancy()).writeAttr("vehicle_count", lane.getVehicleNumber());
    of.closeTag();
}


void
MSFullExport::writeTLS(OutputDevice& of, SUMOTime /* timestep */) {
    of.openTag("tls");
    MSTLLogicControl& vc = MSNet::getInstance()->getTLSControl();
    std::vector<std::string> ids = vc.getAllTLIds();
    for (std::vector<std::string>::const_iterator id_it = ids.begin(); id_it != ids.end(); ++id_it) {
        MSTLLogicControl::TLSLogicVariants& vars = MSNet::getInstance()->getTLSControl().get(*id_it);
        const MSTrafficLightLogic::LaneVectorVector& lanes = vars.getActive()->getLaneVectors();

        std::vector<std::string> laneIDs;
        for (MSTrafficLightLogic::LaneVectorVector::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            const MSTrafficLightLogic::LaneVector& llanes = (*i);
            for (MSTrafficLightLogic::LaneVector::const_iterator j = llanes.begin(); j != llanes.end(); ++j) {
                laneIDs.push_back((*j)->getID());
            }
        }

        std::string lane_output = "";
        for (int i1 = 0; i1 < (int)laneIDs.size(); ++i1) {
            lane_output += laneIDs[i1] + " ";
        }

        std::string state = vars.getActive()->getCurrentPhaseDef().getState();
        of.openTag("trafficlight").writeAttr("id", *id_it).writeAttr("state", state).closeTag();
    }
    of.closeTag();
}


/****************************************************************************/
