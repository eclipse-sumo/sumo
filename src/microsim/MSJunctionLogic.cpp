/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSJunctionLogic.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
///
// kinds of logic-implementations.
/****************************************************************************/
#include <config.h>

#include "MSJunctionLogic.h"



// ===========================================================================
// static variable definitions
// ===========================================================================
MSLogicJunction::LinkBits MSJunctionLogic::myDummyFoes;


// ===========================================================================
// member method definitions
// ===========================================================================
MSJunctionLogic::MSJunctionLogic(int nLinks) :
    myNLinks(nLinks) {}


MSJunctionLogic::~MSJunctionLogic() {}


/****************************************************************************/
