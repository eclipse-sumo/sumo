/***************************************************************************
                          MSPersonControl.h
			  The container for people walking
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.2  2007/01/11 06:34:11  dkrajzew
// some work on person simulation
//
// Revision 1.1  2006/09/18 10:04:43  dkrajzew
// first steps towards a person-based multimodal simulation
//
// Revision 1.2  2002/04/25 14:41:30  dkrajzew
// False order of member variables initialisations fixed
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.4  2001/12/06 13:15:30  traffic
// minor bug removed
//
// Revision 1.3  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:16:20  traffic
// new extension
//
// Revision 1.1  2001/10/23 09:32:26  traffic
// person route implementation
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

#include <vector>
#include <algorithm>
#include <microsim/MSNet.h>
#include "MSPerson.h"
#include "MSPersonControl.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * MSPersonControl::SameDepartureTimeCont - methods
 * ----------------------------------------------------------------------- */
MSPersonControl::SameDepartureTimeCont::SameDepartureTimeCont(SUMOTime time)
    : m_uiArrivalTime(time)
{
}


MSPersonControl::SameDepartureTimeCont::~SameDepartureTimeCont()
{
    for(PersonVector::iterator i=m_pPersons.begin(); i!=m_pPersons.end(); i++) {
        delete (*i);
    }
}


void
MSPersonControl::SameDepartureTimeCont::add(MSPerson *person)
{
    m_pPersons.push_back(person);
}


void
MSPersonControl::SameDepartureTimeCont::add(const PersonVector &cont)
{
    std::copy(cont.begin(), cont.end(), std::back_inserter(m_pPersons));
}


SUMOTime
MSPersonControl::SameDepartureTimeCont::getTime() const
{
    return m_uiArrivalTime;
}


const MSPersonControl::PersonVector &
MSPersonControl::SameDepartureTimeCont::getPersons() const
{
    return m_pPersons;
}


/* -------------------------------------------------------------------------
 * MSPersonControl - methods
 * ----------------------------------------------------------------------- */
MSPersonControl::MSPersonControl()
{
}


MSPersonControl::~MSPersonControl()
{
    // !!! delete
    myWaiting.clear();
}


void
MSPersonControl::add(SUMOTime when, MSPerson *person)
{
    WaitingPersons::iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(when));
    if(i==myWaiting.end()) {
        SameDepartureTimeCont cont(when);
        cont.add(person);
        push(cont);
    } else {
        (*i).add(person);
    }
}


bool
MSPersonControl::hasWaitingPersons(SUMOTime time) const
{
    WaitingPersons::const_iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(time));
    if(i==myWaiting.end()) return false;
    return true;
}


const MSPersonControl::PersonVector &
MSPersonControl::getWaitingPersons(SUMOTime time) const
{
    WaitingPersons::const_iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), equal(time));
    return (*i).getPersons();
}


void
MSPersonControl::push(SUMOTime time, const MSPersonControl::PersonVector &list)
{
    SameDepartureTimeCont toPush(time);
    toPush.add(list);
    push(toPush);
}


void
MSPersonControl::push(const SameDepartureTimeCont &cont)
{
    WaitingPersons::iterator i = std::find_if(myWaiting.begin(), myWaiting.end(), my_greater(cont.getTime()));
    if(i==myWaiting.end())
        myWaiting.push_back(cont);
    else
        myWaiting.insert(i, cont);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
