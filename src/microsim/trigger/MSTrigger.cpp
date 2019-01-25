/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTrigger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// An abstract device that changes the state of the micro simulation
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#include "MSTrigger.h"


// ===========================================================================
// static member definitions
// ===========================================================================
std::set<MSTrigger*> MSTrigger::myInstances;


// ===========================================================================
// method definitions
// ===========================================================================
MSTrigger::MSTrigger(const std::string& id) :
    Named(id) {
    myInstances.insert(this);
}


MSTrigger::~MSTrigger() {
    myInstances.erase(this);
}


void MSTrigger::cleanup() {
    while (!myInstances.empty()) {
        delete *myInstances.begin();
    }
}
