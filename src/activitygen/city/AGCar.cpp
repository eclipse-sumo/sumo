/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
    if (currentUser == nullptr) {
        currentUser = pers;
        return true;
    }
    return false;
}

bool
AGCar::isAssociated() const {
    return (currentUser != nullptr);
}

std::string
AGCar::getName() const {
    return idName;
}

/****************************************************************************/
