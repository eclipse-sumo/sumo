/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSChargingStation.cpp
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @author  Mirko Barthauer
/// @date    20-12-13
///
// Charging Station for Electric vehicles
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/common/StringUtils.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSParkingArea.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/devices/MSDevice_Battery.h>
#include <microsim/MSNet.h>
#include "MSChargingStation.h"


// ===========================================================================
// member method definitions
// ===========================================================================

MSChargingStation::MSChargingStation(const std::string& chargingStationID, MSLane& lane, double startPos, double endPos,
                                     const std::string& name, double chargingPower, double efficency, bool chargeInTransit,
                                     SUMOTime chargeDelay, const std::string& chargeType, SUMOTime waitingTime) :
    MSStoppingPlace(chargingStationID, SUMO_TAG_CHARGING_STATION, std::vector<std::string>(), lane, startPos, endPos, name),
    myChargeInTransit(chargeInTransit) {
    if (chargingPower < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGINGPOWER), getID(), toString(chargingPower)))
    } else {
        myChargingPower = chargingPower;
    }
    if (efficency < 0 || efficency > 1) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_EFFICIENCY), getID(), toString(efficency)))
    } else {
        myEfficiency = efficency;
    }
    if (chargeDelay < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGEDELAY), getID(), toString(chargeDelay)))
    } else {
        myChargeDelay = chargeDelay;
    }
    if ((chargeType != "normal") && (chargeType != "electric") && (chargeType != "fuel")) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGETYPE), getID(), chargeType))
    } else {
        myChargeDelay = chargeDelay;
    }
    if (waitingTime < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_WAITINGTIME), getID(), toString(waitingTime)))
    } else {
        myChargeDelay = chargeDelay;
    }
    if (getBeginLanePosition() > getEndLanePosition()) {
        WRITE_WARNING(TLF("ChargingStation with ID='%' doesn't have a valid position (% < %).", getID(), toString(getBeginLanePosition()), toString(getEndLanePosition())));
    }
}


MSChargingStation::MSChargingStation(const std::string& chargingStationID, const MSParkingArea* parkingArea, const std::string& name, double chargingPower,
                                     double efficency, bool chargeInTransit, SUMOTime chargeDelay, const std::string& chargeType, SUMOTime waitingTime) :
    MSChargingStation(chargingStationID, const_cast<MSLane&>(parkingArea->getLane()), parkingArea->getBeginLanePosition(), parkingArea->getEndLanePosition(),
                      name, chargingPower, efficency, chargeInTransit, chargeDelay, chargeType, waitingTime) {
    myParkingArea = parkingArea;
}


MSChargingStation::~MSChargingStation() {
}


double
MSChargingStation::getChargingPower(bool usingFuel) const {
    if (usingFuel) {
        return myChargingPower;
    } else {
        // Convert from [Ws] to [Wh] (3600s / 1h):
        return myChargingPower / 3600;
    }
}


double
MSChargingStation::getEfficency() const {
    return myEfficiency;
}


bool
MSChargingStation::getChargeInTransit() const {
    return myChargeInTransit;
}


SUMOTime
MSChargingStation::getChargeDelay() const {
    return myChargeDelay;
}


const std::string&
MSChargingStation::getChargeType() const {
    return myChargeType;
}


SUMOTime
MSChargingStation::getWaitingTime() const {
    return myWaitingTime;
}


const MSParkingArea*
MSChargingStation::getParkingArea() const {
    return myParkingArea;
}


void
MSChargingStation::setChargingPower(double chargingPower) {
    myChargingPower = chargingPower;
}


void
MSChargingStation::setEfficiency(double efficiency) {
    myEfficiency = efficiency;
}


void
MSChargingStation::setChargeDelay(SUMOTime delay) {
    myChargeDelay = delay;
}


void
MSChargingStation::setChargeInTransit(bool value) {
    myChargeInTransit = value;
}


void
MSChargingStation::setChargingVehicle(bool value) {
    myChargingVehicle = value;
}


