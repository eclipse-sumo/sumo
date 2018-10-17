/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light definitions and built programs
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <map>
#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/IDSupplier.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicCont.h"
#include "NBOwnTLDef.h"
#include "NBEdgeCont.h"
#include "NBNodeCont.h"


// ===========================================================================
// static members
// ===========================================================================
const NBTrafficLightLogicCont::Program2Def NBTrafficLightLogicCont::EmptyDefinitions = NBTrafficLightLogicCont::Program2Def();

// ===========================================================================
// method definitions
// ===========================================================================
NBTrafficLightLogicCont::NBTrafficLightLogicCont() {}


NBTrafficLightLogicCont::~NBTrafficLightLogicCont() {
    clear();
}


void
NBTrafficLightLogicCont::applyOptions(OptionsCont& oc) {
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if (oc.isSet("tls.half-offset")) {
        std::vector<std::string> ids = oc.getStringVector("tls.half-offset");
        myHalfOffsetTLS.insert(ids.begin(), ids.end());
    }
    // check whether any offsets shall be manipulated by setting
    //  them to a quarter of the duration
    if (oc.isSet("tls.quarter-offset")) {
        std::vector<std::string> ids = oc.getStringVector("tls.quarter-offset");
        myHalfOffsetTLS.insert(ids.begin(), ids.end());
    }
}


bool
NBTrafficLightLogicCont::insert(NBTrafficLightDefinition* logic, bool forceInsert) {
    myExtracted.erase(logic);
    if (myDefinitions.count(logic->getID())) {
        if (myDefinitions[logic->getID()].count(logic->getProgramID())) {
            if (forceInsert) {
                const Program2Def& programs = myDefinitions[logic->getID()];
                IDSupplier idS("", 0);
                for (const auto & program : programs) {
                    idS.avoid(program.first);
                }
                logic->setProgramID(idS.getNext());
            } else {
                return false;
            }
        }
    } else {
        myDefinitions[logic->getID()] = Program2Def();
    }
    myDefinitions[logic->getID()][logic->getProgramID()] = logic;
    return true;
}


bool
NBTrafficLightLogicCont::removeFully(const std::string id) {
    if (myDefinitions.count(id)) {
        // delete all programs
        for (auto & i : myDefinitions[id]) {
            delete i.second;
        }
        myDefinitions.erase(id);
        // also delete any logics that were already computed
        if (myComputed.count(id)) {
            for (auto & i : myComputed[id]) {
                delete i.second;
            }
            myComputed.erase(id);
        }
        return true;
    } else {
        return false;
    }
}


bool
NBTrafficLightLogicCont::removeProgram(const std::string id, const std::string programID, bool del) {
    if (myDefinitions.count(id) && myDefinitions[id].count(programID)) {
        if (del) {
            delete myDefinitions[id][programID];
        }
        myDefinitions[id].erase(programID);
        return true;
    } else {
        return false;
    }
}


void
NBTrafficLightLogicCont::extract(NBTrafficLightDefinition* definition) {
    myExtracted.insert(definition);
    removeProgram(definition->getID(), definition->getProgramID(), false);
}


std::pair<int, int>
NBTrafficLightLogicCont::computeLogics(OptionsCont& oc) {
    // clean previous logics
    Logics logics = getComputed();
    for (auto & logic : logics) {
        delete logic;
    }
    myComputed.clear();

    int numPrograms = 0;
    Definitions definitions = getDefinitions();
    for (auto & definition : definitions) {
        if (computeSingleLogic(oc, definition)) {
            numPrograms++;
        }
    }
    return std::pair<int, int>((int)myComputed.size(), numPrograms);
}


