/****************************************************************************/
/// @file    MSSOTLMarchingPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @author  Federico Caselli
/// @date    2014-03-20
/// @version $Id$
///
// The class for SOTL Marching logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSSOTLMarchingPolicy.h"

MSSOTLMarchingPolicy::MSSOTLMarchingPolicy(
    const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Marching", parameters) {
    init();
}

MSSOTLMarchingPolicy::MSSOTLMarchingPolicy(
    MSSOTLPolicyDesirability* desirabilityAlgorithm) :
    MSSOTLPolicy("Marching", desirabilityAlgorithm) {
    getDesirabilityAlgorithm()->setKeyPrefix("MARCHING");
    init();
}

MSSOTLMarchingPolicy::MSSOTLMarchingPolicy(
    MSSOTLPolicyDesirability* desirabilityAlgorithm,
    const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Marching", desirabilityAlgorithm, parameters) {
    getDesirabilityAlgorithm()->setKeyPrefix("MARCHING");
    init();
}

bool MSSOTLMarchingPolicy::canRelease(SUMOTime elapsed, bool /* thresholdPassed */, bool pushButtonPressed,
                                      const MSPhaseDefinition* stage, int /* vehicleCount */) {
    if (elapsed >= stage->minDuration && pushButtonLogic(elapsed, pushButtonPressed, stage)) {
        return true;
    }
    return (elapsed >= stage->duration);
}

void MSSOTLMarchingPolicy::init() {
    PushButtonLogic::init("MSSOTLMarchingPolicy", this);
}