bool
MSChargingStation::vehicleIsInside(const double position) const {
    if ((position >= getBeginLanePosition()) && (position <= getEndLanePosition())) {
        return true;
    } else {
        return false;
    }
}


bool
MSChargingStation::isCharging() const {
    return myChargingVehicle;
}


void
MSChargingStation::addChargeValueForOutput(double WCharged, MSDevice_Battery* battery) {
    if (!OptionsCont::getOptions().isSet("chargingstations-output")) {
        return;
    }
    std::string status = "";
    if (battery->getChargingStartTime() > myChargeDelay) {
        if (battery->getHolder().getSpeed() < battery->getStoppingThreshold()) {
            status = "chargingStopped";
        } else if (myChargeInTransit) {
            status = "chargingInTransit";
        } else {
            status = "noCharging";
        }
    } else {
        if (myChargeInTransit) {
            status = "waitingChargeInTransit";
        } else if (battery->getHolder().getSpeed() < battery->getStoppingThreshold()) {
            status = "waitingChargeStopped";
        } else {
            status = "noWaitingCharge";
        }
    }
    // update total charge
    myTotalCharge += WCharged;
    // create charge row and insert it in myChargeValues
    const std::string vehID = battery->getHolder().getID();
    if (myChargeValues.count(vehID) == 0) {
        myChargedVehicles.push_back(vehID);
    }
    Charge C(MSNet::getInstance()->getCurrentTimeStep(), vehID, battery->getHolder().getVehicleType().getID(),
             status, WCharged, battery->getActualBatteryCapacity(), battery->getMaximumBatteryCapacity(),
             myChargingPower, myEfficiency, myTotalCharge);
    myChargeValues[vehID].push_back(C);
}


void
MSChargingStation::writeChargingStationOutput(OutputDevice& output) {
    int chargingSteps = 0;
    for (const auto& item : myChargeValues) {
        chargingSteps += (int)item.second.size();
    }
    output.openTag(SUMO_TAG_CHARGING_STATION);
    output.writeAttr(SUMO_ATTR_ID, myID);
    output.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED, myTotalCharge);
    output.writeAttr(SUMO_ATTR_CHARGINGSTEPS, chargingSteps);
    // start writing
    if (myChargeValues.size() > 0) {
        for (const std::string& vehID : myChargedVehicles) {
            int iStart = 0;
            const auto& chargeSteps = myChargeValues[vehID];
            while (iStart < (int)chargeSteps.size()) {
                int iEnd = iStart + 1;
                double charged = chargeSteps[iStart].WCharged;
                while (iEnd < (int)chargeSteps.size() && chargeSteps[iEnd].timeStep == chargeSteps[iEnd - 1].timeStep + DELTA_T) {
                    charged += chargeSteps[iEnd].WCharged;
                    iEnd++;
                }
                writeVehicle(output, chargeSteps, iStart, iEnd, charged);
                iStart = iEnd;
            }
        }
    }
    // close charging station tag
    output.closeTag();
}


