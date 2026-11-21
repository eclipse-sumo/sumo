/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
#include <cmath>
#include <utils/common/StringUtils.h>
#include <utils/common/WrappingCommand.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSEventControl.h>
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
                                     const std::string& name, double chargingPower, double totalPower, double dischargePower, double efficency,
                                     bool chargeInTransit, SUMOTime chargeDelay, const std::string& chargeType, SUMOTime waitingTime,
                                     double chargeRampUp, double chargeRampDown, SUMOTime chargeStopDelay) :
    MSStoppingPlace(chargingStationID, SUMO_TAG_CHARGING_STATION, std::vector<std::string>(), lane, startPos, endPos, name),
    myChargeInTransit(chargeInTransit), myChargeType(stringToChargeType(chargeType)), myTotalPowerCheckEvent(nullptr) {
    if (chargingPower < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGINGPOWER), getID(), toString(chargingPower)))
    } else {
        myNominalChargingPower = chargingPower;
        myTotalChargingPower = totalPower;
    }
    if (dischargePower < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_DISCHARGEPOWER), getID(), toString(dischargePower)))
    } else {
        myNominalDischargePower = dischargePower;
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
    if (chargeRampUp < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGERAMPUP), getID(), toString(chargeRampUp)))
    } else {
        myChargeRampUp = chargeRampUp;
    }
    if (chargeRampDown < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGERAMPDOWN), getID(), toString(chargeRampDown)))
    } else {
        myChargeRampDown = chargeRampDown;
    }
    if (chargeStopDelay < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_CHARGESTOPDELAY), getID(), toString(chargeStopDelay)))
    } else {
        myChargeStopDelay = chargeStopDelay;
    }
    if (waitingTime < 0) {
        WRITE_WARNING(TLF("Attribute % for chargingStation with ID='%' is invalid (%).", toString(SUMO_ATTR_WAITINGTIME), getID(), toString(waitingTime)))
    } else {
        myWaitingTime = waitingTime;
    }
    if (getBeginLanePosition() > getEndLanePosition()) {
        WRITE_WARNING(TLF("ChargingStation with ID='%' doesn't have a valid position (% < %).", getID(), toString(getBeginLanePosition()), toString(getEndLanePosition())));
    }
}


MSChargingStation::MSChargingStation(const std::string& chargingStationID, const MSParkingArea* parkingArea, const std::string& name, double chargingPower,
                                     double totalPower, double dischargePower, double efficency, bool chargeInTransit, SUMOTime chargeDelay,
                                     const std::string& chargeType, SUMOTime waitingTime,
                                     double chargeRampUp, double chargeRampDown, SUMOTime chargeStopDelay) :
    MSChargingStation(chargingStationID, const_cast<MSLane&>(parkingArea->getLane()), parkingArea->getBeginLanePosition(), parkingArea->getEndLanePosition(),
                      name, chargingPower, totalPower, dischargePower, efficency, chargeInTransit, chargeDelay, chargeType, waitingTime,
                      chargeRampUp, chargeRampDown, chargeStopDelay) {
    myParkingArea = parkingArea;
}


MSChargingStation::~MSChargingStation() {
}


