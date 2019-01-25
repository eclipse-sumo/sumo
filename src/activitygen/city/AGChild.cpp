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
/// @file    AGChild.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Person in age to go to school: linked to a school object
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <vector>
#include <limits>
#include "AGChild.h"
#include "AGSchool.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
AGChild::print() const {
    std::cout << "- Child: Age=" << age << " School=" << school << std::endl;
}

bool
AGChild::setSchool(AGSchool* school) {
    if (school == nullptr) {
        return false;
    }
    bool enoughPlace = school->addNewChild();
    if (enoughPlace) {
        this->school = school;
    }
    return enoughPlace;
}

bool
AGChild::allocateASchool(std::list<AGSchool>* schools, AGPosition housePos) {
    double minDist = std::numeric_limits<double>::infinity();
    AGSchool* sch = nullptr;
    if (schools->size() == 0) {
        return false;
    }
    std::list<AGSchool>::iterator it;

    for (it = schools->begin(); it != schools->end(); ++it) {
        if (it->acceptThisAge(age) && it->getPlaces() > 0 && housePos.distanceTo(it->getPosition()) < minDist) {
            minDist = housePos.distanceTo(it->getPosition());
            sch = &(*it);
        }
    }
    return setSchool(sch);
}

bool
AGChild::leaveSchool() {
    if (school != nullptr)
        if (!school->removeChild()) {
            return false;
        }
    school = nullptr;
    return true;
}

bool
AGChild::haveASchool() const {
    return (school != nullptr);
}

AGPosition
AGChild::getSchoolLocation() const {
    return school->getPosition();
}

int
AGChild::getSchoolClosing() const {
    return school->getClosingHour();
}

int
AGChild::getSchoolOpening() const {
    return school->getOpeningHour();
}

/****************************************************************************/