void
MSChargingStation::writeAggregatedChargingStationOutput(OutputDevice& output) {
    std::vector<std::string> terminatedChargers;
    for (const auto& item : myChargeValues) {
        const Charge& lastCharge = item.second.back();
        if (lastCharge.timeStep < SIMSTEP - DELTA_T) {
            // no charge during the last time step == has stopped charging
            terminatedChargers.push_back(item.first);

            // aggregate values
            double charged = 0.;
            double minPower = lastCharge.chargingPower;
            double maxPower = lastCharge.chargingPower;
            double minCharge = lastCharge.WCharged;
            double maxCharge = lastCharge.WCharged;
            double minEfficiency = lastCharge.chargingEfficiency;
            double maxEfficiency = lastCharge.chargingEfficiency;

            for (const auto& charge : item.second) {
                charged += charge.WCharged;
                if (charge.chargingPower < minPower) {
                    minPower = charge.chargingPower;
                }
                if (charge.chargingPower > maxPower) {
                    maxPower = charge.chargingPower;
                }
                if (charge.WCharged < minCharge) {
                    minCharge = charge.WCharged;
                }
                if (charge.WCharged > maxCharge) {
                    maxCharge = charge.WCharged;
                }
                if (charge.chargingEfficiency < minEfficiency) {
                    minEfficiency = charge.chargingEfficiency;
                }
                if (charge.chargingEfficiency > maxEfficiency) {
                    maxEfficiency = charge.chargingEfficiency;
                }
            }

            // actually write the data
            output.openTag(SUMO_TAG_CHARGING_EVENT);
            output.writeAttr(SUMO_ATTR_CHARGINGSTATIONID, myID);
            output.writeAttr(SUMO_ATTR_VEHICLE, lastCharge.vehicleID);
            output.writeAttr(SUMO_ATTR_TYPE, lastCharge.vehicleType);
            output.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED_VEHICLE, charged);
            output.writeAttr(SUMO_ATTR_CHARGINGBEGIN, time2string(item.second.at(0).timeStep));
            output.writeAttr(SUMO_ATTR_CHARGINGEND, time2string(lastCharge.timeStep));
            output.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, lastCharge.actualBatteryCapacity);
            output.writeAttr(SUMO_ATTR_MAXIMUMBATTERYCAPACITY, lastCharge.maxBatteryCapacity);
            output.writeAttr(SUMO_ATTR_MINPOWER, minPower);
            output.writeAttr(SUMO_ATTR_MAXPOWER, maxPower);
            output.writeAttr(SUMO_ATTR_MINCHARGE, minCharge);
            output.writeAttr(SUMO_ATTR_MAXCHARGE, maxCharge);
            output.writeAttr(SUMO_ATTR_MINEFFICIENCY, minEfficiency);
            output.writeAttr(SUMO_ATTR_MAXEFFICIENCY, maxEfficiency);
            output.closeTag();
        }
    }

    // clear charging data of vehicles which terminated charging
    for (auto vehID : terminatedChargers) {
        myChargeValues.erase(vehID);
    }
}


void
MSChargingStation::writeVehicle(OutputDevice& out, const std::vector<Charge>& chargeSteps, int iStart, int iEnd, double charged) {
    const Charge& first = chargeSteps[iStart];
    out.openTag(SUMO_TAG_VEHICLE);
    out.writeAttr(SUMO_ATTR_ID, first.vehicleID);
    out.writeAttr(SUMO_ATTR_TYPE, first.vehicleType);
    out.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED_VEHICLE, charged);
    out.writeAttr(SUMO_ATTR_CHARGINGBEGIN, time2string(first.timeStep));
    out.writeAttr(SUMO_ATTR_CHARGINGEND, time2string(chargeSteps[iEnd - 1].timeStep));
    for (int i = iStart; i < iEnd; i++) {
        const Charge& c = chargeSteps[i];
        out.openTag(SUMO_TAG_STEP);
        out.writeAttr(SUMO_ATTR_TIME, time2string(c.timeStep));
        // charge values
        out.writeAttr(SUMO_ATTR_CHARGING_STATUS, c.status);
        out.writeAttr(SUMO_ATTR_ENERGYCHARGED, c.WCharged);
        out.writeAttr(SUMO_ATTR_PARTIALCHARGE, c.totalEnergyCharged);
        // charging values of charging station in this timestep
        out.writeAttr(SUMO_ATTR_CHARGINGPOWER, c.chargingPower);
        out.writeAttr(SUMO_ATTR_EFFICIENCY, c.chargingEfficiency);
        // battery status of vehicle
        out.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, c.actualBatteryCapacity);
        out.writeAttr(SUMO_ATTR_MAXIMUMBATTERYCAPACITY, c.maxBatteryCapacity);
        // close tag timestep
        out.closeTag();
    }
    out.closeTag();
}


/****************************************************************************/
