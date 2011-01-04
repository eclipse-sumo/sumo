/****************************************************************************/
/// @file    MSJunctionLogic.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// kinds of logic-implementations.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "MSJunctionLogic.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static variable definitions
// ===========================================================================
MSLogicJunction::LinkFoes MSJunctionLogic::myDummyFoes;


// ===========================================================================
// member method definitions
// ===========================================================================
unsigned int
MSJunctionLogic::nLinks() {
    return myNLinks;
}


unsigned int
MSJunctionLogic::nInLanes() {
    return myNInLanes;
}


MSJunctionLogic::MSJunctionLogic(unsigned int nLinks,
                                 unsigned int nInLanes) :
        myNLinks(nLinks),
        myNInLanes(nInLanes) {}


MSJunctionLogic::~MSJunctionLogic() {}



/****************************************************************************/

