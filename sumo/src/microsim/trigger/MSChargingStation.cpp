/****************************************************************************/
/// @file    MSChargingStation.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id$
///
// Chargin Station for Electric vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/common/TplConvert.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSStoppingPlace.h>
#include "MSChargingStation.h"
#include "MSTrigger.h"


// ===========================================================================
// member method definitions
// ===========================================================================

MSChargingStation::MSChargingStation(const std::string& chargingStationID, MSLane& lane, double startPos, double endPos,
    double chargingPower, double efficency, bool chargeInTransit, int chargeDelay) :
    MSStoppingPlace(chargingStationID, std::vector<std::string>(), lane, startPos, endPos),
    myChargingPower(0),
    myEfficiency(0),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(0),
    myChargingVehicle(false) {
    if (chargingPower < 0)
        WRITE_WARNING("Parameter " + toString(SUMO_ATTR_CHARGINGPOWER) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " is invalid (" + toString(getChargingPower()) + ").")
        else {
            myChargingPower = chargingPower;
        }

    if (efficency < 0 || efficency > 1) {
        WRITE_WARNING("Parameter " + toString(SUMO_ATTR_EFFICIENCY) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " is invalid (" + toString(getEfficency()) + ").")
    } else {
        myEfficiency = efficency;
    }

    if (chargeDelay < 0) {
        WRITE_WARNING("Parameter " + toString(SUMO_ATTR_CHARGEDELAY) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " is invalid (" + toString(getEfficency()) + ").")
    } else {
        myChargeDelay = chargeDelay;
    }

    if (getBeginLanePosition() > getEndLanePosition()) {
        WRITE_WARNING(toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " doesn't have a valid range (" + toString(getBeginLanePosition()) + " < " + toString(getEndLanePosition()) + ").");
    }
}


MSChargingStation::~MSChargingStation() {
}


double
MSChargingStation::getChargingPower() const {
    return myChargingPower;
}


double
MSChargingStation::getEfficency() const {
    return myEfficiency;
}


bool
MSChargingStation::getChargeInTransit() const {
    return myChargeInTransit;
}


double
MSChargingStation::getChargeDelay() const {
    return myChargeDelay;
}


void
MSChargingStation::setChargingPower(double chargingPower) {
    if (chargingPower < 0) {
        WRITE_WARNING("New " + toString(SUMO_ATTR_CHARGINGPOWER) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " isn't valid (" + toString(chargingPower) + ").")
    } else {
        myChargingPower = chargingPower;
    }
}


void
MSChargingStation::setEfficency(double efficency) {
    if (efficency < 0 || efficency > 1) {
        WRITE_WARNING("New " + toString(SUMO_ATTR_EFFICIENCY) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " isn't valid (" + toString(efficency) + ").")
    } else {
        myEfficiency = efficency;
    }
}


void
MSChargingStation::setChargeInTransit(bool chargeInTransit) {
    myChargeInTransit = chargeInTransit;
}


void
MSChargingStation::setChargeDelay(int chargeDelay) {
    if (chargeDelay < 0) {
        WRITE_WARNING("New " + toString(SUMO_ATTR_CHARGEDELAY) + " for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = " + getID() + " isn't valid (" + toString(chargeDelay) + ").")
    } else {
        myChargeDelay = chargeDelay;
    }
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
MSChargingStation::writeChargingStationOutput(OutputDevice& output) {
    output.openTag(SUMO_TAG_CHARGING_STATION);
    output.writeAttr(SUMO_ATTR_ID, myID);
    output.closeTag();
}

/****************************************************************************/
