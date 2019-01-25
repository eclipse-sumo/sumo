/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPolicyBasedTrafficLightLogic.cpp
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @date    2014-03-20
/// @version $Id$
///
// The class for SOTL Congestion logics
/****************************************************************************/

#include "MSSOTLPolicyBasedTrafficLightLogic.h"

MSSOTLPolicyBasedTrafficLightLogic::MSSOTLPolicyBasedTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& programID, const TrafficLightType logicType, const Phases& phases, int step,
    SUMOTime delay, const std::map<std::string, std::string>& parameters,
    MSSOTLPolicy* policy) :
    MSSOTLTrafficLightLogic(tlcontrol, id, programID, logicType, phases, step, delay,
                            parameters), myPolicy(policy) {

    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id + " will run using MSSOTL"
        + policy->getName() + "TrafficLightLogic ***");

}

MSSOTLPolicyBasedTrafficLightLogic::MSSOTLPolicyBasedTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& programID, const TrafficLightType logicType, const Phases& phases, int step,
    SUMOTime delay, const std::map<std::string, std::string>& parameters,
    MSSOTLPolicy* policy, MSSOTLSensors* sensors) :
    MSSOTLTrafficLightLogic(tlcontrol, id, programID, logicType, phases, step, delay,
                            parameters, sensors), myPolicy(policy) {
}

MSSOTLPolicyBasedTrafficLightLogic::~MSSOTLPolicyBasedTrafficLightLogic(void) {

}

int MSSOTLPolicyBasedTrafficLightLogic::decideNextPhase() {

    DBG(
        std::ostringstream str; str << "\n" << time2string(MSNet::getInstance()->getCurrentTimeStep()) << " " << getID() << "invoked MSSOTLPolicyBasedTrafficLightLogic::decideNextPhase()"; WRITE_MESSAGE(str.str());)

    return myPolicy->decideNextPhase(getCurrentPhaseElapsed(),
                                     &getCurrentPhaseDef(), getCurrentPhaseIndex(),
                                     getPhaseIndexWithMaxCTS(), isThresholdPassed(), isPushButtonPressed(),
                                     countVehicles(getCurrentPhaseDef()));
}

bool MSSOTLPolicyBasedTrafficLightLogic::canRelease() {

    DBG(
        std::ostringstream str; str << "\n" << time2string(MSNet::getInstance()->getCurrentTimeStep()) << " " << getID() << "invoked MSSOTLPolicyBasedTrafficLightLogic::canRelease()"; WRITE_MESSAGE(str.str());)

    return myPolicy->canRelease(getCurrentPhaseElapsed(), isThresholdPassed(), isPushButtonPressed(),
                                &getCurrentPhaseDef(), countVehicles(getCurrentPhaseDef()));
}
