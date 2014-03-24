/****************************************************************************/
/// @file    PollutantsInterface.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 19.08.2013
/// @version $Id$
///
// Interface to capsulate different emission models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
PollutantsInterface::Helper* PollutantsInterface::Helper::myHelpers[] = {new HelpersHBEFA(), new HelpersHBEFA3(), new HelpersPHEMlight()};


// ===========================================================================
// method definitions
// ===========================================================================
bool
PollutantsInterface::knowsClass(const std::string& eClass) {
    return Helper::myHelpers[0]->knowsClass(eClass) || Helper::myHelpers[1]->knowsClass(eClass) || Helper::myHelpers[2]->knowsClass(eClass);
}


bool
PollutantsInterface::isHeavy(const SUMOEmissionClass c) {
    return (c & HEAVY_BIT) != 0;
}


bool
PollutantsInterface::isSilent(const SUMOEmissionClass c) {
    return Helper::myHelpers[c >> 16]->isSilent(c);
}


SUMOReal
PollutantsInterface::getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->getMaxAccel(c, v, a, slope);
}


SUMOEmissionClass
PollutantsInterface::getClass(const SUMOEmissionClass base, const std::string& vClass,
                              const std::string& fuel, const std::string& eClass, const double weight) {
    return Helper::myHelpers[base >> 16]->getClass(base, vClass, fuel, eClass, weight);
}


std::string
PollutantsInterface::getAmitranVehicleClass(const SUMOEmissionClass c) {
    return Helper::myHelpers[c >> 16]->getAmitranVehicleClass(c);
}


std::string
PollutantsInterface::getFuel(const SUMOEmissionClass c) {
    return Helper::myHelpers[c >> 16]->getFuel(c);
}


int
PollutantsInterface::getEuroClass(const SUMOEmissionClass c) {
    return Helper::myHelpers[c >> 16]->getEuroClass(c);
}


SUMOReal
PollutantsInterface::getWeight(const SUMOEmissionClass c) {
    return Helper::myHelpers[c >> 16]->getWeight(c);
}


SUMOReal
PollutantsInterface::computeCO(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, CO, v, a, slope);
}


SUMOReal
PollutantsInterface::computeCO2(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, CO2, v, a, slope);
}


SUMOReal
PollutantsInterface::computeHC(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, HC, v, a, slope);
}


SUMOReal
PollutantsInterface::computeNOx(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, NO_X, v, a, slope);
}


SUMOReal
PollutantsInterface::computePMx(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, PM_X, v, a, slope);
}


SUMOReal
PollutantsInterface::computeFuel(SUMOEmissionClass c, double v, double a, double slope) {
    return Helper::myHelpers[c >> 16]->compute(c, FUEL, v, a, slope);
}




SUMOReal
PollutantsInterface::computeDefaultCO(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, CO, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, CO, v-a, a, slope)) * tt / 2.;
}


SUMOReal
PollutantsInterface::computeDefaultCO2(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, CO2, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, CO2, v-a, a, slope)) * tt / 2.;
}


SUMOReal
PollutantsInterface::computeDefaultHC(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, HC, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, HC, v-a, a, slope)) * tt / 2.;
}


SUMOReal
PollutantsInterface::computeDefaultNOx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, NO_X, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, NO_X, v-a, a, slope)) * tt / 2.;
}


SUMOReal
PollutantsInterface::computeDefaultPMx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, PM_X, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, PM_X, v-a, a, slope)) * tt / 2.;
}


SUMOReal
PollutantsInterface::computeDefaultFuel(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    return (Helper::myHelpers[c >> 16]->compute(c, FUEL, v, 0, slope) + Helper::myHelpers[c >> 16]->compute(c, FUEL, v-a, a, slope)) * tt / 2.;
}


/****************************************************************************/

