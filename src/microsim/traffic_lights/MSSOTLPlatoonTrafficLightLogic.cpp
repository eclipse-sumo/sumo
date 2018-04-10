/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPlatoonTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Platoon logics
/****************************************************************************/

#include "MSSOTLPlatoonTrafficLightLogic.h"

MSSOTLPlatoonTrafficLightLogic::MSSOTLPlatoonTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& subid, const Phases& phases, int step,
    SUMOTime delay,
    const std::map<std::string, std::string>& parameters) :
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
    MSSOTLSensors* sensors) :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters, sensors) {
}

bool MSSOTLPlatoonTrafficLightLogic::canRelease() {
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