double
MSChargingStation::getChargingPower(bool usingFuel) const {
    if (usingFuel) {
        return myNominalChargingPower;
    } else {
        // Convert from [Ws] to [Wh] (3600s / 1h):
        return myNominalChargingPower / 3600;
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


MSChargingStation::ChargeType
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


double
MSChargingStation::getTotalChargingPower() const {
    return myTotalChargingPower;
}


void
MSChargingStation::setChargingPower(double chargingPower) {
    myNominalChargingPower = chargingPower;
}


void
MSChargingStation::setDischargePower(double dischargePower) {
    myNominalDischargePower = dischargePower;
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
MSChargingStation::setChargeRampUp(double chargeRampUp) {
    myChargeRampUp = chargeRampUp;
}


void
MSChargingStation::setChargeRampDown(double chargeRampDown) {
    myChargeRampDown = chargeRampDown;
}


void
MSChargingStation::setChargeStopDelay(SUMOTime chargeStopDelay) {
    myChargeStopDelay = chargeStopDelay;
}


std::pair<double, double>
MSChargingStation::stepSessionPower(const std::string& vehID, double targetPower, double dt) const {
    double prevPower = 0.;
    const auto it = mySessionState.find(vehID);
    if (it != mySessionState.end() && it->second.lastStep == SIMSTEP - DELTA_T) {
        prevPower = it->second.power;
    }
    if (dt <= 0. || prevPower == targetPower) {
        return std::make_pair(targetPower, targetPower);
    }

    double curPower = prevPower;
    double remDt = dt;
    double impulseWs = 0.;

    // If sign reverses across zero, first ramp magnitude down to 0 using myChargeRampDown
    if ((curPower > 0. && targetPower < 0.) || (curPower < 0. && targetPower > 0.)) {
        if (myChargeRampDown > 0.) {
            const double tZero = std::fabs(curPower) / myChargeRampDown;
            if (tZero >= remDt) {
                double endPower = curPower + (curPower > 0. ? -1. : 1.) * myChargeRampDown * remDt;
                if (std::fabs(endPower) < 1e-9) {
                    endPower = 0.;
                }
                impulseWs += 0.5 * (curPower + endPower) * remDt;
                return std::make_pair(impulseWs / dt, endPower);
            }
            impulseWs += 0.5 * curPower * tZero;
            remDt -= tZero;
        }
        curPower = 0.;
    }

    if (curPower == targetPower || remDt <= 0.) {
        impulseWs += curPower * remDt;
        return std::make_pair(impulseWs / dt, curPower);
    }

    const bool increasingMag = std::fabs(targetPower) > std::fabs(curPower);
    const double rate = increasingMag ? myChargeRampUp : myChargeRampDown;
    if (rate <= 0.) {
        impulseWs += targetPower * remDt;
        return std::make_pair(impulseWs / dt, targetPower);
    }

    const double diff = targetPower - curPower;
    const double tReach = std::fabs(diff) / rate;
    if (tReach >= remDt) {
        double endPower = curPower + (diff > 0. ? 1. : -1.) * rate * remDt;
        if (std::fabs(endPower) < 1e-9) {
            endPower = 0.;
        }
        impulseWs += 0.5 * (curPower + endPower) * remDt;
        return std::make_pair(impulseWs / dt, endPower);
    }
    impulseWs += 0.5 * (curPower + targetPower) * tReach + targetPower * (remDt - tReach);
    return std::make_pair(impulseWs / dt, targetPower);
}


void
MSChargingStation::setVehiclePower(const std::string& vehID, double power) {
    const SUMOTime now = SIMSTEP;
    for (auto it = mySessionState.begin(); it != mySessionState.end();) {
        if (it->first != vehID && it->second.lastStep < now - DELTA_T) {
            it = mySessionState.erase(it);
        } else {
            ++it;
        }
    }
    if (std::fabs(power) < 1e-9) {
        power = 0.;
    }
    mySessionState[vehID] = {power, now};
}


void
MSChargingStation::resetVehiclePower(const std::string& vehID) {
    mySessionState.erase(vehID);
}


double
MSChargingStation::getActualPower() const {
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    double sumPower = 0.;
    for (const auto& kv : mySessionState) {
        if (kv.second.lastStep == now || kv.second.lastStep == now - DELTA_T) {
            const SUMOVehicle* const veh = MSNet::getInstance()->getVehicleControl().getVehicle(kv.first);
            if (veh != nullptr && (veh->isOnRoad() || veh->isParking())) {
                sumPower += kv.second.power;
            }
        }
    }
    if (std::fabs(sumPower) < 1e-9) {
        sumPower = 0.;
    }
    return sumPower;
}


const std::string
MSChargingStation::getParameter(const std::string& key, const std::string defaultValue) const {
    if (key == toString(SUMO_ATTR_CHARGERAMPUP)) {
        return toString(myChargeRampUp);
    } else if (key == toString(SUMO_ATTR_CHARGERAMPDOWN)) {
        return toString(myChargeRampDown);
    } else if (key == toString(SUMO_ATTR_CHARGESTOPDELAY)) {
        return toString(STEPS2TIME(myChargeStopDelay));
    } else if (key == "actualPower") {
        return toString(getActualPower());
    }
    return MSStoppingPlace::getParameter(key, defaultValue);
}


void
MSChargingStation::setParameter(const std::string& key, const std::string& value) {
    if (key == toString(SUMO_ATTR_CHARGERAMPUP)) {
        const double rampUp = StringUtils::toDouble(value);
        if (rampUp < 0.) {
            throw InvalidArgument("Attribute " + key + " for chargingStation '" + getID() + "' must not be negative.");
        }
        setChargeRampUp(rampUp);
    } else if (key == toString(SUMO_ATTR_CHARGERAMPDOWN)) {
        const double rampDown = StringUtils::toDouble(value);
        if (rampDown < 0.) {
            throw InvalidArgument("Attribute " + key + " for chargingStation '" + getID() + "' must not be negative.");
        }
        setChargeRampDown(rampDown);
    } else if (key == toString(SUMO_ATTR_CHARGESTOPDELAY)) {
        const SUMOTime stopDelay = string2time(value);
        if (stopDelay < 0) {
            throw InvalidArgument("Attribute " + key + " for chargingStation '" + getID() + "' must not be negative.");
        }
        setChargeStopDelay(stopDelay);
    } else if (key == "actualPower") {
        throw InvalidArgument("Parameter 'actualPower' is read-only for chargingStation '" + getID() + "'.");
    } else {
        MSStoppingPlace::setParameter(key, value);
    }
}


void
MSChargingStation::setChargeInTransit(bool value) {
    myChargeInTransit = value;
    if (myTotalChargingPower > 0 && myChargeInTransit && myTotalPowerCheckEvent == nullptr) {
        myTotalPowerCheckEvent = new WrappingCommand<MSChargingStation>(this, &MSChargingStation::checkTotalPower);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myTotalPowerCheckEvent);
    }
}


void
MSChargingStation::setChargingVehicle(bool value) {
    myChargingVehicle = value;
    if (myTotalChargingPower > 0 && myChargingVehicle && myTotalPowerCheckEvent == nullptr) {
        myTotalPowerCheckEvent = new WrappingCommand<MSChargingStation>(this, &MSChargingStation::checkTotalPower);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myTotalPowerCheckEvent);
    }
}


void
MSChargingStation::setTotalChargingPower(double totalPower) {
    if (totalPower <= 0) {
        return;
    }
    const bool hadLimit = myTotalChargingPower > 0;
    myTotalChargingPower = totalPower;
    if (!hadLimit && (myChargeInTransit || myChargingVehicle) && myTotalPowerCheckEvent == nullptr) {
        myTotalPowerCheckEvent = new WrappingCommand<MSChargingStation>(this, &MSChargingStation::checkTotalPower);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myTotalPowerCheckEvent);
    }
}


