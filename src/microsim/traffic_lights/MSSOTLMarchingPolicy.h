/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLMarchingPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @author  Federico Caselli
/// @date    Feb 2014
/// @version $Id$
///
// The class for SOTL Marching logics
/****************************************************************************/

#ifndef MSTLMARCHINGPOLICY_H_
#define MSTLMARCHINGPOLICY_H_

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSSOTLPolicy.h"
/**
 * @class MSSOTLMarchingPolicy
 * @brief Class for low-level marching policy.
 *
 */
class MSSOTLMarchingPolicy: public MSSOTLPolicy, public PushButtonLogic {


public:
    MSSOTLMarchingPolicy(const std::map<std::string, std::string>& parameters);
    MSSOTLMarchingPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm);

    MSSOTLMarchingPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                         const std::map<std::string, std::string>& parameters);

    bool canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                    const MSPhaseDefinition* stage, int vehicleCount);

protected:
    void init();

};

#endif /* MSSOTLMARCHINGTRAFFICLIGHTLOGIC_H_ */
/****************************************************************************/
