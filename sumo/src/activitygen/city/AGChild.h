/****************************************************************************/
/// @file    AGChild.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Person in age to go to school: linked to a school object
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
#ifndef AGCHILD_H
#define AGCHILD_H


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
#include "AGPerson.h"
#include "AGPosition.h"
#include "AGSchool.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGChild : public AGPerson {
public:
    AGChild(int age) :
        AGPerson(age),
        school(NULL) {};
    void print();
    bool setSchool(AGSchool* school);
    /**
     * @param schools: school vector from City object
     * @param housepos: Position of the households habitation
     * @return if a school was found corresponding to the child's age.
     */
    bool allocateASchool(std::list<AGSchool>* schools, AGPosition housePos);
    /**
     * @return if the child is now without any school
     */
    bool leaveSchool();
    bool haveASchool() const;
    AGPosition getSchoolLocation() const;
    int getSchoolOpening() const;
    int getSchoolClosing() const;

private:
    AGSchool* school;
};

#endif

/****************************************************************************/
