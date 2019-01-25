/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLCongestionPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @date    2014-03-20
/// @version $Id$
///
// The class for SOTL Congestion logics
/****************************************************************************/

#ifndef MSTLCONGESTIONPOLICY_H_
#define MSTLCONGESTIONPOLICY_H_

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include "MSSOTLPolicy.h"
/**
 * @class MSSOTLCongestionPolicy
 * @brief Class for low-level congestion policy.
 *
 */
class MSSOTLCongestionPolicy: public MSSOTLPolicy {

public:
    MSSOTLCongestionPolicy(
        const std::map<std::string, std::string>& parameters);

    MSSOTLCongestionPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm);

    MSSOTLCongestionPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                           const std::map<std::string, std::string>& parameters);

    int decideNextPhase(SUMOTime elapsed, const MSPhaseDefinition* stage,
                        int currentPhaseIndex, int phaseMaxCTS, bool thresholdPassed, bool pushButtonPressed,
                        int vehicleCount);

    bool canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                    const MSPhaseDefinition* stage, int vehicleCount);

};

#endif /* MSSOTLCONGESTIONTRAFFICLIGHTLOGIC_H_ */
