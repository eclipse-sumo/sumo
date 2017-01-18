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

#ifndef MSTLMARCHINGPOLICY_H_
#define MSTLMARCHINGPOLICY_H_

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
