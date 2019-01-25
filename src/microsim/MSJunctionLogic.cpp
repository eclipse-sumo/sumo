/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSJunctionLogic.h"



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

