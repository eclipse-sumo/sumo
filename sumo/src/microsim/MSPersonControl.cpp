/****************************************************************************/
/// @file    MSPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// »missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
/* -------------------------------------------------------------------------
 * MSPersonControl::SameDepartureTimeCont - methods
 * ----------------------------------------------------------------------- */
MSPersonControl::SameDepartureTimeCont::SameDepartureTimeCont(SUMOTime time)
        : m_uiArrivalTime(time) {}


MSPersonControl::SameDepartureTimeCont::~SameDepartureTimeCont() {
    for (PersonVector::iterator i=m_pPersons.begin(); i!=m_pPersons.end(); ++i) {
        delete(*i);
    }
}


void
MSPersonControl::SameDepartureTimeCont::add(MSPerson *person) {
    m_pPersons.push_back(person);
}


void
MSPersonControl::SameDepartureTimeCont::add(const PersonVector &cont) {
    std::copy(cont.begin(), cont.end(), std::back_inserter(m_pPersons));
}


SUMOTime
MSPersonControl::SameDepartureTimeCont::getTime() const {
    return m_uiArrivalTime;
}


const MSPersonControl::PersonVector &
MSPersonControl::SameDepartureTimeCont::getPersons() const {
    return m_pPersons;
}


/* -------------------------------------------------------------------------
 * MSPersonControl - methods
 * ----------------------------------------------------------------------- */
MSPersonControl::MSPersonControl() {}


MSPersonControl::~MSPersonControl() {
    // !!! delete
    myWaiting.clear();
}


void
MSPersonControl::add(SUMOTime when, MSPerson *person) {
    WaitingPersons::iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(when));
    if (i==myWaiting.end()) {
        SameDepartureTimeCont cont(when);
        cont.add(person);
        push(cont);
    } else {
        (*i).add(person);
    }
}


bool
MSPersonControl::hasWaitingPersons(SUMOTime time) const {
    WaitingPersons::const_iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(time));
    if (i==myWaiting.end()) return false;
    return true;
}


const MSPersonControl::PersonVector &
MSPersonControl::getWaitingPersons(SUMOTime time) const {
    WaitingPersons::const_iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(time));
    return (*i).getPersons();
}


void
MSPersonControl::push(SUMOTime time, const MSPersonControl::PersonVector &list) {
    SameDepartureTimeCont toPush(time);
    toPush.add(list);
    push(toPush);
}


void
MSPersonControl::push(const SameDepartureTimeCont &cont) {
    WaitingPersons::iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), my_greater(cont.getTime()));
    if (i==myWaiting.end())
        myWaiting.push_back(cont);
    else
        myWaiting.insert(i, cont);
}



/****************************************************************************/

