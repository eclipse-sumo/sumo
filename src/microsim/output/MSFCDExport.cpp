/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSFCDExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    2012-04-26
///
// Realises dumping Floating Car Data (FCD) Data
/****************************************************************************/
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomHelper.h>
#include <libsumo/Helper.h>
#include <microsim/devices/MSDevice_FCD.h>
#include <microsim/devices/MSTransportableDevice_FCD.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSVehicleControl.h>
#include "MSFCDExport.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFCDExport::write(OutputDevice& of, SUMOTime timestep, bool elevation) {
    const OptionsCont& oc = OptionsCont::getOptions();
    const SUMOTime period = string2time(oc.getString("device.fcd.period"));
    const SUMOTime begin = string2time(oc.getString("begin"));
    if (period > 0 && (timestep - begin) % period != 0) {
        return;
    }
    const long long int mask = MSDevice_FCD::getWrittenAttributes();
    const bool maskSet = oc.isSet("fcd-output.attributes");
    const bool useGeo = oc.getBool("fcd-output.geo");
    const bool signals = oc.getBool("fcd-output.signals") || (maskSet && of.useAttribute(SUMO_ATTR_SIGNALS, mask));
    const bool writeAccel = oc.getBool("fcd-output.acceleration") || (maskSet && of.useAttribute(SUMO_ATTR_ACCELERATION, mask));
    const bool writeDistance = oc.getBool("fcd-output.distance") || (maskSet && of.useAttribute(SUMO_ATTR_DISTANCE, mask));
    const double maxLeaderDistance = oc.getFloat("fcd-output.max-leader-distance");
    std::vector<std::string> params = oc.getStringVector("fcd-output.params");
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    const double radius = oc.getFloat("device.fcd.radius");
    const bool filter = MSDevice_FCD::getEdgeFilter().size() > 0;
    std::set<const Named*> inRadius;
    if (radius > 0) {
        // collect all vehicles in radius around equipped vehicles
        for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
            const SUMOVehicle* veh = it->second;
            MSDevice_FCD* fcdDevice = (MSDevice_FCD*)veh->getDevice(typeid(MSDevice_FCD));
            if (fcdDevice != nullptr
                    && (veh->isOnRoad() || veh->isParking() || veh->isRemoteControlled())
                    && (!filter || MSDevice_FCD::getEdgeFilter().count(veh->getEdge()) > 0)) {
                PositionVector shape;
                shape.push_back(veh->getPosition());
                libsumo::Helper::collectObjectsInRange(libsumo::CMD_GET_VEHICLE_VARIABLE, shape, radius, inRadius);
                libsumo::Helper::collectObjectsInRange(libsumo::CMD_GET_PERSON_VARIABLE, shape, radius, inRadius);
            }
        }
    }

    of.openTag("timestep").writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
        const MSBaseVehicle* baseVeh = dynamic_cast<const MSBaseVehicle*>(veh);
        if ((veh->isOnRoad() || veh->isParking() || veh->isRemoteControlled())
                // only filter on normal edges
                && (!filter || MSDevice_FCD::getEdgeFilter().count(veh->getEdge()) > 0)
                && (veh->getDevice(typeid(MSDevice_FCD)) != nullptr || (radius > 0 && inRadius.count(veh) > 0))) {
            Position pos = veh->getPosition();
            if (useGeo) {
                of.setPrecision(gPrecisionGeo);
                GeoConvHelper::getFinal().cartesian2geo(pos);
            }
            of.openTag(SUMO_TAG_VEHICLE);
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeOptionalAttr(SUMO_ATTR_X, pos.x(), mask);
            of.writeOptionalAttr(SUMO_ATTR_Y, pos.y(), mask);
            if (elevation) {
                of.writeOptionalAttr(SUMO_ATTR_Z, pos.z(), mask);
            }
            of.writeOptionalAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(veh->getAngle()), mask);
            of.writeOptionalAttr(SUMO_ATTR_TYPE, veh->getVehicleType().getID(), mask);
            of.writeOptionalAttr(SUMO_ATTR_SPEED, veh->getSpeed(), mask);
            of.writeOptionalAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane(), mask);
            if (microVeh != nullptr) {
                of.writeOptionalAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID(), mask);
            } else {
                of.writeOptionalAttr(SUMO_ATTR_EDGE, veh->getEdge()->getID(), mask);
            }
            of.writeOptionalAttr(SUMO_ATTR_SLOPE, veh->getSlope(), mask);
            if (microVeh != nullptr) {
                if (signals) {
                    of.writeOptionalAttr(SUMO_ATTR_SIGNALS, toString(microVeh->getSignals()), mask);
                }
                if (writeAccel) {
                    of.writeOptionalAttr(SUMO_ATTR_ACCELERATION, toString(microVeh->getAcceleration()), mask);
                    if (MSGlobals::gSublane) {
                        of.writeOptionalAttr(SUMO_ATTR_ACCELERATION_LAT, microVeh->getLaneChangeModel().getAccelerationLat(), mask);
                    }
                }
            }
            if (writeDistance) {
                double distance = veh->getEdge()->getDistance();
                if (microVeh != nullptr) {
                    if (microVeh->getLane()->isInternal()) {
                        distance += microVeh->getRoute().getDistanceBetween(0, microVeh->getPositionOnLane(),
                                    microVeh->getEdge(), &microVeh->getLane()->getEdge(), true, microVeh->getRoutePosition());
                    } else {
                        distance += microVeh->getPositionOnLane();
                    }
                } else {
                    distance += veh->getPositionOnLane();
                }
                // if the kilometrage runs counter to the edge direction edge->getDistance() is negative
                of.writeOptionalAttr(SUMO_ATTR_DISTANCE, fabs(distance), mask);
            }
            if (maxLeaderDistance >= 0 && microVeh != nullptr) {
                std::pair<const MSVehicle* const, double> leader = microVeh->getLeader(maxLeaderDistance);
                if (leader.first != nullptr) {
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_ID, toString(leader.first->getID()), mask);
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_SPEED, toString(leader.first->getSpeed()), mask);
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_GAP, toString(leader.second + microVeh->getVehicleType().getMinGap()), mask);
                } else {
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_ID, "", mask);
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_SPEED, -1, mask);
                    of.writeOptionalAttr(SUMO_ATTR_LEADER_GAP, -1, mask);
                }
            }
            for (const std::string& key : params) {
                std::string error;
                const std::string value = baseVeh->getPrefixedParameter(key, error);
                if (value != "") {
                    of.writeAttr(StringUtils::escapeXML(key), StringUtils::escapeXML(value));
                }
            }
            of.closeTag();
            // write persons and containers
            const MSEdge* edge = microVeh == nullptr ? veh->getEdge() : &veh->getLane()->getEdge();

            const std::vector<MSTransportable*>& persons = veh->getPersons();
            for (MSTransportable* person : persons) {
                writeTransportable(of, edge, person, veh, inRadius.count(person), SUMO_TAG_PERSON, useGeo, elevation, mask);
            }
            const std::vector<MSTransportable*>& containers = veh->getContainers();
            for (MSTransportable* container : containers) {
                writeTransportable(of, edge, container, veh, inRadius.count(container), SUMO_TAG_CONTAINER, useGeo, elevation, mask);
            }
        }
    }
    if (MSNet::getInstance()->getPersonControl().hasTransportables()) {
        // write persons
        MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
        const MSEdgeVector& edges = ec.getEdges();
        for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            if (filter && MSDevice_FCD::getEdgeFilter().count(*e) == 0) {
                continue;
            }
            const std::vector<MSTransportable*>& persons = (*e)->getSortedPersons(timestep);
            for (MSTransportable* person : persons) {
                writeTransportable(of, *e, person, nullptr, inRadius.count(person) > 0, SUMO_TAG_PERSON, useGeo, elevation, mask);
            }
        }
    }
    if (MSNet::getInstance()->getContainerControl().hasTransportables()) {
        // write containers
        MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
        const std::vector<MSEdge*>& edges = ec.getEdges();
        for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            if (filter && MSDevice_FCD::getEdgeFilter().count(*e) == 0) {
                continue;
            }
            const std::vector<MSTransportable*>& containers = (*e)->getSortedContainers(timestep);
            for (MSTransportable* container : containers) {
                writeTransportable(of, *e, container, nullptr, inRadius.count(container) > 0, SUMO_TAG_CONTAINER, useGeo, elevation, mask);
            }
        }
    }
    of.closeTag();
}


