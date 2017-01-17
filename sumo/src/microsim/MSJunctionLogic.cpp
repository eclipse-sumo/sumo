/****************************************************************************/
/// @file    MSJunctionLogic.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// kinds of logic-implementations.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
MSLogicJunction::LinkBits MSJunctionLogic::myDummyFoes;


// ===========================================================================
// member method definitions
// ===========================================================================
int
MSJunctionLogic::nLinks() {
    return myNLinks;
}


MSJunctionLogic::MSJunctionLogic(int nLinks) :
    myNLinks(nLinks) {}


MSJunctionLogic::~MSJunctionLogic() {}



/****************************************************************************/

