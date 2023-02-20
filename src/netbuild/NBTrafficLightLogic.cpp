/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    NBTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A SUMO-compliant built logic for a traffic light
/****************************************************************************/
#include <config.h>

#include <vector>
#include <bitset>
#include <utility>
#include <string>
#include <sstream>
#include <cassert>
#include "NBEdge.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightDefinition.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// static members
// ===========================================================================

// ===========================================================================
// member method definitions
// ===========================================================================
NBTrafficLightLogic::NBTrafficLightLogic(const std::string& id,
        const std::string& subid, int noLinks,
        SUMOTime offset, TrafficLightType type) :
    Named(id), myNumLinks(noLinks), mySubID(subid),
    myOffset(offset),
    myType(type) {}


NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic* logic) :
    Named(logic->getID()),
    myNumLinks(logic->myNumLinks),
    mySubID(logic->getProgramID()),
    myOffset(logic->getOffset()),
    myPhases(logic->myPhases.begin(), logic->myPhases.end()),
    myType(logic->getType()) {}


NBTrafficLightLogic::~NBTrafficLightLogic() {}


void
NBTrafficLightLogic::addStep(const SUMOTime duration, const std::string& state, const std::vector<int>& next, const std::string& name, const int index) {
    addStep(duration, state,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            NBTrafficLightDefinition::UNSPECIFIED_DURATION,
            name, next, index);
}


void
NBTrafficLightLogic::addStep(const SUMOTime duration, const std::string& state, const SUMOTime minDur, const SUMOTime maxDur, const SUMOTime earliestEnd,
                             const SUMOTime latestEnd, const SUMOTime vehExt, const SUMOTime yellow, const SUMOTime red,
                             const std::string& name,
                             const std::vector<int>& next,
                             int index) {
    // check state size
    if (myNumLinks == 0) {
        // initialize
        myNumLinks = (int)state.size();
    } else if ((int)state.size() != myNumLinks) {
        throw ProcessError("When adding phase to tlLogic '" + getID() + "': state length of " + toString(state.size()) +
                           " does not match declared number of links " + toString(myNumLinks));
    }
    // check state contents
    const std::string::size_type illegal = state.find_first_not_of(SUMOXMLDefinitions::ALLOWED_TLS_LINKSTATES);
    if (std::string::npos != illegal) {
        throw ProcessError(TLF("When adding phase: illegal character '%' in state", toString(state[illegal])));
    }
    // interpret index
    if (index < 0 || index >= (int)myPhases.size()) {
        // insert at the end
        index = (int)myPhases.size();
    }
    myPhases.insert(myPhases.begin() + index, PhaseDefinition(duration, state, minDur, maxDur, earliestEnd, latestEnd, vehExt, yellow, red, next, name));
}


void
NBTrafficLightLogic::deletePhase(int index) {
    if (index >= (int)myPhases.size()) {
        throw InvalidArgument("Index " + toString(index) + " out of range for logic with "
                              + toString(myPhases.size()) + " phases.");
    }
    myPhases.erase(myPhases.begin() + index);
}


void
NBTrafficLightLogic::swapPhase(int indexPhaseA, int indexPhaseB) {
    if (indexPhaseA >= (int)myPhases.size()) {
        throw InvalidArgument("Index " + toString(indexPhaseA) + " out of range for logic with "
                              + toString(myPhases.size()) + " phases.");
    }
    if (indexPhaseB >= (int)myPhases.size()) {
        throw InvalidArgument("Index " + toString(indexPhaseB) + " out of range for logic with "
                              + toString(myPhases.size()) + " phases.");
    }
    // declare auxiliar PhaseDefinition and swap
    const auto auxPhase = myPhases.at(indexPhaseA);
    myPhases.at(indexPhaseA) = myPhases.at(indexPhaseB);
    myPhases.at(indexPhaseB) = auxPhase;
}


void
NBTrafficLightLogic::swapfirstPhase() {
    const auto firstPhase = myPhases.front();
    myPhases.erase(myPhases.begin());
    myPhases.push_back(firstPhase);
}


void
NBTrafficLightLogic::swaplastPhase() {
    const auto lastPhase = myPhases.back();
    myPhases.pop_back();
    myPhases.insert(myPhases.begin(), lastPhase);
}

