/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPhasePolicy.cpp
/// @author  Gianfilippo Slager
/// @author  Federico Caselli
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Phase logics
/****************************************************************************/

#include "MSSOTLPhasePolicy.h"
#include <cmath>
#include "utils/common/RandHelper.h"


MSSOTLPhasePolicy::MSSOTLPhasePolicy(const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Phase", parameters) {
    init();
}

MSSOTLPhasePolicy::MSSOTLPhasePolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm) :
    MSSOTLPolicy("Phase", desirabilityAlgorithm) {
    getDesirabilityAlgorithm()->setKeyPrefix("PHASE");
    init();
}

MSSOTLPhasePolicy::MSSOTLPhasePolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                                     const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Phase", desirabilityAlgorithm, parameters) {
    getDesirabilityAlgorithm()->setKeyPrefix("PHASE");
    init();
}

bool MSSOTLPhasePolicy::canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                                   const MSPhaseDefinition* stage, int vehicleCount) {
//  DBG(
    std::ostringstream str;
    str << "MSSOTLPhasePolicy::canRelease threshold " << thresholdPassed << " vehicle " << vehicleCount << " elapsed " << elapsed << " min " << stage->minDuration;
    WRITE_MESSAGE(str.str());
//          );
    if (elapsed >= stage->minDuration) {
        if (pushButtonLogic(elapsed, pushButtonPressed, stage)) {
            return true;
        }
        if (thresholdPassed) {
            return thresholdPassed;
        } else if (m_useVehicleTypesWeights) {
            if (sigmoidLogic(elapsed, stage, vehicleCount)) {
                return true;
            }
        }
    }
    return false;
}

void MSSOTLPhasePolicy::init() {
    PushButtonLogic::init("MSSOTLPhasePolicy", this);
    SigmoidLogic::init("MSSOTLPhasePolicy", this);
    m_useVehicleTypesWeights = getParameter("USE_VEHICLE_TYPES_WEIGHTS", "0") == "1";
}