bool
NBTrafficLightLogicCont::computeSingleLogic(OptionsCont& oc, NBTrafficLightDefinition* def) {
    if (def->getNodes().size() == 0) {
        return false;
    }
    const std::string& id = def->getID();
    const std::string& programID = def->getProgramID();
    // build program
    NBTrafficLightLogic* built = def->compute(oc);
    if (built == nullptr) {
        WRITE_WARNING("Could not build program '" + programID + "' for traffic light '" + id + "'");
        return false;
    }
    // compute offset
    SUMOTime T = built->getDuration();
    if (myHalfOffsetTLS.count(id)) {
        built->setOffset((SUMOTime)(T / 2.));
    }
    if (myQuarterOffsetTLS.count(id)) {
        built->setOffset((SUMOTime)(T / 4.));
    }
    // and insert the result after computation
    // make sure we don't leak memory if computeSingleLogic is called externally
    if (myComputed[id][programID] != nullptr) {
        delete myComputed[id][programID];
    }
    myComputed[id][programID] = built;
    return true;
}


void
NBTrafficLightLogicCont::clear() {
    Definitions definitions = getDefinitions();
    for (auto & definition : definitions) {
        delete definition;
    }
    myDefinitions.clear();
    Logics logics = getComputed();
    for (auto & logic : logics) {
        delete logic;
    }
    myComputed.clear();
    for (auto it : myExtracted) {
        delete it;
    }
    myExtracted.clear();
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge* removed, const EdgeVector& incoming,
                                      const EdgeVector& outgoing) {
    Definitions definitions = getDefinitions();
    for (auto & definition : definitions) {
        definition->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge* removed, int removedLane,
                                        NBEdge* by, int byLane) {
    Definitions definitions = getDefinitions();
    for (auto & definition : definitions) {
        definition->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition*
NBTrafficLightLogicCont::getDefinition(const std::string& id, const std::string& programID) const {
    Id2Defs::const_iterator i = myDefinitions.find(id);
    if (i != myDefinitions.end()) {
        Program2Def programs = i->second;
        Program2Def::const_iterator i2 = programs.find(programID);
        if (i2 != programs.end()) {
            return i2->second;
        }
    }
    return nullptr;
}

const NBTrafficLightLogicCont::Program2Def&
NBTrafficLightLogicCont::getPrograms(const std::string& id) const {
    Id2Defs::const_iterator it = myDefinitions.find(id);
    if (it != myDefinitions.end()) {
        return it->second;
    } else {
        return EmptyDefinitions;
    }
}


NBTrafficLightLogic*
NBTrafficLightLogicCont::getLogic(const std::string& id, const std::string& programID) const {
    Id2Logics::const_iterator i = myComputed.find(id);
    if (i != myComputed.end()) {
        Program2Logic programs = i->second;
        Program2Logic::const_iterator i2 = programs.find(programID);
        if (i2 != programs.end()) {
            return i2->second;
        }
    }
    return nullptr;
}


void
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont& ec, const NBNodeCont& nc) {
    Definitions definitions = getDefinitions();
    // set the information about all participants, first
    for (auto & definition : definitions) {
        definition->setParticipantsInformation();
    }
    // clear previous information because tlDefs may have been removed in NETEDIT
    ec.clearControllingTLInformation();
    // insert the information about the tl-controlling
    for (auto & definition : definitions) {
        definition->setTLControllingInformation();
    }
    // handle rail signals which are not instantiated as normal definitions
    for (const auto & it : nc) {
        NBNode* n = it.second;
        if (n->getType() == NODETYPE_RAIL_SIGNAL || n->getType() == NODETYPE_RAIL_CROSSING) {
            NBOwnTLDef dummy(n->getID(), n, 0, TLTYPE_STATIC);
            dummy.setParticipantsInformation();
            dummy.setTLControllingInformation();
            n->setCrossingTLIndices(dummy.getID(), (int)dummy.getControlledLinks().size());
            n->removeTrafficLight(&dummy);
        }
    }
}


NBTrafficLightLogicCont::Logics
NBTrafficLightLogicCont::getComputed() const {
    Logics result;
    for (const auto & it_id : myComputed) {
        const Program2Logic& programs = it_id.second;
        for (const auto & program : programs) {
            result.push_back(program.second);
        }
    }
    return result;
}


NBTrafficLightLogicCont::Definitions
NBTrafficLightLogicCont::getDefinitions() const {
    Definitions result;
    for (const auto & myDefinition : myDefinitions) {
        const Program2Def& programs = myDefinition.second;
        for (const auto & program : programs) {
            result.push_back(program.second);
        }
    }
    return result;
}


/****************************************************************************/

