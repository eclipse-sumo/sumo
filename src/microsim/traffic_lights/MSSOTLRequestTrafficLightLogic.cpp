/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLRequestTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    2013-02-25
/// @version $Id$
///
// The class for SOTL Request logics
/****************************************************************************/

#include "MSSOTLRequestTrafficLightLogic.h"

MSSOTLRequestTrafficLightLogic::MSSOTLRequestTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id, const std::string& subid,
    const Phases& phases, int step, SUMOTime delay,
    const std::map<std::string, std::string>& parameters) :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters) {
    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id
        + " will run using MSSOTLRequestTrafficLightLogic ---");
}

MSSOTLRequestTrafficLightLogic::MSSOTLRequestTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id, const std::string& subid,
    const Phases& phases, int step, SUMOTime delay,
    const std::map<std::string, std::string>& parameters,
    MSSOTLSensors* sensors) :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters, sensors) {
    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id
        + " will run using MSSOTLRequestTrafficLightLogic ***");
}

bool MSSOTLRequestTrafficLightLogic::canRelease() {
    if (getCurrentPhaseElapsed() >= getMinDecisionalPhaseDuration()) {
        return isThresholdPassed();
    }
    return false;
}
