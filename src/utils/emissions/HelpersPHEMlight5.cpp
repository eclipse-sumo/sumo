/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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


std::string
HelpersPHEMlight5::getFuel(const SUMOEmissionClass c) const {
    const std::string name = myEmissionClassStrings.getString(c);
    std::string fuel = "Gasoline";
    if (name.find("_D_") != std::string::npos) {
        fuel = "Diesel";
    }
    if (name.find("_BEV_") != std::string::npos) {
        fuel = "Electricity";
    }
    if (name.find("_HEV") != std::string::npos) {
        fuel = "Hybrid" + fuel;
    }
    return fuel;
}


double
HelpersPHEMlight5::getEmission(PHEMlightdllV5::CEP* currCep, const std::string& e, const double p, const double v, const double drivingPower) const {
    return currCep->GetEmission(e, p, v, &myHelper, drivingPower);
}


double
HelpersPHEMlight5::calcPower(PHEMlightdllV5::CEP* currCep, const double v, const double a, const double slope, const EnergyParams* param) const {
    // copy of CEP::CalcPower
    const double power = calcWheelPower(currCep, v, a, slope, param) / PHEMlightdllV5::Constants::_DRIVE_TRAIN_EFFICIENCY;
    if (!(currCep->getCalcType() == "HEV" || currCep->getCalcType() == "BEV")) {
        return power + param->getDoubleOptional(SUMO_ATTR_CONSTANTPOWERINTAKE, currCep->getAuxPower() * 1000.) / 1000.;
    }
    return power;
}


double
HelpersPHEMlight5::calcWheelPower(PHEMlightdllV5::CEP* currCep, const double v, const double a, const double slope, const EnergyParams* param) const {
    // copy of CEP::CalcWheelPower
    const double rotFactor = currCep->GetRotationalCoeffecient(v);
    const double mass = param->getDoubleOptional(SUMO_ATTR_MASS, currCep->getVehicleMass());
    const double massRot = currCep->getVehicleMassRot();
    const double load = param->getDoubleOptional(SUMO_ATTR_LOADING, currCep->getVehicleLoading());
    const double cw = param->getDoubleOptional(SUMO_ATTR_FRONTSURFACEAREA, currCep->getCrossSectionalArea()) * param->getDoubleOptional(SUMO_ATTR_AIRDRAGCOEFFICIENT, currCep->getCWValue());

    double power = (mass + load) * PHEMlightdllV5::Constants::GRAVITY_CONST * currCep->getResistance(v) * v;
    power += (cw * PHEMlightdllV5::Constants::AIR_DENSITY_CONST / 2) * std::pow(v, 3);
    power += (mass * rotFactor + massRot + load) * a * v;
    power += (mass + load) * PHEMlightdllV5::Constants::GRAVITY_CONST * slope * 0.01 * v;
    return power / 1000.;
}


double
HelpersPHEMlight5::getModifiedAccel(const SUMOEmissionClass c, const double v, const double a, const double slope, const EnergyParams* param) const {
    PHEMlightdllV5::CEP* currCep = myCEPs.count(c) == 0 ? nullptr : myCEPs.find(c)->second;
    if (currCep != nullptr) {
        if (v == 0.) {
            return 0.;
        }
        // this is a copy of CEP::GetMaxAccel
        const double rotFactor = currCep->GetRotationalCoeffecient(v);
        const double mass = param->getDoubleOptional(SUMO_ATTR_MASS, currCep->getVehicleMass());
        const double massRot = currCep->getVehicleMassRot();
        const double load = param->getDoubleOptional(SUMO_ATTR_LOADING, currCep->getVehicleLoading());
        const double pMaxForAcc = currCep->GetPMaxNorm(v) * currCep->getRatedPower() - calcPower(currCep, v, 0, slope, param);
        const double maxAcc = (pMaxForAcc * 1000) / ((mass * rotFactor + massRot + load) * v);
        return MIN2(a, maxAcc);
    }
    return a;
}


