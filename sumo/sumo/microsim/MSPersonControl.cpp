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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
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
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include "MSNet.h"
#include "MSPerson.h"
#include "MSPersonControl.h"

MSPersonControl::SameDepartureTimeCont::SameDepartureTimeCont() :
    m_uiArrivalTime(0) {
}


MSPersonControl::SameDepartureTimeCont::SameDepartureTimeCont(unsigned int time) :
    m_pPersons(0), m_uiArrivalTime(time) {
}

MSPersonControl::SameDepartureTimeCont::~SameDepartureTimeCont() {
  for(MSNet::PersonCont::iterator i=m_pPersons->begin(); i!=m_pPersons->end(); i++) {
    delete (*i);
  }
  m_pPersons->clear();
}

void MSPersonControl::SameDepartureTimeCont::add(MSPerson *person) {
  m_pPersons->push_back(person);
}

void MSPersonControl::SameDepartureTimeCont::add(MSNet::PersonCont *cont) {
  copy(cont->begin(), cont->end(), back_inserter(*m_pPersons));
}

unsigned int MSPersonControl::SameDepartureTimeCont::getTime() {
  return m_uiArrivalTime;
}

MSNet::PersonCont *MSPersonControl::SameDepartureTimeCont::getPersons() {
  return m_pPersons;
}




MSPersonControl::MSPersonControl(WaitingPersons &cont) :
    m_pWaiting(cont) {
}

MSPersonControl::~MSPersonControl() {
  m_pWaiting.clear();
}

void MSPersonControl::add(MSNet::Time when, MSPerson *person) {
  WaitingPersons::iterator i = find_if(m_pWaiting.begin(), m_pWaiting.end(), equal(when));
  if(i==m_pWaiting.end()) {
    SameDepartureTimeCont cont;
    cont.add(person);
    push(cont);
  } else {
    (*i).add(person);
  }
}

MSNet::PersonCont *MSPersonControl::getPersons(MSNet::Time time) {
  WaitingPersons::iterator i = m_pWaiting.begin();
  if(i==m_pWaiting.end()) return 0;
  if((*i).getTime()!=time) return 0;
  return (*i).getPersons(); 
}


void MSPersonControl::push(unsigned int time, MSNet::PersonCont *list) {
  SameDepartureTimeCont *toPush = new SameDepartureTimeCont(time);
  toPush->add(list);
  push(*toPush);
}

void MSPersonControl::push(SameDepartureTimeCont cont) {
  WaitingPersons::iterator i = find_if(m_pWaiting.begin(), m_pWaiting.end(), my_greater(cont.getTime()));
  if(i==m_pWaiting.end())
    m_pWaiting.push_back(cont);
  else 
    m_pWaiting.insert(i, cont);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSPersonControl.iC"
//#endif

// Local Variables:
// mode:C++
// End:
