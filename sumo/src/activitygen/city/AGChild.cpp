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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
    if (school == NULL) {
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
    SUMOReal minDist = std::numeric_limits<SUMOReal>::infinity();
    AGSchool* sch = NULL;
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
    if (school != NULL)
        if (!school->removeChild()) {
            return false;
        }
    school = NULL;
    return true;
}

bool
AGChild::haveASchool() const {
    return (school != NULL);
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
