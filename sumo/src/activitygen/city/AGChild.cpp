/****************************************************************************/
/// @file    AGChild.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Person in age to go to school: linked to a school object
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "AGChild.h"
#include "AGSchool.h"
#include <cfloat>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGChild::print() {
    cout << "- Child: Age=" << age << " School=" << school << endl;
}

bool
AGChild::setSchool(AGSchool *school) {
    if (school == NULL)
        return false;
    bool enoughPlace = school->addNewChild();
    if (enoughPlace)
        this->school = school;
    return enoughPlace;
}

bool
AGChild::alocateASchool(list<AGSchool> *schools, AGPosition housePos) {
    float minDist = FLT_MAX;
    AGSchool *sch = NULL;
    if (schools->size() == 0)
        return false;
    list<AGSchool>::iterator it;

    for (it = schools->begin() ; it!=schools->end() ; ++it) {
        if (it->acceptThisAge(age) && it->getPlaces()>0 && housePos.distanceTo(it->getPosition()) < minDist) {
            minDist = housePos.distanceTo(it->getPosition());
            sch = &(*it);
        }
    }
    return setSchool(sch);
}

bool
AGChild::leaveSchool() {
    if (school != NULL)
        if (!school->removeChild())
            return false;
    school = NULL;
    return true;
}

bool
AGChild::haveASchool() {
    if (school == NULL)
        return false;
    return true;
}

AGPosition
AGChild::getSchoolLocation() {
    return school->getPosition();
}

int
AGChild::getSchoolClosing() {
    return school->getClosingHour();
}

int
AGChild::getSchoolOpeining() {
    return school->getOpeningHour();
}

/****************************************************************************/