SUMOTime
MSChargingStation::checkTotalPower(SUMOTime currentTime) {
    if (!myChargeInTransit && !myChargingVehicle) {
        myTotalPowerCheckEvent = nullptr;
        myChargedBatteries.clear();
        return 0;
    }
    double sumReqWh = 0;
    std::vector<Charge*> thisStepCharges;
    for (auto& kv : myChargeValues) {
        if (MSNet::getInstance()->getVehicleControl().getVehicle(kv.first) == nullptr) {
            continue;
        }
        Charge& lastcharge = kv.second.back();
        // discharging vehicles do not draw from the total charging power
        if (lastcharge.timeStep == currentTime && lastcharge.WDischarged == 0) {
            sumReqWh += lastcharge.WCharged;
            thisStepCharges.push_back(&lastcharge);
        }
    }
    if (thisStepCharges.size() < 2) {
        return DELTA_T;
    }
    const double capWh = myTotalChargingPower * myEfficiency /*W*/ * TS /*s*/ / 3600.0; // convert to Wh
#ifdef DEBUG_SIMSTEP
    std::cout << "checkTotalPower: CS="
              << this->myID << " currentTime=" << currentTime << " myTotalChargingPower=" << myTotalChargingPower;
    if (sumReqWh > capWh && sumReqWh > 0) {
        std::cout << " exceeded, needs rebalancing!";
    }
    std::cout << std::endl;
#endif
    if (sumReqWh > capWh && sumReqWh > 0) {
        const double ratio = capWh / sumReqWh;
        for (auto* charge : thisStepCharges) {
            MSDevice_Battery* battery = myChargedBatteries[charge->vehicleID];
            double abc = battery->getActualBatteryCapacity();

            const double deliveredWh = charge->WCharged * ratio;
            const double excessWh = charge->WCharged - deliveredWh;
            charge->WCharged = deliveredWh;
            if (charge->chargingEfficiency > 0 && TS > 0) {
                // derive power [W] from energy [Wh]: Power = (Energy [Wh] * 3600) [Ws] / (efficiency * TS [s])
                // ergo we are doing [Ws/s -> W]
                charge->chargingPower = (deliveredWh * 3600.0) / (charge->chargingEfficiency * TS);
            }
            charge->actualBatteryCapacity = abc - excessWh;
            charge->totalEnergyCharged -= excessWh;

            //  inform also battery device
            battery->setActualBatteryCapacity(abc - excessWh);
            battery->setEnergyCharged(deliveredWh);
            myTotalCharge -= excessWh;
            auto sIt = mySessionState.find(charge->vehicleID);
            if (sIt != mySessionState.end() && sIt->second.lastStep == currentTime && sIt->second.power > 0.) {
                sIt->second.power *= ratio;
            }

#ifdef DEBUG_SIMSTEP
            std::cout << "time=" << time2string(currentTime)
                      << " vehID=" << charge->vehicleID
                      << " requestedWh=" << (deliveredWh + excessWh)
                      << " deliveredWh=" << deliveredWh
                      << " deliveredW="  << charge->chargingPower
                      << " ratio=" << ratio << std::endl;
#endif
        }
#ifdef DEBUG_SIMSTEP
        std::cout << "===============\n\n";
#endif
    }
    return DELTA_T;
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
MSChargingStation::addChargeValueForOutput(double WCharged, MSDevice_Battery* battery, double WDischarged, double deliveredGridPower) {
    if (!OptionsCont::getOptions().isSet("chargingstations-output")) {
        return;
    }
    std::string status = "";
    if (battery->getChargingStartTime() > myChargeDelay) {
        if (WDischarged > 0) {
            status = "dischargingStopped";
        } else if (battery->getHolder().getSpeed() < battery->getStoppingThreshold()) {
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
    myTotalDischarge += WDischarged;
    // create charge row and insert it in myChargeValues
    const std::string vehID = battery->getHolder().getID();
    if (myChargeValues.count(vehID) == 0) {
        myChargedVehicles.push_back(vehID);
    }
    myChargedBatteries[vehID] = battery;
    const double reportedPower = (deliveredGridPower != INVALID_DOUBLE) ? deliveredGridPower : (WDischarged > 0 ? myNominalDischargePower : myNominalChargingPower);
    Charge C(MSNet::getInstance()->getCurrentTimeStep(), vehID, battery->getHolder().getVehicleType().getID(),
             status, WCharged, battery->getActualBatteryCapacity(), battery->getMaximumBatteryCapacity(),
             reportedPower, myEfficiency, myTotalCharge, WDischarged);
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
    const bool writeDischarge = writeDischargeOutput();
    if (writeDischarge) {
        output.writeAttr(SUMO_ATTR_TOTALENERGYDISCHARGED, myTotalDischarge);
    }
    output.writeAttr(SUMO_ATTR_CHARGINGSTEPS, chargingSteps);
    // start writing
    if (myChargeValues.size() > 0) {
        for (const std::string& vehID : myChargedVehicles) {
            int iStart = 0;
            const auto& chargeSteps = myChargeValues[vehID];
            while (iStart < (int)chargeSteps.size()) {
                int iEnd = iStart + 1;
                double charged = chargeSteps[iStart].WCharged;
                double discharged = chargeSteps[iStart].WDischarged;
                while (iEnd < (int)chargeSteps.size() && chargeSteps[iEnd].timeStep == chargeSteps[iEnd - 1].timeStep + DELTA_T) {
                    charged += chargeSteps[iEnd].WCharged;
                    discharged += chargeSteps[iEnd].WDischarged;
                    iEnd++;
                }
                writeVehicle(output, chargeSteps, iStart, iEnd, charged, discharged, writeDischarge);
                iStart = iEnd;
            }
        }
    }
    // close charging station tag
    output.closeTag();
}


void
MSChargingStation::writeAggregatedChargingStationOutput(OutputDevice& output, bool includeUnfinished) {
    std::vector<std::string> terminatedChargers;
    for (const auto& item : myChargeValues) {
        const Charge& lastCharge = item.second.back();
        // no charge during the last time step == has stopped charging
        bool finished = lastCharge.timeStep < SIMSTEP - DELTA_T;
        if (finished || includeUnfinished) {
            if (finished) {
                terminatedChargers.push_back(item.first);
            }
            // aggregate values
            double charged = 0.;
            double discharged = 0.;
            double minPower = lastCharge.chargingPower;
            double maxPower = lastCharge.chargingPower;
            double minCharge = lastCharge.WCharged;
            double maxCharge = lastCharge.WCharged;
            double minEfficiency = lastCharge.chargingEfficiency;
            double maxEfficiency = lastCharge.chargingEfficiency;
            for (const auto& charge : item.second) {
                charged += charge.WCharged;
                discharged += charge.WDischarged;
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
            if (writeDischargeOutput()) {
                output.writeAttr(SUMO_ATTR_TOTALENERGYDISCHARGED_VEHICLE, discharged);
            }
            output.writeAttr(SUMO_ATTR_CHARGINGBEGIN, time2string(item.second.at(0).timeStep));
            if (finished) {
                output.writeAttr(SUMO_ATTR_CHARGINGEND, time2string(lastCharge.timeStep));
            }
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
MSChargingStation::writeVehicle(OutputDevice& out, const std::vector<Charge>& chargeSteps, int iStart, int iEnd, double charged,
                                double discharged, bool writeDischarge) {
    const Charge& first = chargeSteps[iStart];
    out.openTag(SUMO_TAG_VEHICLE);
    out.writeAttr(SUMO_ATTR_ID, first.vehicleID);
    out.writeAttr(SUMO_ATTR_TYPE, first.vehicleType);
    out.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED_VEHICLE, charged);
    if (writeDischarge) {
        out.writeAttr(SUMO_ATTR_TOTALENERGYDISCHARGED_VEHICLE, discharged);
    }
    out.writeAttr(SUMO_ATTR_CHARGINGBEGIN, time2string(first.timeStep));
    out.writeAttr(SUMO_ATTR_CHARGINGEND, time2string(chargeSteps[iEnd - 1].timeStep));
    for (int i = iStart; i < iEnd; i++) {
        const Charge& c = chargeSteps[i];
        out.openTag(SUMO_TAG_STEP);
        out.writeAttr(SUMO_ATTR_TIME, time2string(c.timeStep));
        // charge values
        out.writeAttr(SUMO_ATTR_CHARGING_STATUS, c.status);
        out.writeAttr(SUMO_ATTR_ENERGYCHARGED, c.WCharged);
        if (writeDischarge) {
            out.writeAttr(SUMO_ATTR_ENERGYDISCHARGED, c.WDischarged);
        }
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