void
NBTrafficLightLogic::setStateLength(int numLinks, LinkState fill) {
    if (myNumLinks > numLinks) {
        for (PhaseDefinition& p : myPhases) {
            p.state = p.state.substr(0, numLinks);
        }
    } else {
        std::string add(numLinks - myNumLinks, (char)fill);
        for (PhaseDefinition& p : myPhases) {
            p.state = p.state + add;
        }
    }
    myNumLinks = numLinks;
}


void
NBTrafficLightLogic::deleteStateIndex(int index) {
    assert(index >= 0);
    assert(index < myNumLinks);
    for (PhaseDefinition& p : myPhases) {
        p.state.erase(index, 1);
    }
    myNumLinks--;
}


void
NBTrafficLightLogic::resetPhases() {
    myNumLinks = 0;
    myPhases.clear();
}


SUMOTime
NBTrafficLightLogic::getDuration() const {
    SUMOTime duration = 0;
    for (PhaseDefinitionVector::const_iterator i = myPhases.begin(); i != myPhases.end(); ++i) {
        duration += (*i).duration;
    }
    return duration;
}


void
NBTrafficLightLogic::closeBuilding(bool checkVarDurations) {
    for (int i = 0; i < (int)myPhases.size() - 1;) {
        if (myPhases[i].state != myPhases[i + 1].state || myPhases[i].next.size() > 0 || myPhases[i + 1].next.size() > 0 || myPhases[i].name != myPhases[i + 1].name) {
            ++i;
            continue;
        }
        myPhases[i].duration += myPhases[i + 1].duration;
        if (myPhases[i + 1].minDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
            if (myPhases[i].minDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                myPhases[i].minDur += myPhases[i + 1].minDur;
            } else {
                myPhases[i].minDur = myPhases[i + 1].minDur;
            }
        }
        if (myPhases[i + 1].maxDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
            if (myPhases[i].maxDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                myPhases[i].maxDur += myPhases[i + 1].maxDur;
            } else {
                myPhases[i].maxDur = myPhases[i + 1].maxDur;
            }
        }
        myPhases.erase(myPhases.begin() + i + 1);
    }
    // check if actuated lights are defined correctly
    if (checkVarDurations) {
        if (myType != TrafficLightType::STATIC) {
            bool found = false;
            for (auto p : myPhases) {
                if (p.minDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION
                        || p.maxDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                WRITE_WARNINGF(TL("Non-static traffic light '%' does not define variable phase length."), getID());
            }
        }
    }
}


void
NBTrafficLightLogic::setPhaseState(int phaseIndex, int tlIndex, LinkState linkState) {
    assert(phaseIndex < (int)myPhases.size());
    std::string& phaseState = myPhases[phaseIndex].state;
    assert(tlIndex < (int)phaseState.size());
    phaseState[tlIndex] = (char)linkState;
}


void
NBTrafficLightLogic::setPhaseDuration(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].duration = duration;
}


void
NBTrafficLightLogic::setPhaseMinDuration(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].minDur = duration;
}


void
NBTrafficLightLogic::setPhaseMaxDuration(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].maxDur = duration;
}


void
NBTrafficLightLogic::setPhaseEarliestEnd(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].earliestEnd = duration;
}


void
NBTrafficLightLogic::setPhaseLatestEnd(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].latestEnd = duration;
}


void
NBTrafficLightLogic::setPhaseVehExt(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].vehExt = duration;
}


void
NBTrafficLightLogic::setPhaseYellow(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].yellow = duration;
}


void
NBTrafficLightLogic::setPhaseRed(int phaseIndex, SUMOTime duration) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].red = duration;
}


void
NBTrafficLightLogic::setPhaseNext(int phaseIndex, const std::vector<int>& next) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].next = next;
}


void
NBTrafficLightLogic::setPhaseName(int phaseIndex, const std::string& name) {
    assert(phaseIndex < (int)myPhases.size());
    myPhases[phaseIndex].name = name;
}


void
NBTrafficLightLogic::overrideState(int phaseIndex, const char c) {
    assert(phaseIndex < (int)myPhases.size());
    for (int i = 0; i < (int)myPhases[phaseIndex].state.size(); i++) {
        myPhases[phaseIndex].state[i] = c;
    }
}

/****************************************************************************/
