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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#ifndef AGSCHOOL_H
#define AGSCHOOL_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    void print();
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
