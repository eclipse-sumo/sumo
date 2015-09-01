/****************************************************************************/
/// @file    MSChrgStn_h.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurzveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id$       //REVISION NEEDED
///
// Chargin Station for Electric vehicles //REVISION NEEDED
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSStoppingPlace.h>
#include "MSChrgStn.h"
#include "MSTrigger.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member method definitions
// ===========================================================================

MSChrgStn::MSChrgStn(const std::string& new_ChrgStnID,
                     const std::vector<std::string>& new_lines,
                     MSLane& new_Lane, SUMOReal new_StartPos, SUMOReal new_EndPos, SUMOReal new_ChrgPower,
                     SUMOReal new_Efficency, SUMOReal new_ChargeInTransit, SUMOReal new_ChargeDelay) :
    MSStoppingPlace(new_ChrgStnID, new_lines, new_Lane, new_StartPos, new_EndPos),
    ChrgPower(new_ChrgPower),
    Efficency(new_Efficency),
    ChargeInTransit(new_ChargeInTransit),
    ChargeDelay(new_ChargeDelay) {
    if (getChrgPower() < 0) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid charging power(" + SUMOReal_str(getChrgPower()) + ").");
    }

    if (getEfficency() < 0 || getEfficency() > 1) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid Efficency (" + SUMOReal_str(getEfficency()) + ").");
    }

    if (getBeginLanePosition() > getEndLanePosition()) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid range (" + SUMOReal_str(getBeginLanePosition()) + " < " + SUMOReal_str(getEndLanePosition()) + ").");
    }
}


MSChrgStn::~MSChrgStn()
{}


// GET FUNCTIONS


SUMOReal
MSChrgStn::getChrgPower() const {
    return ChrgPower;
}

SUMOReal
MSChrgStn::getEfficency() const {
    return Efficency;
}

SUMOReal
MSChrgStn::getChargeInTransit() const {
    return ChargeInTransit;
}

SUMOReal
MSChrgStn::getChargeDelay() const {
    return ChargeDelay;
}


// SET FUNCTIONS

void MSChrgStn::setChrgPower(SUMOReal new_ChrgPower) {
    ChrgPower = new_ChrgPower;

    if (new_ChrgPower < 0) {
        std::cout << std::string("Warning: Chargin Station with ID = " + getID() + " doesn't have a valid charging power(" + SUMOReal_str(getChrgPower()) + ").");
    }
}


void MSChrgStn::setEfficency(SUMOReal new_Efficency) {
    if (new_Efficency < 0 || new_Efficency > 1) {
        std::cout << std::string("Warning: Chargin Station with ID = " + getID() + " doesn't have a valid Efficency (" + SUMOReal_str(getEfficency()) + ").");
    }

    Efficency = new_Efficency;
}

bool MSChrgStn::vehicleIsInside(const SUMOReal position) {
    if ((position >= getBeginLanePosition()) && (position <= getEndLanePosition())) {
        return true;
    } else {
        return false;
    }
}

// Private methods

std::string MSChrgStn::SUMOReal_str(const SUMOReal& var) {
    std::ostringstream convert;
    convert << var;
    return convert.str();
}
