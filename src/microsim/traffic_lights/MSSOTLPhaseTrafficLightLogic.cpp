/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPhaseTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL Phase logics
/****************************************************************************/

#include "MSSOTLPhaseTrafficLightLogic.h"

MSSOTLPhaseTrafficLightLogic::MSSOTLPhaseTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& programID, const Phases& phases, int step,
    SUMOTime delay,
    const std::map<std::string, std::string>& parameters) :
    MSSOTLTrafficLightLogic(tlcontrol, id, programID, TLTYPE_SOTL_PHASE, phases, step, delay,
                            parameters) {
    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id
        + " will run using MSSOTLPhaseTrafficLightLogic ***");
}

MSSOTLPhaseTrafficLightLogic::MSSOTLPhaseTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& programID, const Phases& phases, int step,
    SUMOTime delay, const std::map<std::string, std::string>& parameters,
    MSSOTLSensors* sensors) :
    MSSOTLTrafficLightLogic(tlcontrol, id, programID, TLTYPE_SOTL_PHASE, phases, step, delay,
                            parameters, sensors) {
}

bool MSSOTLPhaseTrafficLightLogic::canRelease() {
    if (getCurrentPhaseElapsed() >= getCurrentPhaseDef().minDuration) {
        return isThresholdPassed();
    }
    return false;
}
