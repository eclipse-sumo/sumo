/****************************************************************************/
/// @file    MSSOTLPlatoonTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Platoon logics
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

#include "MSSOTLPlatoonTrafficLightLogic.h"

MSSOTLPlatoonTrafficLightLogic::MSSOTLPlatoonTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& subid, const Phases& phases, int step,
    SUMOTime delay,
    const std::map<std::string, std::string>& parameters) throw() :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters) {
    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id
        + " will run using MSSOTLPlatoonTrafficLightLogic ***");
}

MSSOTLPlatoonTrafficLightLogic::MSSOTLPlatoonTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& subid, const Phases& phases, int step,
    SUMOTime delay, const std::map<std::string, std::string>& parameters,
    MSSOTLSensors* sensors) throw() :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters, sensors) {
}

bool MSSOTLPlatoonTrafficLightLogic::canRelease() throw() {
    if (getCurrentPhaseElapsed() >= getCurrentPhaseDef().minDuration) {
        if (isThresholdPassed()) {
            //If there are no other vehicles approaching green lights
            //or the declared maximum duration has been reached
            return ((countVehicles(getCurrentPhaseDef()) == 0)
                    || (getCurrentPhaseElapsed()
                        >= getCurrentPhaseDef().maxDuration));
        }
    }
    return false;
}