void
MSFCDExport::writeTransportable(OutputDevice& of, const MSEdge* e, MSTransportable* p, const SUMOVehicle* v, bool inRadius, SumoXMLTag tag, bool useGeo, bool elevation, long long int mask) {
    if (p->getDevice(typeid(MSTransportableDevice_FCD)) == nullptr && !inRadius) {
        return;
    }
    Position pos = p->getPosition();
    if (useGeo) {
        of.setPrecision(gPrecisionGeo);
        GeoConvHelper::getFinal().cartesian2geo(pos);
    }
    of.openTag(tag);
    of.writeAttr(SUMO_ATTR_ID, p->getID());
    of.writeOptionalAttr(SUMO_ATTR_X, pos.x(), mask);
    of.writeOptionalAttr(SUMO_ATTR_Y, pos.y(), mask);
    if (elevation) {
        of.writeOptionalAttr(SUMO_ATTR_Z, pos.z(), mask);
    }
    of.writeOptionalAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(p->getAngle()), mask);
    of.writeOptionalAttr(SUMO_ATTR_SPEED, p->getSpeed(), mask);
    of.writeOptionalAttr(SUMO_ATTR_POSITION, p->getEdgePos(), mask);
    of.writeOptionalAttr(SUMO_ATTR_EDGE, e->getID(), mask);
    of.writeOptionalAttr(SUMO_ATTR_SLOPE, e->getLanes()[0]->getShape().slopeDegreeAtOffset(p->getEdgePos()), mask);
    of.writeOptionalAttr(SUMO_ATTR_VEHICLE, v == nullptr ? "" : v->getID(), mask);
    of.closeTag();
}


/****************************************************************************/
