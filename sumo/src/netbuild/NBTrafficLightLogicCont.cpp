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
// Revision 1.5  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.4  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.3  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <map>
#include <string>
#include <algorithm>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightLogicCont.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
NBTrafficLightLogicCont::ComputedContType NBTrafficLightLogicCont::_computed;

NBTrafficLightLogicCont::DefinitionContType NBTrafficLightLogicCont::_definitions;

/* =========================================================================
 * method definitions
 * ======================================================================= */
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
    for(ComputedContType::iterator i=_computed.begin(); i!=_computed.end(); i++) {
        delete (*i).second;
    }
    _computed.clear();
}


bool
NBTrafficLightLogicCont::computeLogics(OptionsCont &oc)
{
    for(DefinitionContType::iterator i=_definitions.begin(); i!=_definitions.end(); i++) {
        // get the definition
        NBTrafficLightDefinition *def = (*i).second;
        // and insert the result after coputation
        if(!insert((*i).first, def->compute(oc))) {
            // should not happen
            cout << " Warning: Could not build traffic lights '" << def->getID() << "'" << endl;
        }
    }
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightLogicCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


