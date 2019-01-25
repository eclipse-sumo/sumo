/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSJunctionControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// Container for junctions; performs operations on all stored junctions
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <algorithm>
#include "MSInternalJunction.h"
#include "MSJunctionControl.h"


// ===========================================================================
// member method definitions
// ===========================================================================
MSJunctionControl::MSJunctionControl() {
}


MSJunctionControl::~MSJunctionControl() {
}


void
MSJunctionControl::postloadInitContainer() {
    // initialize normal junctions before internal junctions
    // (to allow calling getIndex() during initialization of internal junction links)
    for (const auto& i : *this) {
        if (i.second->getType() != NODETYPE_INTERNAL) {
            i.second->postloadInit();
        }
    }
    for (const auto& i : *this) {
        if (i.second->getType() == NODETYPE_INTERNAL) {
            i.second->postloadInit();
        }
    }
}


/****************************************************************************/

