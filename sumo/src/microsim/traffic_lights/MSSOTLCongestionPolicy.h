/****************************************************************************/
/// @file    MSSOTLCongestionPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @date    Feb 2014
/// @version $Id: MSSOTLCongestionPolicy.h 0 2014-02-28 11:05:00Z riccardo_belletti $
///
// The class for SOTL Congestion logics
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

	MSSOTLCongestionPolicy(MSSOTLPolicyDesirability *desirabilityAlgorithm);

	MSSOTLCongestionPolicy(MSSOTLPolicyDesirability *desirabilityAlgorithm,
			const std::map<std::string, std::string>& parameters);

	size_t decideNextPhase(int elapsed, const MSPhaseDefinition* stage,
			size_t currentPhaseIndex, size_t phaseMaxCTS, bool thresholdPassed, bool pushButtonPressed,
			int vehicleCount);

	bool canRelease(int elapsed, bool thresholdPassed, bool pushButtonPressed,
			const MSPhaseDefinition* stage, int vehicleCount);

};

#endif /* MSSOTLCONGESTIONTRAFFICLIGHTLOGIC_H_ */
