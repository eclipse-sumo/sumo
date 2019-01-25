/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLMarchingPolicy.cpp
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @author  Federico Caselli
/// @date    2014-03-20
/// @version $Id$
///
// The class for SOTL Marching logics
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
