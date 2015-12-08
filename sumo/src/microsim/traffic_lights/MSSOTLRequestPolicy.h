/****************************************************************************/
/// @file    MSSOTLRequestPolicy.h
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    Apr 2013
/// @version $Id: MSSOTLRequestPolicy.h 2 2013-04-05 15:00:00Z gslager $
///
// The class for SOTL Request logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

using namespace std;
/**
 * @class MSSOTLRequestPolicy
 * @brief Class for low-level request policy.
 *
 */
class MSSOTLRequestPolicy: public MSSOTLPolicy {

public:
	MSSOTLRequestPolicy(const std::map<std::string, std::string>& parameters);
	MSSOTLRequestPolicy(MSSOTLPolicyDesirability *desirabilityAlgorithm);

	MSSOTLRequestPolicy(MSSOTLPolicyDesirability *desirabilityAlgorithm,
			const std::map<std::string, std::string>& parameters);

	bool canRelease(int elapsed, bool thresholdPassed,  bool pushButtonPressed,
			const MSPhaseDefinition* stage, int vehicleCount);

	int getMinDecisionalPhaseDuration() {
		std::ostringstream key;
		key << "MIN_DECISIONAL_PHASE_DUR";
		std::ostringstream def;
		def << "5000";
		return (int) s2f(getParameter(key.str(), def.str()));
	}

};

#endif
/****************************************************************************/
