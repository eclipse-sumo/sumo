/****************************************************************************/
/// @file    PollutantsInterface.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 19.08.2013
/// @version $Id$
///
// Interface to capsulate different emission models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
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

#include <limits>
#include <cmath>
#include <utils/common/SUMOVehicleClass.h>
#include "PollutantsInterface.h"
#include "HelpersHBEFA.h"
#include "HelpersHBEFA3.h"
#include "HelpersPHEMlight.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static definitions
// ===========================================================================
HelpersHBEFA PollutantsInterface::myHBEFA2Helper;
HelpersHBEFA3 PollutantsInterface::myHBEFA3Helper;
HelpersPHEMlight PollutantsInterface::myPHEMlightHelper;
PollutantsInterface::Helper* PollutantsInterface::myHelpers[] = {&PollutantsInterface::myHBEFA2Helper, &PollutantsInterface::myHBEFA3Helper, &PollutantsInterface::myPHEMlightHelper};


// ===========================================================================
// method definitions
// ===========================================================================
SUMOEmissionClass
PollutantsInterface::getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
    const std::string::size_type sep = eClass.find("/");
    if (sep != std::string::npos) {
        const std::string model = eClass.substr(0, sep);
        const std::string subClass = eClass.substr(sep + 1);
        for (int i = 0; i < 3; i++) {
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


SUMOReal
PollutantsInterface::getWeight(const SUMOEmissionClass c) {
    return myHelpers[c >> 16]->getWeight(c);
}


SUMOReal
PollutantsInterface::compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope) {
    return myHelpers[c >> 16]->compute(c, e, v, a, slope);
}


PollutantsInterface::Emissions
PollutantsInterface::computeAll(const SUMOEmissionClass c, const double v, const double a, const double slope) {
    const Helper* const h = myHelpers[c >> 16];
    return Emissions(h->compute(c, CO2, v, a, slope), h->compute(c, CO, v, a, slope), h->compute(c, HC, v, a, slope),
                     h->compute(c, FUEL, v, a, slope), h->compute(c, NO_X, v, a, slope), h->compute(c, PM_X, v, a, slope),
                     h->compute(c, ELEC, v, a, slope));
}


SUMOReal
PollutantsInterface::computeDefault(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope, const SUMOReal tt) {
    const Helper* const h = myHelpers[c >> 16];
    return (h->compute(c, e, v, 0, slope) + h->compute(c, e, v - a, a, slope)) * tt / 2.;
}


/****************************************************************************/

