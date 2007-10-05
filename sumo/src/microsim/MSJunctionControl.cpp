/****************************************************************************/
/// @file    MSJunctionControl.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunctionControl.h"
#include "MSJunction.h"
#include <algorithm>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSJunctionControl::MSJunctionControl()
{
}


MSJunctionControl::~MSJunctionControl()
{
}


void
MSJunctionControl::resetRequests()
{
    const vector<MSJunction*> &junctions = buildAndGetStaticVector();
    for_each(junctions.begin(), junctions.end(), mem_fun(& MSJunction::clearRequests));
}


void
MSJunctionControl::setAllowed()
{
    const vector<MSJunction*> &junctions = buildAndGetStaticVector();
    for_each(junctions.begin(), junctions.end(), mem_fun(& MSJunction::setAllowed));
}


void
MSJunctionControl::postloadInitContainer()
{
    const vector<MSJunction*> &junctions = buildAndGetStaticVector();
    for (vector<MSJunction*>::const_iterator i=junctions.begin(); i!=junctions.end(); i++) {
        (*i)->postloadInit();
    }
}


/****************************************************************************/

