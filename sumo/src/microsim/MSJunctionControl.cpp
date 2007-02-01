/****************************************************************************/
/// @file    MSJunctionControl.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id: $
///
// Junction-operations.
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunctionControl.h"
#include "MSJunction.h"
#include <algorithm>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
MSJunctionControl::DictType MSJunctionControl::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
MSJunctionControl::MSJunctionControl(string id, JunctionCont* j) :
        myID(id), myJunctions(j)
{}


MSJunctionControl::~MSJunctionControl()
{
    delete myJunctions;
}

bool
MSJunctionControl::dictionary(string id, MSJunctionControl* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSJunctionControl*
MSJunctionControl::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSJunctionControl::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSJunctionControl::resetRequests()
{
    for_each(myJunctions->begin(), myJunctions->end(),
             mem_fun(& MSJunction::clearRequests));
}


void
MSJunctionControl::setAllowed()
{
    for_each(myJunctions->begin(), myJunctions->end(),
             mem_fun(& MSJunction::setAllowed));
}



/****************************************************************************/

