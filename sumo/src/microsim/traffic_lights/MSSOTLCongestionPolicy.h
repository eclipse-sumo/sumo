/****************************************************************************/
/// @file    MSSOTLCongestionPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @date    2014-03-20
/// @version $Id$
///
// The class for SOTL Congestion logics
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

#ifndef MSTLCONGESTIONPOLICY_H_
#define MSTLCONGESTIONPOLICY_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


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
