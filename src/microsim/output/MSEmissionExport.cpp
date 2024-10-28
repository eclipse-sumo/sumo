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
/// @file    MSEmissionExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2012-04-26
///
// Realises dumping Emission Data
/****************************************************************************/
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_Emissions.h>
#include <mesosim/MEVehicle.h>
#include <microsim/MSVehicleControl.h>
#include "MSEmissionExport.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MSEmissionExport::write(OutputDevice& of, SUMOTime timestep, int precision) {
    const OptionsCont& oc = OptionsCont::getOptions();
    const SUMOTime period = string2time(oc.getString("device.emissions.period"));
    const SUMOTime begin = string2time(oc.getString("device.emissions.begin"));
    const bool scaled = oc.getBool("emission-output.step-scaled");
    const bool useGeo = oc.getBool("emission-output.geo");
    if ((period > 0 && (timestep - begin) % period != 0) || timestep < begin) {
        return;
    }
    const SumoXMLAttrMask mask = MSDevice_Emissions::getWrittenAttributes();

    of.openTag("timestep").writeAttr("time", time2string(timestep));
    of.setPrecision(precision);
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        MSDevice_Emissions* emissionsDevice = (MSDevice_Emissions*)veh->getDevice(typeid(MSDevice_Emissions));
        if (emissionsDevice != nullptr && (veh->isOnRoad() || veh->isIdling())) {
            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));
            PollutantsInterface::Emissions emiss = PollutantsInterface::computeAll(
                    veh->getVehicleType().getEmissionClass(),
                    veh->getSpeed(), veh->getAcceleration(), veh->getSlope(),
                    veh->getEmissionParameters());
            if (scaled) {
                PollutantsInterface::Emissions tmp;
                tmp.addScaled(emiss, TS);
                emiss = tmp;
            }
            of.openTag("vehicle");
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeOptionalAttr(SUMO_ATTR_ECLASS, PollutantsInterface::getName(veh->getVehicleType().getEmissionClass()), mask);
            of.writeOptionalAttr(SUMO_ATTR_CO2, emiss.CO2, mask);
            of.writeOptionalAttr(SUMO_ATTR_CO, emiss.CO, mask);
            of.writeOptionalAttr(SUMO_ATTR_HC, emiss.HC, mask);
            of.writeOptionalAttr(SUMO_ATTR_NOX, emiss.NOx, mask);
            of.writeOptionalAttr(SUMO_ATTR_PMX, emiss.PMx, mask);
            of.writeOptionalAttr(SUMO_ATTR_FUEL, emiss.fuel, mask);
            of.writeOptionalAttr(SUMO_ATTR_ELECTRICITY, emiss.electricity, mask);
            of.writeOptionalAttr(SUMO_ATTR_NOISE, HelpersHarmonoise::computeNoise(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getAcceleration()), mask);
            of.writeOptionalAttr(SUMO_ATTR_ROUTE, veh->getRoute().getID(), mask);
            of.writeOptionalAttr(SUMO_ATTR_TYPE, fclass, mask);
            if (MSGlobals::gUseMesoSim) {
                const MEVehicle* mesoVeh = dynamic_cast<const MEVehicle*>(veh);
                of.writeOptionalAttr(SUMO_ATTR_WAITING, mesoVeh->getWaitingSeconds(), mask);
                of.writeOptionalAttr(SUMO_ATTR_EDGE, veh->getEdge()->getID(), mask);
            } else {
                const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
                of.writeOptionalAttr(SUMO_ATTR_WAITING, microVeh->getWaitingSeconds(), mask);
                of.writeOptionalAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID(), mask);
            }
            of.writeOptionalAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane(), mask);
            of.writeOptionalAttr(SUMO_ATTR_SPEED, veh->getSpeed(), mask);
            of.writeOptionalAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(veh->getAngle()), mask);

            Position pos = veh->getPosition();
            if (useGeo) {
                of.setPrecision(MAX2(gPrecisionGeo, precision));
                GeoConvHelper::getFinal().cartesian2geo(pos);
            }
            of.writeOptionalAttr(SUMO_ATTR_X, pos.x(), mask);
            of.writeOptionalAttr(SUMO_ATTR_Y, pos.y(), mask);
            if (MSNet::getInstance()->hasElevation()) {
                of.writeOptionalAttr(SUMO_ATTR_Z, pos.z(), mask);
            }
            of.setPrecision(precision);

            of.closeTag();
        }
    }
    of.setPrecision(gPrecision);
    of.closeTag();
}
