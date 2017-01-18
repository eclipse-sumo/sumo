/****************************************************************************/
/// @file    MSSOTLRequestPolicy.h
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    Apr 2013
/// @version $Id$
///
// The class for SOTL Request logics
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
#ifndef MSTLRequestPolicy_h
#define MSTLRequestPolicy_h

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
        return TplConvert::_2int(getParameter("MIN_DECISIONAL_PHASE_DUR", "5000").c_str());
    }

};

#endif
/****************************************************************************/
