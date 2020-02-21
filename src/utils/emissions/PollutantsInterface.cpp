/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2020 German Aerospace Center (DLR) and others.
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
#include "HelpersHBEFA.h"
#include "HelpersHBEFA3.h"
#include "HelpersPHEMlight.h"
#include "HelpersEnergy.h"
#include "PollutantsInterface.h"


// ===========================================================================
// static definitions
// ===========================================================================

HelpersHBEFA PollutantsInterface::myHBEFA2Helper;
HelpersHBEFA3 PollutantsInterface::myHBEFA3Helper;
HelpersPHEMlight PollutantsInterface::myPHEMlightHelper;
HelpersEnergy PollutantsInterface::myEnergyHelper;
PollutantsInterface::Helper* PollutantsInterface::myHelpers[] = {
    &PollutantsInterface::myHBEFA2Helper, &PollutantsInterface::myHBEFA3Helper,
    &PollutantsInterface::myPHEMlightHelper, &PollutantsInterface::myEnergyHelper
};
std::vector<std::string> PollutantsInterface::myAllClassesStr;

// ===========================================================================
// method definitions
// ===========================================================================

SUMOEmissionClass
PollutantsInterface::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    const std::string::size_type sep = eClass.find("/");
    if (sep != std::string::npos) {
        const std::string model = eClass.substr(0, sep);
        const std::string subClass = eClass.substr(sep + 1);
        for (int i = 0; i < 4; i++) {
            if (myHelpers[i]->getName() == model) {
                return myHelpers[i]->getClassByName(subClass, vc);
            }
        }
    } else {
        // default HBEFA2
        return myHelpers[0]->getClassByName(eClass, vc);
    }
    throw InvalidArgument("Unknown emission class '" + eClass + "'.");
}


const std::vector<SUMOEmissionClass>
PollutantsInterface::getAllClasses() {
    std::vector<SUMOEmissionClass> result;
    for (int i = 0; i < 3; i++) {
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
        for (int i = 0; i < 3; i++) {
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


/****************************************************************************/
