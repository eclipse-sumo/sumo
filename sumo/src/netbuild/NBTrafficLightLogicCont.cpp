/****************************************************************************/
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light definitions and built programs
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
    if (oc.isSet("tl-logics.half-offset")) {
        myHalfOffsetTLS = oc.getStringVector("tl-logics.half-offset");
    }
    // check whether any offsets shall be manipulated by setting
    //  them to a quarter of the duration
    if (oc.isSet("tl-logics.quarter-offset")) {
        myQuarterOffsetTLS = oc.getStringVector("tl-logics.quarter-offset");
    }
}


bool
NBTrafficLightLogicCont::insert(NBTrafficLightDefinition *logic) throw() {
    DefinitionContType::iterator i=myDefinitions.find(logic->getID());
    if (i!=myDefinitions.end()) {
        return false;
    }
    myDefinitions[logic->getID()] = logic;
    return true;
}


bool
NBTrafficLightLogicCont::remove(const std::string &id) throw() {
    DefinitionContType::iterator i=myDefinitions.find(id);
    if (i==myDefinitions.end()) {
        return false;
    }
    delete(*i).second;
    myDefinitions.erase(i);
    return true;
}


void
NBTrafficLightLogicCont::computeLogics(NBEdgeCont &ec, OptionsCont &oc) throw() {
    unsigned int no = 0;
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        std::string id = (*i).first;
        if (myComputed.find(id)!=myComputed.end()) {
            WRITE_WARNING("Traffic light '" + id + "' was already built.");
            continue;
        }
        // build program
        NBTrafficLightDefinition *def = (*i).second;
        NBTrafficLightLogic *built = def->compute(ec, oc);
        if (built==0) {
            WRITE_WARNING("Could not build traffic lights '" + id + "'");
            continue;
        }
        // compute offset
        SUMOTime T = built->getDuration();
        if (find(myHalfOffsetTLS.begin(), myHalfOffsetTLS.end(), id)!=myHalfOffsetTLS.end()) {
            built->setOffset((SUMOTime)(T/2.));
        }
        if (find(myQuarterOffsetTLS.begin(), myQuarterOffsetTLS.end(), id)!=myQuarterOffsetTLS.end()) {
            built->setOffset((SUMOTime)(T/4.));
        }
        // and insert the result after computation
        myComputed[(*i).first] = built;
        no++;
    }
    WRITE_MESSAGE(toString<int>(no) + " traffic light(s) computed.");
}


void
NBTrafficLightLogicCont::writeXML(OutputDevice &into) throw(IOError) {
    for (ComputedContType::iterator i=myComputed.begin(); i!=myComputed.end(); i++) {
        (*i).second->writeXML(into);
    }
	if(myComputed.size()!=0) {
		into << "\n";
	}
}


void
NBTrafficLightLogicCont::clear() throw() {
    for (ComputedContType::iterator i=myComputed.begin(); i!=myComputed.end(); ++i) {
        delete(*i).second;
    }
    myComputed.clear();
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
        delete(*i).second;
    }
    myDefinitions.clear();
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge *removed, const EdgeVector &incoming,
                                      const EdgeVector &outgoing) throw() {
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge *removed, int removedLane,
                                        NBEdge *by, int byLane) throw() {
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition *
NBTrafficLightLogicCont::getDefinition(const std::string &id) const throw() {
    DefinitionContType::const_iterator i=myDefinitions.find(id);
    if (i!=myDefinitions.end()) {
        return (*i).second;
    }
    return 0;
}


void
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont &ec) throw() {
    DefinitionContType::iterator i;
    // set the information about all participants, first
    for (i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->setParticipantsInformation();
    }
    // insert the information about the tl-controlling
    for (i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->setTLControllingInformation(ec);
    }
}



/****************************************************************************/

