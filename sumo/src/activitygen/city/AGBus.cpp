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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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
AGBus::print() {
    std::cout << "- Bus:" << " name=" << name << " depTime=" << departureTime << std::endl;
}

/****************************************************************************/
