/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    HelpersPHEMlight5.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Nikolaus Furian
/// @date    Sat, 20.04.2013
///
// Helper methods for PHEMlight-based emission computation
/****************************************************************************/
#include <config.h>

#include <limits>
#include <cmath>
#include <foreign/PHEMlight/V5/cpp/Constants.h>
#include <foreign/PHEMlight/V5/cpp/Correction.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>

#include "EnergyParams.h"
#include "HelpersPHEMlight5.h"


// ===========================================================================
// method definitions
// ===========================================================================
HelpersPHEMlight5::HelpersPHEMlight5() :
    HelpersPHEMlight("PHEMlight5", PHEMLIGHT5_BASE, -1),
    myIndex(PHEMLIGHT5_BASE) {
}


HelpersPHEMlight5::~HelpersPHEMlight5() {
    for (const auto& cep : myCEPs) {
        delete cep.second;
    }
}


SUMOEmissionClass
HelpersPHEMlight5::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    if (eClass == "unknown" && !myEmissionClassStrings.hasString("unknown")) {
        myEmissionClassStrings.addAlias("unknown", getClassByName("PC_EU4_G", vc));
    }
    if (eClass == "default" && !myEmissionClassStrings.hasString("default")) {
        myEmissionClassStrings.addAlias("default", getClassByName("PC_EU4_G", vc));
    }
    if (myEmissionClassStrings.hasString(eClass)) {
        return myEmissionClassStrings.get(eClass);
    }
    if (eClass.size() < 6) {
        throw InvalidArgument("Unknown emission class '" + eClass + "'.");
    }
    const OptionsCont& oc = OptionsCont::getOptions();
    myVolumetricFuel = oc.getBool("emissions.volumetric-fuel");
    std::vector<std::string> phemPath;
    phemPath.push_back(oc.getString("phemlight-path") + "/");
    if (getenv("PHEMLIGHT_PATH") != nullptr) {
        phemPath.push_back(std::string(getenv("PHEMLIGHT_PATH")) + "/");
    }
    if (getenv("SUMO_HOME") != nullptr) {
        phemPath.push_back(std::string(getenv("SUMO_HOME")) + "/data/emissions/PHEMlight5/");
    }
    if (myCorrection == nullptr && (!oc.isDefault("phemlight-year") || !oc.isDefault("phemlight-temperature"))) {
        myCorrection = new PHEMlightdllV5::Correction(phemPath);
        if (!oc.isDefault("phemlight-year")) {
            myCorrection->setYear(oc.getInt("phemlight-year"));
            std::string err;
            if (!myCorrection->ReadDet(err)) {
                throw InvalidArgument("Error reading PHEMlight5 deterioration data.\n" + err);
            }
            myCorrection->setUseDet(true);
        }
        if (!oc.isDefault("phemlight-temperature")) {
            myCorrection->setAmbTemp(oc.getFloat("phemlight-temperature"));
            std::string err;
            if (!myCorrection->ReadTNOx(err)) {
                throw InvalidArgument("Error reading PHEMlight5 deterioration data.\n" + err);
            }
            myCorrection->setUseTNOx(true);
        }
    }
    myHelper.setCommentPrefix("c");
    myHelper.setPHEMDataV("V5");
    myHelper.setclass(eClass);
    if (!myCEPHandler.GetCEP(phemPath, &myHelper, myCorrection)) {
        throw InvalidArgument("File for PHEMlight5 emission class " + eClass + " not found.\n" + myHelper.getErrMsg());
    }
    PHEMlightdllV5::CEP* const currCep = myCEPHandler.getCEPS().find(myHelper.getgClass())->second;
    int index = myIndex++;
    if (currCep->getHeavyVehicle()) {
        index |= PollutantsInterface::HEAVY_BIT;
    }
    myEmissionClassStrings.insert(eClass, index);
    myCEPs[index] = currCep;
    myEmissionClassStrings.addAlias(StringUtils::to_lower_case(eClass), index);
    return index;
}


