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

using namespace std;

NBTrafficLightLogicCont::ContType NBTrafficLightLogicCont::_cont;

bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightLogicVector *logics)
{
    ContType::iterator i=_cont.find(id);
    if(i!=_cont.end()) {
        _cont[id]->add(*logics);
    }
    _cont[id] = logics;
    return true;
}


void
NBTrafficLightLogicCont::writeXML(std::ostream &into)
{
    for(ContType::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


void
NBTrafficLightLogicCont::clear()
{
    for(ContType::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i).second;
    }
    _cont.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightLogicCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


