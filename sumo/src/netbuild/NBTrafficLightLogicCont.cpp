//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicCont.cpp -
//  A container for traffic light logic vectors
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.17  2005/10/17 09:02:44  dkrajzew
// got rid of the old MSVC memory leak checker; memory leaks removed
//
// Revision 1.16  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.15  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.14  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.13  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.12  2004/11/23 10:21:41  dkrajzew
// debugging
//
// Revision 1.11  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.10  2003/10/30 09:09:55  dkrajzew
// tl-building order patched
//
// Revision 1.9  2003/10/15 11:49:26  dkrajzew
// unneeded debug-ifs removed
//
// Revision 1.8  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams
//  modality cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.7  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.6  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.5  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.4  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.3  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of
//  edge sorting (false lane connections) debugged
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H
#include <map>
#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightLogicCont.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NBTrafficLightLogicCont::NBTrafficLightLogicCont()
{
}


NBTrafficLightLogicCont::~NBTrafficLightLogicCont()
{
    clear();
}


bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightLogicVector *logics)
{
    if(logics==0) {
        return false;
    }
    ComputedContType::iterator i=_computed.find(id);
    if(i!=_computed.end()) {
        _computed[id]->add(*logics);
    }
    _computed[id] = logics;
    return true;
}


bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightDefinition *logics)
{
    DefinitionContType::iterator i=_definitions.find(id);
    if(i!=_definitions.end()) {
        return false;
    }
    _definitions[id] = logics;
    return true;
}


void
NBTrafficLightLogicCont::writeXML(std::ostream &into)
{
    for(ComputedContType::iterator i=_computed.begin(); i!=_computed.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


void
NBTrafficLightLogicCont::clear()
{
    {
        for(ComputedContType::iterator i=_computed.begin(); i!=_computed.end(); ++i) {
            delete (*i).second;
        }
        _computed.clear();
    }
    {
        for(DefinitionContType::iterator i=_definitions.begin(); i!=_definitions.end(); ++i) {
            delete (*i).second;
        }
        _definitions.clear();
    }
}


bool
NBTrafficLightLogicCont::computeLogics(NBEdgeCont &ec, OptionsCont &oc)
{
    size_t no = 0;
    for(DefinitionContType::iterator i=_definitions.begin(); i!=_definitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        // and insert the result after coputation
        if(!insert((*i).first, def->compute(ec, oc))) {
            // should not happen
            WRITE_WARNING(string("Could not build traffic lights '") + def->getID()+ string("'"));
        } else {
            no++;
        }
    }
    WRITE_MESSAGE(toString<int>(no) + string(" traffic light(s) computed."));
    return true;
}


void
NBTrafficLightLogicCont::remapRemoved(NBEdge *removed, const EdgeVector &incoming,
                                      const EdgeVector &outgoing)
{
    for(DefinitionContType::iterator i=_definitions.begin(); i!=_definitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        def->remapRemoved(removed, incoming, outgoing);
    }
}


void
NBTrafficLightLogicCont::replaceRemoved(NBEdge *removed, int removedLane,
                                        NBEdge *by, int byLane)
{
    for(DefinitionContType::iterator i=_definitions.begin(); i!=_definitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        def->replaceRemoved(removed, removedLane, by, byLane);
    }
}


NBTrafficLightDefinition *
NBTrafficLightLogicCont::getDefinition(const std::string &id)
{
    DefinitionContType::iterator i=_definitions.find(id);
    if(i!=_definitions.end()) {
        return (*i).second;
    }
    return 0;
}




bool
NBTrafficLightLogicCont::setTLControllingInformation(const NBEdgeCont &ec)
{
    DefinitionContType::iterator i;
    // set the information about all participants, first
    for(i=_definitions.begin(); i!=_definitions.end(); i++) {
        (*i).second->setParticipantsInformation();
    }
    // insert the information about the tl-controlling
    for(i=_definitions.begin(); i!=_definitions.end(); i++) {
        (*i).second->setTLControllingInformation(ec);
    }
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


