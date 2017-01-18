/****************************************************************************/
/// @file    MSSOTLPhasePolicy.h
/// @author  Gianfilippo Slager
/// @author  Federico Caselli
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Phase logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSTLPhasePolicy_h
#define MSTLPhasePolicy_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
