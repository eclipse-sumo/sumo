/****************************************************************************/
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light definitions and built programs
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightLogicCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBTrafficLightLogicCont::NBTrafficLightLogicCont() throw()
{}


NBTrafficLightLogicCont::~NBTrafficLightLogicCont() throw()
{
    clear();
}


void
NBTrafficLightLogicCont::applyOptions(OptionsCont &oc)
{
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
NBTrafficLightLogicCont::insert(NBTrafficLightDefinition *logic) throw()
{
    DefinitionContType::iterator i=myDefinitions.find(logic->getID());
    if (i!=myDefinitions.end()) {
        return false;
    }
    myDefinitions[logic->getID()] = logic;
    return true;
}


void
NBTrafficLightLogicCont::computeLogics(NBEdgeCont &ec, OptionsCont &oc) throw()
{
    unsigned int no = 0;
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        // build program
        NBTrafficLightDefinition *def = (*i).second;
        NBTrafficLightLogicVector *built = def->compute(ec, oc);
        // compute offset
        string id = (*i).first;
        if (find(myHalfOffsetTLS.begin(), myHalfOffsetTLS.end(), id)!=myHalfOffsetTLS.end()) {
            built->setOffsetsToHalf();
        }
        if (find(myQuarterOffsetTLS.begin(), myQuarterOffsetTLS.end(), id)!=myQuarterOffsetTLS.end()) {
            built->setOffsetsToQuarter();
        }
        // and insert the result after computation
        if (!insert((*i).first, built)) {
            // should not happen
            WRITE_WARNING("Could not build traffic lights '" + def->getID()+ "'");
        } else {
            no++;
        }
    }
    WRITE_MESSAGE(toString<int>(no) + " traffic light(s) computed.");
}


void
NBTrafficLightLogicCont::writeXML(OutputDevice &into) throw(IOError)
{
    for (ComputedContType::iterator i=myComputed.begin(); i!=myComputed.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << "\n";
}


void
NBTrafficLightLogicCont::clear() throw()
{
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
                                      const EdgeVector &outgoing) throw()
{
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge *removed, int removedLane,
                                        NBEdge *by, int byLane) throw()
{
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        (*i).second->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition *
NBTrafficLightLogicCont::getDefinition(const std::string &id) const throw()
{
    DefinitionContType::const_iterator i=myDefinitions.find(id);
    if (i!=myDefinitions.end()) {
        return (*i).second;
    }
    return 0;
}


void
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont &ec) throw()
{
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


bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightLogicVector *logics) throw()
{
    if (logics==0) {
        return false;
    }
    ComputedContType::iterator i=myComputed.find(id);
    if (i!=myComputed.end()) {
        myComputed[id]->add(*logics);
    }
    myComputed[id] = logics;
    return true;
}




/****************************************************************************/

