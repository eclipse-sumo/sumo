/****************************************************************************/
/// @file    MSTriggerControl.cpp
/// @author  Daniel Krajzewicz
/// @date    :iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
/// @version $Id$
///
// A storage for triggers
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

#include <vector>
#include <algorithm>
#include "MSTriggerControl.h"
#include "MSTrigger.h"

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
MSTriggerControl::MSTriggerControl() throw()
{
}


MSTriggerControl::~MSTriggerControl() throw()
{
    for (TriggerVector::iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
        delete(*i);
    }
}


void
MSTriggerControl::addTrigger(MSTrigger *t) throw()
{
    myTrigger.push_back(t);
}


MSTrigger *
MSTriggerControl::getTrigger(const std::string &id) throw()
{
    for (TriggerVector::iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
        if ((*i)->getID()==id) {
            return (*i);
        }
    }
    return 0;
}


/****************************************************************************/

