/****************************************************************************/
/// @file    MSDiscreteEventControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// (theoretically) A control for described events
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

#include "MSDiscreteEventControl.h"
#include <utils/common/Command.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSDiscreteEventControl::MSDiscreteEventControl()
{}


MSDiscreteEventControl::~MSDiscreteEventControl()
{}


bool
MSDiscreteEventControl::hasAnyFor(EventType et, SUMOTime)
{
    return myEventsForAll.find(et)!=myEventsForAll.end();
}


void
MSDiscreteEventControl::execute(EventType et, SUMOTime currentTime)
{
    TypedEvents::iterator i = myEventsForAll.find(et);
    if (i!=myEventsForAll.end()) {
        const CommandVector &av = (*i).second;
        for (CommandVector::const_iterator j=av.begin(); j!=av.end(); j++) {
            (*j)->execute(currentTime);
        }
    }
}



void
MSDiscreteEventControl::add(EventType et, Command *a)
{
    if (myEventsForAll.find(et)==myEventsForAll.end()) {
        myEventsForAll[et] = CommandVector();
    }
    myEventsForAll[et].push_back(a);
}



/****************************************************************************/

