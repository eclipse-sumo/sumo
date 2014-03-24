/****************************************************************************/
/// @file    AGCar.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Cars owned by people of the city: included in households.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors
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
#include <sstream>
#include <string>
#include "AGCar.h"
#include "AGAdult.h"


// ===========================================================================
// method definitions
// ===========================================================================
std::string
AGCar::createName(int idHH, int idCar) {
    std::ostringstream os;
    os << "h" << idHH << "c" << idCar;
    return os.str();
}

bool
AGCar::associateTo(AGAdult* pers) {
    if (currentUser == NULL) {
        currentUser = pers;
        return true;
    }
    return false;
}

bool
AGCar::isAssociated() const {
    return (currentUser != NULL);
}

std::string
AGCar::getName() const {
    return idName;
}

/****************************************************************************/
