/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGBus.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// A bus driving in the city
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <string>
#include "AGBus.h"


// ===========================================================================
// method definitions
// ===========================================================================
void AGBus::setName(std::string name) {
    this->name = name;
}

int
AGBus::getDeparture() {
    return departureTime;
}

std::string
AGBus::getName() {
    return name;
}

void
AGBus::print() const {
    std::cout << "- Bus:" << " name=" << name << " depTime=" << departureTime << std::endl;
}

/****************************************************************************/
