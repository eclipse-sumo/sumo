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

#include "MSSOTLCongestionPolicy.h"

MSSOTLCongestionPolicy::MSSOTLCongestionPolicy(
		const std::map<std::string, std::string>& parameters) :
		MSSOTLPolicy("Congestion", parameters) {
}

MSSOTLCongestionPolicy::MSSOTLCongestionPolicy(
		MSSOTLPolicyDesirability *desirabilityAlgorithm) :
		MSSOTLPolicy("Congestion", desirabilityAlgorithm) {
	getDesirabilityAlgorithm()->setKeyPrefix("CONGESTION");
}

MSSOTLCongestionPolicy::MSSOTLCongestionPolicy(
		MSSOTLPolicyDesirability *desirabilityAlgorithm,
		const std::map<std::string, std::string>& parameters) :
		MSSOTLPolicy("Congestion", desirabilityAlgorithm, parameters) {
	getDesirabilityAlgorithm()->setKeyPrefix("CONGESTION");

}

size_t MSSOTLCongestionPolicy::decideNextPhase(int elapsed,
		const MSPhaseDefinition* stage, size_t currentPhaseIndex,
		size_t phaseMaxCTS, bool thresholdPassed, bool pushButtonPressed, int vehicleCount) {
	if (stage->isCommit()) {
		// decide which chain to activate. Gotta work on this
		return currentPhaseIndex;
	}
	if (stage->isTransient()) {
		//If the junction was in a transient step
		//=> go to the next step and return computeReturnTime()
		return currentPhaseIndex + 1;
	}

	if (stage->isDecisional()) {

		if (canRelease(elapsed, thresholdPassed, pushButtonPressed, stage, vehicleCount)) {
			return currentPhaseIndex + 1;
		}
	}

	return currentPhaseIndex;
}

bool MSSOTLCongestionPolicy::canRelease(int elapsed, bool thresholdPassed, bool pushButtonPressed,
		const MSPhaseDefinition* stage, int vehicleCount) {
	return (elapsed >= stage->minDuration);
}
