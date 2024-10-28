/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A container for traffic light definitions and built programs
/****************************************************************************/
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
#include "NBLoadedSUMOTLDef.h"
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
        myQuarterOffsetTLS.insert(ids.begin(), ids.end());
    }
}


std::string
NBTrafficLightLogicCont::getNextProgramID(const std::string& id) const {
    IDSupplier idS("", 0);
    if (myDefinitions.count(id)) {
        const Program2Def& programs = myDefinitions.find(id)->second;
        for (auto item : programs) {
            idS.avoid(item.first);
        }
    }
    return idS.getNext();
}


bool
NBTrafficLightLogicCont::insert(NBTrafficLightDefinition* logic, bool forceInsert) {
    myExtracted.erase(logic);
    if (myDefinitions.count(logic->getID())) {
        if (myDefinitions[logic->getID()].count(logic->getProgramID())) {
            if (forceInsert) {
                logic->setProgramID(getNextProgramID(logic->getID()));
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
        for (Program2Def::iterator i = myDefinitions[id].begin(); i != myDefinitions[id].end(); i++) {
            delete i->second;
        }
        myDefinitions.erase(id);
        // also delete any logics that were already computed
        if (myComputed.count(id)) {
            for (Program2Logic::iterator i = myComputed[id].begin(); i != myComputed[id].end(); i++) {
                delete i->second;
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


bool
NBTrafficLightLogicCont::exist(const std::string& newID, bool requireComputed) const {
    const auto itD = myDefinitions.find(newID);
    const auto itC = myComputed.find(newID);
    if ((itD != myDefinitions.end()) && (itC != myComputed.end() || !requireComputed)) {
        return true;
    } else {
        return false;
    }
}


std::pair<int, int>
NBTrafficLightLogicCont::computeLogics(OptionsCont& oc) {
    // clean previous logics
    Logics logics = getComputed();
    for (Logics::iterator it = logics.begin(); it != logics.end(); it++) {
        delete *it;
    }
    myComputed.clear();
    if (oc.getBool("tls.rebuild")) {
        for (NBTrafficLightDefinition* def : getDefinitions()) {
            NBLoadedSUMOTLDef* lDef = dynamic_cast<NBLoadedSUMOTLDef*>(def);
            if (lDef != nullptr) {
                TrafficLightType type = def->getType();
                if (!oc.isDefault("tls.default-type")) {
                    type = SUMOXMLDefinitions::TrafficLightTypes.get(oc.getString("tls.default-type"));
                }
                NBOwnTLDef* oDef = new NBOwnTLDef(def->getID(), def->getNodes(), def->getOffset(), type);
                oDef->setProgramID(def->getProgramID());
                oDef->setParticipantsInformation();
                for (NBEdge* e : oDef->getIncomingEdges()) {
                    e->clearControllingTLInformation();
                }
                oDef->setTLControllingInformation();
                for (NBNode* node : oDef->getNodes()) {
                    node->removeTrafficLight(def);
                }
                removeProgram(def->getID(), def->getProgramID());
                insert(oDef);
            }
        }
    }
    if (oc.getBool("tls.group-signals")) {
        // replace NBOwnTLDef tld with NBLoadedSUMOTLDef
        for (NBTrafficLightDefinition* def : getDefinitions()) {
            NBLoadedSUMOTLDef* lDef = dynamic_cast<NBLoadedSUMOTLDef*>(def);
            if (lDef == nullptr) {
                NBTrafficLightLogic* logic = def->compute(oc);
                if (logic != nullptr) {
                    lDef = new NBLoadedSUMOTLDef(*def, *logic);
                    lDef->setParticipantsInformation();
                    for (NBNode* node : lDef->getNodes()) {
                        node->removeTrafficLight(def);
                        node->addTrafficLight(lDef);
                    }
                    removeProgram(def->getID(), def->getProgramID());
                    insert(lDef);
                    delete logic;
                }
            }
            if (lDef != nullptr) {
                lDef->groupSignals();
            }
        }
    } else if (oc.getBool("tls.ungroup-signals")) {
        for (NBTrafficLightDefinition* def : getDefinitions()) {
            NBLoadedSUMOTLDef* lDef = dynamic_cast<NBLoadedSUMOTLDef*>(def);
            // NBOwnTLDef are always ungrouped
            if (lDef != nullptr) {
                if (lDef->usingSignalGroups()) {
                    lDef->ungroupSignals();
                }
            }
        }
    }
    int numPrograms = 0;
    for (NBTrafficLightDefinition* def : getDefinitions()) {
        if (computeSingleLogic(oc, def)) {
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
        WRITE_WARNINGF(TL("Could not build program '%' for traffic light '%'"), programID, id);
        return false;
    }
    // compute offset
    SUMOTime T = built->getDuration();
    if (myHalfOffsetTLS.count(id)) {
        built->setOffset(TIME2STEPS(floor(STEPS2TIME(T / 2))));
    }
    if (myQuarterOffsetTLS.count(id)) {
        built->setOffset(TIME2STEPS(floor(STEPS2TIME(T / 4))));
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
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        delete *it;
    }
    myDefinitions.clear();
    Logics logics = getComputed();
    for (Logics::iterator it = logics.begin(); it != logics.end(); it++) {
        delete *it;
    }
    myComputed.clear();
    for (std::set<NBTrafficLightDefinition*>::iterator it = myExtracted.begin(); it != myExtracted.end(); it++) {
        delete *it;
    }
    myExtracted.clear();
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge* removed, const EdgeVector& incoming,
                                      const EdgeVector& outgoing) {
    Definitions definitions = getDefinitions();
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge* removed, int removedLane,
                                        NBEdge* by, int byLane, bool incoming) {
    Definitions definitions = getDefinitions();
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->replaceRemoved(removed, removedLane, by, byLane, incoming);
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
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->setParticipantsInformation();
    }
    // clear previous information because tlDefs may have been removed in netedit
    ec.clearControllingTLInformation();
    // insert the information about the tl-controlling
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->setTLControllingInformation();
    }
    // handle rail signals which are not instantiated as normal definitions
    for (std::map<std::string, NBNode*>::const_iterator it = nc.begin(); it != nc.end(); it ++) {
        NBNode* n = it->second;
        if (n->getType() == SumoXMLNodeType::RAIL_SIGNAL || n->getType() == SumoXMLNodeType::RAIL_CROSSING) {
            NBOwnTLDef dummy(n->getID(), n, 0, TrafficLightType::STATIC);
            dummy.setParticipantsInformation();
            dummy.setTLControllingInformation();
            n->setCrossingTLIndices(dummy.getID(), (int)dummy.getControlledLinks().size());
            n->removeTrafficLight(&dummy);
        }
    }
}


void
NBTrafficLightLogicCont::setOpenDriveSignalParameters() {
    Definitions definitions = getDefinitions();
    for (NBTrafficLightDefinition* def : getDefinitions()) {
        std::map<NBEdge*, std::string> defaultSignalIDs;
        for (const NBConnection& con : def->getControlledLinks()) {
            const NBEdge::Connection& c = con.getFrom()->getConnection(con.getFromLane(), con.getTo(), con.getToLane());
            if (c.hasParameter("signalID")) {
                defaultSignalIDs[con.getFrom()] = c.getParameter("signalID");
                def->setParameter("linkSignalID:" + toString(con.getTLIndex()), c.getParameter("signalID"));
            }
        }
        // oftentimes, signals are placed on connecting road but are meant to apply to all connections from the incoming edge
        for (const NBConnection& con : def->getControlledLinks()) {
            const NBEdge::Connection& c = con.getFrom()->getConnection(con.getFromLane(), con.getTo(), con.getToLane());
            if (!c.hasParameter("signalID") && defaultSignalIDs.count(con.getFrom()) != 0) {
                WRITE_WARNINGF(TL("Guessing signalID for link index % at traffic light '%'."), con.getTLIndex(), def->getID());
                def->setParameter("linkSignalID:" + toString(con.getTLIndex()), defaultSignalIDs[con.getFrom()]);
            }
        }
    }
}


void
NBTrafficLightLogicCont::applyOpenDriveControllers(OptionsCont& oc) {
    // collect connections with controllerID which can be controlled together
    Definitions definitions = getDefinitions();
    std::map<std::string, NBTrafficLightDefinition*> defsToGroup;
    std::map<std::string, std::map<std::string, std::set<int>>> controllerID2tlIndex;
    for (NBTrafficLightDefinition* def : definitions) {
        bool makeGroups = false;
        NBConnectionVector& connections = def->getControlledLinks();
        std::string defID = def->getID();

        for (NBConnection conn : connections) {
            std::string controllerID = conn.getFrom()->getConnection(conn.getFromLane(), conn.getTo(), conn.getToLane()).getParameter("controllerID");
            if (controllerID != "") {
                if (controllerID2tlIndex.find(defID) == controllerID2tlIndex.end()) {
                    controllerID2tlIndex[defID][controllerID] = std::set<int>({ conn.getTLIndex() });
                } else if (controllerID2tlIndex[defID].find(controllerID) != controllerID2tlIndex[defID].end()) {
                    controllerID2tlIndex[defID][controllerID].insert(conn.getTLIndex());
                } else {
                    controllerID2tlIndex[defID][controllerID] = std::set<int>({ conn.getTLIndex() });
                }
                makeGroups = controllerID2tlIndex[defID][controllerID].size() > 1 || makeGroups;
            }
        }
        if (makeGroups) {
            defsToGroup[def->getID()] = def;
        }
    }
    bool same = true;
    for (NBTrafficLightLogic* computed : getComputed()) {
        const std::string computedID = computed->getID();
        if (defsToGroup.find(computedID) != defsToGroup.end()) {
            // remember corresponding tl indices and check if they can be joined
            // execute tl index joins if possible
            const std::vector<NBTrafficLightLogic::PhaseDefinition> phases = computed->getPhases();
            std::vector<int> major2minor;
            for (const auto& indexSet : controllerID2tlIndex[computedID]) {
                if (indexSet.second.size() < 2) {
                    continue;
                }
                bool toMinor = false;
                for (NBTrafficLightLogic::PhaseDefinition phase : phases) {
                    std::set<int>::iterator it = indexSet.second.begin();
                    char firstChar = phase.state[*it];
                    if (firstChar == LINKSTATE_TL_GREEN_MAJOR) {
                        firstChar = LINKSTATE_TL_GREEN_MINOR;
                    }
                    it++;
                    for (; it != indexSet.second.end(); it++) {
                        const char compareChar = (phase.state[*it] != LINKSTATE_TL_GREEN_MAJOR) ? phase.state[*it] : (char)LINKSTATE_TL_GREEN_MINOR;
                        if (compareChar != firstChar) {
                            same = false;
                            break;
                        } else if (phase.state[*it] == LINKSTATE_TL_GREEN_MINOR) {
                            toMinor = true;
                        }
                    }
                    if (!same) {
                        break;
                    }
                }
                if (toMinor) {
                    major2minor.push_back(*indexSet.second.begin());
                }
            }
            if (same) {
                NBLoadedSUMOTLDef* lDef = dynamic_cast<NBLoadedSUMOTLDef*>(defsToGroup[computedID]);
                if (lDef == nullptr) {
                    lDef = new NBLoadedSUMOTLDef(*defsToGroup[computedID], *computed);
                    lDef->setParticipantsInformation();
                    for (int index : major2minor) { // update the signal states (G -> g where needed)
                        for (int i = 0; i < (int)phases.size(); i++) {
                            if (phases[i].state[index] == LINKSTATE_TL_GREEN_MAJOR) {
                                lDef->getLogic()->setPhaseState(i, index, LINKSTATE_TL_GREEN_MINOR);
                            }
                        }
                    }
                    // join signal groups, update the connections
                    std::vector<int> indexToRemove;
                    for (const auto& indexSet : controllerID2tlIndex[computedID]) {
                        int minIndex = *indexSet.second.begin();
                        for (int index : indexSet.second) {
                            if (index != minIndex) {
                                lDef->replaceIndex(index, minIndex);
                                indexToRemove.push_back(index);
                            }
                        }
                    }
                    // remove unused indices from signal programs
                    lDef->cleanupStates();
                    for (NBNode* node : lDef->getNodes()) {
                        node->removeTrafficLight(defsToGroup[computedID]);
                        node->addTrafficLight(lDef);
                    }
                    removeProgram(defsToGroup[computedID]->getID(), defsToGroup[computedID]->getProgramID());
                    insert(lDef);
                    computeSingleLogic(oc, lDef);
                }
            } else { // TODO: try other strategy to build signal groups
                WRITE_WARNINGF(TL("Was not able to apply the OpenDRIVE signal group information onto the signal program of traffic light % generated by SUMO."), computed->getID());
            }
            break;
        }
    }
}


NBTrafficLightLogicCont::Logics
NBTrafficLightLogicCont::getComputed() const {
    Logics result;
    for (Id2Logics::const_iterator it_id = myComputed.begin(); it_id != myComputed.end(); it_id++) {
        const Program2Logic& programs = it_id->second;
        for (Program2Logic::const_iterator it_prog = programs.begin(); it_prog != programs.end(); it_prog++) {
            result.push_back(it_prog->second);
        }
    }
    return result;
}


NBTrafficLightLogicCont::Definitions
NBTrafficLightLogicCont::getDefinitions() const {
    Definitions result;
    for (Id2Defs::const_iterator it_id = myDefinitions.begin(); it_id != myDefinitions.end(); it_id++) {
        const Program2Def& programs = it_id->second;
        for (Program2Def::const_iterator it_prog = programs.begin(); it_prog != programs.end(); it_prog++) {
            result.push_back(it_prog->second);
        }
    }
    return result;
}


void
NBTrafficLightLogicCont::rename(NBTrafficLightDefinition* tlDef, const std::string& newID) {
    auto it = myDefinitions.find(tlDef->getID());
    if (it != myDefinitions.end()) {
        for (auto item : it->second) {
            item.second->setID(newID);
        }
        myDefinitions[newID] = it->second;
        myDefinitions.erase(it);
    }
    auto it2 = myComputed.find(tlDef->getID());
    if (it2 != myComputed.end()) {
        for (auto item : it2->second) {
            item.second->setID(newID);
        }
        myComputed[newID] = it2->second;
        myComputed.erase(it2);
    }
}


int
NBTrafficLightLogicCont::getNumExtracted() const {
    return (int)myExtracted.size();
}

/****************************************************************************/
