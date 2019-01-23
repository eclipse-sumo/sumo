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
/// @file    AGSchool.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Correspond to given ages and referenced by children. Has a precise location.
/****************************************************************************/
#ifndef AGSCHOOL_H
#define AGSCHOOL_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include "AGPosition.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGSchool {
public:
    AGSchool(int capacity_, AGPosition pos, int beginAge, int endAge, int open, int close) :
        beginAge(beginAge),
        endAge(endAge),
        capacity(capacity_),
        initCapacity(capacity_),
        location(pos),
        opening(open),
        closing(close) {};
    void print() const;
    int getPlaces();
    bool addNewChild();
    bool removeChild();
    int getBeginAge();
    int getEndAge();
    bool acceptThisAge(int age);
    AGPosition getPosition();
    int getClosingHour();
    int getOpeningHour();

private:
    int beginAge, endAge;
    int capacity;
    int initCapacity;
    AGPosition location;
    int opening, closing;
};

#endif

/****************************************************************************/
