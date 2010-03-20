/****************************************************************************/
/// @file    MSPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// »missingDescription«
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

#include <vector>
#include <algorithm>
#include <microsim/MSNet.h>
#include "MSPerson.h"
#include "MSPersonControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSPersonControl::MSPersonControl() {}


MSPersonControl::~MSPersonControl() {
    for (std::map<std::string, MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end(); ++i) {
        delete(*i).second;
    }
    myPersons.clear();
    myWaiting.clear();
}


bool
MSPersonControl::add(const std::string &id, MSPerson *person) {
    if (myPersons.find(id) == myPersons.end()) {
        myPersons[id] = person;
        return true;
    }
    return false;
}


void
MSPersonControl::setWaiting(const SUMOTime time, MSPerson *person) {
    if (myWaiting.find(time)==myWaiting.end()) {
        myWaiting[time] = PersonVector();
    }
    myWaiting[time].push_back(person);
}


bool
MSPersonControl::hasWaitingPersons(SUMOTime time) const {
    return myWaiting.find(time)!=myWaiting.end();
}


const MSPersonControl::PersonVector &
MSPersonControl::getWaitingPersons(SUMOTime time) const {
    return myWaiting.find(time)->second;
}


/****************************************************************************/
