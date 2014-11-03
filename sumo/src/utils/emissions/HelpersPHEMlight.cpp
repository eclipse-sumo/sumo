/****************************************************************************/
/// @file    HelpersPHEMlight.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Nikolaus Furian
/// @date    Sat, 20.04.2013
/// @version $Id$
///
// Helper methods for PHEMlight-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
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

#include "HelpersPHEMlight.h"
#include "PHEMCEPHandler.h"
#include "PHEMConstants.h"
#include <limits>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
HelpersPHEMlight::HelpersPHEMlight() : PollutantsInterface::Helper("PHEMlight"), myIndex(PHEMLIGHT_BASE) {
    myEmissionClassStrings.insert("zero", myIndex++);
}


SUMOEmissionClass
HelpersPHEMlight::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    if (eClass == "unknown" && !myEmissionClassStrings.hasString("unknown")) {
        myEmissionClassStrings.addAlias("unknown", getClassByName("PKW_G_EU4", vc));
    }
    if (myEmissionClassStrings.hasString(eClass)) {
        return myEmissionClassStrings.get(eClass);
    }
    if (eClass.size() < 8 || (eClass.find("_D_EU") == std::string::npos && eClass.find("_G_EU") == std::string::npos)) {
        return false;
    }
    int index = myIndex++;
    const std::string type = eClass.substr(0, 3);
    if (type == "LB_" || type == "RB_" || type == "LSZ" || eClass.find("LKW") != std::string::npos) {
        index |= PollutantsInterface::HEAVY_BIT;
    }
    myEmissionClassStrings.insert(eClass, index);
    if (!PHEMCEPHandler::getHandlerInstance().Load(index, eClass)) {
        myEmissionClassStrings.remove(eClass, index);
        myIndex--;
        throw InvalidArgument("File for PHEM emission class " + eClass + " not found.");
    }
    std::string eclower = eClass;
    std::transform(eclower.begin(), eclower.end(), eclower.begin(), tolower);
    myEmissionClassStrings.addAlias(eclower, index);
    return index;
}


SUMOReal
HelpersPHEMlight::getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) const {
    PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
    if (currCep == 0) {
        return -1.;
    }
    return currCep->GetMaxAccel(v, a, slope);
}


SUMOEmissionClass
HelpersPHEMlight::getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight) const {
    std::string eClassOffset = "0";
    if (eClass.length() == 5 && eClass.substr(0, 4) == "Euro") {
        if (eClass[4] >= '0' && eClass[4] <= '6') {
            eClassOffset = eClass.substr(4, 1);
        }
    }
    std::string desc;
    if (vClass == "Passenger") {
        desc = "PKW_";
        if (fuel == "Gasoline") {
            desc += "G_";
        } else if (fuel == "Diesel") {
            desc += "D_";
        } else if (fuel == "HybridGasoline") {
            desc = "H_" + desc + "G_";
        } else if (fuel == "HybridDiesel") {
            desc = "H_" + desc + "G_";
        }
        desc += "EU" + eClassOffset;
    } else if (vClass == "Moped") {
        desc = "KKR_G_EU" + eClassOffset;
    } else if (vClass == "Motorcycle") {
        desc = "MR_G_EU" + eClassOffset;
        if (fuel == "Gasoline2S") {
            desc += "_2T";
        } else {
            desc += "_4T";
        }
    } else if (vClass == "Delivery") {
        desc = "LNF_";
        if (fuel == "Gasoline") {
            desc += "G_";
        } else if (fuel == "Diesel") {
            desc += "D_";
        }
        desc += "EU" + eClassOffset + "_I";
        if (weight > 1305.) {
            desc += "I";
            if (weight > 1760.) {
                desc += "I";
            }
        }
    } else if (vClass == "UrbanBus") {
        desc = "LB_D_EU" + eClassOffset;
    } else if (vClass == "Coach") {
        desc = "RB_D_EU" + eClassOffset;
    } else if (vClass == "Truck") {
        desc = "Solo_LKW_D_EU" + eClassOffset + "_I";
        if (weight > 1305.) {
            desc += "I";
        }
    } else if (vClass == "Trailer") {
        desc = "LSZ_D_EU" + eClassOffset;
    }
    if (myEmissionClassStrings.hasString(desc)) {
        return myEmissionClassStrings.get(desc);
    }
    return base;
}


