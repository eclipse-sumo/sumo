/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLRequestPolicy.cpp
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    Apr 2013
/// @version $Id$
///
// The class for SOTL Request logics
/****************************************************************************/

#include "MSSOTLRequestPolicy.h"

MSSOTLRequestPolicy::MSSOTLRequestPolicy(
    const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Request", parameters) {
}

MSSOTLRequestPolicy::MSSOTLRequestPolicy(
    MSSOTLPolicyDesirability* desirabilityAlgorithm) :
    MSSOTLPolicy("Request", desirabilityAlgorithm) {
    getDesirabilityAlgorithm()->setKeyPrefix("REQUEST");
}

MSSOTLRequestPolicy::MSSOTLRequestPolicy(
    MSSOTLPolicyDesirability* desirabilityAlgorithm,
    const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy("Request", desirabilityAlgorithm, parameters) {
    getDesirabilityAlgorithm()->setKeyPrefix("REQUEST");

}

bool MSSOTLRequestPolicy::canRelease(SUMOTime elapsed, bool thresholdPassed,  bool /* pushButtonPressed */,
                                     const MSPhaseDefinition* /* stage */, int /* vehicleCount */) {
    if (elapsed >= getMinDecisionalPhaseDuration()) {
        return thresholdPassed;
    }
    return false;
}
