/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSFCDExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    2012-04-26
/// @version $Id$
///
// Realises dumping Floating Car Data (FCD) Data
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/devices/MSDevice_FCD.h>
#include <microsim/devices/MSTransportableDevice_FCD.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSContainer.h>
#include <microsim/MSVehicleControl.h>
#include "MSFCDExport.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFCDExport::write(OutputDevice& of, SUMOTime timestep, bool elevation) {
    const bool useGeo = OptionsCont::getOptions().getBool("fcd-output.geo");
    const bool signals = OptionsCont::getOptions().getBool("fcd-output.signals");
    const bool writeDistance = OptionsCont::getOptions().getBool("fcd-output.distance");
    const SUMOTime period = string2time(OptionsCont::getOptions().getString("device.fcd.period"));
    if (period > 0 && timestep % period != 0) {
        return;
    }
    of.openTag("timestep").writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    const bool filter = MSDevice_FCD::getEdgeFilter().size() > 0;
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
        if ((veh->isOnRoad() || veh->isParking() || veh->isRemoteControlled())
                && veh->getDevice(typeid(MSDevice_FCD)) != nullptr
                // only filter on normal edges
                && (!filter || MSDevice_FCD::getEdgeFilter().count(veh->getEdge()) > 0)) {
            Position pos = veh->getPosition();
            if (useGeo) {
                of.setPrecision(gPrecisionGeo);
                GeoConvHelper::getFinal().cartesian2geo(pos);
            }
            of.openTag(SUMO_TAG_VEHICLE);
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeAttr(SUMO_ATTR_X, pos.x());
            of.writeAttr(SUMO_ATTR_Y, pos.y());
            if (elevation) {
                of.writeAttr(SUMO_ATTR_Z, pos.z());
            }
            of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(veh->getAngle()));
            of.writeAttr(SUMO_ATTR_TYPE, veh->getVehicleType().getID());
            of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
            of.writeAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane());
            if (microVeh != nullptr) {
                of.writeAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID());
            }
            of.writeAttr(SUMO_ATTR_SLOPE, veh->getSlope());
            if (microVeh != nullptr) {
                if (signals) {
                    of.writeAttr("signals", toString(microVeh->getSignals()));
                }
                if (writeDistance) {
                    double distance = microVeh->getEdge()->getDistance();
                    if (microVeh->getLane()->isInternal()) {
                        distance += microVeh->getRoute().getDistanceBetween(0, microVeh->getPositionOnLane(),
                                    microVeh->getEdge(), &microVeh->getLane()->getEdge(), true, microVeh->getRoutePosition());
                    } else {
                        distance += microVeh->getPositionOnLane();
                    }
                    // if the kilometrage runs counter to the edge direction edge->getDistance() is negative
                    of.writeAttr("distance", fabs(distance));
                }
            }
            of.closeTag();
            // write persons and containers
            const MSEdge* edge = microVeh == nullptr ? veh->getEdge() : &veh->getLane()->getEdge();

            const std::vector<MSTransportable*>& persons = veh->getPersons();
            for (std::vector<MSTransportable*>::const_iterator it_p = persons.begin(); it_p != persons.end(); ++it_p) {
                writeTransportable(of, edge, *it_p, SUMO_TAG_PERSON, useGeo, elevation);
            }
            const std::vector<MSTransportable*>& containers = veh->getContainers();
            for (std::vector<MSTransportable*>::const_iterator it_c = containers.begin(); it_c != containers.end(); ++it_c) {
                writeTransportable(of, edge, *it_c, SUMO_TAG_CONTAINER, useGeo, elevation);
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
            for (std::vector<MSTransportable*>::const_iterator it_p = persons.begin(); it_p != persons.end(); ++it_p) {
                writeTransportable(of, *e, *it_p, SUMO_TAG_PERSON, useGeo, elevation);
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
            for (std::vector<MSTransportable*>::const_iterator it_c = containers.begin(); it_c != containers.end(); ++it_c) {
                writeTransportable(of, *e, *it_c, SUMO_TAG_CONTAINER, useGeo, elevation);
            }
        }
    }
    of.closeTag();
}


void
MSFCDExport::writeTransportable(OutputDevice& of, const MSEdge* e, MSTransportable* p, SumoXMLTag tag, bool useGeo, bool elevation) {
    if (p->getDevice(typeid(MSTransportableDevice_FCD)) == nullptr) {
        return;
    }
    Position pos = p->getPosition();
    if (useGeo) {
        of.setPrecision(gPrecisionGeo);
        GeoConvHelper::getFinal().cartesian2geo(pos);
    }
    of.openTag(tag);
    of.writeAttr(SUMO_ATTR_ID, p->getID());
    of.writeAttr(SUMO_ATTR_X, pos.x());
    of.writeAttr(SUMO_ATTR_Y, pos.y());
    if (elevation) {
        of.writeAttr(SUMO_ATTR_Z, pos.z());
    }
    of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(p->getAngle()));
    of.writeAttr(SUMO_ATTR_SPEED, p->getSpeed());
    of.writeAttr(SUMO_ATTR_POSITION, p->getEdgePos());
    of.writeAttr(SUMO_ATTR_EDGE, e->getID());
    of.writeAttr(SUMO_ATTR_SLOPE, e->getLanes()[0]->getShape().slopeDegreeAtOffset(p->getEdgePos()));
    of.closeTag();
}
/****************************************************************************/