std::string
HelpersPHEMlight::getAmitranVehicleClass(const SUMOEmissionClass c) const {
    const std::string name = myEmissionClassStrings.getString(c);
    if (name.find("KKR_") != std::string::npos) {
        return "Moped";
    } else if (name.find("RB_") != std::string::npos) {
        return "Coach";
    } else if (name.find("LB_") != std::string::npos) {
        return "UrbanBus";
    } else if (name.find("LNF_") != std::string::npos) {
        return "Delivery";
    } else if (name.find("LSZ_") != std::string::npos) {
        return "Trailer";
    } else if (name.find("MR_") != std::string::npos) {
        return "Motorcycle";
    } else if (name.find("LKW_") != std::string::npos) {
        return "Truck";
    }
    return "Passenger";
}


std::string
HelpersPHEMlight::getFuel(const SUMOEmissionClass c) const {
    const std::string name = myEmissionClassStrings.getString(c);
    std::string fuel = "Gasoline";
    if (name.find("_D_") != std::string::npos) {
        fuel = "Diesel";
    }
    if (name.find("H_") != std::string::npos) {
        fuel = "Hybrid" + fuel;
    }
    return fuel;
}


int
HelpersPHEMlight::getEuroClass(const SUMOEmissionClass c) const {
    const std::string name = myEmissionClassStrings.getString(c);
    if (name.find("_EU1") != std::string::npos) {
        return 1;
    } else if (name.find("_EU2") != std::string::npos) {
        return 2;
    } else if (name.find("_EU3") != std::string::npos) {
        return 3;
    } else if (name.find("_EU4") != std::string::npos) {
        return 4;
    } else if (name.find("_EU5") != std::string::npos) {
        return 5;
    } else if (name.find("_EU6") != std::string::npos) {
        return 6;
    }
    return 0;
}


SUMOReal
HelpersPHEMlight::getWeight(const SUMOEmissionClass c) const {
    const std::string name = myEmissionClassStrings.getString(c);
    if (name.find("LNF_") != std::string::npos) {
        if (name.find("_III") != std::string::npos) {
            return 2630.;
        } else if (name.find("_II") != std::string::npos) {
            return 1532.;
        } else if (name.find("_I") != std::string::npos) {
            return 652.;
        }
    }
    if (name.find("Solo_LKW_") != std::string::npos) {
        if (name.find("_II") != std::string::npos) {
            return 8398.;
        } else if (name.find("_I") != std::string::npos) {
            return 18702.;
        }
    }
    return -1.;
}


SUMOReal
HelpersPHEMlight::compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope) const {
    const PHEMCEP* const currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
    if (currCep == 0) {
        return 0.;
    }
    const double power = currCep->CalcPower(v, a, slope);
    switch (e) {
        case PollutantsInterface::CO:
            return currCep->GetEmission("CO", power) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::CO2:
            return currCep->GetEmission("FC", power) * 3.15 / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::HC:
            return currCep->GetEmission("HC", power) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::NO_X:
            return currCep->GetEmission("NOx", power) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::PM_X:
            return currCep->GetEmission("PM", power) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::FUEL: {
            std::string fuelType = currCep->GetVehicleFuelType();
            if (fuelType == "D") { // divide by average diesel density of 836 g/l
                return currCep->GetEmission("FC", power) / 836. / SECONDS_PER_HOUR * 1000.;
            } else if (fuelType == "G") { // divide by average gasoline density of 742 g/l
                return currCep->GetEmission("FC", power) / 742. / SECONDS_PER_HOUR * 1000.;
            } else {
                return currCep->GetEmission("FC", power) / SECONDS_PER_HOUR * 1000.; // surely false, but at least not additionally modified
            }
        }
    }
    // should never get here
    return 0.;
}


/****************************************************************************/

