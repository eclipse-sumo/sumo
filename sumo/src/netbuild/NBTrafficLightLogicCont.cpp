/****************************************************************************/
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light definitions and built programs
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#include <map>
#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
const NBTrafficLightLogicCont::Program2Def NBTrafficLightLogicCont::EmptyDefinitions = NBTrafficLightLogicCont::Program2Def();

// ===========================================================================
// method definitions
// ===========================================================================
NBTrafficLightLogicCont::NBTrafficLightLogicCont() throw() {}


NBTrafficLightLogicCont::~NBTrafficLightLogicCont() throw() {
    clear();
}


void
NBTrafficLightLogicCont::applyOptions(OptionsCont &oc) throw() {
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
NBTrafficLightLogicCont::insert(NBTrafficLightDefinition *logic) throw() {
    if (myDefinitions.count(logic->getID()) && 
            myDefinitions[logic->getID()].count(logic->getProgramID())) {
        return false;
    } else {
        myDefinitions[logic->getID()][logic->getProgramID()] = logic;
        return true;
    }
}


bool
NBTrafficLightLogicCont::removeFully(const std::string &id) {
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
NBTrafficLightLogicCont::remove(const std::string &id, const std::string &programID) {
    if (myDefinitions.count(id) && myDefinitions[id].count(programID)) {
        delete myDefinitions[id][programID];
        myDefinitions[id].erase(programID);
        return true;
    } else {
        return false;
    }
}


void
NBTrafficLightLogicCont::computeLogics(NBEdgeCont &ec, OptionsCont &oc) throw() {
    unsigned int numPrograms = 0;
    for (Id2Defs::iterator it_id = myDefinitions.begin(); it_id != myDefinitions.end(); it_id++) {
        const std::string& id = it_id->first;
        const Program2Def& programs = it_id->second;
        for (Program2Def::const_iterator it_prog = programs.begin(); it_prog != programs.end(); it_prog++) {
            const std::string& programID = it_prog->first; 
            // check for previous computation
            if (myComputed.count(id) && myComputed[id].count(programID)) {
                delete myComputed[id][programID];
                myComputed[id].erase(programID);
            }
            // build program
            NBTrafficLightDefinition *def = it_prog->second;
            NBTrafficLightLogic *built = def->compute(ec, oc);
            if (built==0) {
                WRITE_WARNING("Could not build program '" + programID + "' for traffic light '" + id + "'");
                continue;
            }
            // compute offset
            SUMOTime T = built->getDuration();
            if (myHalfOffsetTLS.count(id)) {
                built->setOffset((SUMOTime)(T/2.));
            }
            if (myQuarterOffsetTLS.count(id)) {
                built->setOffset((SUMOTime)(T/4.));
            }
            // and insert the result after computation
            myComputed[id][programID] = built;
            numPrograms++;
        }

    }
    unsigned int numIDs = myComputed.size();
    std::string progCount = ""; 
    if (numPrograms != numIDs) {
        progCount = "(" + toString(numPrograms) + " programs) ";
    }
    WRITE_MESSAGE(toString(numIDs) + " traffic light(s) " + progCount + "computed.");

}


void
NBTrafficLightLogicCont::clear() throw() {
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
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge *removed, const EdgeVector &incoming,
                                      const EdgeVector &outgoing) throw() {
    Definitions definitions = getDefinitions();
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge *removed, int removedLane,
                                        NBEdge *by, int byLane) throw() {
    Definitions definitions = getDefinitions();
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition *
NBTrafficLightLogicCont::getDefinition(const std::string &id, const std::string &programID) const {
    Id2Defs::const_iterator i=myDefinitions.find(id);
    if (i!=myDefinitions.end()) {
        Program2Def programs = i->second;
        Program2Def::const_iterator i2 = programs.find(programID);
        if (i2 != programs.end()) {
            return i2->second;
        }
    }
    return 0;
}

const NBTrafficLightLogicCont::Program2Def&
NBTrafficLightLogicCont::getPrograms(const std::string &id) const {
    Id2Defs::const_iterator it = myDefinitions.find(id);
    if (it != myDefinitions.end()) {
        return it->second;
    } else {
        return EmptyDefinitions;
    }
}


NBTrafficLightLogic *
NBTrafficLightLogicCont::getLogic(const std::string &id, const std::string &programID) const {
    Id2Logics::const_iterator i=myComputed.find(id);
    if (i!=myComputed.end()) {
        Program2Logic programs = i->second;
        Program2Logic::const_iterator i2 = programs.find(programID);
        if (i2 != programs.end()) {
            return i2->second;
        }
    }
    return 0;
}


void
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont &ec) throw() {
    Definitions definitions = getDefinitions();
    // set the information about all participants, first
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->setParticipantsInformation();
    }
    // insert the information about the tl-controlling
    for (Definitions::iterator it = definitions.begin(); it != definitions.end(); it++) {
        (*it)->setTLControllingInformation(ec);
    }
}


NBTrafficLightLogicCont::Logics 
NBTrafficLightLogicCont::getComputed() const {
    Logics result;
    for (Id2Logics::const_iterator it_id = myComputed.begin(); it_id != myComputed.end(); it_id++) {
        const std::string& id = it_id->first;
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
        const std::string& id = it_id->first;
        const Program2Def& programs = it_id->second;
        for (Program2Def::const_iterator it_prog = programs.begin(); it_prog != programs.end(); it_prog++) {
            result.push_back(it_prog->second);
        }
    }
    return result;
}


/****************************************************************************/

