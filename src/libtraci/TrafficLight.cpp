/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    TrafficLight.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <netload/NLDetectorBuilder.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/TrafficLight.h>
#include "Domain.h"

// TODO remove the following line once the implementation is mature
#ifdef _MSC_VER
#pragma warning(disable: 4100)
#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace libtraci {

typedef Domain<libsumo::CMD_GET_TL_VARIABLE, libsumo::CMD_SET_TL_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
TrafficLight::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
TrafficLight::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::string
TrafficLight::getRedYellowGreenState(const std::string& tlsID) {
    return Dom::getString(libsumo::TL_RED_YELLOW_GREEN_STATE, tlsID);
}


std::vector<libsumo::TraCILogic>
TrafficLight::getAllProgramLogics(const std::string& tlsID) {
    tcpip::Storage& ret = Dom::get(libsumo::TL_COMPLETE_DEFINITION_RYG, tlsID);
    std::vector<libsumo::TraCILogic> result;
    int numLogics = ret.readInt();
    while (numLogics-- > 0) {
        Dom::readCompound(ret, 5);
        libsumo::TraCILogic logic;
        logic.programID = Dom::readTypedString(ret);
        logic.type = Dom::readTypedInt(ret);
        logic.currentPhaseIndex = Dom::readTypedInt(ret);
        int numPhases = Dom::readCompound(ret);
        while (numPhases-- > 0) {
            Dom::readCompound(ret, 6);
            libsumo::TraCIPhase* phase = new libsumo::TraCIPhase();
            phase->duration = Dom::readTypedDouble(ret);
            phase->state = Dom::readTypedString(ret);
            phase->minDur = Dom::readTypedDouble(ret);
            phase->maxDur = Dom::readTypedDouble(ret);
            int numNext = Dom::readCompound(ret);
            while (numNext-- > 0) {
                phase->next.push_back(Dom::readTypedInt(ret));
            }
            phase->name = Dom::readTypedString(ret);
            logic.phases.emplace_back(phase);
        }
        int numParams = Dom::readCompound(ret);
        while (numParams-- > 0) {
            const std::vector<std::string> key_value = Dom::readTypedStringList(ret);
            logic.subParameter[key_value[0]] = key_value[1];
        }
        result.emplace_back(logic);
    }
    return result;
}


std::vector<std::string>
TrafficLight::getControlledJunctions(const std::string& tlsID) {
    return Dom::getStringVector(libsumo::TL_CONTROLLED_JUNCTIONS, tlsID);
}


std::vector<std::string>
TrafficLight::getControlledLanes(const std::string& tlsID) {
    return Dom::getStringVector(libsumo::TL_CONTROLLED_LANES, tlsID);
}


std::vector<std::vector<libsumo::TraCILink> >
TrafficLight::getControlledLinks(const std::string& tlsID) {
    tcpip::Storage& ret = Dom::get(libsumo::TL_CONTROLLED_LINKS, tlsID);
    std::vector< std::vector<libsumo::TraCILink> > result;
    ret.readInt();
    int numSignals = Dom::readTypedInt(ret);
    while (numSignals-- > 0) {
        std::vector<libsumo::TraCILink> controlledLinks;
        int numLinks = Dom::readTypedInt(ret);
        while (numLinks-- > 0) {
            std::vector<std::string> link = Dom::readTypedStringList(ret);
            controlledLinks.emplace_back(link[0], link[2], link[1]);
        }
        result.emplace_back(controlledLinks);
    }
    return result;
}


std::string
TrafficLight::getProgram(const std::string& tlsID) {
    return Dom::getString(libsumo::TL_CURRENT_PROGRAM, tlsID);
}


int
TrafficLight::getPhase(const std::string& tlsID) {
    return Dom::getInt(libsumo::TL_CURRENT_PHASE, tlsID);
}


std::string
TrafficLight::getPhaseName(const std::string& tlsID) {
    return Dom::getString(libsumo::VAR_NAME, tlsID);
}


double
TrafficLight::getPhaseDuration(const std::string& tlsID) {
    return Dom::getDouble(libsumo::TL_PHASE_DURATION, tlsID);
}


double
TrafficLight::getNextSwitch(const std::string& tlsID) {
    return Dom::getDouble(libsumo::TL_NEXT_SWITCH, tlsID);
}

int
TrafficLight::getServedPersonCount(const std::string& tlsID, int index) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(index);
    return Dom::getInt(libsumo::VAR_PERSON_NUMBER, tlsID, &content);
}

std::vector<std::string>
TrafficLight::getBlockingVehicles(const std::string& tlsID, int linkIndex) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(linkIndex);
    return Dom::getStringVector(libsumo::TL_BLOCKING_VEHICLES, tlsID, &content);
}

std::vector<std::string>
TrafficLight::getRivalVehicles(const std::string& tlsID, int linkIndex) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(linkIndex);
    return Dom::getStringVector(libsumo::TL_RIVAL_VEHICLES, tlsID, &content);
}

std::vector<std::string>
TrafficLight::getPriorityVehicles(const std::string& tlsID, int linkIndex) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(linkIndex);
    return Dom::getStringVector(libsumo::TL_PRIORITY_VEHICLES, tlsID, &content);
}

LIBTRACI_PARAMETER_IMPLEMENTATION(TrafficLight, TL)

void
TrafficLight::setRedYellowGreenState(const std::string& tlsID, const std::string& state) {
    Dom::setString(libsumo::TL_RED_YELLOW_GREEN_STATE, tlsID, state);
}


void
TrafficLight::setPhase(const std::string& tlsID, const int index) {
    Dom::setInt(libsumo::TL_PHASE_INDEX, tlsID, index);
}


void
TrafficLight::setPhaseName(const std::string& tlsID, const std::string& name) {
    Dom::setString(libsumo::VAR_NAME, tlsID, name);
}


void
TrafficLight::setProgram(const std::string& tlsID, const std::string& programID) {
    Dom::setString(libsumo::TL_PROGRAM, tlsID, programID);
}


void
TrafficLight::setPhaseDuration(const std::string& tlsID, const double phaseDuration) {
    Dom::setDouble(libsumo::TL_PHASE_DURATION, tlsID, phaseDuration);
}


void
TrafficLight::setProgramLogic(const std::string& tlsID, const libsumo::TraCILogic& logic) {
    tcpip::Storage content;
    Dom::writeCompound(content, 5);
    Dom::writeTypedString(content, logic.programID);
    Dom::writeTypedInt(content, logic.type);
    Dom::writeTypedInt(content, logic.currentPhaseIndex);
    Dom::writeCompound(content, (int)logic.phases.size());
    for (const libsumo::TraCIPhase* const phase : logic.phases) {
        Dom::writeCompound(content, 6);
        Dom::writeTypedDouble(content, phase->duration);
        Dom::writeTypedString(content, phase->state);
        Dom::writeTypedDouble(content, phase->minDur);
        Dom::writeTypedDouble(content, phase->maxDur);
        Dom::writeCompound(content, (int)phase->next.size());
        for (int n : phase->next) {
            Dom::writeTypedInt(content, n);
        }
        Dom::writeTypedString(content, phase->name);
    }
    Dom::writeCompound(content, (int)logic.subParameter.size());
    for (const auto& key_value : logic.subParameter) {
        Dom::writeTypedStringList(content, std::vector<std::string>{key_value.first, key_value.second});
    }
    Dom::set(libsumo::TL_COMPLETE_PROGRAM_RYG, tlsID, &content);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(TrafficLight, TL)

}


/****************************************************************************/
