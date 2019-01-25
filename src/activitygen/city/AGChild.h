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
/// @file    AGChild.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Person in age to go to school: linked to a school object
/****************************************************************************/
#ifndef AGCHILD_H
#define AGCHILD_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
    void print() const;
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
