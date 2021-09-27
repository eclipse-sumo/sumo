/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2021 German Aerospace Center (DLR) and others.
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
/// @file    PollutantsInterface.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 19.08.2013
///
// Interface to capsulate different emission models
/****************************************************************************/
#include <config.h>

#include <limits>
#include <cmath>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>

#include "HelpersHBEFA.h"
#include "HelpersHBEFA3.h"
#include "HelpersPHEMlight.h"
#include "HelpersEnergy.h"
#include "PollutantsInterface.h"


// ===========================================================================
// static definitions
// ===========================================================================

PollutantsInterface::Helper PollutantsInterface::myZeroHelper("Zero", PollutantsInterface::ZERO_EMISSIONS, PollutantsInterface::ZERO_EMISSIONS);
HelpersHBEFA PollutantsInterface::myHBEFA2Helper;
HelpersHBEFA3 PollutantsInterface::myHBEFA3Helper;
HelpersPHEMlight PollutantsInterface::myPHEMlightHelper;
HelpersEnergy PollutantsInterface::myEnergyHelper;
PollutantsInterface::Helper* PollutantsInterface::myHelpers[] = {
    &PollutantsInterface::myZeroHelper,
    &PollutantsInterface::myHBEFA2Helper, &PollutantsInterface::myHBEFA3Helper,
    &PollutantsInterface::myPHEMlightHelper, &PollutantsInterface::myEnergyHelper
};
std::vector<std::string> PollutantsInterface::myAllClassesStr;

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// PollutantsInterface::Emissions - methods
// ---------------------------------------------------------------------------

PollutantsInterface::Emissions::Emissions(double co2, double co, double hc, double f, double nox, double pmx, double elec) :
    CO2(co2),
    CO(co),
    HC(hc),
    fuel(f),
    NOx(nox),
    PMx(pmx),
    electricity(elec) {
}


void PollutantsInterface::Emissions::addScaled(const Emissions& a, const double scale) {
    CO2 += scale * a.CO2;
    CO += scale * a.CO;
    HC += scale * a.HC;
    fuel += scale * a.fuel;
    NOx += scale * a.NOx;
    PMx += scale * a.PMx;
    electricity += scale * a.electricity;
}

// ---------------------------------------------------------------------------
// PollutantsInterface::Helper - methods
// ---------------------------------------------------------------------------

PollutantsInterface::Helper::Helper(std::string name, const int baseIndex, const int defaultClass) :
    myName(name),
    myBaseIndex(baseIndex) {
    if (defaultClass != -1) {
        myEmissionClassStrings.insert("default", defaultClass);
        myEmissionClassStrings.addAlias("unknown", defaultClass);
    }
}


const
std::string& PollutantsInterface::Helper::getName() const {
    return myName;
}


SUMOEmissionClass
PollutantsInterface::Helper::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    UNUSED_PARAMETER(vc);
    if (myEmissionClassStrings.hasString(eClass)) {
        return myEmissionClassStrings.get(eClass);
    }
    return myEmissionClassStrings.get(StringUtils::to_lower_case(eClass));
}


const std::string
PollutantsInterface::Helper::getClassName(const SUMOEmissionClass c) const {
    return myName + "/" + myEmissionClassStrings.getString(c);
}


bool
PollutantsInterface::Helper::isSilent(const SUMOEmissionClass c) {
    return (c & 0xffffffff & ~HEAVY_BIT) == 0;
}


SUMOEmissionClass
PollutantsInterface::Helper::getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight) const {
    UNUSED_PARAMETER(vClass);
    UNUSED_PARAMETER(fuel);
    UNUSED_PARAMETER(eClass);
    UNUSED_PARAMETER(weight);
    return base;
}


std::string
PollutantsInterface::Helper::getAmitranVehicleClass(const SUMOEmissionClass c) const {
    UNUSED_PARAMETER(c);
    return "Passenger";
}


std::string
PollutantsInterface::Helper::getFuel(const SUMOEmissionClass c) const {
    UNUSED_PARAMETER(c);
    return "Gasoline";
}


int
PollutantsInterface::Helper::getEuroClass(const SUMOEmissionClass c) const {
    UNUSED_PARAMETER(c);
    return 0;
}


double
PollutantsInterface::Helper::getWeight(const SUMOEmissionClass c) const {
    UNUSED_PARAMETER(c);
    return -1.;
}


double
PollutantsInterface::Helper::compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope, const std::map<int, double>* param) const {
    UNUSED_PARAMETER(c);
    UNUSED_PARAMETER(e);
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(a);
    UNUSED_PARAMETER(slope);
    UNUSED_PARAMETER(param);
    return 0.;
}


double
PollutantsInterface::Helper::getModifiedAccel(const SUMOEmissionClass c, const double v, const double a, const double slope) const {
    UNUSED_PARAMETER(c);
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(slope);
    return a;
}


