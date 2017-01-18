/****************************************************************************/
/// @file    MSSOTLRequestTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @date    2013-02-25
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

#include "MSSOTLRequestTrafficLightLogic.h"

MSSOTLRequestTrafficLightLogic::MSSOTLRequestTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id, const std::string& subid,
    const Phases& phases, int step, SUMOTime delay,
    const std::map<std::string, std::string>& parameters) throw() :
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
    MSSOTLSensors* sensors) throw() :
    MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
                            parameters, sensors) {
    MsgHandler::getMessageInstance()->inform(
        "*** Intersection " + id
        + " will run using MSSOTLRequestTrafficLightLogic ***");
}

bool MSSOTLRequestTrafficLightLogic::canRelease() throw() {
    if (getCurrentPhaseElapsed() >= getMinDecisionalPhaseDuration()) {
        return isThresholdPassed();
    }
    return false;
}
