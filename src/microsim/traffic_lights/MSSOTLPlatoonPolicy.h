/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPlatoonPolicy.h
/// @author  Gianfilippo Slager
/// @author  Federico Caselli
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Platoon logics
/****************************************************************************/
#ifndef MSTLPlatoonPolicy_h
#define MSTLPlatoonPolicy_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include "MSSOTLPolicy.h"


/**
 * @class MSSOTLPlatoonPolicy
 * @brief Class for low-level platoon policy.
 *
 */
class MSSOTLPlatoonPolicy: public MSSOTLPolicy, public SigmoidLogic, public PushButtonLogic {

public:
    MSSOTLPlatoonPolicy(const std::map<std::string, std::string>& parameters);
    MSSOTLPlatoonPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm);

    MSSOTLPlatoonPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                        const std::map<std::string, std::string>& parameters);

    bool canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                    const MSPhaseDefinition* stage, int vehicleCount);

protected:
    void init();
};

#endif
/****************************************************************************/
