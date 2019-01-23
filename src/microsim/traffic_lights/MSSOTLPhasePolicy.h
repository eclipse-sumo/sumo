/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPhasePolicy.h
/// @author  Gianfilippo Slager
/// @author  Federico Caselli
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Phase logics
/****************************************************************************/
#ifndef MSTLPhasePolicy_h
#define MSTLPhasePolicy_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSSOTLPolicy.h"
#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
/**
 * @class MSSOTLPhasePolicy
 * @brief Class for low-level phase policy.
 *
 */
class MSSOTLPhasePolicy: public MSSOTLPolicy, public PushButtonLogic, public SigmoidLogic {

public:
    MSSOTLPhasePolicy(const std::map<std::string, std::string>& parameters);
    MSSOTLPhasePolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm);

    MSSOTLPhasePolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                      const std::map<std::string, std::string>& parameters);

    bool canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                    const MSPhaseDefinition* stage, int vehicleCount);
protected:
    void init();
    bool m_useVehicleTypesWeights;
};

#endif
/****************************************************************************/