double
HelpersPHEMlight5::getCoastingDecel(const SUMOEmissionClass c, const double v, const double a, const double slope, const EnergyParams* param) const {
    PHEMlightdllV5::CEP* const currCep = myCEPs.find(c)->second;
    // this is a copy of CEP::GetDecelCoast
    if (v < PHEMlightdllV5::Constants::SPEED_DCEL_MIN) {
        return v / PHEMlightdllV5::Constants::SPEED_DCEL_MIN * getCoastingDecel(c, PHEMlightdllV5::Constants::SPEED_DCEL_MIN, a, slope, param);
    }
    const double rotFactor = currCep->GetRotationalCoeffecient(v);
    const double mass = param->getDoubleOptional(SUMO_ATTR_MASS, currCep->getVehicleMass());
    const double load = param->getDoubleOptional(SUMO_ATTR_LOADING, currCep->getVehicleLoading());
    const double cw = param->getDoubleOptional(SUMO_ATTR_FRONTSURFACEAREA, currCep->getCrossSectionalArea()) * param->getDoubleOptional(SUMO_ATTR_AIRDRAGCOEFFICIENT, currCep->getCWValue());

    const double fRoll = currCep->getResistance(v, true) * (mass + load) * PHEMlightdllV5::Constants::GRAVITY_CONST;
    const double fAir = cw * PHEMlightdllV5::Constants::AIR_DENSITY_CONST * 0.5 * std::pow(v, 2);
    const double fGrad = (mass + load) * PHEMlightdllV5::Constants::GRAVITY_CONST * slope / 100;

    return -(currCep->getFMot(v) + fRoll + fAir + fGrad) / ((mass + load) * rotFactor);
}


double
HelpersPHEMlight5::compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const {
    if (param != nullptr && param->isEngineOff()) {
        return 0.;
    }
    const double corrSpeed = MAX2(0.0, v);
    assert(myCEPs.count(c) == 1);
    PHEMlightdllV5::CEP* const currCep = myCEPs.find(c)->second;
    const double corrAcc = getModifiedAccel(c, corrSpeed, a, slope, param);
    const bool isBEV = currCep->getFuelType() == PHEMlightdllV5::Constants::strBEV;
    const bool isHybrid = currCep->getCalcType() == PHEMlightdllV5::Constants::strHybrid;
    const double power_raw = calcPower(currCep, corrSpeed, corrAcc, slope, param);
    const double power = isHybrid ? calcWheelPower(currCep, corrSpeed, corrAcc, slope, param) : currCep->CalcEngPower(power_raw);

    if (!isBEV && corrAcc < getCoastingDecel(c, corrSpeed, corrAcc, slope, param) &&
            corrSpeed > PHEMlightdllV5::Constants::ZERO_SPEED_ACCURACY) {
        return 0.;
    }
    // TODO: this is probably only needed for non-heavy vehicles, so if execution speed becomes an issue
    const double drivingPower = calcPower(currCep, PHEMlightdllV5::Constants::NORMALIZING_SPEED, PHEMlightdllV5::Constants::NORMALIZING_ACCELARATION, 0, param);
    switch (e) {
        case PollutantsInterface::CO:
            return getEmission(currCep, "CO", power, corrSpeed, drivingPower) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::CO2:
            return currCep->GetCO2Emission(getEmission(currCep, "FC", power, corrSpeed, drivingPower),
                                           getEmission(currCep, "CO", power, corrSpeed, drivingPower),
                                           getEmission(currCep, "HC", power, corrSpeed, drivingPower), &myHelper) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::HC:
            return getEmission(currCep, "HC", power, corrSpeed, drivingPower) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::NO_X:
            return getEmission(currCep, "NOx", power, corrSpeed, drivingPower) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::PM_X:
            return getEmission(currCep, "PM", power, corrSpeed, drivingPower) / SECONDS_PER_HOUR * 1000.;
        case PollutantsInterface::FUEL: {
            if (myVolumetricFuel && currCep->getFuelType() == PHEMlightdllV5::Constants::strDiesel) { // divide by average diesel density of 836 g/l
                return getEmission(currCep, "FC", power, corrSpeed, drivingPower) / 836. / SECONDS_PER_HOUR * 1000.;
            }
            if (myVolumetricFuel && currCep->getFuelType() == PHEMlightdllV5::Constants::strGasoline) { // divide by average gasoline density of 742 g/l
                return getEmission(currCep, "FC", power, corrSpeed, drivingPower) / 742. / SECONDS_PER_HOUR * 1000.;
            }
            if (isBEV) {
                return 0.;
            }
            return getEmission(currCep, "FC", power, corrSpeed, drivingPower) / SECONDS_PER_HOUR * 1000.; // still in mg even if myVolumetricFuel is set!
        }
        case PollutantsInterface::ELEC:
            if (isBEV) {
                const double auxPower = param->getDoubleOptional(SUMO_ATTR_CONSTANTPOWERINTAKE, currCep->getAuxPower() * 1000.) / 1000.;
                return (getEmission(currCep, "FC_el", power, corrSpeed, drivingPower) + auxPower) / SECONDS_PER_HOUR * 1000.;
            }
            return 0;
    }
    // should never get here
    return 0.;
}


/****************************************************************************/
