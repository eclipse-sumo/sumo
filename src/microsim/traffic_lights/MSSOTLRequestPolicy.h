/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLRequestPolicy.h
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    Apr 2013
/// @version $Id$
///
// The class for SOTL Request logics
/****************************************************************************/
#ifndef MSTLRequestPolicy_h
#define MSTLRequestPolicy_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSSOTLPolicy.h"

/**
 * @class MSSOTLRequestPolicy
 * @brief Class for low-level request policy.
 *
 */
class MSSOTLRequestPolicy: public MSSOTLPolicy {

public:
    MSSOTLRequestPolicy(const std::map<std::string, std::string>& parameters);
    MSSOTLRequestPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm);

    MSSOTLRequestPolicy(MSSOTLPolicyDesirability* desirabilityAlgorithm,
                        const std::map<std::string, std::string>& parameters);

    bool canRelease(SUMOTime elapsed, bool thresholdPassed,  bool pushButtonPressed,
                    const MSPhaseDefinition* stage, int vehicleCount);

    int getMinDecisionalPhaseDuration() {
        return StringUtils::toInt(getParameter("MIN_DECISIONAL_PHASE_DUR", "5000"));
    }

};

#endif
/****************************************************************************/
