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
/// @file    AGPerson.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Parent object of every person, contains age and any natural characteristic
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "AGPerson.h"
#include <utils/common/RandHelper.h>
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGPerson::AGPerson(int age) : age(age) {}


AGPerson::~AGPerson() {}


void
AGPerson::print() const {
    std::cout << "- Person: Age=" << age << std::endl;
}


int
AGPerson::getAge() const {
    return age;
}


bool
AGPerson::decide(double proba) const {
    return (RandHelper::rand(1000) < static_cast<int>(1000.0f * proba));
}

/****************************************************************************/
