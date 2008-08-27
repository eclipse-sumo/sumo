/****************************************************************************/
/// @file    NBTrafficLightLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light logic vectors
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
NBTrafficLightLogicCont::NBTrafficLightLogicCont()
{}


NBTrafficLightLogicCont::~NBTrafficLightLogicCont()
{
    clear();
}


bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightLogicVector *logics)
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
NBTrafficLightLogicCont::writeXML(OutputDevice &into)
{
    for (ComputedContType::iterator i=myComputed.begin(); i!=myComputed.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << "\n";
}


void
NBTrafficLightLogicCont::clear()
{
    {
        for (ComputedContType::iterator i=myComputed.begin(); i!=myComputed.end(); ++i) {
            delete(*i).second;
        }
        myComputed.clear();
    }
    {
        for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
            delete(*i).second;
        }
        myDefinitions.clear();
    }
}


void
NBTrafficLightLogicCont::computeLogics(NBEdgeCont &ec, OptionsCont &oc)
{
    size_t no = 0;
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        // and insert the result after coputation
        if (!insert((*i).first, def->compute(ec, oc))) {
            // should not happen
            WRITE_WARNING("Could not build traffic lights '" + def->getID()+ "'");
        } else {
            no++;
        }
    }
    WRITE_MESSAGE(toString<int>(no) + " traffic light(s) computed.");
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge *removed, const EdgeVector &incoming,
                                      const EdgeVector &outgoing)
{
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        def->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge *removed, int removedLane,
                                        NBEdge *by, int byLane)
{
    for (DefinitionContType::iterator i=myDefinitions.begin(); i!=myDefinitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        def->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition *
NBTrafficLightLogicCont::getDefinition(const std::string &id)
{
    DefinitionContType::iterator i=myDefinitions.find(id);
    if (i!=myDefinitions.end()) {
        return (*i).second;
    }
    return 0;
}


void
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont &ec)
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


/****************************************************************************/

