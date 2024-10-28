/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
#include <utils/shapes/SUMOPolygon.h>
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
    const SUMOTime begin = string2time(oc.getString("device.fcd.begin"));
    if ((period > 0 && (timestep - begin) % period != 0) || timestep < begin) {
        return;
    }
    const SumoXMLAttrMask mask = MSDevice_FCD::getWrittenAttributes();
    const bool maskSet = oc.isSet("fcd-output.attributes");
    const bool useGeo = oc.getBool("fcd-output.geo");
    const bool signals = oc.getBool("fcd-output.signals") || (maskSet && of.useAttribute(SUMO_ATTR_SIGNALS, mask));
    const bool writeAccel = oc.getBool("fcd-output.acceleration") || (maskSet && of.useAttribute(SUMO_ATTR_ACCELERATION, mask));
    const bool writeDistance = oc.getBool("fcd-output.distance") || (maskSet && of.useAttribute(SUMO_ATTR_DISTANCE, mask));
    const double maxLeaderDistance = oc.getFloat("fcd-output.max-leader-distance");
    std::vector<std::string> params = oc.getStringVector("fcd-output.params");
    MSNet* net = MSNet::getInstance();
    MSVehicleControl& vc = net->getVehicleControl();
    const double radius = oc.getFloat("device.fcd.radius");
    const bool filter = MSDevice_FCD::getEdgeFilter().size() > 0;
    const bool shapeFilter = MSDevice_FCD::hasShapeFilter();
    std::set<const Named*> inRadius;
    if (radius > 0) {
        // collect all vehicles in radius around equipped vehicles
        for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
            const SUMOVehicle* veh = it->second;
            if (isVisible(veh) && hasOwnOutput(veh, filter, shapeFilter)) {
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
        if (isVisible(veh)) {
            const bool hasOutput = hasOwnOutput(veh, filter, shapeFilter, (radius > 0 && inRadius.count(veh) > 0));
            if (hasOutput) {
                Position pos = veh->getPosition();
                if (useGeo) {
                    of.setPrecision(gPrecisionGeo);
                    GeoConvHelper::getFinal().cartesian2geo(pos);
                }
                of.openTag(SUMO_TAG_VEHICLE);
                of.writeAttr(SUMO_ATTR_ID, veh->getID());
                of.writeOptionalAttr(SUMO_ATTR_X, pos.x(), mask);
                of.writeOptionalAttr(SUMO_ATTR_Y, pos.y(), mask);
                of.setPrecision(gPrecision);
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
                    double lanePos = veh->getPositionOnLane();
                    if (microVeh != nullptr && microVeh->getLane()->isInternal()) {
                        lanePos = microVeh->getRoute().getDistanceBetween(0., lanePos, microVeh->getEdge()->getLanes()[0], microVeh->getLane(),
                                  microVeh->getRoutePosition());
                    }
                    of.writeOptionalAttr(SUMO_ATTR_DISTANCE, veh->getEdge()->getDistanceAt(lanePos), mask);
                }
                of.writeOptionalAttr(SUMO_ATTR_ODOMETER, veh->getOdometer(), mask);
                of.writeOptionalAttr(SUMO_ATTR_POSITION_LAT, veh->getLateralPositionOnLane(), mask);
                if (microVeh != nullptr) {
                    of.writeOptionalAttr(SUMO_ATTR_SPEED_LAT, microVeh->getLaneChangeModel().getSpeedLat(), mask);
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
                if (of.useAttribute(SUMO_ATTR_ARRIVALDELAY, mask)) {
                    double arrivalDelay = baseVeh->getStopArrivalDelay();
                    if (arrivalDelay == INVALID_DOUBLE) {
                        // no upcoming stop also means that there is no delay
                        arrivalDelay = 0;
                    }
                    of.writeOptionalAttr(SUMO_ATTR_ARRIVALDELAY, arrivalDelay, mask);
                }
                of.closeTag();
            }
            // write persons and containers
            const MSEdge* edge = microVeh == nullptr ? veh->getEdge() : &veh->getLane()->getEdge();

            const std::vector<MSTransportable*>& persons = veh->getPersons();
            for (MSTransportable* person : persons) {
                writeTransportable(of, edge, person, veh, filter, shapeFilter, inRadius.count(person) > 0, SUMO_TAG_PERSON, useGeo, elevation, mask);
            }
            const std::vector<MSTransportable*>& containers = veh->getContainers();
            for (MSTransportable* container : containers) {
                writeTransportable(of, edge, container, veh, filter, shapeFilter, inRadius.count(container) > 0, SUMO_TAG_CONTAINER, useGeo, elevation, mask);
            }
        }
    }
    if (net->hasPersons() && net->getPersonControl().hasTransportables()) {
        // write persons
        MSEdgeControl& ec = net->getEdgeControl();
        const MSEdgeVector& edges = ec.getEdges();
        for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            if (filter && MSDevice_FCD::getEdgeFilter().count(*e) == 0) {
                continue;
            }
            const std::vector<MSTransportable*>& persons = (*e)->getSortedPersons(timestep);
            for (MSTransportable* person : persons) {
                writeTransportable(of, *e, person, nullptr, filter, shapeFilter, inRadius.count(person) > 0, SUMO_TAG_PERSON, useGeo, elevation, mask);
            }
        }
    }
    if (net->hasContainers() && net->getContainerControl().hasTransportables()) {
        // write containers
        MSEdgeControl& ec = net->getEdgeControl();
        const std::vector<MSEdge*>& edges = ec.getEdges();
        for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            if (filter && MSDevice_FCD::getEdgeFilter().count(*e) == 0) {
                continue;
            }
            const std::vector<MSTransportable*>& containers = (*e)->getSortedContainers(timestep);
            for (MSTransportable* container : containers) {
                writeTransportable(of, *e, container, nullptr, filter, shapeFilter, inRadius.count(container) > 0, SUMO_TAG_CONTAINER, useGeo, elevation, mask);
            }
        }
    }
    of.closeTag();
}

bool
MSFCDExport::isVisible(const SUMOVehicle* veh) {
    return veh->isOnRoad() || veh->isParking() || veh->isRemoteControlled();
}

bool
MSFCDExport::hasOwnOutput(const SUMOVehicle* veh, bool filter, bool shapeFilter, bool isInRadius) {
    return ((!filter || MSDevice_FCD::getEdgeFilter().count(veh->getEdge()) > 0)
            && (!shapeFilter || MSDevice_FCD::shapeFilter(veh))
            && ((veh->getDevice(typeid(MSDevice_FCD)) != nullptr) || isInRadius));
}

bool
MSFCDExport::hasOwnOutput(const MSTransportable* p, bool filter, bool shapeFilter, bool isInRadius) {
    return ((!filter || MSDevice_FCD::getEdgeFilter().count(p->getEdge()) > 0)
            && (!shapeFilter || MSDevice_FCD::shapeFilter(p))
            && ((p->getDevice(typeid(MSTransportableDevice_FCD)) != nullptr) || isInRadius));
}

void
MSFCDExport::writeTransportable(OutputDevice& of, const MSEdge* e, MSTransportable* p, const SUMOVehicle* v,
                                bool filter, bool shapeFilter, bool inRadius,
                                SumoXMLTag tag, bool useGeo, bool elevation, SumoXMLAttrMask mask) {
    if (!hasOwnOutput(p, filter, shapeFilter, inRadius)) {
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
    of.writeOptionalAttr(SUMO_ATTR_TYPE, p->getVehicleType().getID(), mask);
    of.closeTag();
}


/****************************************************************************/