void
PollutantsInterface::Helper::addAllClassesInto(std::vector<SUMOEmissionClass>& list) const {
    myEmissionClassStrings.addKeysInto(list);
}


bool
PollutantsInterface::Helper::includesClass(const SUMOEmissionClass c) const {
    return (c >> 16) == (myBaseIndex >> 16);
}

// ---------------------------------------------------------------------------
// PollutantsInterface - methods
// ---------------------------------------------------------------------------

SUMOEmissionClass
PollutantsInterface::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    const std::string::size_type sep = eClass.find("/");
    const std::string model = eClass.substr(0, sep); // this includes the case of no separator
    for (int i = 0; i < 5; i++) {
        if (myHelpers[i]->getName() == model) {
            if (sep != std::string::npos) {
                const std::string subClass = eClass.substr(sep + 1);
                if (subClass == "zero") {
                    return myZeroHelper.getClassByName("default", vc);
                }
                return myHelpers[i]->getClassByName(subClass, vc);
            }
            return myHelpers[i]->getClassByName("default", vc);
        }
    }
    if (sep == std::string::npos) {
        if (eClass == "zero") {
            return myZeroHelper.getClassByName("default", vc);
        }
        // default HBEFA2
        return myHBEFA2Helper.getClassByName(eClass, vc);
    }
    throw InvalidArgument("Unknown emission class '" + eClass + "'.");
}


const std::vector<SUMOEmissionClass>
PollutantsInterface::getAllClasses() {
    std::vector<SUMOEmissionClass> result;
    for (int i = 0; i < 5; i++) {
        myHelpers[i]->addAllClassesInto(result);
    }
    return result;
}


const std::vector<std::string>&
PollutantsInterface::getAllClassesStr() {
    // first check if myAllClassesStr has to be filled
    if (myAllClassesStr.empty()) {
        // first obtain all emissionClasses
        std::vector<SUMOEmissionClass> emissionClasses;
        for (int i = 0; i < 5; i++) {
            myHelpers[i]->addAllClassesInto(emissionClasses);
        }
        // now write all emissionClasses in myAllClassesStr
        for (const auto& i : emissionClasses) {
            myAllClassesStr.push_back(getName(i));
        }
    }
    return myAllClassesStr;
}

std::string
PollutantsInterface::getName(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getClassName(c);
}


std::string
PollutantsInterface::getPollutantName(const EmissionType e) {
    switch (e) {
        case CO2:
            return "CO2";
        case CO:
            return "CO";
        case HC:
            return "HC";
        case FUEL:
            return "fuel";
        case NO_X:
            return "NOx";
        case PM_X:
            return "PMx";
        case ELEC:
            return "electricity";
        default:
            throw InvalidArgument("Unknown emission type '" + toString(e) + "'");
    }
}

bool
PollutantsInterface::isHeavy(const SUMOEmissionClass c) {
    return (c & HEAVY_BIT) != 0;
}


bool
PollutantsInterface::isSilent(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->isSilent(c);
}


SUMOEmissionClass
PollutantsInterface::getClass(const SUMOEmissionClass base, const std::string& vClass,
                              const std::string& fuel, const std::string& eClass, const double weight) {
    return myHelpers[base >> 16]->getClass(base, vClass, fuel, eClass, weight);
}


std::string
PollutantsInterface::getAmitranVehicleClass(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getAmitranVehicleClass(c);
}


std::string
PollutantsInterface::getFuel(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getFuel(c);
}


int
PollutantsInterface::getEuroClass(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getEuroClass(c);
}


double
PollutantsInterface::getWeight(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getWeight(c);
}


double
PollutantsInterface::compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope, const std::map<int, double>* param) {
    return myHelpers[c >> 16]->compute(c, e, v, a, slope, param);
}


PollutantsInterface::Emissions
PollutantsInterface::computeAll(const SUMOEmissionClass c, const double v, const double a, const double slope, const std::map<int, double>* param) {
    const Helper* const h = myHelpers[c >> 16];
    return Emissions(h->compute(c, CO2, v, a, slope, param), h->compute(c, CO, v, a, slope, param), h->compute(c, HC, v, a, slope, param),
                     h->compute(c, FUEL, v, a, slope, param), h->compute(c, NO_X, v, a, slope, param), h->compute(c, PM_X, v, a, slope, param),
                     h->compute(c, ELEC, v, a, slope, param));
}


double
PollutantsInterface::computeDefault(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope, const double tt, const std::map<int, double>* param) {
    const Helper* const h = myHelpers[c >> 16];
    return (h->compute(c, e, v, 0, slope, param) + h->compute(c, e, v - a, a, slope, param)) * tt / 2.;
}


double
PollutantsInterface::getModifiedAccel(const SUMOEmissionClass c, const double v, const double a, const double slope) {
    return myHelpers[c >> 16]->getModifiedAccel(c, v, a, slope);
}


const HelpersEnergy&
PollutantsInterface::getEnergyHelper() {
    return myEnergyHelper;
}

/****************************************************************************/