double
HelpersPHEMlight5::getEmission(PHEMlightdllV5::CEP* currCep, const std::string& e, const double p, const double v) const {
    return currCep->GetEmission(e, p, v, &myHelper);
}


double
HelpersPHEMlight5::getModifiedAccel(const SUMOEmissionClass c, const double v, const double a, const double slope) const {
    PHEMlightdllV5::CEP* currCep = myCEPs.count(c) == 0 ? nullptr : myCEPs.find(c)->second;
    if (currCep != nullptr) {
        const bool isHBEV = currCep->getFuelType() == PHEMlightdllV5::Constants::strBEV || currCep->getFuelType() == PHEMlightdllV5::Constants::strHybrid;
        return v == 0.0 ? 0.0 : MIN2(a, currCep->GetMaxAccel(v, slope, isHBEV));
    }
    return a;
}


double
HelpersPHEMlight5::getCoastingDecel(const SUMOEmissionClass c, const double v, const double a, const double slope, const EnergyParams* /* param */) const {
    return myCEPs.find(c)->second->GetDecelCoast(v, a, slope);
}


double
HelpersPHEMlight5::compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const {
    if (param != nullptr && param->isEngineOff()) {
        return 0.;
    }
    const double corrSpeed = MAX2(0.0, v);
    assert(myCEPs.count(c) == 1);
    PHEMlightdllV5::CEP* const currCep = myCEPs.find(c)->second;
    const double corrAcc = getModifiedAccel(c, corrSpeed, a, slope);
    const bool isBEV = currCep->getFuelType() == PHEMlightdllV5::Constants::strBEV;
    const bool isHybrid = currCep->getFuelType() == PHEMlightdllV5::Constants::strHybrid;
    const double power_raw = currCep->CalcPower(corrSpeed, corrAcc, slope, isBEV || isHybrid);
    const double power = isHybrid ? currCep->CalcWheelPower(corrSpeed, corrAcc, slope) : currCep->CalcEngPower(power_raw);

    if (!isBEV && corrAcc < currCep->GetDecelCoast(corrSpeed, corrAcc, slope) &&
            corrSpeed > PHEMlightdllV5::Constants::ZERO_SPEED_ACCURACY) {
        return 0.;
    }
    const std::string& fuelType = currCep->getFuelType();
    switch (e) {
        case PollutantsInterface::CO:
            return getEmission(currCep, "CO", power, corrSpeed) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::CO2:
            return currCep->GetCO2Emission(getEmission(currCep, "FC", power, corrSpeed),
                                           getEmission(currCep, "CO", power, corrSpeed),
                                           getEmission(currCep, "HC", power, corrSpeed), &myHelper) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::HC:
            return getEmission(currCep, "HC", power, corrSpeed) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::NO_X:
            return getEmission(currCep, "NOx", power, corrSpeed) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::PM_X:
            return getEmission(currCep, "PM", power, corrSpeed) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::FUEL: {
            if (myVolumetricFuel && fuelType == PHEMlightdllV5::Constants::strDiesel) { // divide by average diesel density of 836 g/l
                return getEmission(currCep, "FC", power, corrSpeed) / 836. / SECONDS_PER_HOUR * 1000.;
            }
            if (myVolumetricFuel && fuelType == PHEMlightdllV5::Constants::strGasoline) { // divide by average gasoline density of 742 g/l
                return getEmission(currCep, "FC", power, corrSpeed) / 742. / SECONDS_PER_HOUR * 1000.;
            }
            if (fuelType == PHEMlightdllV5::Constants::strBEV) {
                return 0.;
            }
            return getEmission(currCep, "FC", power, corrSpeed) / SECONDS_PER_HOUR * 1000.; // still in mg even if myVolumetricFuel is set!
        }
        case PollutantsInterface::ELEC:
            if (fuelType == PHEMlightdllV5::Constants::strBEV) {
                return (getEmission(currCep, "FC_el", power, corrSpeed) + currCep->getAuxPower()) / SECONDS_PER_HOUR * 1000.;
            }
            return 0;
    }
    // should never get here
    return 0.;
}


/****************************************************************************/
