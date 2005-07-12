//---------------------------------------------------------------------------//
//                        MSDiscreteEventControl.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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

//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSDiscreteEventControl.h"
#include "Action.h"

/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSDiscreteEventControl::MSDiscreteEventControl()
{
}


MSDiscreteEventControl::~MSDiscreteEventControl()
{
}


bool
MSDiscreteEventControl::hasAnyFor(EventType et)
{
    return myEventsForAll.find(et)!=myEventsForAll.end()/*
        &&
        myEventsForSet.find(et)!=myEventsForSet.end()*/;
}


void
MSDiscreteEventControl::execute(EventType et/*, const std::string &id*/)
{
    TypedEvents::iterator i = myEventsForAll.find(et);
    if(i!=myEventsForAll.end()) {
        const ActionVector &av = (*i).second;
        for(ActionVector::const_iterator j=av.begin(); j!=av.end(); j++) {
            (*j)->execute();
        }
    }
}



void
MSDiscreteEventControl::add(EventType et, Action *a)
{
    if(myEventsForAll.find(et)==myEventsForAll.end()) {
        myEventsForAll[et] = ActionVector();
    }
    myEventsForAll[et].push_back(a);
}

/*
void
MSDiscreteEventControl::add(EventType et, const std::string &id, Action &act)
{
    if(myEventsForSet.find(et)==myEventsForAll.end()) {
        myEventsForSet[et] = TypedDiscreteEventCont(id, a);
    } else {
        TypedEvents::iterator i = myEventsForSet.find(et);
        while(i!=myEventsForSet.end()) {
            if(!(*i).second.matches(id)) {
                i =
                continue;
            }
            myEventsForSet[et].add(a);
        }
    }
}
*/


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

